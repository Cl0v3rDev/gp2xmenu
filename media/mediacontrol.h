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
#ifndef __MEDIACONTROL_H_
#define __MEDIACONTROL_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <queue>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>
//[*]------------------------------------------------------------------------------------------[*]
extern "C" {
	#include <media/stream.h>
	#include <media/demuxer.h>
	#include <media/stheader.h>
}
//[*]------------------------------------------------------------------------------------------[*]
typedef struct {
	float vpts;
	float nft;
} vptsItem;
//[*]------------------------------------------------------------------------------------------[*]
class MediaControl {
	private:
		int state_initialized;

		// thread items
		SDL_Thread *audio_tid;
		SDL_Thread *video_tid;
		int athread_quit, vthread_quit;

		// a-v sync
		float saved_vpts;
		float ds_apts_timer;
		float AV_delay;
		int drop_frame_cnt;
		float default_max_pts_correction;
		float max_pts_correction;
		float c_total;

		int width, height;

		// demuxer, stream header & audio out
		stream_t *stream;
		demuxer_t *demuxer;
		demux_stream_t *d_audio;
		demux_stream_t *d_video;
		sh_audio_t *sh_audio;
		sh_video_t *sh_video;

		// a-v seek
		int seek_enable;
		float saved_nft;
		float seek_time;
		int seek_flag;
		int seek_processing;
		int demux_seeking;

		// a-v running match variable
		int vthread_run;

		SDL_mutex *vrun_mutex;
		SDL_cond *vrun_cond;
		int timer_run;

		// video thread stop event
		SDL_mutex *vstop_mutex;
		SDL_cond *vstop_cond;
		int video_stop;

		// audio thread rerun event
		SDL_mutex *arun_mutex;
		SDL_cond *arun_cond;
		int audio_run;

		SDL_mutex *apts_lock;
		SDL_mutex *seek_lock;

		int pause_flag, quit_event;

		float next_frame_time;
		int frame_time_remaining;
		float time_frame;

		bool timerPause;
		float relativeTime;
		float pauseTime;

		int playTime, totalTime;

		std::queue<vptsItem> vptsQueue;

		void stream_seek();

		void InitTimer();
		void PauseTimer(bool status);
		float GetRelativeTime();

		static int thread_audio(void *args);
		static int thread_video(void *args);

		int videoDisplay(unsigned char *buf, int size);
		void pre_process(unsigned short yoffset, unsigned short cboffset, unsigned short croffset);
		int post_process();

	public:
		MediaControl();
		~MediaControl();

		int load(const char*, float);
		bool play();
		void stop();
		void seek(bool, float);
		void pause(bool);
		char *getInfo(char *);
		int getStatus(int);
		void setStatus(int, int);
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
