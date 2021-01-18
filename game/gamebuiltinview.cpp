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
#include "mainmenuapp.h"
#include "gamebuiltinview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CGameBuiltinView::CGameBuiltinView()
{
	selectFunc = select;
	iconNo = ICON_GAME;
	iconWidth = 39;
	useStorage = false;
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CGameBuiltinView::Initialize(bool refresh)
{
	// background Load
	pImgBackGround = theApp.LoadImage("game", "filelist");
	theApp.FontColor("game", "list", r, g, b);

	if(!refresh)
		lastPath = BUILTIN_PATH_GAME;

	FileList::Initialize(refresh);
}
//[*]------------------------------------------------------------------------------------------[*]
// 파일 실행
void CGameBuiltinView::OnExecute(SDL_Surface *pScreen, int button)
{
	if((button != VK_FB) && (button != VK_TAT)) return;

#ifndef WIN32
	// 우선 화면을 지운다.
	SDL_Rect rect = { 0, 0, 320, 240 };
	SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, 0x00, 0x00, 0x00));
	SDL_UpdateRect(pScreen, 0, 0, 320, 240);		// 화면 업데이트

	//선택된 ini파일의 경로를 읽어온다
	char gamePath[512]={0,};
	INI_Open( (lastPath + "/" + pList[nPosition].szName).c_str() );
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

	// 실행 파일의 경로를 만든다.
	std::string szExecFile = std::string("./");
	szExecFile += cutname;

	chdir(cutpath);		// 파일이 있는 디렉토리로 이동
	Exitialize(); 	// 리소스 정리

	theApp.SDL_Exitialize();
	execlp(szExecFile.c_str(), szExecFile.c_str(), NULL, 0);		// 파일 실행
#endif
	// 여기는 절대 올수 없는 지역입니다. ㅠ.ㅠ
}
//[*]------------------------------------------------------------------------------------------[*]
int CGameBuiltinView::select(const struct dirent *name)
{
	char *ext = NULL;
	struct stat statbuf;
	lstat(name->d_name, &statbuf);

	if(S_ISDIR(statbuf.st_mode))
		return 0;

	int len = strlen(name->d_name);
	if(len < 4)	return 0;

	ext = (char*)&(name->d_name[len-4]);
	if(strcasecmp(".ini", ext) == 0)
	{
		return 1;
	}
	// ini가 아닌 파일일 경우
	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
