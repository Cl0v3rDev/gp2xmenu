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
#include <inifile.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "movieview.h"
#include "mainmenuapp.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CMovieView::CMovieView()
{
	selectFunc = select;
	iconNo = ICON_MOVIE;
	resumeTime = 0;
	resumePath.clear();
	resumeName.clear();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CMovieView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("movie", "filelist");
	theApp.FontColor("movie", "list", r, g, b);

	if(!refresh)
	{
		INI_Open(INI_PATH"/movie.ini");
		resumeTime = INI_ReadInt("resume", "time", 0);
		if(resumeTime > 0)
		{
			resumePath = INI_ReadText("resume", "path", "");
			resumeName = INI_ReadText("resume", "name", "");
		}
		INI_Close();

		struct stat statbuf;

		theApp.Mount(MOUNT_SD);
		lstat(SD_PATH_MOVIE, &statbuf);
		if( !S_ISDIR(statbuf.st_mode) )
			mkdir(SD_PATH_MOVIE);

		if(resumeTime > 0)
		{
			lastPath = resumePath;
			lastName = resumeName;
		}
		else
			lastName = theApp.menuMedia.getInfo(MEDIA_FILE_NAME);

		if( (lastPath.size() == 0) || (lstat(lastPath.c_str(), &statbuf) != 0) || (!S_ISDIR(statbuf.st_mode)) )
		    lastPath = SD_PATH_MOVIE;
	}
	
	FileList::Initialize(refresh);

	if((resumeTime > 0) && (lastPath == resumePath) && (pList[nPosition].szName == resumeName))
	{
		SDL_Event event;
		event.type = SDL_JOYBUTTONDOWN;
		event.jbutton.button = VK_FB;
		event.jbutton.state = SDL_PRESSED;
		SDL_PushEvent(&event);

		event.type = SDL_JOYBUTTONUP;
		event.jbutton.button = VK_FB;
		event.jbutton.state = SDL_RELEASED;
		SDL_PushEvent(&event);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMovieView::OnExecute(SDL_Surface *pScreen, int button)
{
	if((button != VK_FX) && (button != VK_FB) && (button != VK_TAT)) return;
	
	if((resumeTime > 0) && (lastPath == resumePath) && (pList[nPosition].szName == resumeName))
	{
		if(waitMode != WAIT_QUERY)
		{
			SDL_Surface *resumeImage = theApp.LoadImage("common", "resume");
			if(resumeImage != NULL)
			{
				SDL_Rect rect;
				rect.x = (XRES - resumeImage->w) >> 1;
				rect.y = (YRES - resumeImage->h) >> 1;
				SDL_BlitSurface(resumeImage, NULL, pScreen, &rect);
				SDL_UpdateRect(pScreen, rect.x, rect.y, resumeImage->w, resumeImage->h);
				SDL_SAFE_FREE(resumeImage);
				waitMode = WAIT_QUERY;
				queryRect[0].x = 128;
				queryRect[1].x = 194;
				queryRect[0].y = queryRect[1].y = 120;
				queryRect[0].w = queryRect[1].w = 31;
				queryRect[0].h = queryRect[1].h = 43;
				return;
			}
			resumeTime = 0;
			return;
		}

		waitMode = WAIT_NONE;
		if(button == VK_FX)
		{
			resumeTime = 0;
			OnDraw(pScreen);
			return;
		}
	}
	else
	    resumeTime = 0;

	SDL_Surface *loadingImage = theApp.LoadImage("common", "loading");
	if(loadingImage != NULL)
	{
		SDL_Rect rect;
		rect.x = (XRES - loadingImage->w) >> 1;
		rect.y = (YRES - loadingImage->h) >> 1;
		SDL_BlitSurface(loadingImage, NULL, pScreen, &rect);
		SDL_UpdateRect(pScreen, rect.x, rect.y, loadingImage->w, loadingImage->h);
		SDL_SAFE_FREE(loadingImage);
	}

	theApp.menuMedia.reset(lastPath);
	for(int i=0; i<nCount; i++)
		if(pList[i].nAttribute != FOLDER_FORMAT)
			theApp.menuMedia.addList(pList[i].szName, i == nPosition);

	if(theApp.menuMedia.load(resumeTime)) {
		theApp.SetView(MEDIA_PLAY_VIEW);
		theApp.menuMedia.play();
		printf("media play: %d\n", resumeTime);
	} else {
		SDL_Surface *error = theApp.LoadImage("common", "notsupported");
		if(error) {
			SDL_Rect rect;
			rect.x = (320 - error->w) >> 1;
			rect.y = (240 - error->h) >> 1;
			SDL_BlitSurface(error, NULL, pScreen, &rect);
			SDL_FreeSurface(error);
			waitMode = WAIT_ANY_BUTTON;

			SDL_UpdateRect(pScreen, 0, 0, 0, 0);
		}
		resumeTime = 0;
		if(waitMode != WAIT_ANY_BUTTON) OnDraw(pScreen);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
int CMovieView::select(const struct dirent *name)
{
	char *ext = NULL;
	struct stat statbuf;
	lstat(name->d_name, &statbuf);

	// 폴더일 경우
	if(S_ISDIR(statbuf.st_mode))
	{
		// 폴더 인데 "."(현재 폴더)인 경우는 0을 리턴
		if(strcmp(".", name->d_name) == 0) return 0;

		return 1;	// 그 외의 경우(일반 폴더, ..(상위))는 1을 리턴
	}

	int len = strlen(name->d_name);
	if(len < 4)	return 0;

	ext = (char*)&(name->d_name[len-4]);
	if(
		strcasecmp(".avi", ext) == 0 ||
		strcasecmp(".ogm", ext) == 0
	)
		return 1;

	// mp3, ogg, wav가 아닌 파일일 경우
	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
