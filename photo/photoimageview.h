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
#ifndef __PHOTOIMAGEVIEW_H_
#define __PHOTOIMAGEVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
class CPhotoImageView : public ViewForm
{
	private :
		SDL_Surface *pImgMenuButton;
		SDL_Surface *pImgMenuFrame;
		SDL_Surface *pImgMenuFrame_cp;
		SDL_Surface *pImgDirectGo;
		SDL_Surface *pImgMenuFrame_up;
		SDL_Surface *pImgMenuFrame_up_cp;
		//SDL_Surface *pImgProgressBar;
		//SDL_Surface *pImgHeightBar;
		SDL_Surface *pImgOptTitle;

		bool menuStatus;
		bool viewInfo;
		int menuPos, menuRow, menuCol;

    public:
		CPhotoImageView();
		~CPhotoImageView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnTimerProc(SDL_Surface *pScreen);
		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);
		void OnPenUp(SDL_Surface *pScreen, int x, int y);

		int action(int cmd = -1);
		bool getMenuStatus() { return menuStatus; }
		ViewMode getMode() { return PHOTO_IMAGE_VIEW; };
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
