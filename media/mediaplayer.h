/*
    GP2X Applications
    Copyright (C) 2005-2008 Gamepark Holdings Co,Ltd. <www.gp2x.com>
    Copyright (C) 2008 GP2X OSS Project. <dev.gp2x.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

//[*]------------------------------------------------------------------------------------------[*]
#ifndef __MEDIAPLAYER_H_
#define __MEDIAPLAYER_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <string>
#include <vector>
//[*]------------------------------------------------------------------------------------------[*]
#include "mediacontrol.h"
#include "subtitle.h"
//[*]------------------------------------------------------------------------------------------[*]
class MediaPlayer
{
	private :
		std::vector<std::string> playList;
		std::string path;
		std::string Album, Title;

		int cursor;
		int repeat;
		int eq;
		int vol;
		
		std::vector<int> shuffleList;
		int shuffleCount;

		bool useSubtitle;

		MediaControl *mediaControl;
		Subtitles subtitle;

		void initShuffle();

	public :
		MediaPlayer();
		~MediaPlayer();

		void reset(std::string &path);
		void addList(std::string &filename, bool active = false);

		bool load(int resume = 0);
		bool play();
		void stop();
		bool pause();
		void close();

		void prev();
		void next();
		void seek(int);
		void jump(int);

		int getStatus(int);
		void setStatus(int, int);
		const char* getInfo(int);

		void updateStatus(int, int);
		void volChange();
		void eqChange();

		MediaControl* getMediaControl() {return mediaControl;};
		Subtitle* getSubtitle() { return subtitle.getSubtitle(); };
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
