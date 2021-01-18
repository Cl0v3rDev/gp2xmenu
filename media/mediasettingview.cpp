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
#include <string.h>
#include <SDL/SDL.h>
#include <inifile.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "mediasettingview.h"
#include "typed.h"
#include "mmsp2_if.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
#define MMSP2_FB_CONTRAST 9
//[*]------------------------------------------------------------------------------------------[*]
CMediaSettingView::CMediaSettingView()
{
	pImgSetting = NULL;
	pImgSettingText = NULL;
	pImgHeightBar = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
CMediaSettingView::~CMediaSettingView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CMediaSettingView::Initialize(bool refresh)
{
	pImgSetting = theApp.LoadImage("media", "setting");
	pImgSettingText = theApp.LoadImage("media", "setting_text");
	pImgHeightBar = theApp.LoadImage("common", "height_bar");

	INI_Open(INI_PATH"/common.ini");
	brightness = INI_ReadInt("display", "brightness", 0);
	contrast = INI_ReadInt("display", "contrast", 0);
	INI_Close();

	RGBLayerMenu(true);
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CMediaSettingView::Exitialize(bool refresh)
{
	if(brightness != -1)
	{
		INI_Open(INI_PATH"/common.ini");
		INI_WriteInt("display", "brightness", brightness);
		INI_WriteInt("display", "contrast", contrast);
		INI_Close();
	}

	SDL_SAFE_FREE(pImgSetting);
	SDL_SAFE_FREE(pImgSettingText);
	SDL_SAFE_FREE(pImgHeightBar);
}
//[*]------------------------------------------------------------------------------------------[*]
void CMediaSettingView::OnDraw(SDL_Surface *pScreen)
{
	SDL_Rect srcrect, dstrect, barrect;

	dstrect.x = (XRES - pImgSetting->w) >> 1;
	dstrect.y = (YRES - pImgSetting->h) >> 1;
	SDL_BlitSurface(pImgSetting, NULL, pScreen, &dstrect);	//셋팅 프레임을 메인에 로딩

	srcrect.w = pImgSettingText->w >> 1;
	srcrect.h = pImgSettingText->h >> 1;
	srcrect.x = cursor == 0 ? 0 : srcrect.w;
	srcrect.y = 0;
	dstrect.x += 10;
	dstrect.y += 11;
	SDL_BlitSurface(pImgSettingText, &srcrect, pScreen, &dstrect);	//셋팅 텍스트 로딩

	barrect.x = dstrect.x + 82 + (173 * contrast / 7) - (pImgHeightBar->w >> 1);
	barrect.y = dstrect.y + 6 - (pImgHeightBar->h >> 1);
	SDL_BlitSurface(pImgHeightBar, NULL, pScreen, &barrect);

	srcrect.w = pImgSettingText->w >> 1;
	srcrect.h = pImgSettingText->h >> 1;
	srcrect.x = cursor == 1 ? 0 : srcrect.w;
	srcrect.y = srcrect.h;
	dstrect.y += 16;
	SDL_BlitSurface(pImgSettingText, &srcrect, pScreen, &dstrect);

	barrect.x = dstrect.x + 82 + (173 * brightness / 13) - (pImgHeightBar->w >> 1);
	barrect.y = dstrect.y + 6 - (pImgHeightBar->h >> 1);
	SDL_BlitSurface(pImgHeightBar, NULL, pScreen, &barrect);

	SDL_UpdateRect(pScreen, (XRES - pImgSetting->w) >> 1, (YRES - pImgSetting->h) >> 1, pImgSetting->w, pImgSetting->h);
}
//[*]------------------------------------------------------------------------------------------[*]
void CMediaSettingView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_UP :
			if (cursor > 0) {
				cursor--;
				OnDraw(pScreen);
			}
			break;
		case VK_DOWN :
			if (cursor < 1) {
				cursor++;
				OnDraw(pScreen);
			}
			break;
		case VK_LEFT :
			switch(cursor) {
				case 0:
					if(contrast > 0)
					{
						contrast--;
						setDisplay();
					}
					break;
				case 1:
					if(brightness > 0)
					{
						brightness--;
						setDisplay();
					}
					break;
			}
			OnDraw(pScreen);
			break;
		case VK_RIGHT :
			switch(cursor) {
				case 0:
					if(contrast < 7)
					{
						contrast++;
						setDisplay();
					}
					break;
				case 1:
					if(brightness < 13)
					{
						brightness++;
						setDisplay();
					}
					break;
			}
			OnDraw(pScreen);
			break;

		case VK_FA :
		case VK_FB :
			theApp.SwapReverse();
			theApp.SetView(MEDIA_PLAY_VIEW);
			break;

		case VK_FX :
			brightness = -1;
			theApp.SwapReverse();
			theApp.SetView(MEDIA_PLAY_VIEW);
			break;

		case VK_START :
			brightness = -1;
			theApp.menuMedia.close();
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMediaSettingView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	int i, w, h, px, py, frame_gap_x, frame_gap_y, frameW, frameH, frameX, frameY, vol_x, px_setting, py_setting;

	px_setting = (XRES - pImgSetting->w) >> 1;
	py_setting = (YRES - pImgSetting->h) >> 1;
	py_setting += 12;
	px_setting += 92;
	py = py_setting + 2 - (pImgHeightBar->h >> 1);
	h = pImgHeightBar->h + 6;
	px = px_setting - (173 / 7 / 2);

	for (i = 0; i < 8; i++) {
		w = (173 * (i + 1) / 7) - (173 * i / 7);
		if ((x >= px) && (x < (px + w)) && (y >= py) && (y < (py + h))) {
			cursor = 0;
			contrast = i;
			setDisplay();
			OnDraw(pScreen);
			break;
		}
		px += w;
	}

	py_setting += 16;
	py = py_setting + 2 - (pImgHeightBar->h >> 1);
	px = px_setting - (173 / 13 / 2);
	for (i = 0; i < 14; i++) {
		w = (173 * (i + 1) / 13) - (173 * i / 13);
		if ((x >= px) && (x < (px + w)) && (y >= py) && (y < (py + h))) {
			cursor = 1;
			brightness = i;
			OnDraw(pScreen);
			break;
		}
		px += w;
	}

	// 'A OK' 버튼 대응
	px = 108;
	py = 132;
	w = 31;
	h = 12;
	if ((x >= px) && (x < (px + w)) && (y >= py) && (y < (py + h)))
	{
		theApp.SwapReverse();
		theApp.SetView(MEDIA_PLAY_VIEW);
		return;
	}
	// 'X CANCEL' 버튼 대응
	px = 166;
	py = 132;
	w = 52;
	h = 12;
	if ((x >= px) && (x < (px + w)) && (y >= py) && (y < (py + h)))
	{
		brightness = -1;
		theApp.SwapReverse();
		theApp.SetView(MEDIA_PLAY_VIEW);
		return;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMediaSettingView::setDisplay()
{
#ifdef GP2X
	if(contrast < 0) contrast = 0;
	else if(contrast > 7) contrast = 7;
	if(brightness < 0) brightness = 0;
	else if(brightness > 13) brightness = 13;

	Msgdummy dummymsg, *pdmsg;
	memset((char *) &dummymsg, 0x00, sizeof(Msgdummy));
	pdmsg = &dummymsg;
	MSG(pdmsg) = MMSP2_FB_CONTRAST;
	LEN(pdmsg) = 0;
	pdmsg->msgdata[0] = contrast;
	pdmsg->msgdata[1] = brightness;
	if (ioctl(SDL_videofd, FBMMSP2CTRL, pdmsg) < 0) {
		printf("MMSP2_FB_CONTRAST error\n");
	}
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
