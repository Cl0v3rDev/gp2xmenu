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
#ifndef __SYSTEMVIEW_H_
#define __SYSTEMVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include "config.h"
//[*]------------------------------------------------------------------------------------------[*]
#include <string>
#include <vector>
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "typed.h"
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
//#define MAXVALUE 50
//[*]------------------------------------------------------------------------------------------[*]
class CSystemView : public ViewForm
{
	private :
		SDL_Surface 	*m_pImgBackGround;		// 배경 Surface
		//SDL_Surface		*m_pImgUpArrow;
		//SDL_Surface		*m_pImgDownArrow;
		//SDL_Surface		*m_pImgSelectBar;
		SDL_Surface     *pImgButton;
		SDL_Surface		*pImgArrow;
		SDL_Surface		*pImgLeftTitle;

		int 			m_posCursor;			// 커서 위치
		int 			m_nKeyStatus;			// 키 상태
		bool			m_needScroll;			// 스크롤이 필요할 경우 체크
		int				m_drawBegin;
		//std::vector<int>	itemValue;
		std::map<std::string, int>	itemValue;
		//std::vector<int>	itemInfo;
		//int				m_itemValue[MAXVALUE];
		//int				m_itemInfo[MAXVALUE];
		int             m_itemCursor;
		Uint32			m_joyDown;
		int				m_joyButton;
		//CDirList		listDir;
		//SDirInfomation 	skinDir;
	    int				r0, g0, b0;
		int				r1, g1, b1;
		langTable		lang;

	public :
		CSystemView();
		~CSystemView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);
		void OnPenUp(SDL_Surface *pScreen, int x, int y);

		void OnPrintSetting(SDL_Surface *pScreen, int x, int y, int type, unsigned long value, bool active);

		void OnCursorLeft(SDL_Surface *pScreen);
		void OnCursorRight(SDL_Surface *pScreen);
		void OnCursorUp(SDL_Surface *pScreen);
		void OnCursorDown(SDL_Surface *pScreen);

		void OnMoveSub_Execute(SDL_Surface *pScreen);
		void OnExecute(SDL_Surface *pScreen);

		ViewMode getMode() { return SYSTEM_VIEW; }

		void loadLang();
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
