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
#include "settingview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
#define ICONCOUNT 6
#define MENUCOUNT 5
#ifdef GP2XF100
const int menuItems[] = {0, 1, 2, 5, 4, -1};
#else
const int menuItems[] = {0, 1, 2, 3, 4, -1};
#endif
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CSettingView::CSettingView()
{
	pImgBackGround = NULL;
	pImgButton = NULL;
	pImgButtonTitle = NULL;
	pImgHome = NULL;
	posCursor  = 0;			// 커서 위치
}
//[*]------------------------------------------------------------------------------------------[*]
CSettingView::~CSettingView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CSettingView::Initialize(bool refresh)
{
	// 이미지를 미리 로드한다.
	pImgBackGround = theApp.LoadImage("setting", "body", "SETTING");
	pImgButton = theApp.LoadImage("setting", "button");
	pImgButtonTitle = theApp.LoadImage("setting", "button_title");
	pImgHome = theApp.LoadImage("setting", "home");

	lastDraw = -1;
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CSettingView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
	SDL_SAFE_FREE(pImgButton);
	SDL_SAFE_FREE(pImgButtonTitle);
	SDL_SAFE_FREE(pImgHome);
}
//[*]------------------------------------------------------------------------------------------[*]
// 타이머 처리 함수
void CSettingView::OnTimerProc(SDL_Surface *pScreen)
{
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw 함수
void CSettingView::OnDraw(SDL_Surface *pScreen)
{
	SDL_Rect	srcrect, rect;

	// 배경에 화면 출력
	if(lastDraw == -1)
		SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	if(pImgButton == NULL)
	{
		SDL_UpdateRect(pScreen, 0, 0, 0, 0);		// 화면 업데이트
		return;
	}

	// 아이콘을 그린다.
	for(int i=0; i<MENUCOUNT; i++)
	{
		if((i != lastDraw) && (i != posCursor) && (lastDraw != -1))
			continue;

		srcrect.w = pImgButton->w / ICONCOUNT;
		srcrect.h = pImgButton->h / 2;
		srcrect.x = srcrect.w * menuItems[i];
		srcrect.y = (posCursor == i) ? srcrect.h : 0;

		rect.x = ((i % 3) * (37+32)) + 15;
		rect.y = ((i < 3) ? 31: 94) ;

		if(lastDraw != -1) {
			SDL_Rect	bgrect;

			bgrect.x = rect.x;
			bgrect.y = rect.y;
			rect.w = srcrect.w;
			rect.h = srcrect.h;

			SDL_BlitSurface(pImgBackGround, &rect, pScreen, &bgrect);
		}

		// 배경에 아이콘 출력
		SDL_BlitSurface(pImgButton, &srcrect, pScreen, &rect);

		if(lastDraw != -1)
			SDL_UpdateRect(pScreen, rect.x, rect.y, rect.w, rect.h);		// 화면 업데이트

		// 버튼 타이틀 출력
		srcrect.w = pImgButtonTitle->w ;
		srcrect.h = pImgButtonTitle->h / ICONCOUNT;
		srcrect.x = 0;
		srcrect.y = srcrect.h * menuItems[i];
		rect.x = ((i % 3) * (37+32)) + 4;
		rect.y = ((i < 3) ? 72: 136) ;

		SDL_BlitSurface(pImgButtonTitle, &srcrect, pScreen, &rect);
	}

#ifdef GP2XF200
	rect.x = 270;
	rect.y = 220;
	SDL_BlitSurface(pImgHome, NULL, pScreen, &rect);
#endif

	//배터리 아이콘 로딩 시작
	theApp.getStatusRect(&rect);
	theApp.drawStatusIcon(pScreen, &rect);

	if(lastDraw == -1)
		SDL_UpdateRect(pScreen, 0, 0, 0, 0);		// 화면 업데이트

	//usleep(20000);

	lastDraw = posCursor;


}
//[*]------------------------------------------------------------------------------------------[*]
// 2005.12.13 미스콜이아
// 조이스틱을 상하좌우로 처리 변경
// 조이스틱 다운 처리
void CSettingView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_UP 		:	OnCursorUp(pScreen);		break;	// 상 방향키
		case VK_DOWN 	:   OnCursorDown(pScreen);		break;	// 하 방향키
		case VK_LEFT 	:   OnCursorLeft(pScreen);		break;	// 좌 방향키
		case VK_RIGHT 	:   OnCursorRight(pScreen);		break;	// 우 방향키
		case VK_TAT	  	:     // TAT 버튼
		case VK_FB	  	:     // B 버튼
			OnMoveSub_Execute(pScreen);	break;	// B 버튼
		case VK_START :
		case VK_FX	  :


			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);	break;	// X 버튼 	// 이전 메뉴인 View로 이동한다.
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CSettingView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
}
//[*]------------------------------------------------------------------------------------------[*]
// 터치 또는 마우스 입력 처리
void CSettingView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	int i, w, h, px, py;
	bool runItem = false;

	//home 기능
	px=270;
	py=220;

	if( (x >= px) && (y >= py) )
	{
		theApp.SwapReverse();
		theApp.SetView(MAIN_VIEW);
		return;
	}

	w = pImgButton->w / ICONCOUNT;
	h = pImgButton->h / 2;

	for(int i=0; i<MENUCOUNT; i++)
	{
		px = ((i % 3) * (37+32)) + 15;
		py = ((i < 3) ? 31: 94) ;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			posCursor = i;
			theApp.PlaySound(MOVE);

			OnDraw(pScreen);

			runItem = true;

			break;
		}
	}

	if(!runItem) return;
	OnMoveSub_Execute(pScreen);
}
//[*]----------------------------------------------------------------------------------------------------[*
// 터치 또는 마우스 입력 처리
void CSettingView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{
	/*
	int i, w, h, px, py;
	bool runItem = false;

	w = pImgButton->w / 5;
	h = pImgButton->h / (theApp.mainMaxFrame + 1);

	for(int i=0; i<ICONCOUNT; i++)
	{
		px = ((i % 3) * (37+32)) + 15;
		py = ((i < 3) ? 31: 94) ;


		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h))) {
			if(posCursor != i) {
				posCursor = i;
				theApp.PlaySound(MOVE);

				OnDraw(pScreen);
			}
			runItem = true;
			break;
		}
	}

	if(!runItem) return;

	OnMoveSub_Execute(pScreen);

	*/
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 왼쪽로 이동
void CSettingView::OnCursorUp(SDL_Surface *pScreen)
{
	if(posCursor-3 >= 0) {
		posCursor -= 3;

		theApp.PlaySound(MOVE);
		OnDraw(pScreen);	// 다시 그린다.
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 오른쪽으로 이동
void CSettingView::OnCursorDown(SDL_Surface *pScreen)
{
	if(posCursor+3 < MENUCOUNT) {
		posCursor += 3;

		// 다시 그린다.
		theApp.PlaySound(MOVE);
		OnDraw(pScreen);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 왼쪽로 이동
void CSettingView::OnCursorLeft(SDL_Surface *pScreen)
{
	posCursor = (posCursor-1+MENUCOUNT)%MENUCOUNT;
	//if(posCursor-1 >= 0)	posCursor -= 1;

	// 다시 그린다.
	theApp.PlaySound(MOVE);
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 오른쪽으로 이동
void CSettingView::OnCursorRight(SDL_Surface *pScreen)
{
	posCursor = (posCursor+1+MENUCOUNT)%MENUCOUNT;
//	if(posCursor+1 < MENUCOUNT)	posCursor += 1;

	// 다시 그린다.
	theApp.PlaySound(MOVE);
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
void CSettingView::OnMoveSub_Execute(SDL_Surface *pScreen)
{
	if(posCursor != 2)
		theApp.PlaySound(RUN);
	switch(posCursor)
	{
		case 0 : 	// INFORMATION
			theApp.SetView(INFOMATION_VIEW);
			break;

		case 1 :	// TEST MODE
			theApp.SetView(TESTMODE_VIEW);
			break;

		case 2 : 	// SYSTEM SETTING
			theApp.SetView(SYSTEM_VIEW);
			break;

#if !defined(PND) && !defined(WIZ)
		case 3 : 	// Touch LCD Setting
#ifdef GP2XF200
			theApp.SetView(TOUCH_VIEW);
#else
			theApp.SetView(USB_VIEW);
#endif
			break;

		case 4 : 	// TV-OUT
			theApp.SetView(TVOUT_VIEW);
			break;
#endif
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 실행
void CSettingView::OnExecute(SDL_Surface *pScreen)
{
}
//[*]------------------------------------------------------------------------------------------[*]
