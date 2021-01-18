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
#include "gameview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CGameView::CGameView()
{
	pImgBackGround = NULL;
	pImgButton = NULL;
	pImgButtonTitle =NULL;
	posCursor  = 0;			// Ŀ�� ��ġ
}
//[*]------------------------------------------------------------------------------------------[*]
CGameView::~CGameView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// �ʱ�ȭ �Լ�
void CGameView::Initialize(bool refresh)
{
	// �̹����� �̸� �ε��Ѵ�.
	pImgBackGround = theApp.LoadImage("game", "filelist");
	SDL_Surface *icon = theApp.LoadImage("common", "icon_body");
	if(icon != NULL)
	{
		SDL_Rect srcrect, dstrect;
		srcrect.w = icon->w / 3;
		srcrect.h = icon->h / 3;
		srcrect.x = srcrect.w * (ICON_GAME % 3);
		srcrect.y = srcrect.h * (ICON_GAME / 3);
		dstrect.x = 1;
		dstrect.y = 7;
		SDL_BlitSurface(icon, &srcrect, pImgBackGround, &dstrect);
		SDL_SAFE_FREE(icon);
	}

	pImgButton = theApp.LoadImage("game", "button");
	pImgButtonTitle = theApp.LoadImage("game", "button_title");
	lastDraw = -1;
}
//[*]------------------------------------------------------------------------------------------[*]
// ���� �Լ�
void CGameView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
	SDL_SAFE_FREE(pImgButton);
	SDL_SAFE_FREE(pImgButtonTitle);
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw �Լ�
void CGameView::OnDraw(SDL_Surface *pScreen)
{
	SDL_Rect	srcrect, rect;

	// ��濡 ȭ�� ���
	if(lastDraw == -1)
		SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	if(pImgButton == NULL)
	{
		SDL_UpdateRect(pScreen, 0, 0, 0, 0);		// ȭ�� ������Ʈ
		return;
	}

	// �������� �׸���.
	for(int i=0; i<ICONCOUNT_GAME; i++)
	{
		if((i != lastDraw) && (i != posCursor) && (lastDraw != -1))
			continue;

		srcrect.w = pImgButton->w / 3;
		srcrect.h = pImgButton->h / 2;
		srcrect.x = srcrect.w * i;
		srcrect.y = (posCursor == i) ? srcrect.h : 0;

		rect.x = 20;
		rect.y = ((i % 3) * (37+20)) + 60;

		if(lastDraw != -1) {
			SDL_Rect	bgrect;

			bgrect.x = rect.x;
			bgrect.y = rect.y;
			rect.w = srcrect.w;
			rect.h = srcrect.h;

			SDL_BlitSurface(pImgBackGround, &rect, pScreen, &bgrect);
		}

		// ��濡 ������ ���
		SDL_BlitSurface(pImgButton, &srcrect, pScreen, &rect);

		if(lastDraw != -1)
			SDL_UpdateRect(pScreen, rect.x, rect.y, rect.w, rect.h);		// ȭ�� ������Ʈ

		// ��ư Ÿ��Ʋ ���
		srcrect.w = pImgButtonTitle->w ;
		srcrect.h = pImgButtonTitle->h / 2;
		srcrect.x = 0;
		srcrect.y = srcrect.h * i;
		rect.x = 65;
		rect.y = ((i % 3) * (37+20)) + 70;

		SDL_BlitSurface(pImgButtonTitle, &srcrect, pScreen, &rect);

	}

	//���͸� ������ �ε� ����
	theApp.getStatusRect(&rect);
	theApp.drawStatusIcon(pScreen, &rect);

	if(lastDraw == -1)
		SDL_UpdateRect(pScreen, 0, 0, 0, 0);		// ȭ�� ������Ʈ

	//usleep(20000);

	lastDraw = posCursor;
}
//[*]------------------------------------------------------------------------------------------[*]
// ���̽�ƽ �ٿ� ó��
void CGameView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_UP 		:	OnCursorUp(pScreen);		break;	// �� ����Ű
		case VK_DOWN 	:   OnCursorDown(pScreen);		break;	// �� ����Ű
		//case VK_LEFT 	:   OnCursorLeft(pScreen);		break;	// �� ����Ű
		//case VK_RIGHT 	:   OnCursorRight(pScreen);		break;	// �� ����Ű
		case VK_TAT	  	:     // TAT ��ư
		case VK_FB	  	:     // B ��ư
			OnExecute(pScreen);	break;	// B ��ư
		case VK_START :
		case VK_FX	  :
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);	break;	// X ��ư 	// ���� �޴��� View�� �̵��Ѵ�.
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// ���̽�ƽ �ٿ� ó��
void CGameView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	// Nothing...
}
//[*]------------------------------------------------------------------------------------------[*]
// ��ġ �Ǵ� ���콺 �Է� ó��
void CGameView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	int i, w, h, px, py;

	for(int i=0; i<ICONCOUNT_GAME; i++)
	{
		w = pImgButton->w / 3;
		h = pImgButton->h / 2;
		px = 20;
		py = ((i % 3) * (37+20)) + 60;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{

			if(posCursor != i) {
				posCursor = i;
				theApp.PlaySound(MOVE);

				OnDraw(pScreen);
			}
			break;
		}

		// ��ư Ÿ��Ʋ
		w = pImgButtonTitle->w ;
		h = pImgButtonTitle->h / 2;
		px = 65;
		py = ((i % 3) * (37+20)) + 70;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			if(posCursor != i) {
				posCursor = i;
				theApp.PlaySound(MOVE);

				OnDraw(pScreen);
			}
			break;
		}
	}
}
//[*]----------------------------------------------------------------------------------------------------[*
// ��ġ �Ǵ� ���콺 �Է� ó��
void CGameView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{
	int i, w, h, px, py;
	bool runItem = false;

	for(int i=0; i<ICONCOUNT_GAME; i++)
	{
		w = pImgButton->w / 3;
		h = pImgButton->h / 2;
		px = 20;
		py = ((i % 3) * (37+20)) + 60;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			if(posCursor != i)
			{
				posCursor = i;
				theApp.PlaySound(MOVE);

				OnDraw(pScreen);
			}
			runItem = true;
			break;
		}

		// ��ư Ÿ��Ʋ
		w = pImgButtonTitle->w ;
		h = pImgButtonTitle->h / 2;
		px = 65;
		py = ((i % 3) * (37+20)) + 70;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			if(posCursor != i)
			{
				posCursor = i;
				theApp.PlaySound(MOVE);

				OnDraw(pScreen);
			}

			runItem = true;
			break;
		}
	}

	if(!runItem) return;

	OnExecute(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// Ŀ���� ���� �̵�
void CGameView::OnCursorUp(SDL_Surface *pScreen)
{
	posCursor = (posCursor-1+ICONCOUNT_GAME)%ICONCOUNT_GAME;

	// �ٽ� �׸���.
	theApp.PlaySound(MOVE);
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// Ŀ���� �Ʒ��� �̵�
void CGameView::OnCursorDown(SDL_Surface *pScreen)
{
	posCursor = (posCursor+1+ICONCOUNT_GAME)%ICONCOUNT_GAME;

	// �ٽ� �׸���.
	theApp.PlaySound(MOVE);
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
void CGameView::OnExecute(SDL_Surface *pScreen)
{
	if(posCursor != 2)
		theApp.PlaySound(RUN);
	switch(posCursor)
	{
		case 0 : 	// SDī��
			theApp.SetView(GAME_SD_VIEW);
			break;

		case 1 :	// �������
			theApp.SetView(GAME_BUILTIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
