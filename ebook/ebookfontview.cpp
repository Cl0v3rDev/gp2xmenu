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
#include "ebookfontview.h"
#include "mainmenuapp.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CEBookFontView::CEBookFontView()
{
	selectFunc = select;
	iconNo = ICON_FONT;
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CEBookFontView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("ebook", "filelist");
	theApp.FontColor("ebook", "list", r, g, b);

	if(!refresh)
	{
		struct stat statbuf;

		theApp.Mount(MOUNT_SD);
		lstat(SD_PATH_FONT, &statbuf);
		if( !S_ISDIR(statbuf.st_mode) )
			mkdir(SD_PATH_FONT);

		if( (lastPath.size() == 0) || (lstat(lastPath.c_str(), &statbuf) != 0) || (!S_ISDIR(statbuf.st_mode)) )
		    lastPath = SD_PATH_FONT;
	}

	FileList::Initialize(refresh);
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookFontView::OnExecute(SDL_Surface *pScreen, int button)
{
	std::string fontPath;

	switch (button)
	{
		case VK_FB:
		case VK_TAT:
			fontPath = lastPath + "/" + pList[nPosition].szName;
			break;
		case VK_FA:
			fontPath.clear();
			break;
		default:
			return;
	}

    theApp.menuEBook.setFont(fontPath.c_str());

	theApp.SwapReverse();
	theApp.SetView(EBOOK_TEXT_VIEW);

	theApp.SwapReverse();
    theApp.SetView(EBOOK_SET_VIEW);
}
//[*]------------------------------------------------------------------------------------------[*]
int CEBookFontView::select(const struct dirent *name)
{
	struct stat statbuf;
	lstat(name->d_name, &statbuf);

	if(S_ISDIR(statbuf.st_mode))
	{
		if(strcmp(".", name->d_name) == 0)
			return 0;
		return 1;
	}

	int len = strlen(name->d_name);
	if(len < 4)	return 0;

	char *ext = (char*)&(name->d_name[len-4]);
	if(strcasecmp(".ttf", ext) == 0)
		return 1;

	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
