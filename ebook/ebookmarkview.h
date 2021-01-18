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
#ifndef __EBOOKMARKVIEW_H_
#define __EBOOKMARKVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
class CEBookMarkView : public ViewForm
{
	private :
		SDL_Surface *pImgButton;
		SDL_Surface *pImgBmFrame;
		SDL_Surface *pImgButtonTitle;
		SDL_Surface *pImgBmLineSelect;
		SDL_Surface *pImgBmLineSelect_bk;

		unsigned short *cursor;
		int pos;
		int font_color;

		void SetPos(SDL_Surface *pScreen, int x, int y);
		int PutChar(SDL_Surface *pScreen, unsigned short code, int limit);
		void PutString(SDL_Surface *pScreen, int x, int y, char *str);
		void PutFileName(SDL_Surface *pScreen, int x, int y, char* buf, int limit);

		void ShowStatusLine(SDL_Surface *pScreen);

	public :
		CEBookMarkView();
		~CEBookMarkView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);
		void OnPenUp(SDL_Surface *pScreen, int x, int y);

};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
