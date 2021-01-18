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
#include "usbview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CUSBView::CUSBView()
{
	pImgBackGround = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
CUSBView::~CUSBView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// �ʱ�ȭ �Լ�
void CUSBView::Initialize(bool refresh)
{
	// background Load
	pImgBackGround = theApp.LoadImage("usb", "body");
	SDL_Surface *connected = theApp.LoadImage("usb", "connected");
	if(connected != NULL)
	{
		SDL_Rect srcrect = {0, 0, connected->w, connected->h};
		SDL_Rect dstrect = {79, 84, 0, 0};
#ifdef GP2XF200
		srcrect.h = 75;
#endif
		SDL_BlitSurface(connected, &srcrect, pImgBackGround, &dstrect);
		SDL_SAFE_FREE(connected);
	}

	if(!refresh)
		theApp.setUSBConnect(true);
}
//[*]------------------------------------------------------------------------------------------[*]
// ���� �Լ�
void CUSBView::Exitialize(bool refresh)
{
	if(!refresh)
	    theApp.setUSBConnect(false);
	SDL_SAFE_FREE(pImgBackGround);
}
//[*]------------------------------------------------------------------------------------------[*]
void CUSBView::OnDraw(SDL_Surface *pScreen)
{
	SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	SDL_UpdateRect(pScreen, 0, 0, 320, 240);		// ȭ�� ������Ʈ
}
//[*]------------------------------------------------------------------------------------------[*]
void CUSBView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_FX :	// X ��ư 	// ���� �޴��� View�� �̵��Ѵ�.
#ifdef GP2XF100
			theApp.SwapReverse();
			theApp.SetView(SETTING_VIEW);
			break;
#endif
		case VK_START :	// Start ��ư 	// Main View�� �̵��Ѵ�.
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CUSBView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_USB_CON :
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
