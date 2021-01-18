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
#ifndef __MAINVIEW_H_
#define __MAINVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
class CMainView : public ViewForm
{
	private :
		int				posCursor;	// 커서 위치
		SDL_Surface 	*pImgBackGround;	// 배경 Surface
		SDL_Surface		*pImgIcon_small;		// 작은 아이콘 Surface
		SDL_Surface		*pImgIcon_middle;		// 중간 아이콘 Surface
		SDL_Surface		*pImgIcon_big;		    // 큰   아이콘 Surface
		SDL_Surface		*pImgIcon_title;	    // 아이콘 title Surface
		SDL_Surface     *pImgArrow;             // 화살표 아이콘 Surface
		SDL_Surface     *pImgBattery;           // 배터리 아이콘 Surface

		SDL_Surface		*pImgBackGround_SD;

		bool			useSDIcon;
		bool			SDMounted;
		int				iconCount;

		SDL_Thread		*threadBoot;

	public :
		CMainView();
		~CMainView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnTimerProc(SDL_Surface *pScreen);
		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);
		void OnPenUp(SDL_Surface *pScreen, int x, int y);

		void OnCursorUp(SDL_Surface *pScreen);
		void OnCursorDown(SDL_Surface *pScreen);
		void OnCursorLeft(SDL_Surface *pScreen);
		void OnCursorRight(SDL_Surface *pScreen);


		void OnMoveSub_Execute(SDL_Surface *pScreen);
		void OnExecute(SDL_Surface *pScreen);

		ViewMode getMode() { return MAIN_VIEW; }
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
