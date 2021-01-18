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
#include "config.h"
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "photoview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
#define FILELIST_COUNT 8
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CPhotoView::CPhotoView()
{
	selectFunc = select;
	iconNo = ICON_PHOTO;
}
//[*]------------------------------------------------------------------------------------------[*]
void CPhotoView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("photo", "filelist");
	theApp.FontColor("photo", "list", r, g, b);

	if(!refresh)
	{
		struct stat statbuf;

		theApp.Mount(MOUNT_SD);
		lstat(SD_PATH_PHOTO, &statbuf);
		if( !S_ISDIR(statbuf.st_mode) )
			mkdir(SD_PATH_PHOTO);

		lastName = theApp.menuPhoto.getInfo(PHOTO_FILE_NAME);
		if( (lastPath.size() == 0) || (lstat(lastPath.c_str(), &statbuf) != 0) || (!S_ISDIR(statbuf.st_mode)) )
		    lastPath = SD_PATH_PHOTO;
	}

	FileList::Initialize(refresh);
}
//[*]------------------------------------------------------------------------------------------[*]
void CPhotoView::OnExecute(SDL_Surface *pScreen, int button)
{
	if((button != VK_FB) && (button != VK_TAT)) return;

	SDL_Surface *loading = theApp.LoadImage("common", "loading");
	if(loading) {
		SDL_Rect rect;
		rect.x = (320 - loading->w) >> 1;
		rect.y = (240 - loading->h) >> 1;
		SDL_BlitSurface(loading, NULL, pScreen, &rect);
		SDL_FreeSurface(loading);

		SDL_UpdateRect(pScreen, 0, 0, 0, 0);
	}

	theApp.menuPhoto.reset(lastPath);
	for(int i=0; i<nCount; i++)
		if(pList[i].nAttribute != FOLDER_FORMAT)
			theApp.menuPhoto.addList(pList[i].szName, pList[i].nAttribute, i == nPosition);

	if(theApp.menuPhoto.LoadImage())
	{
		theApp.photovs = ON_STATUS;
		theApp.SetView(PHOTO_IMAGE_VIEW);
	}
	else
	{
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
int CPhotoView::select(const struct dirent *name)
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
		strcasecmp(".bmp", ext) == 0 ||
		strcasecmp(".gif", ext) == 0 ||
		strcasecmp(".png", ext) == 0 ||
		strcasecmp(".pcx", ext) == 0 ||
		strcasecmp(".jpg", ext) == 0 ||
		strcasecmp(".jpeg", ext) == 0
	)
		return 1;

	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
