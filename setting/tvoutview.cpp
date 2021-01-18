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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "tvoutview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CTvOutView::CTvOutView()
{
	pImgBackGround = NULL;
	pImgButton = NULL;
	pImgBox = NULL;
	pImgArrow = NULL;
	pImgLeftTitle = NULL;
	pImgNTSC_PAL = NULL;
	pImgButtonTitle=NULL;

    m_Pos = 0;	// NTSC : 0, PAL : 1
	mode = LCD_MODE;
}
//[*]------------------------------------------------------------------------------------------[*]
CTvOutView::~CTvOutView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// �ʱ�ȭ �Լ�
void CTvOutView::Initialize(bool refresh)
{
	// background Load
	pImgBackGround = theApp.LoadImage("tvout", "body", "TV-OUT");
	pImgButton = theApp.LoadImage("tvout", "tv_button");
	pImgBox = theApp.LoadImage("tvout", "box");
	pImgArrow = theApp.LoadImage("tvout", "arrow");
	pImgLeftTitle = theApp.LoadImage("tvout", "left_title");
	pImgNTSC_PAL = theApp.LoadImage("tvout", "ntsc_pal");
	pImgButtonTitle= theApp.LoadImage("tvout", "tv_button_title");;

	if(!refresh) {
		mode = theApp.getDisplayMode();
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// ���� �Լ�
void CTvOutView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
	SDL_SAFE_FREE(pImgButton);
	SDL_SAFE_FREE(pImgBox);
	SDL_SAFE_FREE(pImgArrow);
	SDL_SAFE_FREE(pImgLeftTitle);
	SDL_SAFE_FREE(pImgNTSC_PAL);
	SDL_SAFE_FREE(pImgButtonTitle);
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw �Լ�
void CTvOutView::OnDraw(SDL_Surface *pScreen)
{
	int i;
	SDL_Rect srcrect, dstrect;
	int textWidth, textHeight;

	// ��濡 ȭ�� ���
	SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	if(mode == LCD_MODE)			//  LCD ����� ���
	{

		srcrect.w = pImgNTSC_PAL->w / 2;
		srcrect.h = pImgNTSC_PAL->h / 2;

		srcrect.x = (m_Pos == 0) ? srcrect.w : 0;
		srcrect.y = 0;
		dstrect.x = 131;
		dstrect.y = 37;
		SDL_BlitSurface(pImgNTSC_PAL, &srcrect, pScreen, &dstrect);   //����,�帰 NTSC ���


		srcrect.x = (m_Pos != 0) ? srcrect.w : 0;
		srcrect.y = srcrect.h * 1;
		dstrect.x = 222;
		dstrect.y = 37;
		SDL_BlitSurface(pImgNTSC_PAL, &srcrect, pScreen, &dstrect);   //����,�帰 PAL ���



		srcrect.w = pImgButton->w / 3;
		srcrect.h = pImgButton->h / 1;

		srcrect.x = 0;
		srcrect.y = 0;
		dstrect.x = 127;
		dstrect.y = 200;
		SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);   //B ��ư ���


		srcrect.w = pImgButtonTitle->w ;
		srcrect.h = pImgButtonTitle->h / 3;

		srcrect.x = 0;
		srcrect.y = 0;
		dstrect.x = 148;
		dstrect.y = 203;
		SDL_BlitSurface(pImgButtonTitle, &srcrect, pScreen, &dstrect);   //'OK' ĸ�� ���




		srcrect.w = pImgButton->w / 3;
		srcrect.h = pImgButton->h / 1;

		srcrect.x = srcrect.w;
		srcrect.y = 0;
		dstrect.x = 200;
		dstrect.y = 200;
		SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);   //X ��ư ���




		srcrect.w = pImgButtonTitle->w ;
		srcrect.h = pImgButtonTitle->h / 3;

		srcrect.x = 0;
		srcrect.y = srcrect.h;
		dstrect.x = 221;
		dstrect.y = 203;
		SDL_BlitSurface(pImgButtonTitle, &srcrect, pScreen, &dstrect);   //'Cancel' ĸ�� ���



	}
	else			//  TV ����� ���
	{

		srcrect.w = pImgNTSC_PAL->w / 2;
		srcrect.h = pImgNTSC_PAL->h / 2;

		srcrect.x = (mode == TV_NTSC_MODE) ? srcrect.w : 0;
		srcrect.y = 0;
		dstrect.x = 131;
		dstrect.y = 37;
		SDL_BlitSurface(pImgNTSC_PAL, &srcrect, pScreen, &dstrect);  //���õ� ����,�帰 NTSC ���


		srcrect.x = (mode == TV_PAL_MODE) ? srcrect.w : 0;
		srcrect.y = srcrect.h * 1;
		dstrect.x = 222;
		dstrect.y = 37;
		SDL_BlitSurface(pImgNTSC_PAL, &srcrect, pScreen, &dstrect);  //���õ� ����,�帰 NTSC ���






		srcrect.w = pImgButton->w / 3;
		srcrect.h = pImgButton->h / 1;

		srcrect.x = srcrect.w * 2;
		srcrect.y = 0;
		dstrect.x = 93;
		dstrect.y = 200;
		SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);   //Y ��ư ���


		srcrect.w = pImgButtonTitle->w ;
		srcrect.h = pImgButtonTitle->h / 3;

		srcrect.x = 0;
		srcrect.y = srcrect.h * 2;
		dstrect.x = 113;
		dstrect.y = 203;
		SDL_BlitSurface(pImgButtonTitle, &srcrect, pScreen, &dstrect);   //'LCD Mode' ĸ�� ���




		srcrect.w = pImgButton->w / 3;
		srcrect.h = pImgButton->h / 1;

		srcrect.x = srcrect.w;
		srcrect.y = 0;
		dstrect.x = 180;
		dstrect.y = 200;
		SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);   //X ��ư ���




		srcrect.w = pImgButtonTitle->w ;
		srcrect.h = pImgButtonTitle->h / 3;

		srcrect.x = 0;
		srcrect.y = srcrect.h;
		dstrect.x = 201;
		dstrect.y = 203;
		SDL_BlitSurface(pImgButtonTitle, &srcrect, pScreen, &dstrect);   //'Cancel' ĸ�� ���




	}

	srcrect.w = pImgLeftTitle->w/2;
	srcrect.h = pImgLeftTitle->h/2;

	if(mode == LCD_MODE)			//  LCD ����� ���
		srcrect.x = srcrect.w;
	else
		srcrect.x = 0;
	srcrect.y = 0;
	dstrect.x = 56;
	dstrect.y = 37;

	SDL_BlitSurface(pImgLeftTitle, &srcrect, pScreen, &dstrect);    //TV Mode ��� Ÿ��Ʋ ���


	if(mode == LCD_MODE)			//  LCD ����� ���
		srcrect.x = 0;
	else
		srcrect.x = srcrect.w;

	srcrect.y = srcrect.h;
	dstrect.x = 56;
	dstrect.y = 88;

	SDL_BlitSurface(pImgLeftTitle, &srcrect, pScreen, &dstrect);   //Position �̶�� Ÿ��Ʋ ���

	//���͸� ������ �ε� ����
	theApp.getStatusRect(&dstrect);
	theApp.drawStatusIcon(pScreen, &dstrect);

	SDL_UpdateRect(pScreen, 0, 0, 320, 240);		// ȭ�� ������Ʈ
}
//[*]------------------------------------------------------------------------------------------[*]
void CTvOutView::MoveTVScreen(SDL_Surface *pScreen, int direction, bool flag)
{
	if(flag)
		theApp.moveTVPosition(direction);

	arrowStatus[direction] = flag;

	if(pImgArrow == NULL) return;

	int i;
	SDL_Rect srcrect;
	for(i=0;i<4;i++) {
		static SDL_Rect rectArrow[4] = {
			{132, 134, 0, 0}, // LEFT
			{195, 134, 0, 0}, // RIGHT
			{164, 102, 0, 0}, // UP
			{164, 165, 0, 0}  // DOWN
		};

		srcrect.w = pImgArrow->w / 2;
		srcrect.h = pImgArrow->h / 4;

		srcrect.x = rectArrow[i].x;
		srcrect.y = rectArrow[i].y;
		SDL_BlitSurface(pImgBackGround, &srcrect, pScreen, &rectArrow[i]);

		srcrect.x = arrowStatus[i] ? srcrect.w : 0;
		srcrect.y = i * srcrect.h;
		//SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &rectArrow[i]);

		SDL_UpdateRect(pScreen, rectArrow[i].x, rectArrow[i].y, rectArrow[i].w, rectArrow[i].h);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CTvOutView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	if(mode == LCD_MODE)			//  LCD ����� ���
	{
		switch(button)
		{
			case VK_FX :	// X ��ư 	// ���� �޴��� View�� �̵��Ѵ�.
				theApp.SwapReverse();
				theApp.SetView(SETTING_VIEW);

				break;
			case VK_START :	// X ��ư 	// Main View�� �̵��Ѵ�.
				theApp.SwapReverse();
				theApp.SetView(MAIN_VIEW);
				break;
			case VK_TAT:
			case VK_FB :	// ���⼭ ��带 �ٲ۴�.
				theApp.PlaySound(CONNECT);
				theApp.setDisplayMode((m_Pos == 0) ? TV_NTSC_MODE : TV_PAL_MODE);
				theApp.SwapReverse();
				theApp.SetView(SETTING_VIEW);
				break;
			case VK_LEFT 	:
			case VK_RIGHT 	:		// NTSC, PAL ����
				m_Pos = 1 - m_Pos;
				OnDraw(pScreen);
				break;
		}
	}
	else
	{
		switch(button)
		{
			case VK_FX :	// X ��ư 	// ���� �޴��� View�� �̵��Ѵ�.
				theApp.SwapReverse();
				theApp.SetView(SETTING_VIEW);
				break;
			case VK_START :	// X ��ư 	// Main View�� �̵��Ѵ�.
				theApp.SwapReverse();
				theApp.SetView(MAIN_VIEW);
				break;
			case VK_FY :	// LCD ���� ����
				theApp.PlaySound(CONNECT);
			    theApp.setDisplayMode(LCD_MODE);
				theApp.SwapReverse();
				theApp.SetView(SETTING_VIEW);
				break;
			case VK_UP: MoveTVScreen(pScreen, TV_POS_UP, true); break;
			case VK_DOWN: MoveTVScreen(pScreen, TV_POS_DOWN, true); break;
			case VK_LEFT: MoveTVScreen(pScreen, TV_POS_LEFT, true); break;
			case VK_RIGHT: MoveTVScreen(pScreen, TV_POS_RIGHT, true); break;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CTvOutView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	if(mode != LCD_MODE)			//  LCD ����� ���
	{
		switch(button)
		{
			case VK_UP: MoveTVScreen(pScreen, TV_POS_UP, false); break;
			case VK_DOWN: MoveTVScreen(pScreen, TV_POS_DOWN, false); break;
			case VK_LEFT: MoveTVScreen(pScreen, TV_POS_LEFT, false); break;
			case VK_RIGHT: MoveTVScreen(pScreen, TV_POS_RIGHT, false); break;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CTvOutView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{

	int i, w, h, px, py;
	w=h=px=py=0;
	bool pushbutton=false;

	if(mode == LCD_MODE)			//  LCD ����� ���
	{
		w = pImgNTSC_PAL->w / 2;
		h = pImgNTSC_PAL->h / 2;
		px = 131;
		py = 37;
		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))  //����,�帰 NTSC
		{
			OnJoystickDown(pScreen, VK_LEFT);
			return;
		}

		px = 222;
		py = 37;
		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))  //����,�帰 PAL
		{
			OnJoystickDown(pScreen, VK_LEFT);
			return;
		}

		w = 22;//(pImgButton->w / 3) + 11;
		h = pImgButton->h / 1;
		px = 127;
		py = 200;
		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)) && (!pushbutton))  //B ��ư
		{
			pushbutton=true;
			OnJoystickDown(pScreen, VK_FB);
			return;

		}

		w = (pImgButton->w / 3) + (pImgButtonTitle->w) ;
		h = pImgButton->h / 1;
		px = 200;
		py = 200;
		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h))  && (!pushbutton) )  //X ��ư + 'Cancel' ĸ��
		{
			pushbutton=true;
			OnJoystickDown(pScreen, VK_FX);
			return;
		}

		/*
		w = 11;
		h = pImgButtonTitle->h / 3;
		px = 148;
		py = 203;
		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))  //'OK' ĸ��
		{
			OnJoystickDown(pScreen, VK_FB);
		}
		*/
	}
}
//[*]------------------------------------------------------------------------------------------[*]
