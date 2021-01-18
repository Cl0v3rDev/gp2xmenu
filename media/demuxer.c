//=================== DEMUXER v2.5 =========================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"

#include "stream_internal.h"
#include "demuxer.h"
#include "stheader.h"

#include "afmt.h"

// Should be set to 1 by demux module if ids it passes to new_sh_audio and
// new_sh_video don't match aids and vids it accepts from the command line
int demux_aid_vid_mismatch = 0;
// defined in mplayer.c and mencoder.c
int verbose = 0;
int identify = 0;

void free_demuxer_stream(demux_stream_t * ds)
{
	ds_free_packs(ds);
	free(ds);
}

demux_stream_t *new_demuxer_stream(struct demuxer_st *demuxer, int id)
{
	demux_stream_t *ds = malloc(sizeof(demux_stream_t));
	ds->buffer_pos = ds->buffer_size = 0;
	ds->buffer = NULL;
	ds->pts = 0;
	ds->pts_bytes = 0;
	ds->eof = 0;
	ds->pos = 0;
	ds->dpos = 0;
	ds->pack_no = 0;
//---------------
	ds->packs = 0;
	ds->bytes = 0;
	ds->first = ds->last = ds->current = NULL;
	ds->id = id;
	ds->demuxer = demuxer;
//----------------
	ds->asf_seq = -1;
	ds->asf_packet = NULL;
//----------------
	ds->ss_mul = ds->ss_div = 0;
//----------------
	ds->sh = NULL;
	return ds;
}

demuxer_t *new_demuxer(stream_t * stream, int type, int a_id, int v_id, int s_id)
{
	demuxer_t *d = malloc(sizeof(demuxer_t));
	memset(d, 0, sizeof(demuxer_t));
	d->stream = stream;
	d->movi_start = stream->start_pos;
	d->movi_end = stream->end_pos;
	d->seekable = 1;
	d->synced = 0;
	d->filepos = 0;
	d->audio = new_demuxer_stream(d, a_id);
	d->video = new_demuxer_stream(d, v_id);
	d->sub = new_demuxer_stream(d, s_id);
	d->type = type;
	stream_reset(stream);
	stream_seek(stream, stream->start_pos);

	// 20050227 ghcstop add
#ifdef GP2X
	pthread_mutex_init(&d->dmutex, NULL);	// demux_fill_buffer locking용
#endif
	d->rhd = 0;

	return d;
}

sh_audio_t *new_sh_audio(demuxer_t * demuxer, int id)
{
	if (id > MAX_A_STREAMS - 1 || id < 0) {
		mp_msg(MSGT_DEMUXER, MSGL_WARN, "Requested audio stream id overflow (%d > %d)\n", id, MAX_A_STREAMS);
		return NULL;
	}
	if (demuxer->a_streams[id]) {
		mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_AudioStreamRedefined, id);
	} else {
		sh_audio_t *sh;
		mp_msg(MSGT_DEMUXER, MSGL_V, MSGTR_FoundAudioStream, id);
		demuxer->a_streams[id] = malloc(sizeof(sh_audio_t));
		memset(demuxer->a_streams[id], 0, sizeof(sh_audio_t));
		sh = demuxer->a_streams[id];
		// set some defaults
		sh->samplesize = 2;
		sh->sample_format = AFMT_S16_NE;
		sh->audio_out_minsize = 8192;	/* default size, maybe not enough for Win32/ACM */
		if (identify && !demux_aid_vid_mismatch)
			mp_msg(MSGT_GLOBAL, MSGL_INFO, "ID_AUDIO_ID=%d\n", id);
	}
	return demuxer->a_streams[id];
}

void free_sh_audio(sh_audio_t * sh)
{
	mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing sh_audio at %p\n", sh);
	if (sh->wf)
		free(sh->wf);
	free(sh);
}

sh_video_t *new_sh_video(demuxer_t * demuxer, int id)
{
	if (id > MAX_V_STREAMS - 1 || id < 0) {
		mp_msg(MSGT_DEMUXER, MSGL_WARN, "Requested video stream id overflow (%d > %d)\n", id, MAX_V_STREAMS);
		return NULL;
	}
	if (demuxer->v_streams[id]) {
		mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_VideoStreamRedefined, id);
	} else {
		mp_msg(MSGT_DEMUXER, MSGL_V, MSGTR_FoundVideoStream, id);
		demuxer->v_streams[id] = malloc(sizeof(sh_video_t));
		memset(demuxer->v_streams[id], 0, sizeof(sh_video_t));
		if (identify && !demux_aid_vid_mismatch)
			mp_msg(MSGT_GLOBAL, MSGL_INFO, "ID_VIDEO_ID=%d\n", id);
	}
	return demuxer->v_streams[id];
}

void free_sh_video(sh_video_t * sh)
{
	mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing sh_video at %p\n", sh);
	if (sh->bih)
		free(sh->bih);
	free(sh);
}

extern void demux_close_audio(demuxer_t * demuxer);
extern void demux_close_ogg(demuxer_t * demuxer);
extern void demux_close_demuxers(demuxer_t * demuxer);
extern void demux_close_avi(demuxer_t * demuxer);
extern void demux_close_mkv(demuxer_t * demuxer);

void free_demuxer(demuxer_t * demuxer)
{
	int i;
	mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing demuxer at %p\n", demuxer);
	switch (demuxer->type) {
	case DEMUXER_TYPE_AUDIO:
		demux_close_audio(demuxer);
		break;
	case DEMUXER_TYPE_OGG:
		demux_close_ogg(demuxer);
		break;
	case DEMUXER_TYPE_DEMUXERS:
		demux_close_demuxers(demuxer);
		return;
	case DEMUXER_TYPE_AVI:
	case DEMUXER_TYPE_AVI_NI:
	case DEMUXER_TYPE_AVI_NINI:
		demux_close_avi(demuxer);
		return;
	}
	// free streams:
	for (i = 0; i < 256; i++) {
		if (demuxer->a_streams[i])
			free_sh_audio(demuxer->a_streams[i]);
		if (demuxer->v_streams[i])
			free_sh_video(demuxer->v_streams[i]);
	}
	//if(sh_audio) free_sh_audio(sh_audio);
	//if(sh_video) free_sh_video(sh_video);
	// free demuxers:
	free_demuxer_stream(demuxer->audio);
	free_demuxer_stream(demuxer->video);
	free_demuxer_stream(demuxer->sub);
	if (demuxer->info) {
		for (i = 0; demuxer->info[i] != NULL; i++)
			free(demuxer->info[i]);
		free(demuxer->info);
	}
	// ghcstop add 050227 ====================
#ifdef GP2X
	pthread_mutex_destroy(&demuxer->dmutex);
#endif
	// ghcstop add 050227 ====================

	free(demuxer);
}

inline void ds_add_packet(demux_stream_t * ds, demux_packet_t * dp)
{
	// demux_packet_t* dp=new_demux_packet(len);
	// stream_read(stream,dp->buffer,len);
	// dp->pts=pts; //(float)pts/90000.0f;
	// dp->pos=pos;
	// append packet to DS stream:
	++ds->packs;
	ds->bytes += dp->len;
	if (ds->last) {
		// next packet in stream
		ds->last->next = dp;
		ds->last = dp;
	} else {
		// first packet in stream
		ds->first = ds->last = dp;
	}
	//mp_dbg(MSGT_DEMUXER,MSGL_DBG2,"DEMUX: Append packet to %s, len=%d  pts=%5.3f  pos=%u  [packs: A=%d V=%d]\n",
	//    (ds==ds->demuxer->audio)?"d_audio":"d_video",
	//    dp->len,dp->pts,(unsigned int)dp->pos,ds->demuxer->audio->packs,ds->demuxer->video->packs);
}

inline void ds_read_packet(demux_stream_t * ds, stream_t * stream, int len, float pts, off_t pos, int flags)
{
	demux_packet_t *dp = new_demux_packet(len);
	len = stream_read(stream, dp->buffer, len);
	if (dp->len != len)
		resize_demux_packet(dp, len);
	dp->pts = pts;		//(float)pts/90000.0f;
	dp->pos = pos;
	dp->flags = flags;
	// append packet to DS stream:
	ds_add_packet(ds, dp);
}

// return value:
//     0 = EOF or no stream found or invalid type
//     1 = successfully read a packet
int demux_avi_fill_buffer(demuxer_t * demux);
int demux_avi_fill_buffer_ni(demuxer_t * demux, demux_stream_t * ds);
int demux_avi_fill_buffer_nini(demuxer_t * demux, demux_stream_t * ds);
int demux_audio_fill_buffer(demux_stream_t * ds);
extern int demux_demuxers_fill_buffer(demuxer_t * demux, demux_stream_t * ds);
extern int demux_ogg_fill_buffer(demuxer_t * d);

inline int demux_fill_buffer(demuxer_t * demux, demux_stream_t * ds)
{
	// Note: parameter 'ds' can be NULL!
	//printf("demux->type=%d\n",demux->type);
	//printf("demux->type=%d, %s\n", demux->type, (ds==ds->demuxer->audio)?"d_audio":"d_video"); // ghcstop
	int result = 0;

	if (demux->type == DEMUXER_TYPE_DEMUXERS) {
		return demux_demuxers_fill_buffer(demux, ds);
	}

#ifdef GP2X
	pthread_mutex_lock(&demux->dmutex);
#endif

	switch (demux->type) {
	case DEMUXER_TYPE_AVI:
		result = demux_avi_fill_buffer(demux);
		break;
	case DEMUXER_TYPE_AVI_NI:
		result = demux_avi_fill_buffer_ni(demux, ds);
		break;
	case DEMUXER_TYPE_AVI_NINI:
		result = demux_avi_fill_buffer_nini(demux, ds);
		break;
	case DEMUXER_TYPE_OGG:
		result = demux_ogg_fill_buffer(demux);
		break;
	case DEMUXER_TYPE_AUDIO:
		result = demux_audio_fill_buffer(ds);
		break;
	case DEMUXER_TYPE_DEMUXERS:
		result = 0;
		break;
	}

#ifdef GP2X
	pthread_mutex_unlock(&demux->dmutex);
#endif

	return result;
}

// return value:
//     0 = EOF
//     1 = succesfull
inline int ds_fill_buffer(demux_stream_t * ds)
{
	demuxer_t *demux = ds->demuxer;
	if (ds->current)
		free_demux_packet(ds->current);
	if (verbose > 2) {
		if (ds == demux->audio)
			mp_dbg(MSGT_DEMUXER, MSGL_DBG3, "ds_fill_buffer(d_audio) called\n");
		else if (ds == demux->video)
			mp_dbg(MSGT_DEMUXER, MSGL_DBG3, "ds_fill_buffer(d_video) called\n");
		else if (ds == demux->sub)
			mp_dbg(MSGT_DEMUXER, MSGL_DBG3, "ds_fill_buffer(d_sub) called\n");
		else
			mp_dbg(MSGT_DEMUXER, MSGL_DBG3, "ds_fill_buffer(unknown 0x%X) called\n", (unsigned int) ds);
	}
	while (1) {
		if ((ds->packs) && (ds->first)) {
			demux_packet_t *p = ds->first;
			// copy useful data:
			ds->buffer = p->buffer;
			ds->buffer_pos = 0;
			ds->buffer_size = p->len;
			ds->pos = p->pos;
			ds->dpos += p->len;	// !!!
			++ds->pack_no;
			if (p->pts) {
				ds->pts = p->pts;
				ds->pts_bytes = 0;
			}
			ds->pts_bytes += p->len;	// !!!
			ds->flags = p->flags;
			// unlink packet:
			ds->bytes -= p->len;
			ds->current = p;
			ds->first = p->next;
			if (!ds->first)
				ds->last = NULL;
			--ds->packs;
			return 1;	//ds->buffer_size;
		}
		if (demux->audio->packs >= MAX_PACKS || demux->audio->bytes >= MAX_PACK_BYTES) {
			mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyAudioInBuffer, demux->audio->packs, demux->audio->bytes);
			mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
			break;
		}
		if (demux->video->packs >= MAX_PACKS || demux->video->bytes >= MAX_PACK_BYTES) {
			mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyVideoInBuffer, demux->video->packs, demux->video->bytes);
			mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
			break;
		}
		if (!demux_fill_buffer(demux, ds)) {
			mp_dbg(MSGT_DEMUXER, MSGL_DBG2, "ds_fill_buffer()->demux_fill_buffer() failed\n");
			break;	// EOF
		}
	}
	ds->buffer_pos = ds->buffer_size = 0;
	ds->buffer = NULL;
	ds->current = NULL;
	mp_msg(MSGT_DEMUXER, MSGL_V, "ds_fill_buffer: EOF reached (stream: %s)  \n", ds == demux->audio ? "audio" : "video");
	ds->eof = 1;
	return 0;
}

inline int demux_read_data(demux_stream_t * ds, unsigned char *mem, int len)
{
	int x;
	int bytes = 0;
	while (len > 0) {
		x = ds->buffer_size - ds->buffer_pos;
		if (x == 0) {
			if (!ds_fill_buffer(ds))
				return bytes;
		} else {
			if (x > len)
				x = len;
			if (mem)
				memcpy(mem + bytes, &ds->buffer[ds->buffer_pos], x);
			bytes += x;
			len -= x;
			ds->buffer_pos += x;
		}
	}
	return bytes;
}

void ds_free_packs(demux_stream_t * ds)
{
	demux_packet_t *dp = ds->first;
	while (dp) {
		demux_packet_t *dn = dp->next;
		free_demux_packet(dp);
		dp = dn;
	}
	if (ds->asf_packet) {
		// free unfinished .asf fragments:
		free(ds->asf_packet->buffer);
		free(ds->asf_packet);
		ds->asf_packet = NULL;
	}
	ds->first = ds->last = NULL;
	ds->packs = 0;		// !!!!!
	ds->bytes = 0;
	if (ds->current)
		free_demux_packet(ds->current);
	ds->current = NULL;
	ds->buffer = NULL;
	ds->buffer_pos = ds->buffer_size;
	ds->pts = 0;
	ds->pts_bytes = 0;
}

inline int ds_get_packet(demux_stream_t * ds, unsigned char **start)
{
	int len;
	if (ds->buffer_pos >= ds->buffer_size) {
		if (!ds_fill_buffer(ds)) {
			// EOF
			*start = NULL;
			return -1;
		}
	}
	len = ds->buffer_size - ds->buffer_pos;
	*start = &ds->buffer[ds->buffer_pos];
	ds->buffer_pos += len;
	return len;
}

// ====================================================================

// feed-back from demuxers:
extern int num_elementary_packets100;	// for MPEG-ES fileformat detection
extern int num_elementary_packets101;
extern int num_elementary_packetsPES;
extern int num_elementary_packets1B6;
extern int num_elementary_packets12x;
extern int num_h264_slice;	//combined slice
extern int num_h264_dpa;	//DPA Slice
extern int num_h264_dpb;	//DPB Slice
extern int num_h264_dpc;	//DPC Slice
extern int num_h264_idr;	//IDR Slice
extern int num_h264_sps;
extern int num_h264_pps;
extern int num_mp3audio_packets;

// commandline options, flags:
extern int force_ni;
extern int pts_from_bps;

//extern int audio_id;
//extern int video_id;
static int dvdsub_id = -1;

int asf_check_header(demuxer_t * demuxer);
int read_asf_header(demuxer_t * demuxer);
demux_stream_t *demux_avi_select_stream(demuxer_t * demux, unsigned int id);
demuxer_t *demux_open_avi(demuxer_t * demuxer);
int mov_check_file(demuxer_t * demuxer);
int mov_read_header(demuxer_t * demuxer);
int demux_open_fli(demuxer_t * demuxer);
int demux_open_mf(demuxer_t * demuxer);
int demux_open_film(demuxer_t * demuxer);
int demux_open_roq(demuxer_t * demuxer);

extern int vivo_check_file(demuxer_t * demuxer);
extern void demux_open_vivo(demuxer_t * demuxer);
extern int y4m_check_file(demuxer_t * demuxer);
extern void demux_open_y4m(demuxer_t * demuxer);
extern int roq_check_file(demuxer_t * demuxer);
extern int pva_check_file(demuxer_t * demuxer);
extern demuxer_t *demux_open_pva(demuxer_t * demuxer);
extern int real_check_file(demuxer_t * demuxer);
extern void demux_open_real(demuxer_t * demuxer);
extern int nsv_check_file(demuxer_t * demuxer);
extern int nuv_check_file(demuxer_t * demuxer);
extern void demux_open_nsv(demuxer_t * demuxer);
extern void demux_open_nuv(demuxer_t * demuxer);
extern int demux_audio_open(demuxer_t * demuxer);
extern int demux_ogg_open(demuxer_t * demuxer);
extern int demux_mpg_open(demuxer_t * demuxer);
extern int demux_rawaudio_open(demuxer_t * demuxer);
extern int demux_rawvideo_open(demuxer_t * demuxer);
extern int smjpeg_check_file(demuxer_t * demuxer);
extern int demux_open_smjpeg(demuxer_t * demuxer);
extern int demux_xmms_open(demuxer_t * demuxer);
extern int gif_check_file(demuxer_t * demuxer);
extern int demux_open_gif(demuxer_t * demuxer);
extern int lmlm4_check_file(demuxer_t * demuxer);
extern int demux_open_lmlm4(demuxer_t * demuxer);
extern int ts_check_file(demuxer_t * demuxer);
extern int demux_open_ts(demuxer_t * demuxer);
extern int demux_open_mkv(demuxer_t * demuxer);
extern int ra_check_file(demuxer_t * demuxer);
extern int lavf_check_file(demuxer_t * demuxer);
extern int demux_open_lavf(demuxer_t * demuxer);

extern demuxer_t *init_avi_with_ogg(demuxer_t * demuxer);

int extension_parsing = 1;	// 0=off 1=mixed (used only for unstable formats)

/*
  NOTE : Several demuxers may be opened at the same time so
  demuxers should NEVER rely on an external var to enable them
  self. If a demuxer can't do any autodection it should only use
  file_format. The user can explictly set file_format with the -demuxer
  option so there is really no need for another extra var.
  For conivence an option can be added to set file_format directly
  to the right type (ex: rawaudio,rawvideo).
  Also the stream can override the file_format so a demuxer wich rely
  on a special stream type can set file_format at the stream level
  (ex: tv,mf).
*/

static demuxer_t *demux_open_stream(stream_t * stream, int file_format, int audio_id, int video_id, int dvdsub_id, char *filename)
{

//int file_format=(*file_format_ptr);

	demuxer_t *demuxer = NULL;

	demux_stream_t *d_audio = NULL;
	demux_stream_t *d_video = NULL;

	sh_audio_t *sh_audio = NULL;
	sh_video_t *sh_video = NULL;

//printf("demux_open(%p,%d,%d,%d,%d)  \n",stream,file_format,audio_id,video_id,dvdsub_id);

	if (file_format == DEMUXER_TYPE_RAWAUDIO) {
		demuxer = new_demuxer(stream, DEMUXER_TYPE_RAWAUDIO, audio_id, video_id, dvdsub_id);
	}
	if (file_format == DEMUXER_TYPE_RAWVIDEO) {
		demuxer = new_demuxer(stream, DEMUXER_TYPE_RAWVIDEO, audio_id, video_id, dvdsub_id);
	}
//=============== Try to open as AVI file: =================
	if (file_format == DEMUXER_TYPE_UNKNOWN || file_format == DEMUXER_TYPE_AVI) {
		demuxer = new_demuxer(stream, DEMUXER_TYPE_AVI, audio_id, video_id, dvdsub_id);
		{		//---- RIFF header:
			int id = stream_read_dword_le(demuxer->stream);	// "RIFF"
			if ((id == mmioFOURCC('R', 'I', 'F', 'F'))
			    || (id == mmioFOURCC('O', 'N', '2', ' '))) {
				stream_read_dword_le(demuxer->stream);	//filesize
				id = stream_read_dword_le(demuxer->stream);	// "AVI "
				if (id == formtypeAVI) {
					mp_msg(MSGT_DEMUXER, MSGL_INFO, MSGTR_Detected_XXX_FileFormat, "AVI");
					file_format = DEMUXER_TYPE_AVI;
				}
				if (id == mmioFOURCC('O', 'N', '2', 'f')) {
					mp_msg(MSGT_DEMUXER, MSGL_INFO, MSGTR_Detected_XXX_FileFormat, "ON2 AVI");
					file_format = DEMUXER_TYPE_AVI;
				}
			}
			if (file_format == DEMUXER_TYPE_UNKNOWN) {
				free_demuxer(demuxer);
				demuxer = NULL;
			}
		}
	}
//=============== Try to open as Ogg file: =================
	if (file_format == DEMUXER_TYPE_UNKNOWN || file_format == DEMUXER_TYPE_OGG) {
		demuxer = new_demuxer(stream, DEMUXER_TYPE_OGG, audio_id, video_id, dvdsub_id);
		if (demux_ogg_open(demuxer)) {
			mp_msg(MSGT_DEMUXER, MSGL_INFO, MSGTR_Detected_XXX_FileFormat, "Ogg");
			file_format = DEMUXER_TYPE_OGG;
		} else {
			free_demuxer(demuxer);
			demuxer = NULL;
		}
	}
//=============== Try to open as audio file: =================
	if (file_format == DEMUXER_TYPE_UNKNOWN || file_format == DEMUXER_TYPE_AUDIO) {
		demuxer = new_demuxer(stream, DEMUXER_TYPE_AUDIO, audio_id, video_id, dvdsub_id);
		if (demux_audio_open(demuxer)) {
			mp_msg(MSGT_DEMUXER, MSGL_INFO, MSGTR_DetectedAudiofile);
			file_format = DEMUXER_TYPE_AUDIO;
		} else {
			free_demuxer(demuxer);
			demuxer = NULL;
		}
	}
//=============== Unknown, exiting... ===========================
	if (file_format == DEMUXER_TYPE_UNKNOWN || demuxer == NULL) {
		//mp_msg(MSGT_DEMUXER,MSGL_ERR,MSGTR_FormatNotRecognized); // will be done by mplayer.c after fallback to playlist-parsing
		return NULL;
	}
//====== File format recognized, set up these for compatibility: =========
	d_audio = demuxer->audio;
	d_video = demuxer->video;
//d_dvdsub=demuxer->sub;

	demuxer->file_format = file_format;

	switch (file_format) {
	case DEMUXER_TYPE_AVI:{
			sh_audio_t *sh_a;
			demuxer = (demuxer_t *) demux_open_avi(demuxer);
			if (!demuxer)
				return NULL;	// failed to open
			sh_a = (sh_audio_t *) demuxer->audio->sh;
			if (demuxer->audio->id != -2 && sh_a) {
				// support for Ogg-in-AVI:
				if (sh_a->format == 0xFFFE)
					demuxer = init_avi_with_ogg(demuxer);
				else if (sh_a->format == 0x674F) {
					stream_t *s;
					demuxer_t *od;
					s = new_ds_stream(demuxer->audio);
					od = new_demuxer(s, DEMUXER_TYPE_OGG, -1, -2, -2);
					if (!demux_ogg_open(od)) {
						mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_ErrorOpeningOGGDemuxer);
						free_stream(s);
						demuxer->audio->id = -2;
					} else
						demuxer = new_demuxers_demuxer(demuxer, od, demuxer);
				}
			}
			return demuxer;
//  break;
		}
	}			// switch(file_format)
	pts_from_bps = 0;	// !!!
	if ((sh_video = demuxer->video->sh) && sh_video->bih)
		mp_msg(MSGT_DEMUX, MSGL_INFO, "VIDEO:  [%.4s]  %ldx%ld  %dbpp  %5.3f fps  %5.1f kbps (%4.1f kbyte/s)\n", (char *) &sh_video->bih->biCompression, sh_video->bih->biWidth, sh_video->bih->biHeight, sh_video->bih->biBitCount,
		       sh_video->fps, sh_video->i_bps * 0.008f, sh_video->i_bps / 1024.0f);
	return demuxer;
}

char *audio_stream = NULL;
char *sub_stream = NULL;
int demuxer_type = 0, audio_demuxer_type = 0, sub_demuxer_type = 0;
int audio_stream_cache = 0;

extern int hr_mp3_seek;

extern float stream_cache_min_percent;
extern float stream_cache_prefill_percent;

demuxer_t *demux_open(stream_t * vs, int file_format, int audio_id, int video_id, int dvdsub_id, char *filename)
{
	stream_t *as = NULL, *ss = NULL;
	demuxer_t *vd, *ad = NULL, *sd = NULL;
	int afmt = DEMUXER_TYPE_UNKNOWN, sfmt = DEMUXER_TYPE_UNKNOWN;

	demux_aid_vid_mismatch = 0;

	dprintf("demux_open: ");
	if (audio_stream) {
		dprintf("0\n");
		as = open_stream(audio_stream, 0, &afmt);
		if (!as) {
			mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_CannotOpenAudioStream, audio_stream);
			return NULL;
		}
		// ghcstop caution: as는 바로 위에서 잡힌 넘이다.. 이것은 audio가 있건 둘다 잇는 동영상이건
		// cache를 돌리기 위해서는 그냥 audio stream만 있을 경우 그냥 caching을 하면 되기 때문에
		// 이렇게 만들어 놓은 것이다.
		if (audio_stream_cache) {
			if (!stream_enable_cache(as, audio_stream_cache * 1024, audio_stream_cache * 1024 * (stream_cache_min_percent / 100.0), audio_stream_cache * 1024 * (stream_cache_prefill_percent / 100.0))) {
				free_stream(as);
				mp_msg(MSGT_DEMUXER, MSGL_ERR, "Can't enable audio stream cache\n");
				return NULL;
			}
		}
	}
	if (sub_stream) {
		dprintf("1\n");
		ss = open_stream(sub_stream, 0, &sfmt);
		if (!ss) {
			mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_CannotOpenSubtitlesStream, sub_stream);
			return NULL;
		}
	}

	dprintf("2\n");		// 요기랑
	vd = demux_open_stream(vs, demuxer_type ? demuxer_type : file_format, audio_stream ? -2 : audio_id, video_id, sub_stream ? -2 : dvdsub_id, filename);
	if (!vd) {
		dprintf("3\n");
		if (as)
			free_stream(as);
		if (ss)
			free_stream(ss);
		return NULL;
	}
	if (as) {
		dprintf("4\n");
		ad = demux_open_stream(as, audio_demuxer_type ? audio_demuxer_type : afmt, audio_id, -2, -2, audio_stream);
		if (!ad) {
			mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_OpeningAudioDemuxerFailed, audio_stream);
			free_stream(as);
		} else if (ad->audio->sh && ((sh_audio_t *) ad->audio->sh)->format == 0x55)	// MP3
			hr_mp3_seek = 1;	// Enable high res seeking
	}
	if (ss) {
		dprintf("5\n");
		sd = demux_open_stream(ss, sub_demuxer_type ? sub_demuxer_type : sfmt, -2, -2, dvdsub_id, sub_stream);
		if (!sd) {
			mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_OpeningSubtitlesDemuxerFailed, sub_stream);
			free_stream(ss);
		}
	}

	if (ad && sd) {
		dprintf("6\n");
		return new_demuxers_demuxer(vd, ad, sd);
	} else if (ad) {
		dprintf("7\n");
		return new_demuxers_demuxer(vd, ad, vd);
	} else if (sd) {
		dprintf("8\n");
		return new_demuxers_demuxer(vd, vd, sd);
	} else {
		dprintf("9\n");	// 요기만 거친다.
		return vd;
	}
}

int demux_seek_avi(demuxer_t * demuxer, float rel_seek_secs, int flags);
extern void demux_audio_seek(demuxer_t * demuxer, float rel_seek_secs, int flags);
extern void demux_demuxers_seek(demuxer_t * demuxer, float rel_seek_secs, int flags);
extern void demux_ogg_seek(demuxer_t * demuxer, float rel_seek_secs, int flags);

int demux_seek(demuxer_t * demuxer, float rel_seek_secs, int flags)
{
	demux_stream_t *d_audio = demuxer->audio;
	demux_stream_t *d_video = demuxer->video;
	sh_audio_t *sh_audio = d_audio->sh;
	sh_video_t *sh_video = d_video->sh;

	if (!demuxer->seekable) {
		if (demuxer->file_format == DEMUXER_TYPE_AVI)
			mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_CantSeekRawAVI);
		else
			mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_CantSeekFile);
		return 0;
	}
	// clear demux buffers:
	if (sh_audio) {
		ds_free_packs(d_audio);
		sh_audio->a_buffer_len = 0;
	}
	ds_free_packs(d_video);

	demuxer->stream->eof = 0;	// clear eof flag
	demuxer->video->eof = 0;
	demuxer->audio->eof = 0;

#if 0
	if (sh_audio)
		sh_audio->timer = sh_video->timer;
#else
	if (sh_audio)
		sh_audio->delay = 0;
	if (sh_video)
		sh_video->timer = 0;	// !!!!!!
#endif

	switch (demuxer->file_format) {

	case DEMUXER_TYPE_AVI:
		demux_seek_avi(demuxer, rel_seek_secs, flags);
		break;

	case DEMUXER_TYPE_AUDIO:
		demux_audio_seek(demuxer, rel_seek_secs, flags);
		break;
	case DEMUXER_TYPE_DEMUXERS:
		demux_demuxers_seek(demuxer, rel_seek_secs, flags);
		break;
	case DEMUXER_TYPE_OGG:
		demux_ogg_seek(demuxer, rel_seek_secs, flags);
		break;
	}			// switch(demuxer->file_format)

	return 1;
}

int demux_info_add(demuxer_t * demuxer, char *opt, char *param)
{
	char **info = demuxer->info;
	int n = 0;


	for (n = 0; info && info[2 * n] != NULL; n++) {
		if (!strcasecmp(opt, info[2 * n])) {
			mp_msg(MSGT_DEMUX, MSGL_WARN, MSGTR_DemuxerInfoAlreadyPresent, opt);
			return 0;
		}
	}

	info = demuxer->info = (char **) realloc(info, (2 * (n + 2)) * sizeof(char *));
	info[2 * n] = strdup(opt);
	info[2 * n + 1] = strdup(param);
	memset(&info[2 * (n + 1)], 0, 2 * sizeof(char *));

	return 1;
}

int demux_info_print(demuxer_t * demuxer)
{
	char **info = demuxer->info;
	int n;

	if (!info)
		return 0;

	mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_ClipInfo);
	for (n = 0; info[2 * n] != NULL; n++)
		mp_msg(MSGT_DEMUX, MSGL_INFO, " %s: %s\n", info[2 * n], info[2 * n + 1]);

	return 0;
}

char *demux_info_get(demuxer_t * demuxer, char *opt)
{
	int i;
	char **info = demuxer->info;

	for (i = 0; info && info[2 * i] != NULL; i++) {
		if (!strcasecmp(opt, info[2 * i]))
			return info[2 * i + 1];
	}

	return NULL;
}

extern int demux_avi_control(demuxer_t * demuxer, int cmd, void *arg);
extern int demux_audio_control(demuxer_t * demuxer, int cmd, void *arg);
extern int demux_ogg_control(demuxer_t * demuxer, int cmd, void *arg);

int demux_control(demuxer_t * demuxer, int cmd, void *arg)
{
	switch (demuxer->type) {
	case DEMUXER_TYPE_AVI:
	case DEMUXER_TYPE_AVI_NI:
	case DEMUXER_TYPE_AVI_NINI:
		return demux_avi_control(demuxer, cmd, arg);
	case DEMUXER_TYPE_AUDIO:
		return demux_audio_control(demuxer, cmd, arg);
	case DEMUXER_TYPE_OGG:
		return demux_ogg_control(demuxer, cmd, arg);

	default:
		return DEMUXER_CTRL_NOTIMPL;
	}
}



unsigned long demuxer_get_time_length(demuxer_t * demuxer)
{
	unsigned long get_time_ans;
	if (demux_control(demuxer, DEMUXER_CTRL_GET_TIME_LENGTH, (void *) &get_time_ans) <= 0) {
		get_time_ans = 0;
	}
	return get_time_ans;
}

int demuxer_get_percent_pos(demuxer_t * demuxer)
{
	int ans;
	if (demux_control(demuxer, DEMUXER_CTRL_GET_PERCENT_POS, &ans) <= 0) {
		ans = 0;
	}
	if (ans > 100 || ans < 0)
		ans = 0;
	return ans;
}
