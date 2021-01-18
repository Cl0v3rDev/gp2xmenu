#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream.h"
#include "demuxer.h"

//#include "codec-cfg.h"
#include "stheader.h"

#include "dec_audio.h"
#include "ad.h"

/* used for ac3surround decoder - set using -channels option */
int audio_output_channels = 2;

static ad_functions_t *mpadec;

void afm_help()
{
	int i;
	mp_msg(MSGT_DECAUDIO, MSGL_INFO, MSGTR_AvailableAudioFm);
	mp_msg(MSGT_DECAUDIO, MSGL_INFO, "    afm:    info:  (comment)\n");
	for (i = 0; mpcodecs_ad_drivers[i] != NULL; i++)
		if (mpcodecs_ad_drivers[i]->info->comment && mpcodecs_ad_drivers[i]->info->comment[0])
			mp_msg(MSGT_DECAUDIO, MSGL_INFO, "%9s  %s (%s)\n", mpcodecs_ad_drivers[i]->info->short_name, mpcodecs_ad_drivers[i]->info->name, mpcodecs_ad_drivers[i]->info->comment);
		else
			mp_msg(MSGT_DECAUDIO, MSGL_INFO, "%9s  %s\n", mpcodecs_ad_drivers[i]->info->short_name, mpcodecs_ad_drivers[i]->info->name);
}

int init_audio_codec(sh_audio_t * sh_audio)
{
	if (!mpadec->preinit(sh_audio)) {
		mp_msg(MSGT_DECAUDIO, MSGL_ERR, MSGTR_ADecoderPreinitFailed);
		return 0;
	}

/* allocate audio in buffer: */
	if (sh_audio->audio_in_minsize > 0) {
		sh_audio->a_in_buffer_size = sh_audio->audio_in_minsize;
		mp_msg(MSGT_DECAUDIO, MSGL_V, MSGTR_AllocatingBytesForInputBuffer, sh_audio->a_in_buffer_size);
		sh_audio->a_in_buffer = malloc(sh_audio->a_in_buffer_size);
		memset(sh_audio->a_in_buffer, 0, sh_audio->a_in_buffer_size);
		sh_audio->a_in_buffer_len = 0;
	}

/* allocate audio out buffer: */
	sh_audio->a_buffer_size = sh_audio->audio_out_minsize + MAX_OUTBURST;	/* worst case calc. */

	mp_msg(MSGT_DECAUDIO, MSGL_V, MSGTR_AllocatingBytesForOutputBuffer, sh_audio->audio_out_minsize, MAX_OUTBURST, sh_audio->a_buffer_size);

	sh_audio->a_buffer = malloc(sh_audio->a_buffer_size);
	if (!sh_audio->a_buffer) {
		mp_msg(MSGT_DECAUDIO, MSGL_ERR, MSGTR_CantAllocAudioBuf);
		return 0;
	}
	memset(sh_audio->a_buffer, 0, sh_audio->a_buffer_size);
	sh_audio->a_buffer_len = 0;

	if (!mpadec->init(sh_audio)) {
		mp_msg(MSGT_DECAUDIO, MSGL_WARN, MSGTR_ADecoderInitFailed);
		uninit_audio(sh_audio);	// free buffers
		return 0;
	}

	sh_audio->inited = 1;

	if (!sh_audio->channels || !sh_audio->samplerate) {
		mp_msg(MSGT_DECAUDIO, MSGL_WARN, MSGTR_UnknownAudio);
		uninit_audio(sh_audio);	// free buffers
		return 0;
	}

	if (!sh_audio->o_bps)
		sh_audio->o_bps = sh_audio->channels * sh_audio->samplerate * sh_audio->samplesize;

	/*
	   ghcstop
	   AUDIO: 44100 Hz, 2 ch, 16 bit (0x10), ratio: 24000->176400 (192.0 kbit)
	 */
	mp_msg(MSGT_DECAUDIO, MSGL_INFO, "AUDIO: %d Hz, %d ch, %d bit (0x%X), ratio: %d->%d (%3.1f kbit)\n", sh_audio->samplerate, sh_audio->channels, sh_audio->samplesize * 8, sh_audio->sample_format, sh_audio->i_bps, sh_audio->o_bps,
	       sh_audio->i_bps * 8 * 0.001);

	sh_audio->a_out_buffer_size = sh_audio->a_buffer_size;
	sh_audio->a_out_buffer = sh_audio->a_buffer;
	sh_audio->a_out_buffer_len = sh_audio->a_buffer_len;

	return 1;
}

int init_audio(sh_audio_t * sh_audio)
{
	ad_functions_t **drivers = mpcodecs_ad_drivers;
	sh_audio->inited = 0;

	while (*drivers != NULL) {
		unsigned int *format = (*drivers)->info->format;
		while (*format != 0xffffffff) {
			if (*format == sh_audio->format)
				break;
			++format;
		}
		if (*format != 0xffffffff) {
			mpadec = *drivers;
			mp_msg(MSGT_DECAUDIO, MSGL_INFO, MSGTR_OpeningAudioDecoder, mpadec->info->short_name, mpadec->info->name);
			if (init_audio_codec(sh_audio))
				return 1;
		}
		++drivers;
	}
	if (!sh_audio->inited) {
		mp_msg(MSGT_DECAUDIO, MSGL_ERR, MSGTR_CantFindAudioCodec, sh_audio->format);
		mp_msg(MSGT_DECAUDIO, MSGL_HINT, MSGTR_RTFMCodecs);
	}
	return 0;
}

extern char *get_path(char *filename);

void uninit_audio(sh_audio_t * sh_audio)
{
	if (sh_audio->inited) {
		mp_msg(MSGT_DECAUDIO, MSGL_V, MSGTR_UninitAudioStr, mpadec->info->name);
		mpadec->uninit(sh_audio);
		sh_audio->inited = 0;
	}
	if (sh_audio->a_out_buffer != sh_audio->a_buffer)
		free(sh_audio->a_out_buffer);
	sh_audio->a_out_buffer = NULL;
	if (sh_audio->a_buffer)
		free(sh_audio->a_buffer);
	sh_audio->a_buffer = NULL;
	if (sh_audio->a_in_buffer)
		free(sh_audio->a_in_buffer);
	sh_audio->a_in_buffer = NULL;
	if (sh_audio->wf)
		free(sh_audio->wf);
	sh_audio->wf = NULL;
}

// ghcstop, minlen: device driver에 채울수 있는 크기, maxlen: 상윗단 버퍼에 남아 있는 크기
int decode_audio(sh_audio_t * sh_audio, unsigned char *buf, int minlen, int maxlen)
{
	if (!sh_audio->inited)
		return -1;	// no codec

	// no filter, just decode:
	// FIXME: don't drop initial decoded data in a_buffer!
	return mpadec->decode_audio(sh_audio, buf, minlen, maxlen);
}

void resync_audio_stream(sh_audio_t * sh_audio)
{
	sh_audio->a_in_buffer_len = 0;	// clear audio input buffer
	if (!sh_audio->inited)
		return;
	mpadec->control(sh_audio, ADCTRL_RESYNC_STREAM, NULL);
}

void skip_audio_frame(sh_audio_t * sh_audio)
{
	if (!sh_audio->inited)
		return;
	if (mpadec->control(sh_audio, ADCTRL_SKIP_FRAME, NULL) == CONTROL_TRUE)
		return;
	// default skip code:
	ds_fill_buffer(sh_audio->ds);	// skip block
}
