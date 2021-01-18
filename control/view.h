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
#ifndef __VIEW_H_
#define __VIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include "config.h"
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
class ViewForm
{
	protected:
		int lastDraw;

	public:
		ViewForm();
		virtual ~ViewForm();

		virtual void Initialize(bool refresh = false) {};
		virtual void Exitialize(bool refresh = false) {};

		// 화면 출력 및 제어 관련
		virtual void OnRefresh(SDL_Surface *pScreen);
		virtual void OnTimerProc(SDL_Surface *pScreen) {};
		virtual void OnDraw(SDL_Surface *pScreen) {};
		virtual void OnProc(SDL_Surface *pScreen, SDL_Event *pEvent);

		// 키보드 및 스틱 입력
		virtual void OnKeyDown(SDL_Surface *pScreen, SDLKey key);
		virtual void OnKeyUp(SDL_Surface *pScreen, SDLKey key);
		virtual void OnJoystickDown(SDL_Surface *pScreen, Uint8 button)	{};
		virtual void OnJoystickUp(SDL_Surface *pScreen, Uint8 button)	{};
		virtual void OnPenDown(SDL_Surface *pScreen, int x, int y) {};
		virtual void OnPenUp(SDL_Surface *pScreen, int x, int y) {};
		virtual void OnPenMove(SDL_Surface *pScreen, int x, int y, int xrel, int yrel) {};
		virtual void OnEvent(SDL_Surface *pScreen, int code, void *data1, void *data2) {};
		virtual void OnBatteryChange(SDL_Surface *pScreen, int status) { OnDraw(pScreen); };

		// 뷰 전환
		virtual void ActiveView(ViewForm **pView);
		virtual ViewMode getMode() { return NONE; };
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
