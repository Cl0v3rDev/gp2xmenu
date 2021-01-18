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
#include "ebookview.h"
#include "mainmenuapp.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CEBookView::CEBookView()
{
	selectFunc = select;
	iconNo = ICON_EBOOK;
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("ebook", "filelist");
	theApp.FontColor("ebook", "list", r, g, b);

	if(!refresh)
	{
		struct stat statbuf;

		theApp.Mount(MOUNT_SD);
		lstat(SD_PATH_EBOOK, &statbuf);
		if( !S_ISDIR(statbuf.st_mode) )
			mkdir(SD_PATH_EBOOK);

		lastName = theApp.menuEBook.getName();
		lastPath = theApp.menuEBook.getPath();
		if( (lastPath.size() == 0) || (lstat(lastPath.c_str(), &statbuf) != 0) || (!S_ISDIR(statbuf.st_mode)) )
		    lastPath = SD_PATH_EBOOK;
	}

	FileList::Initialize(refresh);
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookView::OnExecute(SDL_Surface *pScreen, int button)
{
	if((button != VK_FB) && (button != VK_TAT)) return;

	std::string path = lastPath + "/" + pList[nPosition].szName;

	if(theApp.menuEBook.TextLoad(path.c_str()))
	{
		theApp.textvs = TEXT_ON_STATUS;
		theApp.SetView(EBOOK_TEXT_VIEW);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
int CEBookView::select(const struct dirent *name)
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

	if(len == 7) {
		if(strcasecmp("COPYING", name->d_name) == 0) return 1;
		if(strcasecmp("LICENSE", name->d_name) == 0) return 1;
		if(strcasecmp("CREDITS", name->d_name) == 0) return 1;
		if(strcasecmp("AUTHORS", name->d_name) == 0) return 1;
	}

	ext = (char*)&(name->d_name[len-4]);
	if(strcasecmp(".txt", ext) == 0)
		return 1;

	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
