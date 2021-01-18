#include <stdio.h>
#include "demuxer.h"

int demux_ogg_open(demuxer_t * demuxer)
{
	return 0;
}

void demux_close_ogg(demuxer_t * demuxer)
{
}

void demux_ogg_seek(demuxer_t * demuxer, float rel_seek_secs, int flags)
{
}

int demux_ogg_control(demuxer_t * demuxer, int cmd, void *arg)
{
	return DEMUXER_CTRL_NOTIMPL;
}

int demux_ogg_fill_buffer(demuxer_t * d)
{
	return 0;
}

demuxer_t *init_avi_with_ogg(demuxer_t * demuxer)
{
	demuxer->audio->id = -2;
	return demuxer;
}


#include "config.h"
#include "ad_internal.h"

static ad_info_t info = {
	"", "", "", "", "",
	{
	 0,
	 0xffffffff}
};

LIBAD_EXTERN(libvorbis)

static int preinit(sh_audio_t * sh)
{
	return 0;
}

static int init(sh_audio_t * sh)
{
	return 0;
}

static void uninit(sh_audio_t * sh)
{
}

static int control(sh_audio_t * sh, int cmd, void *arg, ...)
{
	return CONTROL_UNKNOWN;
}

static int decode_audio(sh_audio_t * sh, unsigned char *buf, int minlen, int maxlen)
{
	return 0;
}
