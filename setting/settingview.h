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
#ifndef __SETTINGVIEW_H_
#define __SETTINGVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
class CSettingView : public ViewForm
{
	private :
		int 			posCursor;			// Ŀ�� ��ġ
		int 			drawFrame;	// Ű ����
		int				loopIcon;
		SDL_Surface 	*pImgBackGround;	// ��� Surface
		SDL_Surface		*pImgButton;		// ������ Surface
		SDL_Surface		*pImgButtonTitle;	// ������ Ÿ��Ʋ Surface
		SDL_Surface		*pImgHome;

	public :
		CSettingView();
		~CSettingView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnTimerProc(SDL_Surface *pScreen);
		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);
		void OnPenUp(SDL_Surface *pScreen, int x, int y);

		void OnCursorLeft(SDL_Surface *pScreen);
		void OnCursorRight(SDL_Surface *pScreen);
		void OnCursorUp(SDL_Surface *pScreen);
		void OnCursorDown(SDL_Surface *pScreen);

		void OnMoveSub_Execute(SDL_Surface *pScreen);
		void OnExecute(SDL_Surface *pScreen);

		ViewMode getMode() { return SETTING_VIEW; };
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
