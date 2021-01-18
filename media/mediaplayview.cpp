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
#include "../config.h"
//[*]------------------------------------------------------------------------------------------[*]
#include <inifile.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "mediaplayview.h"
#include "typed.h"
#include "textout.h"
#include "mmsp2_if.h"
//[*]------------------------------------------------------------------------------------------[*]
#define COLORKEY	0xCE79
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
static const int musicItems[3][10] =
	{
		{8, 9, 10, 11, 12, -1, -1, -1, -1, -1},
		{0, 1, 2, 3, 4, 5, 6, 7, -1, -1},
		{102, 101, 100, -1, -1, -1, -1, -1, -1, -1} // reverse
	};
//[*]------------------------------------------------------------------------------------------[*]
static const int movieItems[3][10] =
	{
		{4, 5, 6, 13, 15, -1, -1, -1, -1, -1},
		{0, 1, 2, 3, 7, -1, -1, -1, -1, -1},
		{102, 101, -1, -1, -1, -1, -1, -1, -1, -1}
	};
//[*]------------------------------------------------------------------------------------------[*]
CMediaPlayView::CMediaPlayView()
{
	pImgBackGround = NULL;
	pImgSmallFont = NULL;
	pImgLargeFont = NULL;
	pImgProgressBar = NULL;
	pImgEqualizer = NULL;           // EQ 아이콘 Surface
	pImgStatus = NULL;              // 반복,id3 보기등의 상태 아이콘  Surface
	pImgVolume = NULL;
	pImgVolumeBar=NULL;        // volume 백그라운드 아이콘 Surface
	pImgButton = NULL;
	pImgMenuFrame = NULL;
	pImgSubtitle = NULL;
	pImgHeightBar = NULL;
	pImgDirectGo=NULL;         // e-book,photo,exit 바로가기 아이콘 Surface
	pImgMovieTitle = NULL;
	pImgMovieMenu = NULL;

	menuStatus = false;
	fileOpenStatus = false;
	areaRepeat_play = false;
	hold = false;
	pushShift = false;
	seeking = false;
	seekTime = -1;
	menuItems[0] = NULL;
	menuStatus = false;

	INI_Open(INI_PATH"/common.ini");
	drawInfo = INI_ReadBool("sound", "info", false);
	wantShowLyrics = INI_ReadBool("sound", "lyrics", true);
	INI_Close();
}
//[*]------------------------------------------------------------------------------------------[*]
CMediaPlayView::~CMediaPlayView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CMediaPlayView::Initialize(bool refresh)
{
	// background Load
	pImgBackGround = theApp.LoadImage("media", "body");
	pImgSmallFont = theApp.LoadImage("media", "smallfont");
	pImgLargeFont = theApp.LoadImage("media", "largefont");
	pImgProgressBar = theApp.LoadImage("media", "progressbar");
	pImgEqualizer = theApp.LoadImage("media", "equalizer");
	pImgStatus = theApp.LoadImage("media", "status");
	pImgVolume = theApp.LoadImage("media", "vol");
	pImgVolumeBar = theApp.LoadImage("media", "volbar");
	pImgButton = theApp.LoadImage("media", "button");
	pImgMenuFrame = theApp.LoadImage("media", "menuframe");
	pImgHeightBar = theApp.LoadImage("common", "height_bar");
	pImgDirectGo = theApp.LoadImage("common", "direct_go");

	pImgMovieTitle = theApp.LoadImage("media", "movietitle");
	pImgMovieMenu = theApp.LoadImage("media", "menu");

	pImgSubtitle = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, COLORBIT, REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CMediaPlayView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
	SDL_SAFE_FREE(pImgSmallFont);
	SDL_SAFE_FREE(pImgLargeFont);
	SDL_SAFE_FREE(pImgProgressBar);
	SDL_SAFE_FREE(pImgEqualizer);
	SDL_SAFE_FREE(pImgStatus);
	SDL_SAFE_FREE(pImgVolume);
	SDL_SAFE_FREE(pImgVolumeBar);
	SDL_SAFE_FREE(pImgButton);
	SDL_SAFE_FREE(pImgMenuFrame);
	SDL_SAFE_FREE(pImgSubtitle);
	SDL_SAFE_FREE(pImgHeightBar);
	SDL_SAFE_FREE(pImgDirectGo);
	SDL_SAFE_FREE(pImgMovieTitle);
	SDL_SAFE_FREE(pImgMovieMenu);

	RGBLayerMenu(true);

	INI_Open(INI_PATH"/common.ini");
	INI_WriteBool("sound", "info", drawInfo);
	INI_WriteBool("sound", "lyrics", wantShowLyrics);
	INI_Close();
}
//[*]------------------------------------------------------------------------------------------[*]
int CMediaPlayView::drawSmallNumber(SDL_Surface *pScreen, int value, int x, int y, int len, int type)
{
	int width = 6;
	if (value > 9 || len > 1)
		width += 6;
	if (value > 99 || len > 2)
		width += 6;

	if (pScreen != NULL) {
		SDL_Rect srcrect, dstrect;
		srcrect.y = 0;
		srcrect.w = 6;
		srcrect.h = 9;
		dstrect.x = x + width;
		dstrect.y = y;
		len = width / 6;
		while (len--) {
			dstrect.x -= srcrect.w;
			srcrect.x = (value % 10) * srcrect.w;
			SDL_BlitSurface(pImgSmallFont, &srcrect, pScreen, &dstrect);
			value /= 10;
		}
	}
	return width;
}
//[*]------------------------------------------------------------------------------------------[*]
int CMediaPlayView::drawLargeNumber(SDL_Surface *pScreen, int value, int x, int y, int len, int type)
{
	int width = 10;
	if (value > 9 || len > 1)
		width += 10;
	if (value > 99 || len > 2)
		width += 10;

	if (pScreen != NULL) {
		SDL_Rect srcrect, dstrect;
		srcrect.y = type == 0 ? 0 : 14;
		srcrect.w = 10;
		srcrect.h = 14;
		dstrect.x = x + width;
		dstrect.y = y;
		len = width / 10;
		while (len--) {
			dstrect.x -= srcrect.w;
			srcrect.x = (value % 10) * srcrect.w;
			SDL_BlitSurface(pImgLargeFont, &srcrect, pScreen, &dstrect);
			value /= 10;
		}
	}
	return width;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMediaPlayView::drawPlayStatus(SDL_Surface *pScreen, bool update)
{
	if ((theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE) && !menuStatus && (!seeking || (seekTime == -1))) return;

	SDL_Rect srcrect, dstrect, updaterect;
	int totaltime = theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME);
	int playtime;
	if(seekTime == -1)
		playtime = theApp.menuMedia.getStatus(MEDIA_PLAY_TIME);
	else
	    playtime = seekTime;

	srcrect.w = pImgProgressBar->w;
	srcrect.h = 3;
	srcrect.x = 30;
	srcrect.y = 30;

	updaterect.x = 30;
	updaterect.y = 30;

	SDL_BlitSurface((theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE) ? pImgMovieTitle : pImgBackGround, &srcrect, pScreen, &updaterect);

	if(totaltime == 0) totaltime = 1;
    srcrect.w = playtime * (pImgProgressBar->w) / totaltime;

	if(srcrect.w > 1)
	{
		srcrect.h = pImgProgressBar->h;
		srcrect.x = 0;
		srcrect.y = 0;
		dstrect.x = 30;
		dstrect.y = 31;
		SDL_BlitSurface(pImgProgressBar, &srcrect, pScreen, &dstrect);
	}
	else
	    srcrect.w = 1;

	//프로그래스 바 끝에 붙는 세로 바 출력
	dstrect.x = srcrect.w + 29;
	dstrect.y = 30;
	SDL_BlitSurface(pImgHeightBar, NULL, pScreen, &dstrect);

	if(update) SDL_UpdateRect(pScreen, updaterect.x, updaterect.y, updaterect.w, updaterect.h);

	if ((theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE) && !menuStatus) return;

	int width = drawLargeNumber(NULL, playtime / 60, 0, 0, 2) + drawLargeNumber(NULL, totaltime / 60, 0, 0, 2) + 54;

	if (theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE)
	{
		srcrect.x = 80; srcrect.y = 0; srcrect.w = 120; srcrect.h = 14;
		dstrect.x = ((XRES - pImgMovieMenu->w) >> 1) + srcrect.x;
		dstrect.y = YRES - pImgMovieMenu->h;
		SDL_BlitSurface(pImgMovieMenu, &srcrect, pScreen, &dstrect);
	}
	else
	{
        srcrect.w = 120;
		srcrect.h = 14;
		srcrect.x = (XRES - srcrect.w) >> 1;
		srcrect.y = 120;
		dstrect.x = srcrect.x;
		dstrect.y = srcrect.y;
		SDL_BlitSurface(pImgBackGround, &srcrect, pScreen, &dstrect);
	}

	dstrect.x = (XRES - width) >> 1;
	dstrect.y = theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE ? YRES - 70 : 120;
	dstrect.x += drawLargeNumber(pScreen, playtime / 60, dstrect.x, dstrect.y, 2, 0);
	srcrect.x = 100;
	srcrect.y = 0;
	srcrect.w = 4;
	srcrect.h = 14;
	SDL_BlitSurface(pImgLargeFont, &srcrect, pScreen, &dstrect);
	dstrect.x += srcrect.w;
	dstrect.x += drawLargeNumber(pScreen, (int) playtime % 60, dstrect.x, dstrect.y, 2, 0);
	srcrect.x = 104;
	srcrect.y = 0;
	srcrect.w = 6;
	srcrect.h = 14;
	SDL_BlitSurface(pImgLargeFont, &srcrect, pScreen, &dstrect);
	dstrect.x += srcrect.w;
	dstrect.x += drawLargeNumber(pScreen, totaltime / 60, dstrect.x, dstrect.y, 2, 1);
	srcrect.x = 100;
	srcrect.y = 14;
	srcrect.w = 4;
	srcrect.h = 14;
	SDL_BlitSurface(pImgLargeFont, &srcrect, pScreen, &dstrect);
	dstrect.x += srcrect.w;
	dstrect.x += drawLargeNumber(pScreen, (int) totaltime % 60, dstrect.x, dstrect.y, 2, 1);

	if(update) SDL_UpdateRect(pScreen, (XRES - width) >> 1, dstrect.y, width, 14);

	//if(areaRepeat_play)
	//{
	//	if(	(areaRepeat_st > 0) && (areaRepeat_ed >0) )
	//	{
	//		if(playtime==areaRepeat_ed)
	//			theApp.menuMedia.jumpTime(areaRepeat_st);
	//	}
	//}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMediaPlayView::drawSubtitle(SDL_Surface *pScreen, bool update)
{
	if(theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE)
	{
		Subtitle *subtitle = theApp.menuMedia.getSubtitle();
		if(subtitle == NULL) return;

		SDL_Rect rect;
		rect.x = 10;
		rect.y = ((menuStatus || seeking) ? pImgMovieTitle->h : 0) + 10;
		rect.w = XRES - 20;
		rect.h = YRES - rect.y - (menuStatus ? pImgMovieMenu->h : 0) - 10;

		SDL_FillRect(pScreen, &rect, COLORKEY);
		SDL_SetColorKey(pImgSubtitle, 0, 0);

		bool first = true;

		while(subtitle != NULL)
		{
			int height = subtitle->drawSubtitle(pImgSubtitle, &rect);

			if(height > 0)
			{
				SDL_Rect srcrect, dstrect;
				srcrect.x = rect.x;
				srcrect.y = 0;
				srcrect.w = rect.w;
				srcrect.h = height;

				dstrect.x = rect.x;
				if(first)
				{
					if(height < 30) height = 30;
					dstrect.y = rect.y + rect.h - height;
				}
				else
					dstrect.y = rect.y;

				SDL_BlitSurface(pImgSubtitle, &srcrect, pScreen, &dstrect);

				rect.h -= height;
			}

			if(first)
			{
				subtitle = subtitle->getChain();
				first = false;
			}
			else
			{
				break;
			}
		}

		if(update)
			SDL_UpdateRect(pScreen, 0, 0, 0, 0);
	}
	else if(!menuStatus && wantShowLyrics)
	{
		Subtitle *subtitle = theApp.menuMedia.getSubtitle();
		if(subtitle == NULL) return;

		SDL_Rect rect, srcrect, dstrect;
		rect.x = srcrect.x = dstrect.x = 10;
		rect.y = srcrect.y = dstrect.y = 188;
		rect.w = srcrect.w = XRES - 20;
		rect.h = srcrect.h = YRES - rect.y - 10;

		SDL_BlitSurface(pImgBackGround, &srcrect, pScreen, &dstrect);
		SDL_SetColorKey(pImgSubtitle, SDL_SRCCOLORKEY, COLORKEY);

		int i;
		for(i=0;i<3;i++)
		{
			int height = subtitle->drawSubtitle(pImgSubtitle, &rect, i, false);

			if(height > 0)
			{
				srcrect.x = rect.x;
				srcrect.y = 0;
				srcrect.w = rect.w;
				srcrect.h = height;

				dstrect.x = rect.x;
				dstrect.y = rect.y;

				SDL_BlitSurface(pImgSubtitle, &srcrect, pScreen, &dstrect);

				rect.y += height;
				rect.h -= height;
			}
			else
			{
				rect.y += 14;
				rect.h -= 14;
			}

			if(rect.h < 14) break;
		}

		if(update)
			SDL_UpdateRect(pScreen, 10, 188, XRES - 20, 42);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw 함수
void CMediaPlayView::OnDraw(SDL_Surface *pScreen)
{
	SDL_Rect srcrect, dstrect;
   	unsigned short dest[512];		// 유니코드용 저장 버퍼
	int r, g, b;

	if( ((theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE) && (menuItems[0] != movieItems[0])) ||
	    ((theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MUSIC) && (menuItems[0] != musicItems[0])) )
	{
		int i;
		for(i=0; i<3; i++)
			menuItems[i] = (theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE) ? movieItems[i] : musicItems[i];

		menuRow = 0;
		menuCol = 0;
		menuPos = menuItems[menuRow][menuCol];
	}

	if (theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE)
	{
		SDL_FillRect(pScreen, NULL, COLORKEY);
		if(!seeking && !menuStatus)
		{
            RGBLayerMenu(false);
			drawSubtitle(pScreen, false);
			SDL_UpdateRect(pScreen, 0, 0, 0, 0);
			return;
		}
		RGBLayerMenu(true);
		SDL_BlitSurface(pImgMovieTitle, NULL, pScreen, NULL);
	}
	else
	{
		SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);
		drawPlayStatus(pScreen, false);  //playtime,totaltime 부분 찍음,실시간 갱신되는 부분

		int bps = theApp.menuMedia.getStatus(MEDIA_BIT_RATE);
		int rate = theApp.menuMedia.getStatus(MEDIA_SAMPLE_RATE);
		int width = drawSmallNumber(NULL, rate, 0, 0) + drawSmallNumber(NULL, bps / 1000, 0, 0) + 20 + 4 + 27;

		dstrect.x = (XRES - width) >> 1; dstrect.y = 97;

		dstrect.x += drawSmallNumber(pScreen, bps / 1000, dstrect.x, dstrect.y); // Bitrate
		srcrect.x = 87; srcrect.y = 0; srcrect.w = 27; srcrect.h = 9; // KBPS
		SDL_BlitSurface(pImgSmallFont, &srcrect, pScreen, &dstrect);
		dstrect.x += srcrect.w;
		srcrect.x = 63; srcrect.y = 0; srcrect.w = 4; srcrect.h = 9; // /(Slash)
		SDL_BlitSurface(pImgSmallFont, &srcrect, pScreen, &dstrect);
		dstrect.x += srcrect.w;
		dstrect.x += drawSmallNumber(pScreen, rate, dstrect.x, dstrect.y); // Samplerate
		srcrect.x = 67; srcrect.y = 0; srcrect.w = 20; srcrect.h = 9;  //KHZ
		SDL_BlitSurface(pImgSmallFont, &srcrect, pScreen, &dstrect);

		// Play Status(Stop,Play,Pause)
		srcrect.x = 0;
		srcrect.w = 72;
		srcrect.h = pImgButton->h / 18;
		dstrect.x = (XRES - srcrect.w) >> 1;
		dstrect.y = 140;

		if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) == MEDIA_PLAY)
			srcrect.y=(pImgButton->h/18)*6;
		else if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) == MEDIA_PAUSE)
			srcrect.y=(pImgButton->h/18)*7;
		else if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) == MEDIA_STOP)
			srcrect.y=(pImgButton->h/18)*8;

		SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);

		// option status
		int channel = theApp.menuMedia.getStatus(MEDIA_CHANNEL);
		int repeat = theApp.menuMedia.getStatus(MEDIA_REPEAT_MODE);
		int eq = theApp.menuMedia.getStatus(MEDIA_EQUALIZER);

		srcrect.w = pImgStatus->w / 2;
		srcrect.h = pImgStatus->h / 4;
		width = srcrect.w + srcrect.w-1 + pImgEqualizer->w-4 + 43 + 43 - 8;

		// repeat
		srcrect.x = (repeat != 0 && repeat != 3) ? srcrect.w : 0; srcrect.y = srcrect.h * 0;
		dstrect.x = (XRES - width) >> 1;
		dstrect.y = 150;
		SDL_BlitSurface(pImgStatus, &srcrect, pScreen, &dstrect);

		// info
		srcrect.x = drawInfo ? srcrect.w : 0; srcrect.y = srcrect.h * 1;
		dstrect.x = dstrect.x + srcrect.w; dstrect.y = 150;
		SDL_BlitSurface(pImgStatus, &srcrect, pScreen, &dstrect);

		// equalizer
		dstrect.x = dstrect.x + srcrect.w-1; dstrect.y = 150;
		srcrect.w = pImgEqualizer->w;
		srcrect.h = pImgEqualizer->h / 11;
		srcrect.x = 0; srcrect.y = srcrect.h * eq;
		SDL_BlitSurface(pImgEqualizer, &srcrect, pScreen, &dstrect);

		// shuffle
		srcrect.x = (repeat == 2 || repeat ==3) ? 43 : 0; srcrect.y = srcrect.h * 2;
		dstrect.x = dstrect.x + srcrect.w-4; dstrect.y = 150;
		srcrect.w = 43;
		SDL_BlitSurface(pImgStatus, &srcrect, pScreen, &dstrect);

		// lyric
		srcrect.x = (wantShowLyrics) ? 43 : 0; srcrect.y = srcrect.h * 3;
		dstrect.x = dstrect.x + srcrect.w; dstrect.y = 150;
		srcrect.w = 43;
		SDL_BlitSurface(pImgStatus, &srcrect, pScreen, &dstrect);


	   	if(drawInfo) {   //id3tag TRUE
			const char* album = theApp.menuMedia.getInfo(MEDIA_ALBUM);
			theApp.FontColor("media", "album", r, g, b);
			if(album) {
				toUnicode("UTF-8", album, dest, strlen(album));
				DrawTextOut(pScreen, 23, 64, 285, dest, r, g, b);
				dstrect.y = 50;
			} else {
				dstrect.y = 64;
			}

			const char* title = theApp.menuMedia.getInfo(MEDIA_TITLE);
			theApp.FontColor("media", "title", r, g, b);
			if(title) {
				toUnicode("UTF-8", title, dest, strlen(title));
				DrawTextOut(pScreen, 23, dstrect.y, 285, dest, r, g, b);
			}
		}
	}

	const char* filename = theApp.menuMedia.getInfo(MEDIA_FILE_NAME);
	theApp.FontColor("media", "name", r, g, b);
#ifdef WIN32
	toUnicode(NULL, filename, dest, strlen(filename));
#else
	toUnicode("UTF-8", filename, dest, strlen(filename));
#endif
	DrawTextOut(pScreen, 32, 10, 280-30, dest, r, g, b);

	//배터리 아이콘 로딩 시작
	theApp.getStatusRect(&dstrect);
	theApp.drawStatusIcon(pScreen, &dstrect);

	if(menuStatus)
	{
		int i, j;
		SDL_Rect srcrect, dstrect;
		SDL_Surface *pFrame = (theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE) ? pImgMovieMenu : pImgMenuFrame;

		//메뉴 프레임을 메인 배경에 띄움
		dstrect.x = (XRES - pFrame->w) >> 1;dstrect.y = YRES - pFrame->h;
		SDL_BlitSurface(pFrame, NULL, pScreen, &dstrect);

		int vol = theApp.menuMedia.getStatus(MEDIA_VOLUME) / 5;
		if(vol < 0) vol = 0;
		else if(vol > 19) vol = 20;

		dstrect.x = XRES - 30 - pImgVolume->w;
		dstrect.y = YRES - 40 - pImgVolume->h;
		srcrect.w = pImgVolume->w + 5;
		srcrect.h = pImgVolume->h;
		srcrect.x = dstrect.x - ((XRES - pFrame->w) >> 1);
		srcrect.y = pFrame->h - 40 - pImgVolume->h;
		SDL_BlitSurface(pFrame, &srcrect, pScreen, &dstrect);
		SDL_BlitSurface(pImgVolume, NULL, pScreen, &dstrect);

		dstrect.x = XRES - 30 - (21 - vol) * 3 - ((pImgVolumeBar->w - 3) >> 1);
		dstrect.y = YRES - 40 - pImgVolumeBar->h;
		SDL_BlitSurface(pImgVolumeBar, NULL, pScreen, &dstrect);

		for(i=0; i<3; i++)
		{
			const int *items = menuItems[i];
			int step;
			if (i == 2)
			{
				srcrect.w = pImgDirectGo->w / 5;
				srcrect.h = pImgDirectGo->h / 2;
				dstrect.x = ((XRES - pFrame->w) >> 1) + 260;
				dstrect.y = YRES - 34;
				step = -38;
			}
			else
			{
				srcrect.w = 15;
				srcrect.h = 15;
				dstrect.x = ((XRES - pFrame->w) >> 1) + 6;
				dstrect.y = (YRES - 44) + (22 * i);
				step = 22;
			}

			for(j=0; *items != -1; ++items, ++j)
			{
				srcrect.x = srcrect.w * (*items % 100);
				srcrect.y = 0;
				if(menuPos == *items)
				{
					srcrect.y = srcrect.h;
					menuRow = i;
					menuCol = j;
				}
				if(*items < 100)
					SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);
				else
					SDL_BlitSurface(pImgDirectGo, &srcrect, pScreen, &dstrect);
				dstrect.x += step;
			}
		}

		//메뉴 버튼 캡션 아이콘 출력
		if(menuPos < 100)
		{
			srcrect.w = 72;
			srcrect.h = 15;
			srcrect.x = 0;
			srcrect.y = 30 + menuPos * 15;

			dstrect.x = ((XRES - pFrame->w) >> 1) + 113;
			dstrect.y = YRES - 44;

			SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);
		}

		if(theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE) drawPlayStatus(pScreen, false);
	}

	drawSubtitle(pScreen, false);

	SDL_UpdateRect(pScreen, 0, 0, 0, 0);
}
//[*]------------------------------------------------------------------------------------------[*]
int CMediaPlayView::action(int cmd)
{
	int repeat = theApp.menuMedia.getStatus(MEDIA_REPEAT_MODE);
	int chgValue = 0;
	int result = UPDATE_NONE;
	if(cmd == -1) cmd = menuPos;

	switch(cmd)
	{
		case 0: //이전 구간
			if(seeking)
			{
				if(seekTime == -1)
				{
				    seekTime = theApp.menuMedia.getStatus(MEDIA_PLAY_TIME);
				    seekStep = theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME) / 100;
				    if(seekStep < 5) seekStep = 5;
					result = UPDATE_FULL;
				}
				else
				{
					seekTime -= seekStep;
					if(seekTime < 0) seekTime = 0;
					result = UPDATE_STATUS;
				}
			}
			else
			{
				seeking = true;
			    seekTime = -1;
			}
			break;
		case 1: //다음 구간
			if(seeking)
			{
				if(seekTime == -1)
				{
				    seekTime = theApp.menuMedia.getStatus(MEDIA_PLAY_TIME);
				    seekStep = theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME) / 100;
				    if(seekStep < 5) seekStep = 5;
					result = UPDATE_FULL;
				}
				else
				{
					seekTime += seekStep;
					if(seekTime > theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME))
						seekTime = theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME);
					result = UPDATE_STATUS;
				}
			}
			else
			{
				seeking = true;
			    seekTime = -1;
			}
			break;

		case 2: //이전 곡
			theApp.menuMedia.prev();
			result = UPDATE_FULL;
			break;
		case 3: //다음 곡
			theApp.menuMedia.next();
			result = UPDATE_FULL;
			break;

		case 4: //Play
			theApp.menuMedia.stop();
			if(theApp.menuMedia.load())
				theApp.menuMedia.play();
			else
				theApp.menuMedia.next();
			result = UPDATE_FULL;
			break;
		case 5: //Pause
			theApp.menuMedia.pause();
			result = UPDATE_FULL;
			break;

		case 6: //Stop
			theApp.menuMedia.stop();
			result = UPDATE_FULL;
			break;

		case 7: //Open
			theApp.SwapReverse();
			if(theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE)
				theApp.SetView(MOVIE_VIEW);
			else
				theApp.SetView(MUSIC_VIEW);
			break;

		case 8: //repeat
			if(repeat==0)
				chgValue=1;
			else if(repeat==1)
				chgValue=0;
			else if(repeat==2)
				chgValue=3;
			else if(repeat==3)
				chgValue=2;

			theApp.menuMedia.setStatus(MEDIA_REPEAT_MODE, chgValue);
			result = UPDATE_FULL;
			break;

		case 9: //ID3 Tag
			drawInfo = !drawInfo;
			result = UPDATE_FULL;
			break;

		case 10: //EQ
			theApp.menuMedia.setStatus(MEDIA_EQUALIZER, -1);
			result = UPDATE_FULL;
			break;

		case 11: //Shuffle
			if(repeat==0)
				chgValue=3;
			else if(repeat==1)
				chgValue=2;
			else if(repeat==2)
				chgValue=1;
			else if(repeat==3)
				chgValue=0;

			theApp.menuMedia.setStatus(MEDIA_REPEAT_MODE, chgValue);
			result = UPDATE_FULL;
			break;

		case 12: //lylic
			wantShowLyrics = !wantShowLyrics;
			result = UPDATE_FULL;
			break;

		case 13: // display setting
			theApp.SetView(MEDIA_SET_VIEW);
		    break;

		case 14: // A-B repeat
		    break;

		case 15: // zoom
		    YUVLayerZoomMode(-1);
			break;

		case 100: //Photo
			if(theApp.photovs == ON_STATUS)
				theApp.SetView(PHOTO_IMAGE_VIEW);
			else
				theApp.SetView(PHOTO_VIEW);
			break;

		case 101: //EBook
			if(theApp.textvs == TEXT_ON_STATUS)
				theApp.SetView(EBOOK_TEXT_VIEW);
			else
				theApp.SetView(EBOOK_VIEW);
			break;

		case 102: //EXIT
			theApp.menuMedia.stop();
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}

	return result;
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CMediaPlayView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	//printf("*********musicplayview.cpp의 OnJoystickDown()\n");
	if (seeking && (seekButton != button))
	{
		seeking = false;
		if(seekTime != -1)
		    theApp.menuMedia.jump(seekTime);
		seekTime = -1;
	    OnDraw(pScreen);
	}

	if (button == VK_FA)
	{
		pushShift = true;
		useShortcuts = false;
		return;
	}
	else if (pushShift)
	{
		if(useShortcuts)
			return;

		if(button == VK_FL)
		{
			if(hold == true)
			{
				hold = false;
				OnDraw(pScreen);
				theApp.GP2XControl(LCD_ON);
				usleep(100000);
			}
			else
			{
				hold = true;
				if(theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MUSIC)
					theApp.GP2XControl(LCD_OFF);
				usleep(100000);
			}

			useShortcuts = true;
		}
		return;
	}

	if (hold) return;

	switch(button)
	{
		case VK_UP 	  : 	// 상 방향키
			if(menuStatus)
			{
				if((menuRow == 1) && (menuItems[0][menuCol] != -1))
				{
					menuRow = 0;
					menuPos = menuItems[menuRow][menuCol];
					OnDraw(pScreen);
				}
			}
			break;

		case VK_DOWN  :     // 하 방향키
			if(menuStatus)
			{
				if((menuRow == 0) && (menuItems[1][menuCol] != -1))
				{
					menuRow = 1;
					menuPos = menuItems[menuRow][menuCol];
					OnDraw(pScreen);
				}
			}
			break;

		case VK_LEFT  :     // 좌 방향키
			if(pushShift)
			{
				//if(areaRepeat_set)
				//	areaRepeat_st=theApp.menuMedia.getStatus(MEDIA_PLAY_TIME);
				//else
				//	theApp.menuMedia.setStatus(MEDIA_REPEAT_MODE, -1);
			}
			else
			{
				if(menuStatus)
				{
					if(menuRow == 2)
					{
					    menuCol++;
					    if(menuItems[menuRow][menuCol] == -1)
					    {
							menuRow = 1;
							menuCol = 0;
						    while(menuItems[menuRow][menuCol + 1] != -1) menuCol++;
						}
					}
					else if(menuCol > 0)
					    menuCol--;
					else if(menuRow == 0)
					    while(menuItems[menuRow][menuCol + 1] != -1) menuCol++;
					else
						menuRow = 2;
					menuPos = menuItems[menuRow][menuCol];
					OnDraw(pScreen);
				}
				else
				{
					if(seeking)
					{
						if(seekTime == -1)
						{
						    seekTime = theApp.menuMedia.getStatus(MEDIA_PLAY_TIME);
						    seekStep = theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME) / 100;
						    if(seekStep < 5) seekStep = 5;
						    OnDraw(pScreen);
						}
						else
						{
							seekTime -= seekStep;
							if(seekTime < 0) seekTime = 0;
							drawPlayStatus(pScreen);
						}
					}
					else
					{
						seeking = true;
						seekButton = button;
					    seekTime = -1;
					}
					//theApp.menuMedia.PrevSeek();
				}
			}
			break;

		case VK_RIGHT :     // 우 방향키
			if(menuStatus) {
				if(menuRow == 2)
				{
					if(menuCol > 0)
					    menuCol--;
					else
				    {
						menuRow = 1;
						menuCol = 0;
					}
				}
				else
				{
					menuCol++;
					if(menuItems[menuRow][menuCol] == -1)
					{
						if(menuRow == 0)
						    menuCol = 0;
						else
						{
							menuRow = 2;
							menuCol = 0;
						    while(menuItems[menuRow][menuCol + 1] != -1) menuCol++;
						}
					}
				}
				menuPos = menuItems[menuRow][menuCol];
				OnDraw(pScreen);
			}
			else
			{
				if(seeking)
				{
					if(seekTime == -1)
					{
					    seekTime = theApp.menuMedia.getStatus(MEDIA_PLAY_TIME);
					    seekStep = theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME) / 100;
					    if(seekStep < 5) seekStep = 5;
					    OnDraw(pScreen);
					}
					else
					{
						seekTime += seekStep;
						if(seekTime > theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME))
							seekTime = theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME);
						drawPlayStatus(pScreen);
					}
				}
				else
				{
					seeking = true;
					seekButton = button;
				    seekTime = -1;
				}
			}
			break;

		case VK_TAT	  :     // TAT 버튼
		case VK_FB	  :     // B 버튼
			//if(areaRepeat_set)
			//{
			//	areaRepeat_ed=theApp.menuMedia.getStatus(MEDIA_PLAY_TIME);
			//}
			//else
			//{
				if(menuStatus)
				{
					switch(action())
					{
						case UPDATE_FULL:
							OnDraw(pScreen);
							break;
						case UPDATE_STATUS:
							drawPlayStatus(pScreen);
							break;
					}
					if(seeking && (seekButton != button))
						seekButton = button;
				}
				else if(button == VK_TAT)
					theApp.menuMedia.pause();
			//}
			break;

		case VK_FY  :     	// Y 버튼
			//if(areaRepeat_set)
			//{
			//	areaRepeat_play=!areaRepeat_play;

			//	if(areaRepeat_play)
			//	{
			//		if(	(areaRepeat_st > 0) && (areaRepeat_ed >0) )
			//		{
			//			theApp.menuMedia.jumpTime(areaRepeat_st);
			//		}

			//	}
			//	else  //구간 반복 해지
			//	{
			//		areaRepeat_st=areaRepeat_ed=0;

			//	}

			//}
			//else
			//{
				menuStatus = !menuStatus;
			//}
			OnDraw(pScreen);
			break;

		case VK_FX  :     	// X 버튼
			theApp.SwapReverse();
			if(theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE)
				theApp.SetView(MOVIE_VIEW);
			else
				theApp.SetView(MUSIC_VIEW);
			break;

		case VK_FL	:
            if(theApp.menuMedia.getStatus(MEDIA_PLAY_TIME) < 5)
				theApp.menuMedia.prev();
			else
				theApp.menuMedia.jump(0);
			OnDraw(pScreen);
			break;

		case VK_FR	:
			theApp.menuMedia.next();
			OnDraw(pScreen);
			break;

		case VK_VOL_UP :
			theApp.menuMedia.setStatus(MEDIA_VOLUME, MEDIA_VOLUME_UP);
			OnDraw(pScreen);
			break;

		case VK_VOL_DOWN :
			theApp.menuMedia.setStatus(MEDIA_VOLUME, MEDIA_VOLUME_DOWN);
			OnDraw(pScreen);
			break;

		case VK_START :
			theApp.menuMedia.close();
			theApp.SwapReverse();
			if(theApp.photovs == ON_STATUS)
				theApp.SetView(PHOTO_IMAGE_VIEW);
			else if(theApp.textvs == TEXT_ON_STATUS)
				theApp.SetView(EBOOK_TEXT_VIEW);
			else
				theApp.SetView(MAIN_VIEW);
			break;

		case VK_SELECT :
			RGBLayerMenu(true);
			switch(theApp.lastTask)
			{
				case 0:
					return;

				case 1: //go to photo
					if(theApp.photovs == ON_STATUS)
						theApp.SetView(PHOTO_IMAGE_VIEW);
					else
						theApp.SetView(PHOTO_VIEW);
					break;

				case 2: //go to ebook
					if(theApp.textvs == TEXT_ON_STATUS)
						theApp.SetView(EBOOK_TEXT_VIEW);
					else
						theApp.SetView(EBOOK_VIEW);
					break;
			}
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 업 처리
void CMediaPlayView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_LEFT:
		case VK_RIGHT:
		case VK_TAT:
		case VK_FB:
			if(seeking && (seekButton == button))
			{
				if(seekTime == -1)
				    if((seekButton == VK_LEFT) || (menuStatus && (menuPos == 0)))
						theApp.menuMedia.seek(-5);
					else
						theApp.menuMedia.seek(5);
				else
				    theApp.menuMedia.jump(seekTime);
			}
			seeking = false;
			seekTime = -1;
			if(!menuStatus && (theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE))
			    OnDraw(pScreen);
			break;
		case VK_FA :
			areaRepeat_set = false;
			pushShift = false;
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMediaPlayView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	if (hold) return;
	
	SDL_Rect rect;
	bool isMovie = (theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE);

	if(!isMovie)
	{
		// option status
		rect.w = pImgStatus->w - 1 + pImgEqualizer->w-4 + 43 + 43 - 8;
		rect.h = pImgStatus->h / 4;
		rect.x = (XRES - rect.w) >> 1;
		rect.y = 150;

		if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
		{
			int result = UPDATE_NONE;

			rect.w = pImgStatus->w >> 1;
			if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
				result = action(8); //MEDIA_REPEAT

			rect.x += rect.w;
			if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
			    result = action(9); //MEDIA_INFO

			rect.x += rect.w - 1;
			rect.w = pImgEqualizer->w;
			if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
			    result = action(10); //MEDIA_EQUALIZER

			rect.x += rect.w - 4;
			rect.w = 43;
			if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
			    result = action(11); //MEDIA_SHUFFLE

			rect.x += rect.w;
			if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
			    result = action(12); //MEDIA_LYRIC

			switch(result)
			{
				case UPDATE_FULL:
					OnDraw(pScreen);
					break;
				case UPDATE_STATUS:
					drawPlayStatus(pScreen);
					break;
			}
			return;
		}
	}

	//재생 바 바로가기 대응
	rect.w = pImgProgressBar->w;
	rect.h = 7+3+5;
	rect.y = 31-7;
	rect.x = 30;

	if(!menuStatus && ( isMovie || (y > (rect.y + rect.h)) ))
	{
		menuStatus = true;
		OnDraw(pScreen);
		return;
	}
	else if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
	{
		int totaltime = theApp.menuMedia.getStatus(MEDIA_TOTAL_TIME);
		int jumptime = totaltime * (x - rect.x) / rect.w;
		theApp.menuMedia.jump(jumptime);
		return;
	}

	if(y < (rect.y + rect.h)) return;

	//메뉴 프레임 위치
	SDL_Surface *pFrame = isMovie ? pImgMovieMenu : pImgMenuFrame;
	rect.w = pFrame->w;
	rect.h = pFrame->h;
	rect.x = (XRES - rect.w) >> 1;
	rect.y = YRES - rect.h;

	if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
	{
		int i;

		for(i=0; i<3; i++)
		{
			const int *items = menuItems[i];
			int step;
			if (i == 2)
			{
				rect.x = ((XRES - pFrame->w) >> 1) + 260;
				rect.y = YRES - 34;
				rect.w = pImgDirectGo->w / 5;
				rect.h = pImgDirectGo->h / 2;
				step = -38;
			}
			else
			{
				rect.x = ((XRES - pFrame->w) >> 1) + 6;
				rect.y = (YRES - 44) + (22 * i);
				rect.w = 15;
				rect.h = 15;
				step = 22;
			}

			while(*items != -1)
			{
				if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
				{
					menuPos = *items;
					switch(action())
					{
						case UPDATE_FULL:
							OnDraw(pScreen);
							break;
						case UPDATE_STATUS:
							drawPlayStatus(pScreen);
							break;
					}
					return;
				}
				rect.x += step;
				++items;
			}
		}

		//볼륨영역 대응
		rect.w = pImgVolumeBar->w;
		rect.h = pImgVolumeBar->h;
		rect.x = XRES - 30 - (21 * 3) - ((rect.w - 3) >> 1);
		rect.y = YRES - 40 - rect.h;

		for(i=0;i<21;i++, rect.x += 3)
		{
			if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
			{
				theApp.menuMedia.setStatus(MEDIA_VOLUME, i * 5);
				OnDraw(pScreen);
				return;
			}
		}
	}
	else
	{
		menuStatus = false;
		OnDraw(pScreen);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMediaPlayView::OnEvent(SDL_Surface *pScreen, int code, void *data1, void *data2)
{
	switch (code) {
		case EVENT_UPDATE:
			drawPlayStatus(pScreen);
			break;
		case EVENT_SUBTITLE_CHANGE:
			drawSubtitle(pScreen);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
