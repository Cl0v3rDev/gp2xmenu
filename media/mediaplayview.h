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
#ifndef __MEDIAPLAYVIEW_H_
#define __MEDIAPLAYVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
class CMediaPlayView : public ViewForm
{
	private :
		SDL_Surface *pImgBackGround;

		SDL_Surface *pImgSmallFont;
		SDL_Surface *pImgLargeFont;
		SDL_Surface *pImgProgressBar;
		SDL_Surface *pImgEqualizer;
		SDL_Surface *pImgStatus;
		SDL_Surface *pImgVolume;
		SDL_Surface *pImgVolumeBar;
		SDL_Surface *pImgButton;
		SDL_Surface *pImgMenuFrame;
		SDL_Surface *pImgHeightBar;
		SDL_Surface *pImgDirectGo;
		SDL_Surface *pImgMovieTitle;
		SDL_Surface *pImgMovieMenu;
		SDL_Surface *pImgSubtitle;

		int menuPos, menuRow, menuCol;
		bool drawInfo;
		bool fileOpenStatus;
		bool wantShowLyrics;
		bool areaRepeat_set;
		int areaRepeat_st;
		int areaRepeat_ed;
		bool areaRepeat_play;
		bool hold;
		bool pushShift, useShortcuts;
		int seekButton, seekTime, seekStep;

		const int *menuItems[3];

		Uint32 lastPlayer;

		int drawSmallNumber(SDL_Surface *pScreen, int value, int x, int y, int len = 1, int type = 0);
		int drawLargeNumber(SDL_Surface *pScreen, int value, int x, int y, int len = 1, int type = 0);
		void drawPlayStatus(SDL_Surface *pScreen, bool update = true);
		void drawSubtitle(SDL_Surface *pScreen, bool update = true);

	public :
		bool menuStatus;
		bool seeking;

		CMediaPlayView();
		~CMediaPlayView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);
		void OnEvent(SDL_Surface *pScreen, int code, void *data1, void *data2);

		int action(int cmd = -1);
		bool getHoldValue() { return hold; };
		ViewMode getMode() { return MEDIA_PLAY_VIEW; };
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
