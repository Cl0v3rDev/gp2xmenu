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
#include <string>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "infoview.h"
#include "textout.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CInfomationView::CInfomationView()
{
	pImgBackGround = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
CInfomationView::~CInfomationView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CInfomationView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("info", "body", "INFORMATION");
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CInfomationView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw 함수
void CInfomationView::OnDraw(SDL_Surface *pScreen)
{
	int i = 0;
	unsigned char ch;
	char szdate[35]		= { 0, };
	char szBuffer[34]	= {	0, };
	unsigned short szMark[2];
	int r, g, b;
	FILE *stream = NULL;

	// 화면을 하얀색 출력
	SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	theApp.FontColor("info", "text", r, g, b);

	if(theApp.cfgLanguage == "_kor")
	{
		PrintKorMessage(pScreen, 54, 34, "버전"     , r, g, b);
		PrintKorMessage(pScreen, 54, 54, "모델"     , r, g, b);
		PrintKorMessage(pScreen, 54, 74, "시리얼"   , r, g, b);
	}
	else
	{
		PrintMessage(pScreen,	  54,  34, "VERSION"	, r, g, b);	// 버젼
		PrintMessage(pScreen, 	  54,  54, "MODEL"		, r, g, b);	// 모델
		PrintMessage(pScreen, 	  54,  74, "S/N"		, r, g, b);	// 시리얼
	}

	// 펌웨어 정보 출력
	PrintMessage(pScreen, 109, 34, VERSION, r, g, b);
#ifdef GP2XF200
	PrintMessage(pScreen, 109, 54, "GP2X - F200", r, g, b);
#else
	PrintMessage(pScreen, 109, 54, "GP2X - F100", r, g, b);
#endif

	// ⓒ 0x24D2

	// ⓒ 2004-2006 Gamepark Holdings Co,Ltd. All rights
	// reserved. Gamepark Holdings and the Gamepark
	// Holdings logo are trademarks of Gamepark Holdings
	// Co,Ltd., registered in Korea and other countries.
	// GP2X and GP2X logo are trademarks of Gamepark
	// Holdings Co,Ltd.

	// ⓒ 2004-2008 ㈜게임파크홀딩스.
	// Gamepark Holdings(게임파크홀딩스) 및 GPH 로고는
	// 대한민국 및 전세계 국가에서 법으로 보호받고 있는
	// ㈜게임파크 홀딩스의 등록상표입니다.
	// GP2X 및 GP2X 로고는 ㈜게임파크 홀딩스의 등록상표
	// 입니다.

	szMark[0] = 0x24D2;
	szMark[1] = '\0';
	DrawText(pScreen, 15, 114, szMark, -1, r, g, b);
	
	if(theApp.cfgLanguage == "_kor")
	{
		PrintKorMessage(pScreen, 27, 114, " 2004-2008 ㈜게임파크홀딩스."                       , r, g, b);
		PrintKorMessage(pScreen, 15, 129, "Gamepark Holdings(게임파크홀딩스) 및 GPH 로고는"    , r, g, b);
		PrintKorMessage(pScreen, 15, 144, "대한민국 및 전세계 국가에서 법으로 보호받고 있는"   , r, g, b);
		PrintKorMessage(pScreen, 15, 159, "㈜게임파크 홀딩스의 등록상표입니다."                , r, g, b);
		PrintKorMessage(pScreen, 15, 174, "GP2X 및 GP2X 로고는 ㈜게임파크 홀딩스의 등록상표"   , r, g, b);
		PrintKorMessage(pScreen, 15, 189, "입니다."                                            , r, g, b);
	}
	else
	{
		PrintMessage(pScreen, 27, 114, " 2004-2008 Gamepark Holdings Co,Ltd. All rights"	, r, g, b);
		PrintMessage(pScreen, 15, 129, "reserved. Gamepark Holdings and the Gamepark"		, r, g, b);
		PrintMessage(pScreen, 15, 144, "Holdings logo are trademarks of Gamepark Holdings"	, r, g, b);
		PrintMessage(pScreen, 15, 159, "Co,Ltd., registered in Korea and other countries."	, r, g, b);
		PrintMessage(pScreen, 15, 174, "GP2X and GP2X logo are trademarks of Gamepark"		, r, g, b);
		PrintMessage(pScreen, 15, 189, "Holdings Co,Ltd."									, r, g, b);
	}

	// 시리얼 정보를 보여주는 부분

	theApp.ReadSerial((unsigned char*)szBuffer);

	szdate[ 0] = szBuffer[ 0];	// 년
	szdate[ 1] = szBuffer[ 1];
	szdate[ 2] = szBuffer[ 2];
	szdate[ 3] = szBuffer[ 3];
	szdate[ 4] = '.';
	szdate[ 5] = szBuffer[ 4];	// 월
	szdate[ 6] = szBuffer[ 5];
	szdate[ 7] = '.';
	szdate[ 8] = szBuffer[ 6];	// 일
	szdate[ 9] = szBuffer[ 7];
	szdate[10] = ' ';
	szdate[11] = '-';
	szdate[12] = ' ';
	szdate[13] = szBuffer[ 8];	// G
	szdate[14] = szBuffer[ 9];	// P
	szdate[15] = szBuffer[10];	// 2
	szdate[16] = szBuffer[11];	// X
	szdate[17] = szBuffer[12];	// V
	szdate[18] = szBuffer[13];	// 1
	szdate[19] = szBuffer[14];	// 2
	szdate[20] = szBuffer[15];	// 3
	szdate[21] = ' ';
	szdate[22] = '-';
	szdate[23] = ' ';
	szdate[24] = szBuffer[16];	// 1
	szdate[25] = szBuffer[17];	// 2
	szdate[26] = szBuffer[18];	// 3
	szdate[27] = szBuffer[19];	// 4
	szdate[28] = szBuffer[20];	// 5
	szdate[29] = szBuffer[21];	// 6
	szdate[30] = szBuffer[22];	// 7
	szdate[31] = szBuffer[23];	// 8
	szdate[32] = '\0';

	PrintMessage(pScreen, 109, 74, szdate, r, g, b);

	//배터리 아이콘 로딩 시작
	SDL_Rect rect;
	theApp.getStatusRect(&rect);
	theApp.drawStatusIcon(pScreen, &rect);

	SDL_UpdateRect(pScreen, 0, 0, 0, 0);		// 화면 업데이트
}
//[*]------------------------------------------------------------------------------------------[*]
void CInfomationView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_FX :	// X 버튼 	// 이전 메뉴인 View로 이동한다.
			theApp.SwapReverse();
			theApp.SetView(SETTING_VIEW);
			break;
		case VK_START :	// X 버튼 	// Main View로 이동한다.
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CInfomationView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	theApp.SwapReverse();
	theApp.SetView(SETTING_VIEW);
}
//[*]------------------------------------------------------------------------------------------[*]
