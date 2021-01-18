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
#include "config.h"
//[*]------------------------------------------------------------------------------------------[*]
#include <fcntl.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "touchview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
#define OFFSET 30
//[*]------------------------------------------------------------------------------------------[*]
typedef struct {
	unsigned short x;
	unsigned short y;
} TS_EVENT;
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CTouchView::CTouchView()
{
	pImgBackGround = NULL;
	pImgStart = NULL;
	pImgEnd = NULL;
	handle = -1;
}
//[*]------------------------------------------------------------------------------------------[*]
CTouchView::~CTouchView()
{
	Exitialize();

#ifdef GP2X
	if(handle != -1)
	{
		close(handle);
		handle = -1;
	}
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CTouchView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("touch", "body");
	pImgStart = theApp.LoadImage("touch", "start");
	pImgEnd = theApp.LoadImage("touch", "end");

#ifdef GP2X
	if (handle == -1)
		handle = open("/dev/touchscreen/wm97xx",O_RDWR|O_NDELAY);
#endif

	if (!refresh)
	{
		calibration = 1;
#ifdef GP2X
		SDL_ShowCursor(SDL_DISABLE);
#endif
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CTouchView::Exitialize(bool refresh)
{
#ifdef GP2X
	SDL_ShowCursor(SDL_DISABLE);
#endif
	SDL_SAFE_FREE(pImgBackGround);
	SDL_SAFE_FREE(pImgStart);
	SDL_SAFE_FREE(pImgEnd);

#ifdef GP2X
	if(handle != -1)
	{
		close(handle);
		handle = -1;
	}
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw 함수
void CTouchView::OnDraw(SDL_Surface *pScreen)
{
	SDL_Rect r, dstRect;
	SDL_Surface *surface;
	SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	switch(calibration)
	{
		case 1:
			r.x = OFFSET - 5;
			r.y = OFFSET - 5;
			r.w = r.h = 10;
			surface = pImgStart;
			break;
		case 2:
			r.x = (319 - OFFSET) - 5;
			r.y = (239 - OFFSET) - 5;
			r.w = r.h = 10;
			surface = pImgStart;
			break;
		default:
			r.w = 0;
			r.h = 0;
			surface = pImgEnd;
			break;
	}

	dstRect.x = (320 - surface->w) >> 1;
	dstRect.y = (240 - surface->h) >> 1;
	SDL_BlitSurface(surface, NULL, pScreen, &dstRect);

	if((r.w) && (r.h))
	{
		SDL_FillRect(pScreen, &r, next_calibration ? 0xF800 : 0xFFFF);
	}

	SDL_UpdateRect(pScreen, 0, 0, 320, 240);
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CTouchView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_FX : //Retry
#ifdef GP2X
			SDL_ShowCursor(SDL_DISABLE);
#endif
		    calibration = 1;
		    OnDraw(pScreen);
		    break;
		case VK_FB :
			theApp.SwapReverse();
			theApp.SetView(SETTING_VIEW);
			break;
		case VK_START : // Main View로 이동한다.
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CTouchView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
}
//[*]------------------------------------------------------------------------------------------[*]
void CTouchView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	if (!calibration)
		return;

	TS_EVENT ts_event[10];
	int nread;
#ifdef GP2X
	nread = read(handle,(char *)&ts_event,sizeof(ts_event));
#else
    nread = sizeof(ts_event);
#endif

	if(nread == sizeof(ts_event))
	{
		//printf("touch down\n");
		int x = 0, y = 0, i;
		int count = (nread >> 2);

		for (i = 0; i < count; i++)
		{
			x += ts_event[i].x;
			y += ts_event[i].y;
		}

		switch(calibration)
		{
			case 1:
				left = x / count;
				top = y / count;
				break;
			case 2:
				right = x / count;
				bottom = y / count;
				break;
		}

		next_calibration = 1;
		OnDraw(pScreen);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CTouchView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{
	if (!next_calibration)
		return;
	next_calibration = 0;

	switch(calibration)
	{
		case 1:
			calibration = 2;
			OnDraw(pScreen);
			break;
		case 2:
			{
				int a, b, c, d, e, f, s;
				FILE *fp;

				s = 1 << 16;

				a = s * (OFFSET - (319 - OFFSET)) / (left - right);
				b = 0;
				c = s * OFFSET - a * left;

				d = 0;
				e = s * (OFFSET - (239 - OFFSET)) / (top - bottom);
				f = s * OFFSET - e * top;

				if (handle != -1)
				{
					fp = fopen("/etc/pointercal", "w");
					if(fp) {
						fprintf(fp, "%d %d %d %d %d %d %d", a, b, c, d, e, f, s);
						fclose(fp);
					}
				}

				calibration = 0;
				OnDraw(pScreen);
				SDL_ShowCursor(SDL_ENABLE);
				break;
			}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
