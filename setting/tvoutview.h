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
#ifndef __TVOUTVIEW_H_
#define __TVOUTVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
class CTvOutView : public ViewForm
{
	private :
		SDL_Surface 	*pImgBackGround;			// ¹è°æ Surface
		SDL_Surface		*pImgButton;
		SDL_Surface		*pImgBox;
		SDL_Surface		*pImgArrow;
		SDL_Surface		*pImgLeftTitle;
		SDL_Surface		*pImgNTSC_PAL;
		SDL_Surface		*pImgButtonTitle;


		DisplayMode		mode;
		int				tvfd;
		int 			m_Pos;
		bool			arrowStatus[4];

	public :
		CTvOutView();
		~CTvOutView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);

		void MoveTVScreen(SDL_Surface *pScreen, int direction, bool flag);
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
