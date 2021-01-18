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
#include "tmodeview.h"
#include "textout.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
static SDL_Rect rectButton[19] = {
	{	 87,  96, 23, 23	}, // UP

	{	 54,  96, 23, 23	}, // UP_LEFT
	{	 54, 126, 23, 23	}, // LEFT
	{	 54, 157, 23, 23	}, // DOWN_LEFT

	{	 87, 157, 23, 23	}, // DOWN
	{	119, 157, 24, 23	}, // DOWN_RIGHT
	{	119, 126, 24, 23	}, // RIGHT
	{	119,  96, 24, 23	}, // UP_RIGHT


	{	249, 197, 34, 11	}, // START
	{	192, 197, 34, 11	}, // SELECT

	{	 54,  61, 89, 23	}, // L
	{	192,  61, 90, 23	}, // R

	{	192, 130, 24, 23	}, // A

	{	258, 130, 24, 23	}, // B

	{	226, 157, 24, 23	}, // X

	{	226,  96, 24, 23	}, // Y



	{	109, 197, 34, 11	}, // VOL_UP
	{	 54, 197, 33, 11	}, // VOL_DOWN
	{	 87, 126, 23, 23	}  // TAT
};
//[*]------------------------------------------------------------------------------------------[*]
CTestModeView::CTestModeView()
{
	pImgBackGround = NULL;
	pImgButtonUp = NULL;
	pImgButtonDown = NULL;
	drawStatus = 0;
}
//[*]------------------------------------------------------------------------------------------[*]
CTestModeView::~CTestModeView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CTestModeView::Initialize(bool refresh)
{
	// background Load
	pImgBackGround = theApp.LoadImage("tmode", "body", "T-Mode");

	pImgButtonUp = theApp.LoadImage("tmode", "buttonup");
	pImgButtonDown = theApp.LoadImage("tmode", "buttondown");
	if(pImgButtonUp == NULL) {
		pImgButtonUp = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, 16, 0xf800, 0x07e0, 0x001f, 0x0000);
		SDL_FillRect(pImgButtonUp, NULL, 0xFFFF);
		if(theApp.cfgSkin)
			SDL_SetAlpha(pImgButtonUp, SDL_SRCALPHA, 128);
	}
	if(pImgButtonDown == NULL) {
		pImgButtonDown = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, 16, 0xf800, 0x07e0, 0x001f, 0x0000);
		SDL_FillRect(pImgButtonDown, NULL, 0x0000);
		if(theApp.cfgSkin)
			SDL_SetAlpha(pImgButtonDown, SDL_SRCALPHA, 128);
	}
	if(!refresh) {
		drawStatus = 0;
		memset(buttonStatus, 0, sizeof(buttonStatus));
		memset(buttonDraw, 0, sizeof(buttonDraw));
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CTestModeView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
	SDL_SAFE_FREE(pImgButtonUp);
	SDL_SAFE_FREE(pImgButtonDown);
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw 함수
void CTestModeView::OnDraw(SDL_Surface *pScreen)
{
	switch(drawStatus)
	{

		case 0 :
			{
			// 화면을 하얀색으로 바꾼다.
			OnTestWhite(pScreen);
			SDL_UpdateRect(pScreen, 0, 0, 0, 0);

			// 화면에 펌웨어 정보를 보여준다.
			// 정보는 24바이트 이다.
			// 20051101  8byte (양산날짜)
			// GPX2V001  8byte	(양산주차및월차버젼)
			// 00000000  8byte (양산 카운트번호로 카운트해서 등록함 )

			// 실제 뷰화면 예제
			// 2005.11.01 - GPX2V001 - 00000000

			char szdate[34]		= {	0,	};
			char szBuffer[34]	= {	0,	};

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

			// 시리얼 넘버 표시
		   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
			dest = OnUTF8ToUnicode(szdate, 0);
			DrawText(pScreen, 20, 60, dest, -1, 0x00, 0x00, 0x00);
			free(dest);
			break;
		}
		case 1 :	// Key Test
		case 2 :	// Key Test
			OnTestDraw(pScreen);
			break;
		case 3 :	// Black Test
			OnTestBlack(pScreen);
			break;
		case 4 :	// White Test
			OnTestWhite(pScreen);
			break;
		case 5 :	// Red Test
			OnTestRed(pScreen);
			break;
		case 6 :	// Green Test
			OnTestGreen(pScreen);
			break;
		case 7 :	// Blue Test
			OnTestBlue(pScreen);
			break;
	}

	if(drawStatus != 2) {
		SDL_UpdateRect(pScreen, 0, 0, 0, 0); // 화면 업데이트
		if(drawStatus == 1) drawStatus = 2;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CTestModeView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	if(button < 19)
		buttonStatus[button] = true;

	if(buttonStatus[VK_FL] && buttonStatus[VK_FR]) {
		if(drawStatus == 7) {
			theApp.SwapReverse();
			theApp.SetView(SETTING_VIEW);
			return;
		}
		drawStatus++;
		memset(buttonStatus, 0, sizeof(buttonStatus));
		OnDraw(pScreen);
	} else if(drawStatus == 2)
		OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CTestModeView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	if(button < 19)
		buttonStatus[button] = false;

	if(drawStatus == 2)
		OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
void CTestModeView::OnTestBlack(SDL_Surface *pScreen)
{
	SDL_FillRect(pScreen, NULL, SDL_MapRGB(pScreen->format, 0x00, 0x00, 0x00));

	// 메세지
   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
	dest = OnUTF8ToUnicode("Press L Button + R Button", 0);
	DrawText(pScreen, 20, 20, dest, -1, 0xFF, 0xFF, 0xFF);
	free(dest);
}
//[*]------------------------------------------------------------------------------------------[*]
void CTestModeView::OnTestWhite(SDL_Surface *pScreen)
{
	SDL_FillRect(pScreen, NULL, SDL_MapRGB(pScreen->format, 0xFF, 0xFF, 0xFF));

	// 메세지
   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
	dest = OnUTF8ToUnicode("Press L Button + R Button", 0);
	DrawText(pScreen, 20, 20, dest, -1, 0x00, 0x00, 0x00);
	free(dest);
}
//[*]------------------------------------------------------------------------------------------[*]
void CTestModeView::OnTestRed(SDL_Surface *pScreen)
{
	SDL_FillRect(pScreen, NULL, SDL_MapRGB(pScreen->format, 0xFF, 0x00, 0x00));

	// 메세지
   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
	dest = OnUTF8ToUnicode("Press L Button + R Button", 0);
	DrawText(pScreen, 20, 20, dest, -1, 0x00, 0x00, 0x00);
	free(dest);
}
//[*]------------------------------------------------------------------------------------------[*]
void CTestModeView::OnTestGreen(SDL_Surface *pScreen)
{
	SDL_FillRect(pScreen, NULL, SDL_MapRGB(pScreen->format, 0x00, 0xFF, 0x00));

	// 메세지
   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
	dest = OnUTF8ToUnicode("Press L Button + R Button", 0);
	DrawText(pScreen, 20, 20, dest, -1, 0x00, 0x00, 0x00);
	free(dest);
}
//[*]------------------------------------------------------------------------------------------[*]
void CTestModeView::OnTestBlue(SDL_Surface *pScreen)
{
	SDL_FillRect(pScreen, NULL, SDL_MapRGB(pScreen->format, 0x00, 0x00, 0xFF));

	// 메세지
   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
	dest = OnUTF8ToUnicode("Press L Button + R Button", 0);
	DrawText(pScreen, 20, 20, dest, -1, 0x00, 0x00, 0x00);
	free(dest);
}
//[*]------------------------------------------------------------------------------------------[*]
void CTestModeView::OnTestDraw(SDL_Surface *pScreen)
{
	int i;

	if(drawStatus == 1)
		SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	for(i=0;i<19;i++)
		if( (buttonStatus[i] != buttonDraw[i]) || (drawStatus == 1) ) {
			SDL_Rect dstrect;
			dstrect.x = rectButton[i].x;
			dstrect.y = rectButton[i].y;
			if(drawStatus == 2)
				SDL_BlitSurface(pImgBackGround, &rectButton[i], pScreen, &dstrect);
			if(buttonStatus[i])
				SDL_BlitSurface(pImgButtonDown, &rectButton[i], pScreen, &dstrect);
			else
				SDL_BlitSurface(pImgButtonUp, &rectButton[i], pScreen, &dstrect);
			if(drawStatus == 2)
				SDL_UpdateRect(pScreen, rectButton[i].x, rectButton[i].y, rectButton[i].w, rectButton[i].h);
			buttonDraw[i] = buttonStatus[i];
		}
/*
	// 메세지
   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
	dest = OnUTF8ToUnicode("Press L Button + R Button", 0);
	DrawText(pScreen, 20, 20, dest, -1, 0x00, 0x00, 0x00);
	free(dest);
*/
}
//[*]------------------------------------------------------------------------------------------[*]
