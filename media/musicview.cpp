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
#include "musicview.h"
#include "mainmenuapp.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CMusicView::CMusicView()
{
	selectFunc = select;
	iconNo = ICON_MUSIC;
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CMusicView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("music", "filelist");
	theApp.FontColor("music", "list", r, g, b);

	if(!refresh)
	{
		struct stat statbuf;

		theApp.Mount(MOUNT_SD);
		lstat(SD_PATH_MUSIC, &statbuf);
		if( !S_ISDIR(statbuf.st_mode) )
			mkdir(SD_PATH_MUSIC);

		lastName = theApp.menuMedia.getInfo(MEDIA_FILE_NAME);
		if( (lastPath.size() == 0) || (lstat(lastPath.c_str(), &statbuf) != 0) || (!S_ISDIR(statbuf.st_mode)) )
		    lastPath = SD_PATH_MUSIC;
	}

	FileList::Initialize(refresh);
}
//[*]------------------------------------------------------------------------------------------[*]
void CMusicView::OnExecute(SDL_Surface *pScreen, int button)
{
	if((button != VK_FB) && (button != VK_TAT)) return;

	theApp.menuMedia.reset(lastPath);
	for(int i=0; i<nCount; i++)
		if(pList[i].nAttribute != FOLDER_FORMAT)
			theApp.menuMedia.addList(pList[i].szName, i == nPosition);

	if(theApp.menuMedia.load()) {
		theApp.SetView(MEDIA_PLAY_VIEW);
		theApp.menuMedia.play();
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
		if(waitMode != WAIT_ANY_BUTTON) OnDraw(pScreen);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
int CMusicView::select(const struct dirent *name)
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
	if(strcasecmp(".mp3", ext) == 0 || strcasecmp(".ogg", ext) == 0 || strcasecmp(".wav", ext) == 0)
		return 1;

	// mp3, ogg, wav가 아닌 파일일 경우
	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
