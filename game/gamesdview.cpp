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
#include <inifile.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "gamesdview.h"
#include "mainmenuapp.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CGameSdView::CGameSdView()
{
	selectFunc = select;
	iconNo = ICON_GAME;
	iconWidth = 39;
}
//[*]------------------------------------------------------------------------------------------[*]
void CGameSdView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("game", "filelist");
	theApp.FontColor("game", "list", r, g, b);

	if(!refresh)
	{
		struct stat statbuf;

		theApp.Mount(MOUNT_SD);
		lstat(SD_PATH_GAME, &statbuf);
		if( !S_ISDIR(statbuf.st_mode) )
			mkdir(SD_PATH_GAME);

		if( (lastPath.size() == 0) || (lstat(lastPath.c_str(), &statbuf) != 0) || (!S_ISDIR(statbuf.st_mode)) )
		    lastPath = SD_PATH_GAME;
	}

	FileList::Initialize(refresh);
}
//[*]------------------------------------------------------------------------------------------[*]
void CGameSdView::OnExecute(SDL_Surface *pScreen, int button)
{
	if((button != VK_FB) && (button != VK_TAT)) return;

#ifndef WIN32
	// �켱 ȭ���� �����.
	SDL_Rect rect = { 0, 0, 320, 240 };
	SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, 0x00, 0x00, 0x00));
	SDL_UpdateRect(pScreen, 0, 0, 320, 240);		// ȭ�� ������Ʈ

	// ���� ������ ��θ� �����.
	std::string execFile = "./";

	if(pList[nPosition].nAttribute == INI_FORMAT) //ini���� �̶��
	{
		//���õ� ini������ ��θ� �о�´�
		char gamePath[512]={0,};
		INI_Open( ("/mnt/nand/" + pList[nPosition].szName).c_str() );
		strcpy(gamePath, INI_ReadText("info", "path", "") );
		INI_Close();
		if (access(gamePath, X_OK) != 0) return;

		char cutname[512];
		char cutpath[512];

		char *cut = strrchr(gamePath, '/');
		if(cut == NULL) cut = strrchr(gamePath, '\\');
		if(cut == NULL)
		{
			strcpy(cutname, gamePath);
			strcpy(cutpath, "");
		}
		else
		{
			strcpy(cutname, cut + 1);
			strncpy(cutpath, gamePath, cut - gamePath);
			cutpath[cut - gamePath] = 0;
		}

		execFile += cutname;
		chdir(cutpath);		// ������ �ִ� ���丮�� �̵�
	}
	else //�Ϲ� gpe,gpu ���� �̶��
	{
		execFile += pList[nPosition].szName;
		chdir(lastPath.c_str());
	}

	Exitialize(); 	// ���ҽ� ����

	theApp.SDL_Exitialize();
	execlp(execFile.c_str(), execFile.c_str(), NULL, 0);		// ���� ����
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
int CGameSdView::select(const struct dirent *name)
{
	char *ext = NULL;
	struct stat statbuf;
	lstat(name->d_name, &statbuf);

	// ������ ���
	if(S_ISDIR(statbuf.st_mode))
	{
		// ���� �ε� "."(���� ����)�� ���� 0�� ����
		if(strcmp(".", name->d_name) == 0) return 0;

		return 1;
	}

	int len = strlen(name->d_name);
	if(len < 4)	return 0;

	ext = (char*)&(name->d_name[len-4]);
	if( strcasecmp(".gpu", ext) == 0 ||
		strcasecmp(".gpe", ext) == 0 )
		return 1;

	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
