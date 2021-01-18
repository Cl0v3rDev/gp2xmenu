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
#include <string.h>
#include <math.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "textout.h"
#include "photoimageview.h"
#include "photoviewer.h"

#include "fileload.h"
#include "jpegload.h"
//[*]------------------------------------------------------------------------------------------[*]
static const int menuItems[3][10] =
	{
		{5, 6, 7, 8, 10, -1, -1, -1, -1, -1},
		{0, 1, 2, 3, 4, 9, 11, -1, -1, -1},
		{102, 103, -1, -1, -1, -1, -1, -1, -1, -1} // reverse
	};
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
// SDL 초기화
//[*]------------------------------------------------------------------------------------------[*]
CPhotoImageView::CPhotoImageView()
{
	//pImgBackGround = NULL;
	//pImgButton = NULL;

	pImgMenuButton = NULL;
	pImgMenuFrame = NULL;
	pImgMenuFrame_cp = NULL;
	pImgDirectGo = NULL;
	pImgMenuFrame_up = NULL;
	pImgMenuFrame_up_cp = NULL;
	//pImgProgressBar = NULL;
	//pImgHeightBar = NULL;
	pImgOptTitle =NULL;

	menuStatus = false;
}
//[*]------------------------------------------------------------------------------------------[*]
CPhotoImageView::~CPhotoImageView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CPhotoImageView::Initialize(bool refresh)
{
	//pImgBackGround = theApp.LoadImage("photo", "menu");
	//pImgButton = theApp.LoadImage("photo", "button");


	pImgMenuButton = theApp.LoadImage("photo", "pt_menu_button");
	pImgMenuFrame = theApp.LoadImage("photo", "pt_menu_frame");
	pImgMenuFrame_cp = theApp.LoadImage("photo", "pt_menu_frame");
	pImgDirectGo = theApp.LoadImage("common", "direct_go");
	pImgMenuFrame_up = theApp.LoadImage("photo", "pt_menu_frame_up");
	pImgMenuFrame_up_cp = theApp.LoadImage("photo", "pt_menu_frame_up");
	//pImgProgressBar = theApp.LoadImage("photo", "progressbar");
	//pImgHeightBar = theApp.LoadImage("common", "height_bar");
	pImgOptTitle = theApp.LoadImage("photo", "pt_opt_title");

	theApp.lastTask = 1;
	if(theApp.menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MOVIE)
	{
		// same area using photo & movie buffer, movie stop.
		theApp.menuMedia.close();
	}

	if(!refresh) {
		menuStatus = false;
		theApp.AddTimer(30);
		theApp.menuPhoto.setSetting(PHOTO_SET_MOVE, -1);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CPhotoImageView::Exitialize(bool refresh)
{
	if(!refresh) {
		//theApp.menuPhoto.Break();
		theApp.RemoveTimer();
	}
	//SDL_SAFE_FREE(pImgBackGround);
	//SDL_SAFE_FREE(pImgButton);

	SDL_SAFE_FREE(pImgMenuButton);
	SDL_SAFE_FREE(pImgMenuFrame);
	SDL_SAFE_FREE(pImgMenuFrame_cp);
	SDL_SAFE_FREE(pImgDirectGo);
	SDL_SAFE_FREE(pImgMenuFrame_up);
	SDL_SAFE_FREE(pImgMenuFrame_up_cp);
	//SDL_SAFE_FREE(pImgProgressBar);
	//SDL_SAFE_FREE(pImgHeightBar);
	SDL_SAFE_FREE(pImgOptTitle);
}
//[*]------------------------------------------------------------------------------------------[*]
// 타이머 처리 함수
void CPhotoImageView::OnTimerProc(SDL_Surface *pScreen)
{
	if(theApp.menuPhoto.Moving())
		OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// 화면에 이미지를 표시한다.
// ImageViewMode 에 따라 화면에 꽉찬 모드(PHOTO_FIT_VIEW), 혹은 임의 배율 모드(PHOTO_SCALE_VIEW)을 설정한다.
//[*]------------------------------------------------------------------------------------------[*]
void CPhotoImageView::OnDraw(SDL_Surface *pScreen)
{
	theApp.menuPhoto.Draw(pScreen);
    if(menuStatus)
    {
		int i, j;
    	SDL_Rect srcrect,dstrect;
    	bool drawButton[12];
    	int viewMode = theApp.menuPhoto.getSetting(PHOTO_VIEW_MODE);
		unsigned short dest[512];

    	memset(drawButton, 0, sizeof(drawButton));
        //SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

    	//if( theApp.menuPhoto.isComicMode() && ((menuPos == 3) || (menuPos == 4)) ) menuPos = 9;  //원래는 5;

        //drawButton[menuPos] = true;
        //if(viewMode == PHOTO_COMIC_LEFT_VIEW) drawButton[9] = true;//원래는 drawButton[5] = true;
        //if(viewMode == PHOTO_COMIC_RIGHT_VIEW) drawButton[10] = true;//원래는 drawButton[6] = true;
        //if(viewMode == PHOTO_FIT_VIEW) drawButton[7] = true; //f-200에서는 fit 모드가 빠짐?

		dstrect.x = (XRES - pImgMenuFrame->w) >> 1;
		dstrect.y = YRES - pImgMenuFrame->h;
		SDL_BlitSurface(pImgMenuFrame, NULL, pScreen, &dstrect);

		for(i=0; i<3; i++)
		{
			const int *items = menuItems[i];
			int step;
			if (i == 2)
			{
				srcrect.w = pImgDirectGo->w / 5;
				srcrect.h = pImgDirectGo->h / 2;
				dstrect.x = ((XRES - pImgMenuFrame->w) >> 1) + 260;
				dstrect.y = YRES - 34;
				step = -38;
			}
			else
			{
				srcrect.w = 15;
				srcrect.h = 15;
				dstrect.x = ((XRES - pImgMenuFrame->w) >> 1) + 6;
				dstrect.y = (YRES - 44) + (22 * i);
				step = 22;
			}

			for(j=0; *items != -1; ++items, ++j)
			{
				if(*items < 100) {
					srcrect.x = srcrect.w;
					srcrect.y = srcrect.h * (*items % 100);
					if(menuPos == *items)
					{
						srcrect.x = 0;
						menuRow = i;
						menuCol = j;
					}
					SDL_BlitSurface(pImgMenuButton, &srcrect, pScreen, &dstrect);
				} else {
					srcrect.x = srcrect.w * (*items % 100);
					srcrect.y = 0;
					if(menuPos == *items)
					{
						srcrect.y = srcrect.h;
						menuRow = i;
						menuCol = j;
					}
					SDL_BlitSurface(pImgDirectGo, &srcrect, pScreen, &dstrect);
				}
				dstrect.x += step;
			}
		}

		//메뉴 버튼 캡션 아이콘 출력
		if(menuPos < 100)
		{
			srcrect.w = pImgOptTitle->w;
			srcrect.h = pImgOptTitle->h / 12;
			srcrect.x = 0;
			srcrect.y = menuPos * srcrect.h;

			//dstrect.x = ((XRES - pImgMenuFrame->w) >> 1) + 113;
			dstrect.x = ((XRES - srcrect.w) >> 1);
			dstrect.y = YRES - 44 - 2; // - ((15 - srcrect.h) >> 1);

			SDL_BlitSurface(pImgOptTitle, &srcrect, pScreen, &dstrect);
		}

		//메뉴 프레임을 메인 배경에 띄움(위 프레임)
		dstrect.x = 0;dstrect.y = 0;
		SDL_BlitSurface(pImgMenuFrame_up, NULL, pScreen, &dstrect);

        std::string viewStr;
        int r, g, b;

        theApp.FontColor("photo", "text", r, g, b);

        if(viewInfo) {
            viewStr = theApp.menuPhoto.getInfo(PHOTO_INFO_STR);
        } else {
            viewStr = theApp.menuPhoto.getInfo(PHOTO_FILE_NAME);
        }

#ifdef WIN32
   		toUnicode(NULL, viewStr.c_str(), dest, strlen(viewStr.c_str()));
#else
   		toUnicode("UTF-8", viewStr.c_str(), dest, strlen(viewStr.c_str()));
#endif
		//DrawTextOut(pScreen, 23, 6, 224, dest, r, g, b);
		DrawTextOut(pScreen, 35, 8, 224, dest, r, g, b);

        /*
        char p[10];
        if(viewMode == PHOTO_FIT_VIEW) {
            strcpy(p, "Fit");
        } else if(viewMode == PHOTO_SCALE_VIEW) {
            sprintf(p, "%d%%", theApp.menuPhoto.getSetting(PHOTO_SCALE));
        } else if(viewMode == PHOTO_COMIC_LEFT_VIEW) {
            strcpy(p, "Comic");
        } else if(viewMode == PHOTO_COMIC_RIGHT_VIEW) {
            strcpy(p, "Comic");
        }

		// UTF8을 유니코드로 변경
   		toUnicode("UTF-8", p, dest, strlen(p));

        int xalign = (6 - strlen(p)) * 3;
		DrawTextOut(pScreen, 267 + xalign, 6, 36, dest, r, g, b);

    	*/

    	//배터리 아이콘 로딩 시작
		theApp.getStatusRect(&dstrect);
		theApp.drawStatusIcon(pScreen, &dstrect);


    }
	SDL_UpdateRect(pScreen, 0, 0, 0, 0);
}
//[*]------------------------------------------------------------------------------------------[*]
int CPhotoImageView::action(int cmd)
{
	int result = UPDATE_NONE;
	if(cmd == -1) cmd = menuPos;

	switch(cmd)
	{
		case PHOTO_ZOOM_IN:
		case PHOTO_ZOOM_OUT:
		case PHOTO_ORIGINAL:
			theApp.menuPhoto.setSetting(PHOTO_SCALE, menuPos);
			result = UPDATE_FULL;
			break;

		case PHOTO_ROTATE_LEFT:
		case PHOTO_ROTATE_RIGHT:
            if(theApp.menuPhoto.isComicMode()) break;
			theApp.menuPhoto.setSetting(PHOTO_ROTATE, menuPos);
			result = UPDATE_FULL;
			break;

		case GOTO_MP3 :
			theApp.SwapReverse();
			if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
				theApp.SetView(MEDIA_PLAY_VIEW);
			else
				theApp.SetView(MUSIC_VIEW);
			break;

		case GOTO_EXIT :
			theApp.photovs=OFF_STATUS;
			theApp.menuPhoto.Break();
			theApp.SwapReverse();

			if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
				theApp.SetView(MEDIA_PLAY_VIEW);
			else
				theApp.SetView(MAIN_VIEW);
			break;

		case PREV_FILE :
			if(theApp.menuPhoto.PrevImage())
				result = UPDATE_FULL;
			break;

		case NEXT_FILE :
			if(theApp.menuPhoto.NextImage())
				result = UPDATE_FULL;
			break;

		case PHOTO_COMIC_LEFT:
			theApp.menuPhoto.setSetting(PHOTO_VIEW_MODE, PHOTO_COMIC_LEFT_VIEW);
			result = UPDATE_FULL;
			break;

		case PHOTO_COMIC_RIGHT:
			theApp.menuPhoto.setSetting(PHOTO_VIEW_MODE, PHOTO_COMIC_RIGHT_VIEW);
			result = UPDATE_FULL;
			break;

		case OPEN_FILE :
			theApp.SwapReverse();
			theApp.SetView(PHOTO_VIEW);
			break;

		case PHOTO_FIT:
			theApp.menuPhoto.setSetting(PHOTO_VIEW_MODE, PHOTO_FIT_VIEW);
			result = UPDATE_FULL;
			break;

		case PHOTO_WALLPAPER:
			{
				SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, COLORBIT, REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);
				theApp.menuPhoto.Draw(surface);
				theApp.setBackground(surface);
				SDL_SAFE_FREE(surface);
				result = UPDATE_FULL;
			}
			break;
	}

	return result;
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CPhotoImageView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	if(menuStatus) {
		// 메뉴 모드
		switch(button)
		{
			case VK_UP 	  : 	// 상 방향키
				if((menuRow == 1) && (menuItems[0][menuCol] != -1))
				{
					menuRow = 0;
					menuPos = menuItems[menuRow][menuCol];
					OnDraw(pScreen);
				}
				break;

			case VK_DOWN  :     // 하 방향키
				if((menuRow == 0) && (menuItems[1][menuCol] != -1))
				{
					menuRow = 1;
					menuPos = menuItems[menuRow][menuCol];
					OnDraw(pScreen);
				}
				break;

			case VK_LEFT:
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
				break;

			case VK_RIGHT:
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
				break;

			case VK_FB:
			case VK_TAT:
				switch(action())
				{
					case UPDATE_FULL:
						OnDraw(pScreen);
						break;
				}
				// run menu
				break;
			case VK_FL :  // 이전 파일
				if(theApp.menuPhoto.PrevImage())
					OnDraw(pScreen);
				break;
			case VK_FR :  // 다음 파일
				if(theApp.menuPhoto.NextImage())
					OnDraw(pScreen);
				break;
			case VK_FX :        // 파일 선택 메뉴로 이동
				theApp.SwapReverse();
				theApp.SetView(PHOTO_VIEW);
				break;
			case VK_FY:
				menuStatus = false;
				OnDraw(pScreen);
				break;
			case VK_FA:
				viewInfo = !viewInfo;
				OnDraw(pScreen);
				break;
			case VK_START:
				theApp.photovs = OFF_STATUS;
				theApp.menuPhoto.Break();
				theApp.SwapReverse();

				//MP3 파일이 실행 중 이라면 mp3 플레이어 로...
				if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
					theApp.SetView(MEDIA_PLAY_VIEW);
				else
					theApp.SetView(MAIN_VIEW);
				break;

			case VK_SELECT:
				theApp.SwapReverse();
				if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
					theApp.SetView(MEDIA_PLAY_VIEW);
				else
					theApp.SetView(MUSIC_VIEW);
				break;
		}
	} else {
		// 포토 뷰어 모드
		switch(button)
		{
			case VK_FL :  // 이전 파일
				if(theApp.menuPhoto.PrevImage())
					OnDraw(pScreen);
				break;
			case VK_FR :  // 다음 파일
			case VK_TAT:
				if(theApp.menuPhoto.NextImage())
					OnDraw(pScreen);
				break;
			case VK_FX :        // 파일 선택 메뉴로 이동
				theApp.SwapReverse();
				theApp.SetView(PHOTO_VIEW);
				break;
			case VK_FY :        // 메뉴 표시
				theApp.menuPhoto.setSetting(PHOTO_SET_MOVE, -1);
				menuStatus = true;
				OnDraw(pScreen);
				break;
			case VK_VOL_UP:
			case VK_VOL_DOWN:
				theApp.menuPhoto.setSetting(PHOTO_SCALE, (button == VK_VOL_UP) ? PHOTO_ZOOM_IN : PHOTO_ZOOM_OUT);
				OnDraw(pScreen);
				break;
			case VK_SELECT:
				//기존 방식
				//theApp.menuPhoto.setSetting(PHOTO_SCALE, PHOTO_FIT);
				//OnDraw(pScreen);

				//새 방식
				theApp.SwapReverse();
				if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
					theApp.SetView(MEDIA_PLAY_VIEW);
				else
					theApp.SetView(MUSIC_VIEW);

				break;
			case VK_FA:
				if(theApp.menuPhoto.isComicMode()) {
					if(theApp.menuPhoto.PrevImage(true))
						OnDraw(pScreen);
				} else {
					theApp.menuPhoto.setSetting(PHOTO_ROTATE, PHOTO_ROTATE_LEFT);
					OnDraw(pScreen);
				}
				break;
			case VK_FB:
				if(theApp.menuPhoto.isComicMode()) {
					if(theApp.menuPhoto.NextImage(true))
						OnDraw(pScreen);
				} else {
					theApp.menuPhoto.setSetting(PHOTO_ROTATE, PHOTO_ROTATE_RIGHT);
					OnDraw(pScreen);
				}
				break;
			case VK_START:
				theApp.photovs = OFF_STATUS;
				theApp.menuPhoto.Break();
				theApp.SwapReverse();

				//MP3 파일이 실행 중 이라면 mp3 플레이어 로...
				if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
					theApp.SetView(MEDIA_PLAY_VIEW);
				else
					theApp.SetView(MAIN_VIEW);
				break;
			case VK_UP:
			case VK_UP_LEFT:
			case VK_LEFT:
			case VK_DOWN_LEFT:
			case VK_DOWN:
			case VK_DOWN_RIGHT:
			case VK_RIGHT:
			case VK_UP_RIGHT:
				theApp.menuPhoto.setSetting(PHOTO_SET_MOVE, button);
				break;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 업 처리
void CPhotoImageView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	if(!menuStatus) {
		switch(button)
		{
			case VK_UP:
			case VK_UP_LEFT:
			case VK_LEFT:
			case VK_DOWN_LEFT:
			case VK_DOWN:
			case VK_DOWN_RIGHT:
			case VK_RIGHT:
			case VK_UP_RIGHT:
				theApp.menuPhoto.setSetting(PHOTO_SET_MOVE, button + 10);
				break;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CPhotoImageView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	if(!menuStatus)
	{
		menuStatus = true;
		OnDraw(pScreen);
		return;
	}

	//메뉴 프레임 위치
	SDL_Rect rect;
	rect.w = pImgMenuFrame->w;
	rect.h = pImgMenuFrame->h;
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
				rect.x = ((XRES - pImgMenuFrame->w) >> 1) + 260;
				rect.y = YRES - 34;
				rect.w = pImgDirectGo->w / 5;
				rect.h = pImgDirectGo->h / 2;
				step = -38;
			}
			else
			{
				rect.x = ((XRES - pImgMenuFrame->w) >> 1) + 6;
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
					}
					return;
				}
				rect.x += step;
				++items;
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
void CPhotoImageView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{


}
//[*]------------------------------------------------------------------------------------------[*]
