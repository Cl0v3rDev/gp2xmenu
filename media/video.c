// read video frame

#include "config.h"

#include <stdio.h>
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mp_msg.h"
#include "help_mp.h"

#include "stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "parse_es.h"
#include "mpeg_hdr.h"

/* biCompression constant */
#define BI_RGB        0L

static mp_mpeg_header_t picture;

static int telecine = 0;
static float telecine_cnt = -2.5;

int video_read_properties(sh_video_t * sh_video)
{
	demux_stream_t *d_video = sh_video->ds;

	enum {
		VIDEO_MPEG12,
		VIDEO_MPEG4,
		VIDEO_H264,
		VIDEO_OTHER
	} video_codec;

	if ((d_video->demuxer->file_format == DEMUXER_TYPE_PVA) || (d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_ES) || (d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_PS) || (d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_TY)
	    || (d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_TS && ((sh_video->format == 0x10000001)
									  || (sh_video->format == 0x10000002)))
	    )
		video_codec = VIDEO_MPEG12;
	else if ((d_video->demuxer->file_format == DEMUXER_TYPE_MPEG4_ES)
		 || ((d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_TS)
		     && (sh_video->format == 0x10000004))
	    )
		video_codec = VIDEO_MPEG4;
	else if ((d_video->demuxer->file_format == DEMUXER_TYPE_H264_ES) || ((d_video->demuxer->file_format == DEMUXER_TYPE_MPEG_TS)
									     && (sh_video->format == 0x10000005))
	    )
		video_codec = VIDEO_H264;
	else
		video_codec = VIDEO_OTHER;

	// demuxer의 파일 포맷에 따른 정보 설정?, videobuffer도 malloc하고...
	// video format & video aspect ratio등 설정
	// Determine image properties:
	switch (video_codec) {
		//ghcstop avi or asf의 경우(DivX의 경우 여기 걸리겠지롱)
	case VIDEO_OTHER:{
			if ((d_video->demuxer->file_format == DEMUXER_TYPE_ASF)
			    || (d_video->demuxer->file_format == DEMUXER_TYPE_AVI)) {
				// display info:

#if 0
				if (sh_video->bih->biCompression == BI_RGB && (sh_video->video.fccHandler == mmioFOURCC('D', 'I', 'B', ' ')
									       || sh_video->video.fccHandler == mmioFOURCC('R', 'G', 'B', ' ')
									       || sh_video->video.fccHandler == mmioFOURCC('R', 'A', 'W', ' ')
									       || sh_video->video.fccHandler == 0)) {
					sh_video->format = mmioFOURCC(0, 'R', 'G', 'B') | sh_video->bih->biBitCount;
				} else
#endif
					sh_video->format = sh_video->bih->biCompression;

				sh_video->disp_w = sh_video->bih->biWidth;
				sh_video->disp_h = abs(sh_video->bih->biHeight);

				// avi인데 내용물이 mpeg인 넘들
#if 1
				/* hack to support decoding of mpeg1 chunks in AVI's with libmpeg2 -- 2002 alex */
				if ((sh_video->format == 0x10000001) || (sh_video->format == 0x10000002) || (sh_video->format == mmioFOURCC('m', 'p', 'g', '1'))
				    || (sh_video->format == mmioFOURCC('M', 'P', 'G', '1'))
				    || (sh_video->format == mmioFOURCC('m', 'p', 'g', '2'))
				    || (sh_video->format == mmioFOURCC('M', 'P', 'G', '2'))
				    || (sh_video->format == mmioFOURCC('m', 'p', 'e', 'g'))
				    || (sh_video->format == mmioFOURCC('M', 'P', 'E', 'G'))) {
					int saved_pos, saved_type;

					/* demuxer pos saving is required for libavcodec mpeg decoder as it's
					   reading the mpeg header self! */

					saved_pos = d_video->buffer_pos;
					saved_type = d_video->demuxer->file_format;

					// ghcstop: format을 바꾼 후에
					d_video->demuxer->file_format = DEMUXER_TYPE_MPEG_ES;
					// ghcstop: 다시 이 함수를 호출한다.왜냐?....mpeg 1혹은 2에 필요한 정보들을
					// 설정하기 위해서 밑에 잇는 mpeg1/2 stream 처리 루틴을 볼 것
					video_read_properties(sh_video);

					// ghcstop: 다시 원래의 포맷으로 설정(백업된 놈 같고)
					d_video->demuxer->file_format = saved_type;
					d_video->buffer_pos = saved_pos;
//      goto mpeg_header_parser;
				}
#endif
			}
			break;
		}
	case VIDEO_MPEG4:{
			videobuf_len = 0;
			videobuf_code_len = 0;
			mp_msg(MSGT_DECVIDEO, MSGL_V, "Searching for Video Object Start code... ");
			fflush(stdout);
			while (1) {
				int i = sync_video_packet(d_video);
				if (i <= 0x11F)
					break;	// found it!
				if (!i || !skip_video_packet(d_video)) {
					mp_msg(MSGT_DECVIDEO, MSGL_V, "NONE :(\n");
					return 0;
				}
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "OK!\n");

			//ghcstop:  8byte단위로 boundary가 된 VIDEOBUFFER_SIZE 만큼의 메모리를 allocation한다.
			if (!videobuffer)
				videobuffer = (char *) memalign(8, VIDEOBUFFER_SIZE);
			if (!videobuffer) {
				mp_msg(MSGT_DECVIDEO, MSGL_ERR, MSGTR_ShMemAllocFail);
				return 0;
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "Searching for Video Object Layer Start code... ");
			fflush(stdout);
			while (1) {
				int i = sync_video_packet(d_video);
				mp_msg(MSGT_DECVIDEO, MSGL_V, "M4V: 0x%X\n", i);
				if (i >= 0x120 && i <= 0x12F)
					break;	// found it!
				if (!i || !read_video_packet(d_video)) {
					mp_msg(MSGT_DECVIDEO, MSGL_V, "NONE :(\n");
					return 0;
				}
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "OK!\nSearching for Video Object Plane Start code... ");
			fflush(stdout);
			while (1) {
				int i = sync_video_packet(d_video);
				if (i == 0x1B6)
					break;	// found it!
				if (!i || !read_video_packet(d_video)) {
					mp_msg(MSGT_DECVIDEO, MSGL_V, "NONE :(\n");
					return 0;
				}
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "OK!\n");

			// video format 지정
			sh_video->format = 0x10000004;
			break;
		}
	case VIDEO_H264:{
			videobuf_len = 0;
			videobuf_code_len = 0;
			mp_msg(MSGT_DECVIDEO, MSGL_V, "Searching for sequence parameter set... ");
			fflush(stdout);
			while (1) {
				int i = sync_video_packet(d_video);
				if ((i & ~0x60) == 0x107 && i != 0x107)
					break;	// found it!
				if (!i || !skip_video_packet(d_video)) {
					mp_msg(MSGT_DECVIDEO, MSGL_V, "NONE :(\n");
					return 0;
				}
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "OK!\n");
			if (!videobuffer)
				videobuffer = (char *) memalign(8, VIDEOBUFFER_SIZE);
			if (!videobuffer) {
				mp_msg(MSGT_DECVIDEO, MSGL_ERR, MSGTR_ShMemAllocFail);
				return 0;
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "Searching for picture parameter set... ");
			fflush(stdout);
			while (1) {
				int i = sync_video_packet(d_video);
				mp_msg(MSGT_DECVIDEO, MSGL_V, "H264: 0x%X\n", i);
				if ((i & ~0x60) == 0x108 && i != 0x108)
					break;	// found it!
				if (!i || !read_video_packet(d_video)) {
					mp_msg(MSGT_DECVIDEO, MSGL_V, "NONE :(\n");
					return 0;
				}
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "OK!\nSearching for Slice... ");
			fflush(stdout);
			while (1) {
				int i = sync_video_packet(d_video);
				if ((i & ~0x60) == 0x101 || (i & ~0x60) == 0x102 || (i & ~0x60) == 0x105)
					break;	// found it!
				if (!i || !read_video_packet(d_video)) {
					mp_msg(MSGT_DECVIDEO, MSGL_V, "NONE :(\n");
					return 0;
				}
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "OK!\n");

			// video format설정
			sh_video->format = 0x10000005;
			break;
		}
	case VIDEO_MPEG12:{
//mpeg_header_parser:
			// Find sequence_header first:
			videobuf_len = 0;
			videobuf_code_len = 0;
			telecine = 0;
			telecine_cnt = -2.5;
			mp_msg(MSGT_DECVIDEO, MSGL_V, "Searching for sequence header... ");
			fflush(stdout);
			while (1) {
				int i = sync_video_packet(d_video);
				if (i == 0x1B3)
					break;	// found it!
				if (!i || !skip_video_packet(d_video)) {
					if (verbose > 0)
						mp_msg(MSGT_DECVIDEO, MSGL_V, "NONE :(\n");
					mp_msg(MSGT_DECVIDEO, MSGL_ERR, MSGTR_MpegNoSequHdr);
					return 0;
				}
			}
			mp_msg(MSGT_DECVIDEO, MSGL_V, "OK!\n");
//   sh_video=d_video->sh;sh_video->ds=d_video;
//   mpeg2_init();
			// ========= Read & process sequence header & extension ============
			if (!videobuffer)
				videobuffer = (char *) memalign(8, VIDEOBUFFER_SIZE);
			if (!videobuffer) {
				mp_msg(MSGT_DECVIDEO, MSGL_ERR, MSGTR_ShMemAllocFail);
				return 0;
			}

			if (!read_video_packet(d_video)) {
				mp_msg(MSGT_DECVIDEO, MSGL_ERR, MSGTR_CannotReadMpegSequHdr);
				return 0;
			}
			if (mp_header_process_sequence_header(&picture, &videobuffer[4])) {
				mp_msg(MSGT_DECVIDEO, MSGL_ERR, MSGTR_BadMpegSequHdr);
				return 0;
			}
			if (sync_video_packet(d_video) == 0x1B5) {	// next packet is seq. ext.
//    videobuf_len=0;
				int pos = videobuf_len;
				if (!read_video_packet(d_video)) {
					mp_msg(MSGT_DECVIDEO, MSGL_ERR, MSGTR_CannotReadMpegSequHdrEx);
					return 0;
				}
// ghcstop: mpeg 확장 헤더 읽어옴?
				if (mp_header_process_extension(&picture, &videobuffer[pos + 4])) {
					mp_msg(MSGT_DECVIDEO, MSGL_ERR, MSGTR_BadMpegSequHdrEx);
					return 0;
				}
			}
//   printf("picture.fps=%d\n",picture.fps);

			// fill aspect info:
			switch (picture.aspect_ratio_information) {
			case 2:	// PAL/NTSC SVCD/DVD 4:3
			case 8:	// PAL VCD 4:3
			case 12:	// NTSC VCD 4:3
				sh_video->aspect = 4.0 / 3.0;
				break;
			case 3:	// PAL/NTSC Widescreen SVCD/DVD 16:9
			case 6:	// (PAL?)/NTSC Widescreen SVCD 16:9
				sh_video->aspect = 16.0 / 9.0;
				break;
			case 4:	// according to ISO-138182-2 Table 6.3
				sh_video->aspect = 2.21;
				break;
			case 9:	// Movie Type ??? / 640x480
				sh_video->aspect = 0.0;
				break;
			default:
				mp_msg(MSGT_DECVIDEO, MSGL_ERR,
				       "Detected unknown aspect_ratio_information in mpeg sequence header.\n" "Please report the aspect value (%i) along with the movie type (VGA,PAL,NTSC,"
				       "SECAM) and the movie resolution (720x576,352x240,480x480,...) to the MPlayer" " developers, so that we can add support for it!\nAssuming 1:1 aspect for now.\n", picture.aspect_ratio_information);
			case 1:	// VGA 1:1 - do not prescale
				sh_video->aspect = 0.0;
				break;
			}
			// display info:
			//ghcstop: mpeg1이냐 아니면 mpeg2나 설정? 0x10000001 = mpeg1같음
			sh_video->format = picture.mpeg1 ? 0x10000001 : 0x10000002;	// mpeg video
			sh_video->fps = picture.fps * 0.0001f;
			if (!sh_video->fps) {
//     if(!force_fps){
//       fprintf(stderr,"FPS not specified (or invalid) in the header! Use the -fps option!\n");
//       return 0;
//     }
				sh_video->frametime = 0;
			} else {
				sh_video->frametime = 10000.0f / (float) picture.fps;
			}
			sh_video->disp_w = picture.display_picture_width;
			sh_video->disp_h = picture.display_picture_height;
			// bitrate:
			if (picture.bitrate != 0x3FFFF)	// unspecified/VBR ?
				sh_video->i_bps = picture.bitrate * 400 / 8;
			// info:
			mp_dbg(MSGT_DECVIDEO, MSGL_DBG2, "mpeg bitrate: %d (%X)\n", picture.bitrate, picture.bitrate);
			mp_msg(MSGT_DECVIDEO, MSGL_INFO, "VIDEO:  %s  %dx%d  (aspect %d)  %5.3f fps  %5.1f kbps (%4.1f kbyte/s)\n", picture.mpeg1 ? "MPEG1" : "MPEG2", sh_video->disp_w, sh_video->disp_h, picture.aspect_ratio_information,
			       sh_video->fps, sh_video->i_bps * 8 / 1000.0, sh_video->i_bps / 1000.0);
			break;
		}
	}			// switch(file_format)

	return 1;
}

#if 0				// ghcstop delete ==> no closed caption
void ty_processuserdata(unsigned char *buf, int len);
#endif

static void process_userdata(unsigned char *buf, int len)
{
	int i;

#if 0				// ghcstop delete ==> no closed caption
	/* if the user data starts with "CC", assume it is a CC info packet */
	if (len > 2 && buf[0] == 'C' && buf[1] == 'C') {
//      mp_msg(MSGT_DECVIDEO,MSGL_DBG2,"video.c: process_userdata() detected Closed Captions!\n");
		if (subcc_enabled)
			subcc_process_data(buf + 2, len - 2);
	}
#endif

#if 0				// ghcstop delete ==> no closed caption
	if (len > 2 && buf[0] == 'T' && buf[1] == 'Y') {
		ty_processuserdata(buf + 2, len - 2);
		return;
	}
#endif

	if (verbose < 2)
		return;
	printf("user_data: len=%3d  %02X %02X %02X %02X '", len, buf[0], buf[1], buf[2], buf[3]);
	for (i = 0; i < len; i++)
//      if(buf[i]>=32 && buf[i]<127) putchar(buf[i]);
		if (buf[i] & 0x60)
			putchar(buf[i] & 0x7F);
	printf("'\n");
}

int video_read_frame(sh_video_t * sh_video, float *frame_time_ptr, unsigned char **start, int force_fps)
{
	demux_stream_t *d_video = sh_video->ds;
	demuxer_t *demuxer = d_video->demuxer;
	float frame_time = 1;
	float pts1 = d_video->pts;
	float pts = 0;
	int picture_coding_type = 0;
	//unsigned char* start=NULL;
	int in_size = 0;

	*start = NULL;
	// frame-based file formats: (AVI,ASF,MOV)
	in_size = ds_get_packet(d_video, start);
	if (in_size < 0)
		return -1;	// EOF

	//------------------------ frame decoded. --------------------
	// Increase video timers:
	sh_video->num_frames += frame_time;	// frame_time이 위에서 1로 지정되어 있는것 같음. 즉, frame수 증가하는 효과가남
	++sh_video->num_frames_decoded;

	//ghcstop:  frame_time이 1이니깐 frame_time = sh_video->frametime이 된다. 즉, 한 프레임 유지시간. 1/fps
	frame_time *= sh_video->frametime;

	sh_video->pts = d_video->pts;

	// frame_time = 1/fps 를 리턴해줌...
	if (frame_time_ptr)
		*frame_time_ptr = frame_time;

	return in_size;
}
