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
#ifndef __TXTVIEW_H_
#define __TXTVIEW_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
#define	BOTTOM_MARGIN	26
//[*]------------------------------------------------------------------------------------------[*]
typedef struct tagRGBA {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} RGBA;
//[*]------------------------------------------------------------------------------------------[*]
enum EBOOKRENDER
{
	EBOOK_RENDER_INTERNAL = 0,
	EBOOK_RENDER_EXTERNAL
};
//[*]------------------------------------------------------------------------------------------[*]
extern RGBA color_index[10];
//[*]------------------------------------------------------------------------------------------[*]
class CEBookTextView : public ViewForm
{
	private :
		TTF_Font *ttf_font;
		enum EBOOKRENDER RenderOneLine;

		SDL_Surface *pImgMenuButton;
		SDL_Surface *pImgMenuFrame;
		SDL_Surface *pImgMenuFrame_cp;
		SDL_Surface *pImgDirectGo;
		SDL_Surface *pImgMenuFrame_up;
		SDL_Surface *pImgMenuFrame_up_cp;
		SDL_Surface *pImgProgressBar;
		SDL_Surface *pImgHeightBar;
		SDL_Surface *pImgSmallFont;
		SDL_Surface *pImgOptTitle;

		bool ShowStatus;
		unsigned short *cursor;
		bool SeekMode;
		int menuPos;
		double curPercent;


		int	LoadFont(const char *);
		void ChangeFontProcess();

		void DrawChar(SDL_Surface *pScreen, unsigned short code);
		void SetPos(SDL_Surface *pScreen, int x, int y);
		void RenderOneLine_internal_font(SDL_Surface*, int, const unsigned short*);
		void RenderOneLine_external_font(SDL_Surface*, int, const unsigned short*);

		void PutString(SDL_Surface *pScreen, int x, int y, char *str);
		void ShowStatusLine(SDL_Surface *pScreen);

		int drawNumber(SDL_Surface *pScreen, int value, int x, int y, int len,short use);

	public :
		CEBookTextView();
		~CEBookTextView();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnJoystickUp(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);
		void OnPenUp(SDL_Surface *pScreen, int x, int y);

		bool getShowStatus() { return ShowStatus; }

		ViewMode getMode() { return EBOOK_TEXT_VIEW; }
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
