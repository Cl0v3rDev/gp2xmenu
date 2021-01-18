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
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
#define SKINEXTRACT 0xfb
#define SKINEXPORT 0xfc
#define REFRESH 0xfe
#define TERMINATE 0xff
//[*]------------------------------------------------------------------------------------------[*]
#ifdef PND
extern const struct KEYMAP {
	SDLKey key;
	Uint8 button;
} keymap[];
#else
static const struct KEYMAP {
	SDLKey key;
	Uint8 button;
} keymap[] = {
	{SDLK_UP, VK_UP},
	{SDLK_i, VK_UP},
	{SDLK_KP8, VK_UP},
	{SDLK_DOWN, VK_DOWN},
	{SDLK_k, VK_DOWN},
	{SDLK_KP2, VK_DOWN},
	{SDLK_LEFT, VK_LEFT},
	{SDLK_j, VK_LEFT},
	{SDLK_KP4, VK_LEFT},
	{SDLK_RIGHT, VK_RIGHT},
	{SDLK_l, VK_RIGHT},
	{SDLK_KP6, VK_RIGHT},
	{SDLK_F10, VK_START},
	{SDLK_BACKSPACE, VK_START},
	{SDLK_HOME, VK_START},
	{SDLK_KP7, VK_START},
	{SDLK_SPACE, VK_FB},
	{SDLK_RETURN, VK_TAT},
	{SDLK_KP5, VK_TAT},
	{SDLK_ESCAPE, VK_FX},
	{SDLK_KP1, VK_FX},
	{SDLK_KP_PERIOD, VK_FX},
	{SDLK_PAGEUP, VK_FL},
	{SDLK_KP9, VK_FL},
	{SDLK_PAGEDOWN, VK_FR},
	{SDLK_KP3, VK_FR},

	{SDLK_a, VK_FA},
	{SDLK_s, VK_FB},
	{SDLK_z, VK_FX},
	{SDLK_w, VK_FY},
	{SDLK_q, VK_FL},
	{SDLK_e, VK_FR},

	{SDLK_d, VK_VOL_DOWN},
	{SDLK_KP_MINUS, VK_VOL_DOWN},
	{SDLK_f, VK_VOL_UP},
	{SDLK_KP_PLUS, VK_VOL_UP},
	{SDLK_c, VK_SELECT},
	{SDLK_KP0, VK_SELECT},
	{SDLK_v, VK_START},
	
	{SDLK_F1, VK_USB_CON},
	{SDLK_F7, SKINEXTRACT},
	{SDLK_F8, SKINEXPORT},
	{SDLK_F11, REFRESH},
	{SDLK_KP_DIVIDE, REFRESH},
	{SDLK_F12, TERMINATE},
	{SDLK_KP_MULTIPLY, TERMINATE},
	{SDLK_UNKNOWN, 0}
};
#endif
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
ViewForm::ViewForm()
{
}
//[*]------------------------------------------------------------------------------------------[*]
ViewForm::~ViewForm()
{
}
//[*]------------------------------------------------------------------------------------------[*]
void ViewForm::OnRefresh(SDL_Surface *pScreen)
{
	Exitialize(true);
	Initialize(true);
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
void ViewForm::OnProc(SDL_Surface *pScreen, SDL_Event *pEvent)
{
	switch(pEvent->type)
	{
		case SDL_JOYBUTTONDOWN :
			OnJoystickDown(pScreen, pEvent->jbutton.button);
			break;
		case SDL_JOYBUTTONUP :
			OnJoystickUp(pScreen, pEvent->jbutton.button);
			break;
		case SDL_KEYDOWN :
			OnKeyDown(pScreen, pEvent->key.keysym.sym);
			break;
		case SDL_KEYUP :
			OnKeyUp(pScreen, pEvent->key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN :
			OnPenDown(pScreen, pEvent->button.x, pEvent->button.y);
			break;
		case SDL_MOUSEBUTTONUP :
			OnPenUp(pScreen, pEvent->button.x, pEvent->button.y);
			break;
	    case SDL_MOUSEMOTION :
			OnPenMove(pScreen, pEvent->motion.x, pEvent->motion.y, pEvent->motion.xrel, pEvent->motion.yrel);
			break;
		case SDL_USEREVENT :
			OnEvent(pScreen, pEvent->user.code, pEvent->user.data1, pEvent->user.data2);
			break;
	    default:
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void ViewForm::OnKeyDown(SDL_Surface *pScreen, SDLKey key)
{
	int i;
	for(i=0;keymap[i].key != SDLK_UNKNOWN;i++) {
		if(keymap[i].key == key) {
			switch(keymap[i].button) {
#ifdef SKINMAKER
				case SKINEXTRACT:
					theApp.SkinExtract();
					break;
				case SKINEXPORT:
#ifndef SKINPACKED
					theApp.SkinLoad();
					OnRefresh(pScreen);
					if(!theApp.SkinExport()) {
						SDL_Surface *img = theApp.LoadImage("common", "skinerror");
						if(img != NULL) {
							SDL_Rect dstrect;
							dstrect.x = (XRES - img->w) >> 1;
							dstrect.y = (YRES - img->h) >> 1;
							SDL_BlitSurface(img, NULL, pScreen, &dstrect);
							SDL_UpdateRect(pScreen, 0, 0, 0, 0);
							SDL_FreeSurface(img);
							SDL_Delay(2000);
						}
					}
					OnRefresh(pScreen);
#endif
					break;
#endif
				case REFRESH:
					theApp.SkinLoad();
					OnRefresh(pScreen);
					break;
				case TERMINATE:
					exit(0);
					break;
				default:
					OnJoystickDown(pScreen, keymap[i].button);
					break;
			}
			break;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void ViewForm::OnKeyUp(SDL_Surface *pScreen, SDLKey key)
{
	int i;
	for(i=0;keymap[i].key != SDLK_UNKNOWN;i++) {
		if(keymap[i].key == key) {
			switch(keymap[i].button) {
				case SKINEXPORT:
					// skin export code here...
					break;
				case REFRESH:
					// refresh code here...
					break;
				case TERMINATE:
					// exit code here...
					break;
				default:
					OnJoystickUp(pScreen, keymap[i].button);
					break;
			}
			break;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void ViewForm::ActiveView(ViewForm **pView)
{
#ifndef SWAPEFFECT
	if(*pView != NULL)	(*pView)->Exitialize();
	*pView = this;

	this->Initialize();		// 새로운 자료를 로딩한다.
#else
	SDL_Surface *viewOld = NULL;
	SDL_Surface *viewNew = NULL;

	if(*pView != NULL) {
		viewOld = theApp.SwapSurface(SWAP_PREV);
		if(viewOld != NULL) {
			(*pView)->lastDraw = -1;
			(*pView)->OnDraw(viewOld);
		}
		(*pView)->Exitialize();
	}

	this->Initialize();		// 새로운 자료를 로딩한다.
	if(viewOld != NULL) {
		viewNew = theApp.SwapSurface(SWAP_NEXT);
		if(viewNew != NULL)
		{
			this->OnDraw(viewNew);
			theApp.SwapScreen();
		}
		(*pView)->lastDraw = -1;
	}

	*pView = this;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
