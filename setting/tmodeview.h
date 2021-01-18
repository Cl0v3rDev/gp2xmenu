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
#ifndef __TESTMODEVIEW_H_
#define __TESTMODEVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
class CTestModeView : public ViewForm
{
	private :
		SDL_Surface 	*pImgBackGround;			// ¹è°æ Surface
		SDL_Surface		*pImgButtonUp;
		SDL_Surface		*pImgButtonDown;
		int				drawStatus;
		bool			buttonStatus[19];
		bool			buttonDraw[19];

	public :
		CTestModeView();
		~CTestModeView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);

		void OnTestWhite(SDL_Surface *pScreen);
		void OnTestBlack(SDL_Surface *pScreen);
		void OnTestRed(SDL_Surface *pScreen);
		void OnTestGreen(SDL_Surface *pScreen);
		void OnTestBlue(SDL_Surface *pScreen);
		void OnTestDraw(SDL_Surface *pScreen);
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
