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
#include "../config.h"
//[*]------------------------------------------------------------------------------------------[*]
#include <fcntl.h>
#include <sys/soundcard.h>
#include <inifile.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "mediaplayer.h"
#include "textout.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
MediaPlayer::MediaPlayer()
{
	cursor = 0;
	playList.clear();
	path.clear();

	INI_Open(INI_PATH"/common.ini");
	vol = INI_ReadInt("sound", "volume", 80);
	eq = INI_ReadInt("sound", "eq", NORMAL_EQ);
	repeat = INI_ReadInt("sound", "repeat", 0);
	INI_Close();

	useSubtitle = false;
	mediaControl = new MediaControl;
}
//[*]------------------------------------------------------------------------------------------[*]
MediaPlayer::~MediaPlayer()
{
	close();
	delete mediaControl;
}
//[*]------------------------------------------------------------------------------------------[*]
int MediaPlayer::getStatus(int type)
{
	switch (type) {
		case MEDIA_REPEAT_MODE :
			return repeat;
		case MEDIA_EQUALIZER :
			return eq;
		case MEDIA_VOLUME :
			return vol;
		default:
			return mediaControl->getStatus(type);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::setStatus(int type, int value)
{
	switch (type) {
		case MEDIA_REPEAT_MODE :
			if(value < 0)
				repeat++;
			else
				repeat = value;
			repeat %= 4;

			initShuffle();
			return;

		case MEDIA_EQUALIZER :
			if(value < 0)
				eq++;
			else
				eq = value;
			eq %= 11;

			eqChange();
			return;

		case MEDIA_VOLUME :
			if(value == MEDIA_VOLUME_UP)
				vol += 5;
			else if(value == MEDIA_VOLUME_DOWN)
				vol -= 5;
			else
				vol = value;

			if(vol > 100)
				vol = 100;
			else if(vol < 0)
				vol = 0;

			volChange();
			return;

		default:
			printf("MediaPlayer setStatus unsupported type: %d\n", type);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
const char* MediaPlayer::getInfo(int type)
{
	switch (type) {
		case MEDIA_TITLE:
			return Title.c_str();
		case MEDIA_ALBUM:
			return Album.c_str();
		case MEDIA_FILE_NAME:
			if((cursor < 0) || (cursor >= playList.size()))
			    return "";
			return playList[cursor].c_str();
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::updateStatus(int last, int now)
{
	SDL_Event event;

	if((last / 1000) != (now / 1000))
	{
		event.type = SDL_USEREVENT;
		event.user.code = EVENT_UPDATE;
		SDL_PushEvent(&event);
	}

	if(useSubtitle && subtitle.setPlayTime(now))
	{
		event.type = SDL_USEREVENT;
		event.user.code = EVENT_SUBTITLE_CHANGE;
		SDL_PushEvent(&event);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::reset(std::string &value)
{
	stop();
	playList.clear();
	cursor = 0;

	path = value;
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::addList(std::string &name, bool active)
{
	int no = playList.size();
	if(active) cursor = no;
	playList.push_back(name);
	if(shuffleList.size() < playList.size())
		shuffleList.push_back(no);
	else
	    shuffleList[no] = no;
	shuffleCount = no + 1;
}
//[*]------------------------------------------------------------------------------------------[*]
bool MediaPlayer::load(int resume)
{
	if(playList.size() == 0) return false;
	if((cursor < 0) || (cursor >= playList.size())) cursor = 0;

	Album.clear();
	Title.clear();

	std::string fullName = path + "/" + playList[cursor];
	printf("fullName = %s\n", fullName.c_str());

	if(mediaControl->load(fullName.c_str(), resume) == -1)
	{
		stop();
		return false;
	}

	char *info;
	char u8buf[1024];

	info = mediaControl->getInfo("Title");
	if(info && !isUTF8((unsigned char*)info))
	{
		toUTF8(NULL, info, (unsigned char *)u8buf, strlen(info));
		info = u8buf;
	}
	if(info)
	{
		if(((unsigned char)info[0] == 0xEF) && ((unsigned char)info[1] == 0xBB) &&
			((unsigned char)info[2] == 0xBF)) info += 3;
		Title = std::string(": ") + info;
		Title.erase(Title.find_last_not_of(' ') + 1);
		if(Title.size() > 1)
		    Title = "Title " + Title;
		else
		    Title.clear();
	}

	info = mediaControl->getInfo("Album");
	if(info && !isUTF8((unsigned char*)info))
	{
		toUTF8(NULL, info, (unsigned char *)u8buf, strlen(info));
		info = u8buf;
	}
	if(info)
	{
		if(((unsigned char)info[0] == 0xEF) && ((unsigned char)info[1] == 0xBB) &&
			((unsigned char)info[2] == 0xBF)) info += 3;
		Album = std::string(": ") + info;
		Album.erase(Album.find_last_not_of(' ') + 1);
		if(Album.size() > 1)
		    Album = "Album " + Album;
		else
		    Album.clear();
	}

	subtitle.clear();
	subtitle.load(fullName, SUBTITLE_LRC);
	subtitle.load(fullName, SUBTITLE_SMI);
	subtitle.load(fullName, SUBTITLE_SAMI);
	useSubtitle = subtitle.reorder();
	printf("using Subtitle: %s\n", useSubtitle ? "true" : "false");

	if((getStatus(MEDIA_TYPE) == MEDIA_MOVIE) && (theApp.isSDMount()))
	{
	    changeFont(SD_PATH_FONT"/sub.ttf", 16, 0);

	    INI_Open(INI_PATH"/movie.ini");
		INI_WriteInt("resume", "time", 0);
		INI_Close();
	}

	return true;
}
//[*]------------------------------------------------------------------------------------------[*]
bool MediaPlayer::play()
{
	mediaControl->pause(false);
	return mediaControl->play();
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::prev()
{
	if((getStatus(MEDIA_TYPE) == MEDIA_MUSIC) && (repeat >= 2))
	{
		next();
		return;
	}

	--cursor;
	if(cursor < 0)
		cursor = playList.size() - 1;

	stop();
	if(load())
		play();
	else
		prev();
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::next()
{
	if((getStatus(MEDIA_TYPE) == MEDIA_MUSIC) && (repeat >= 2) && (playList.size() > 1))
	{
		int last = cursor;
		int index;
		do {
			index = rand() % shuffleCount;
			cursor = shuffleList[index];
		} while(last == cursor);

		shuffleList[index] = shuffleList[--shuffleCount];
		if(shuffleCount == 0)
		    initShuffle();
		printf("%d, %d\n", cursor, shuffleCount);
	}
	else
	{
		++cursor;
		if(cursor >= playList.size())
			cursor = 0;
	}

	stop();
	if(load())
		play();
	else
		next();
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::seek(int time)
{
	mediaControl->seek(false, time);
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::jump(int time)
{
	mediaControl->seek(true, time);
}
//[*]------------------------------------------------------------------------------------------[*]
bool MediaPlayer::pause()
{
	bool result = getStatus(MEDIA_PLAY_STATUS) == MEDIA_PLAY;
	if(result)
		mediaControl->pause(true);
	else if(getStatus(MEDIA_PLAY_STATUS) == MEDIA_PAUSE)
		mediaControl->pause(false);
	return result;
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::stop()
{
	INI_Open(INI_PATH"/common.ini");
	INI_WriteInt("sound", "volume", vol);
	INI_WriteInt("sound", "eq", eq);
	INI_WriteInt("sound", "repeat", repeat);
	INI_Close();

	if((getStatus(MEDIA_TYPE) == MEDIA_MOVIE) && (getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP))
	{
	    INI_Open(INI_PATH"/movie.ini");
		INI_WriteInt("resume", "time", getStatus(MEDIA_PLAY_TIME));
		INI_WriteText("resume", "path", path.c_str());
		INI_WriteText("resume", "name", playList[cursor].c_str());
		INI_Close();
	}
	
	sync();

	mediaControl->stop();
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::close()
{
	stop();
    changeFont(NULL, 0, 0);
	theApp.SoundInit(-1, -1, -1);
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::initShuffle()
{
	if(repeat < 2) return;
	shuffleCount = playList.size();
	for(int i=0; i < shuffleCount; ++i)
	    shuffleList[i] = i;
	srand(SDL_GetTicks());
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::volChange()
{
#if defined(GP2X) || defined(WIZ)
	int fd = open("/dev/sound/mixer", O_RDONLY);
	if (fd != -1)
	{
		int volume = vol | (vol << 8);
		ioctl(fd, MIXER_WRITE(SOUND_MIXER_PCM), &volume);
		::close(fd);
	}
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
void MediaPlayer::eqChange()
{
#if defined(GP2X) || defined(WIZ)
	int fd = open("/dev/sound/mixer", O_RDONLY);
	if (fd != -1)
	{
		int v = eq;
		ioctl(fd, MIXER_WRITE(SOUND_MIXER_EQ_HW), &v);
		::close(fd);
	}
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
