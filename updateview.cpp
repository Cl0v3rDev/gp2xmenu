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
#include "updateview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CUpdateView::CUpdateView()
{
	pImgBackGround = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
CUpdateView::~CUpdateView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CUpdateView::Initialize(bool refresh)
{
	// background Load
	pImgBackGround = theApp.LoadImage("update", "body");
	SDL_Surface *update = theApp.LoadImage("common", "update");
	if(update != NULL)
	{
		SDL_Rect rect;
		rect.x = (320 - update->w) >> 1;
		rect.y = (240 - update->h) >> 1;
		SDL_BlitSurface(update, NULL, pImgBackGround, &rect);
		SDL_FreeSurface(update);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CUpdateView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw 함수
void CUpdateView::OnDraw(SDL_Surface *pScreen)
{
	SDL_Rect rect;

	rect.x = (pScreen->w - pImgBackGround->w) >> 1;
	rect.y = (pScreen->h - pImgBackGround->h) >> 1;

	SDL_FillRect(pScreen, NULL, 0);
	SDL_BlitSurface(pImgBackGround, NULL, pScreen, &rect);
	SDL_UpdateRect(pScreen, 0, 0, 0, 0);		// 화면 업데이트
}
//[*]------------------------------------------------------------------------------------------[*]
void CUpdateView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	SDL_Event event;
	switch(button)
	{
		case VK_FB :
			theApp.updateCancel = false;
		case VK_FX :
			event.type = SDL_QUIT;
			SDL_PushEvent(&event);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CUpdateView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	int i, w, h, px, py;
	w=h=px=py=0;

	w = 51;
	h = 26;
	px = 129;
	py = 133;
	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))  //진한,흐린 NTSC
	{
		OnJoystickDown(pScreen, VK_FB);
		return;
	}

	px = 180;
	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))  //진한,흐린 PAL
	{
		OnJoystickDown(pScreen, VK_FX);
		return;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
