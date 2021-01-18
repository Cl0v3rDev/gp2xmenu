/*
    GP2X Media Control (fork from MPlayer, www.mplayerhq.hu)
    Copyright (C) 2004 godori <ghcstop>, www.aesop-embedded.org
	Copyright (C) 2004 Magiceyes. <www.mesdigital.com>
	Copyright (C) 2005 DIGNSYS Inc. <www.dignsys.com>
    Copyright (C) 2005-2008 Gamepark Holdings Co,Ltd. <www.gp2x.com>
    Copyright (C) 2008 GP2X OSS Project. <dev.gp2x.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

//[*]------------------------------------------------------------------------------------------[*]
#include "../config.h"
//[*]------------------------------------------------------------------------------------------[*]
#include <string.h>
#include <inifile.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "mediacontrol.h"
#include "mmsp2_if.h"
//[*]------------------------------------------------------------------------------------------[*]
extern "C" {
	#include <media/stream.h>
	#include <media/demuxer.h>
	#include <media/stheader.h>
	#include <media/dec_audio.h>
	extern int force_backward_seek;
#ifdef WIN32
	unsigned long __stdcall timeGetTime(void);
#else
	#include <sys/time.h>
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
#define MAX_OUTBURST 65535
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
MediaControl::MediaControl()
{
	seek_lock = SDL_CreateMutex();
	apts_lock = SDL_CreateMutex();

	vrun_mutex = SDL_CreateMutex();
	vrun_cond = SDL_CreateCond();

	vstop_mutex = SDL_CreateMutex();
	vstop_cond = SDL_CreateCond();

	arun_mutex = SDL_CreateMutex();
	arun_cond = SDL_CreateCond();

	stream = NULL;
	demuxer = NULL;

	audio_tid = NULL;
	video_tid = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
MediaControl::~MediaControl()
{
	stop();

	SDL_DestroyMutex(apts_lock);
	SDL_DestroyMutex(seek_lock);

	SDL_DestroyMutex(vrun_mutex);
	SDL_DestroyCond(vrun_cond);

	SDL_DestroyMutex(vstop_mutex);
	SDL_DestroyCond(vstop_cond);

	SDL_DestroyMutex(arun_mutex);
	SDL_DestroyCond(arun_cond);
}
//[*]------------------------------------------------------------------------------------------[*]
int MediaControl::load(const char *filename, float resume)
{
	int file_format = DEMUXER_TYPE_UNKNOWN;
	int i;

	if (!filename || (access(filename, F_OK) != 0))
	{
		printf("cannot access: %s\n", filename);
		return -1;
	}

	if(stream || demuxer) stop();

	next_frame_time = 0;
	frame_time_remaining = 0;
	time_frame = 0;

	drop_frame_cnt = 0;
	default_max_pts_correction = -1;
	max_pts_correction = 0;
	c_total = 0;
	AV_delay = 0;

	d_audio = NULL;
	d_video = NULL;

	sh_audio = NULL;
	sh_video = NULL;

	stream = open_stream((char*)filename, 0, &file_format);
	if (!stream) {
		printf("open stream failed...\n");
		return -1;
	}

	demuxer = demux_open(stream, file_format, -1, -1, -1, (char*)filename);
	if (!demuxer) {
		printf("demux open failed...\n");
		return -1;
	}

	d_audio = demuxer->audio;
	d_video = demuxer->video;

	sh_audio = (sh_audio_t *)d_audio->sh;
	sh_video = (sh_video_t *)d_video->sh;

	if (sh_video) {
		if (!video_read_properties(sh_video)) {
			sh_video = NULL;
		} else {
			printf("[V] filefmt:%d  fourcc:0x%X  size:%dx%d  fps:%5.2f  ftime:=%6.4f\n",
				demuxer->file_format, sh_video->format, sh_video->disp_w, sh_video->disp_h,
				sh_video->fps, sh_video->frametime);

			if (!sh_video->fps) {
				sh_video = NULL;
			}
		}
	}

	if (sh_audio) {
		if (!init_audio(sh_audio))
		{
			printf("audio codec initialize failed\n");
			sh_audio = NULL;
		}
		else if(theApp.SoundInit(sh_audio->samplerate, sh_audio->sample_format, sh_audio->channels) == false)
		{
			printf("sound initialize failed...\n");
			sh_audio = NULL;
		}
	}

	if (sh_video) {
		//InitMMSP2Decoder(sh_video->disp_w, sh_video->disp_h, sh_video->format);
		printf("width = %d, height = %d, comp: 0x%X\n", sh_video->disp_w, sh_video->disp_h, sh_video->format);
		//theApp.GP2XControl(FCLK_200);

		if ((mmsp2_940_interface_buffer_init() < 0) ||
			(open_dualcpu_vpp() < 0) ||
			(mmsp2_setup_940_interface_buffer(sh_video->disp_w, sh_video->disp_h, sh_video->format) < 0)) {
			printf("MMSP2 initialize failed.\n");
			CloseMMSP2Decoder();
			sh_audio = NULL;
			sh_video = NULL;
		}
		else
		{
			int real_width = ((sh_video->disp_w >> 4) << 4);
			int real_height = ((sh_video->disp_h >> 4) << 4);
			set_FDC_YUVB_plane(real_width, real_height);
			theApp.setBatteryMode(MOVIE_MODE);
		}

		// same area using photo & movie buffer, photo buffer is borken.
		theApp.menuPhoto.Break();
	}

	if (!sh_audio) {
		ds_free_packs(d_audio);
		d_audio->sh = NULL;
		d_audio->id = -2;
	}

	if (!sh_video) {
		ds_free_packs(d_video);
		d_video->sh = NULL;
		d_video->id = -2;
	}

	if (!sh_video && !sh_audio)
	{
		printf("no stream found...\n");
		return -1;
	}

	if (sh_video) sh_video->timer = 0;
	if (sh_audio) sh_audio->delay = 0;

	totalTime = demuxer_get_time_length(demuxer);

	playTime = 0;
	resume -= 5;
	if (resume > 0) {
		printf("### RESUME SEEK %f\n", resume);
		force_backward_seek = 1;
		seek(true, resume);
		stream_seek();
	}

	athread_quit = 0;
	vthread_quit = 0;

	timer_run = 0;
	seek_enable = 0;
	saved_nft = 0.0;

	seek_processing = 0;
	demux_seeking = 0;

	video_stop = 0;
	audio_run = 0;

	while(!vptsQueue.empty()) vptsQueue.pop();

	if (sh_video && sh_audio)
		vthread_run = 0;
	else
		vthread_run = 1;

	video_tid = NULL;
	audio_tid = NULL;
	quit_event = 0;
	
	pause_flag = false;

	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
bool MediaControl::play()
{
	if(!sh_video && !sh_audio) return false;
	if (sh_video) video_tid = SDL_CreateThread(thread_video, &theApp);
	if (sh_audio) audio_tid = SDL_CreateThread(thread_audio, &theApp);
	return true;
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaControl::stop()
{
	quit_event = 1;

	if (audio_tid) {
		printf("audio thread wait\n");
		SDL_WaitThread(audio_tid, NULL);
		audio_tid = NULL;
		theApp.SoundReset();
	}

	if (video_tid) {
		if (vthread_run)
		{
			printf("video thread wait(pause_flag = %d, quit_event = %d)\n", pause_flag, quit_event);
			SDL_WaitThread(video_tid, NULL);
			video_tid = NULL;
		}

#ifdef GP2X
		CloseMMSP2Decoder();
		theApp.setBatteryMode(MENU_MODE);
#endif
	}

	if(stream)
	{
		free_stream(stream);
		stream = NULL;
	}

	if(demuxer)
	{
		free_demuxer(demuxer);
		demuxer = NULL;
	}

}
//[*]------------------------------------------------------------------------------------------[*]
void MediaControl::pause(bool status)
{
	if(status)
	{
		theApp.SoundReset();
	}
	else
	{
		seek(false, -1);
	}
	pause_flag = status;
}
//[*]------------------------------------------------------------------------------------------[*]
char* MediaControl::getInfo(char *opt)
{
	if(demuxer == NULL) return "";
	return demux_info_get(demuxer, opt);
}
//[*]------------------------------------------------------------------------------------------[*]
int MediaControl::getStatus(int type)
{
	switch (type) {
		case MEDIA_BIT_RATE :
			if (sh_audio == NULL)
				return 0;
			else if (sh_audio->audio.dwSampleSize)
				return sh_audio->i_bps;
			else
				return sh_audio->i_bps * 8;
		case MEDIA_SAMPLE_RATE :
			if (sh_audio == NULL)
				return 0;
			return sh_audio->samplerate / 1000;
		case MEDIA_PLAY_TIME :
			return playTime / 1000;
		case MEDIA_TOTAL_TIME :
			return totalTime;
		case MEDIA_CHANNEL :
			if (sh_audio == NULL)
				return 0;
			return sh_audio->channels;
		case MEDIA_TYPE:
			return (sh_video == NULL) ? MEDIA_MUSIC : MEDIA_MOVIE;
		case MEDIA_PLAY_STATUS:
			if(audio_tid && !athread_quit && !quit_event)
			{
				if(pause_flag)
				    return MEDIA_PAUSE;
				else
				    return MEDIA_PLAY;
			}
			else
				return MEDIA_STOP;
		default:
			printf("MediaPlayer getStatus unsupported type: %d\n", type);
			break;
	}
	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaControl::setStatus(int type, int value)
{
	switch (type) {
		case MEDIA_PLAY_TIME :
			if(playTime != value)
			{
				theApp.menuMedia.updateStatus(playTime, value);
				playTime = value;
			}
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
float MediaControl::GetRelativeTime()
{
	float t, r;
#ifndef WIN32
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	t = (tv.tv_sec + tv.tv_usec * 0.000001f);
#else
	t = timeGetTime() * 0.001f;
#endif
	r = t - relativeTime;
	relativeTime = t;

	return r;
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaControl::InitTimer()
{
	timerPause = false;
	GetRelativeTime();
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaControl::PauseTimer(bool status)
{
	unsigned int r;
	timerPause = status;
	if (status)
		pauseTime = GetRelativeTime();
	else
		relativeTime = pauseTime;
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaControl::seek(bool isAbs, float time)
{
	SDL_LockMutex(seek_lock);
	seek_time = time;
	seek_flag = (isAbs ? 1 : 0);
	seek_enable = 1;
	if(!isAbs && (time < 0))
		force_backward_seek = 1;
	SDL_UnlockMutex(seek_lock);
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaControl::stream_seek(void)
{
	if (demux_seek(demuxer, seek_time, seek_flag)) {
		if (sh_audio)
		    theApp.SoundReset();

		if (sh_video) {
			sh_video->pts = d_video->pts;

			c_total = 0;
			max_pts_correction = 0.1;

			while(!vptsQueue.empty()) vptsQueue.pop();
			vptsItem item;
			item.vpts = sh_video->pts;
			item.nft = saved_nft;
			vptsQueue.push(item);
			// 맨처음 frame_time때문에
			//frame_time_remaining = 0;
		}
	}

	demux_seeking = 0;
}
//[*]------------------------------------------------------------------------------------------[*]
int MediaControl::thread_audio(void *args)
{
	CMainMenuApp *app = (CMainMenuApp *)args;
	MediaControl *is = (app->menuMedia).getMediaControl();
	SDL_Event event;
	int ret, playsize;

	is->sh_audio->delay = 0.0;

	while (!is->quit_event) {
		while (is->pause_flag && !is->quit_event) usleep(0);

		SDL_LockMutex(is->seek_lock);
		if (is->seek_enable == 1) {
			if (is->seek_processing == 1) {
				SDL_UnlockMutex(is->seek_lock);
				continue;
			}

			is->demux_seeking = 1;
			is->seek_processing = 1;

			is->timer_run = 0;

			if (is->sh_video) {
				is->vthread_run = 0;

				SDL_LockMutex(is->vstop_mutex);
				while (is->video_stop == 0)
					SDL_CondWait(is->vstop_cond, is->vstop_mutex);
				SDL_UnlockMutex(is->vstop_mutex);
			}

			is->stream_seek();
#ifdef DEBUG
			printf("athread rerun, %d\n", is->seek_enable);
#endif
		}
		SDL_UnlockMutex(is->seek_lock);

		playsize = app->GetSoundSpace();
		if(!is->sh_video)
		    is->setStatus(MEDIA_PLAY_TIME, (int) ((is->sh_audio->delay - app->GetSoundDelay()) * 1000));

		if (!playsize) {
			usleep(10000);
			continue;
		}

		if (playsize > MAX_OUTBURST)
		    playsize = MAX_OUTBURST;

		if (is->d_audio->eof == 1) {
			if(app->GetSoundDelay() > 0)
			{
				usleep(10000);
				continue;
			}
			else
				goto quit;
		}

		while (is->sh_audio->a_out_buffer_len < playsize && !is->d_audio->eof) {
			if (is->quit_event == 1)
				goto break_signal;

			ret = decode_audio(is->sh_audio,
				(unsigned char *)&is->sh_audio->a_out_buffer[is->sh_audio->a_out_buffer_len],
				playsize - is->sh_audio->a_out_buffer_len,
				is->sh_audio->a_out_buffer_size - is->sh_audio->a_out_buffer_len);

			if (ret <= 0) {
				printf("AUDIO decode end?\n");
				goto quit;
			}

			is->sh_audio->a_out_buffer_len += ret;
		}

		//printf("%d, %d\n", playsize, is->sh_audio->a_out_buffer_len);

		if (playsize > is->sh_audio->a_out_buffer_len)
			playsize = is->sh_audio->a_out_buffer_len;

		if (is->timer_run == 0) {
			if (is->sh_video) {
				SDL_LockMutex(is->vrun_mutex);
				is->timer_run = 1;
				SDL_CondSignal(is->vrun_cond);
				SDL_UnlockMutex(is->vrun_mutex);
			} else
				is->timer_run = 1;

			if (is->seek_enable == 1) {
				if (is->sh_video) {
					SDL_LockMutex(is->arun_mutex);
					while (is->audio_run == 0)
						SDL_CondWait(is->arun_cond, is->arun_mutex);
					SDL_UnlockMutex(is->arun_mutex);
				}

				SDL_LockMutex(is->seek_lock);
				is->seek_enable = 0;
				is->seek_processing = 0;
				is->video_stop = 0;
				is->audio_run = 0;
				SDL_UnlockMutex(is->seek_lock);
			}
		}

		SDL_LockMutex(is->apts_lock);
		playsize = app->PlaySound(is->sh_audio->a_out_buffer, playsize);

		if (playsize > 0) {
			is->sh_audio->a_out_buffer_len -= playsize;
			memmove(is->sh_audio->a_out_buffer, &is->sh_audio->a_out_buffer[playsize], is->sh_audio->a_out_buffer_len);

			is->sh_audio->delay += playsize / (float) (is->sh_audio->o_bps);

			is->ds_apts_timer = is->d_audio->pts;
			is->ds_apts_timer += (ds_tell_pts(is->d_audio) - is->sh_audio->a_in_buffer_len) / (float) is->sh_audio->i_bps;
		}

		if(!is->sh_video)
		    is->setStatus(MEDIA_PLAY_TIME, (int)((is->sh_audio->delay - app->GetSoundDelay()) * 1000));
		SDL_UnlockMutex(is->apts_lock);
	}

break_signal:
	printf("audio thread quit: break_signal\n");

quit:
	//printf("audio thread quit: audio decode done or error\n");

	is->athread_quit = 1;

	if(is->quit_event == 0)
	{
	    event.type = EVENT_MEDIA_END;
		SDL_PushEvent(&event);
	}

	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
int MediaControl::thread_video(void *args)
{
	CMainMenuApp *app = (CMainMenuApp *)args;
	MediaControl *is = (app->menuMedia).getMediaControl();

	unsigned char *start = NULL;
	int force_fps = 0;
	int in_size = -1;
	int dframe = 0;
	float next_frame_time = 0;
	float nft_sum = 0;
	vptsItem item;
	is->InitTimer();

	while (1) {
		if (is->pause_flag) {
			is->PauseTimer(true);
			while (is->pause_flag && !is->quit_event)
				usleep(0);	// 추가 : 미스콜이아
			is->PauseTimer(false);
		}

		if (is->quit_event) {
			printf("quit_event = %d\n", is->quit_event);
			break;
		}

		if (is->vthread_run == 0) {
			if (is->seek_enable == 1) {
				printf("vthread stop ==\n");
				SDL_LockMutex(is->vstop_mutex);
				is->video_stop = 1;
				SDL_CondSignal(is->vstop_cond);
				SDL_UnlockMutex(is->vstop_mutex);
				nft_sum = 0;
			}

			printf("vthread wait, tr:%d, se: %d\n", is->timer_run, is->seek_enable);
			SDL_LockMutex(is->vrun_mutex);
			while (is->timer_run == 0)	// 안의 내용이 참이면 cond wait, 내가 원하는게 아니면
				SDL_CondWait(is->vrun_cond, is->vrun_mutex);
			is->vthread_run = 1;
			SDL_UnlockMutex(is->vrun_mutex);

			printf("vthread run, %d\n", is->seek_enable);

			if (is->seek_enable == 1) {
				SDL_LockMutex(is->arun_mutex);
				is->audio_run = 1;
				SDL_CondSignal(is->arun_cond);
				SDL_UnlockMutex(is->arun_mutex);
			}
		}

		in_size = video_read_frame(is->sh_video, &next_frame_time, &start, force_fps);
		//printf("size: %d\n", in_size);

		if (in_size < 0)
			break;
		nft_sum += next_frame_time;
		if (in_size == 0)
			continue;	// for 120fps movie

		item.vpts = is->sh_video->pts;
		item.nft = nft_sum;
		is->vptsQueue.push(item);
		nft_sum = 0;

		dframe = is->videoDisplay(start, in_size);
		if (dframe == -1) break;
	} // while

	if (is->quit_event)
		printf("break_signal\n");
	else
		printf("video thread quit\n");

	is->vthread_quit = 1;

	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
int MediaControl::videoDisplay(unsigned char *buf, int size)
{
#ifdef GP2X
	int decoded_frame_number = 0;

	if (size <= 8) return 0;

	memcpy(dbuf, buf, size);
	memset(dbuf + size, 0, 2048);

	// decoding loop for b frame, first come in
	do {
		if (RunMMSP2Decoder() < 0) {
			printf("run MMSP2 decoder error\n");
			return -1;
		}

		if (mp4d_disp.Command == MP_CMD_DISPLAY) {
			if (mp4d_disp.display_frames != 0) {
				decoded_frame_number++;
				pre_process(mp4d_disp.luma_offset_addr, mp4d_disp.cb_offset_addr, mp4d_disp.cr_offset_addr);
			}

			if (mp4d_disp.display_frames == 2) {
				decoded_frame_number++;
				post_process();
				pre_process(mp4d_disp.remain_frame_luma_offset_addr, mp4d_disp.remain_frame_cb_offset_addr, mp4d_disp.remain_frame_cr_offset_addr);
			}
		} else if (mp4d_disp.Command == MP_CMD_FAIL) {
			printf("MP_CMD_FAIL ==> exit routine\n");
			decoded_frame_number = 0;
			break;
		}
	} while (mp4d_disp.has_bframe);	// b frame != 0, b frame이 있으면

	if(decoded_frame_number > 0) post_process();

	return decoded_frame_number;
#else
	pre_process(0, 0, 0);
	post_process();

	return 1;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaControl::pre_process(unsigned short yoffset, unsigned short cboffset, unsigned short croffset)
{
	float delay;
	float min;
	float d_time;
	float frame_time;

	saved_vpts = vptsQueue.front().vpts;
	saved_nft = vptsQueue.front().nft;
	vptsQueue.pop();

	// 20050306, 피구의제왕가 같은 골때린넘 처리를 위하여 윗쪽으로 뺀다.
	frame_time = saved_nft;
	sh_video->timer += frame_time;
	time_frame += frame_time;

	frame_time_remaining = 0;	// frame에 대한 시간이 남을 경우
	time_frame -= GetRelativeTime();	// reset timer,

	if (sh_audio && !d_audio->eof) {
		SDL_LockMutex(apts_lock);
		sh_audio->delay -= frame_time;
		delay = theApp.GetSoundDelay();
		d_time = sh_audio->delay;
		SDL_UnlockMutex(apts_lock);

		// 오디오 delay시간과(Play할 시간이 어느정도인가) a-v간 delay와의 차
		time_frame = delay - d_time;

		//printf("%.4f = %.4f - %.4f\n", time_frame, delay, d_time);

		// delay = amount of audio buffered in soundcard/driver
		if (delay > 0.25)	// 250ms?
			delay = 0.25;
		else if (delay < 0.10)	// 100ms?
			delay = 0.10;

		if (time_frame > delay * 0.6) {
			printf("too much gap === %.4f, %.4f\n", time_frame, delay * 0.6);
			// sleep time too big - may cause audio drops (buffer underrun)
			frame_time_remaining = 1;
			time_frame = delay * 0.5;
		}
	} else {		// if(sh_audio && !d_audio->eof)
		// NOSOUND:
		if (time_frame < -3 * frame_time || time_frame > 3 * frame_time)
			time_frame = 0;
	}
	// if(verbose>1)printf("sleep: %5.3f a:%6.3f v:%6.3f \n",time_frame,sh_audio->timer,sh_video->timer);

	// ============================== SLEEP: ===================================
	if (time_frame > 0.001) {
		min = 0.005;
		while (time_frame > min) {
			if (time_frame <= 0.020) {
				usleep(0);
			} else {
				usleep((int)(1000000 * (time_frame - 0.020)));
			}
			time_frame -= GetRelativeTime();
		}
	}
#ifdef GP2X
	// ====================== FLIP PAGE (VIDEO BLT): =========================
	RunMMSP2Display(yoffset, cboffset, croffset);
#endif
}

//[*]------------------------------------------------------------------------------------------[*]
int MediaControl::post_process()
{
	float v_pts = saved_vpts ? saved_vpts : sh_video->pts;

	// ====================== A-V TIMESTAMP CORRECTION: =========================
	if (sh_audio) {
		SDL_LockMutex(apts_lock);
		float a_pts = ds_apts_timer;
		float delay = theApp.GetSoundDelay();
		SDL_UnlockMutex(apts_lock);

		AV_delay = (a_pts - delay) - v_pts;

		if (AV_delay > 0.5 && drop_frame_cnt > 50) {
			printf("system is too slow\n");
		}

		float x = AV_delay * 0.1f;
		if (x < -max_pts_correction)
			x = -max_pts_correction;
		else if (x > max_pts_correction)
			x = max_pts_correction;

		if (default_max_pts_correction >= 0)
			max_pts_correction = default_max_pts_correction;
		else {
			// sh_video->frametime = 1.0f/sh_video->fps = 즉, 1 프레임을 보여줘야 하는 시간
			max_pts_correction = sh_video->frametime * 0.10;	// +-10% of time
		}

		if (!frame_time_remaining)	// correction
		{
			SDL_LockMutex(apts_lock);
			sh_audio->delay += x;
			SDL_UnlockMutex(apts_lock);

			c_total += x;
		}
	}

	setStatus(MEDIA_PLAY_TIME, (int)(v_pts * 1000));

	return 1;
}
//[*]------------------------------------------------------------------------------------------[*]
