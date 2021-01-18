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
#include <SDL/SDL.h>
#include <UnicodeFont.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "ebooktextview.h"
#include "ebookmarkview.h"
#include "typed.h"
#include "textout.h"
//[*]------------------------------------------------------------------------------------------[*]
#define	STATUS_LINE_BG_R	230
#define	STATUS_LINE_BG_G	230
#define	STATUS_LINE_BG_B	230
#define	STATUS_LINE_FONT_R	0
#define	STATUS_LINE_FONT_G	0
#define	STATUS_LINE_FONT_B	0
#define	BOTTOM_MARGIN	26
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CEBookMarkView::CEBookMarkView()
{
	pImgButton = NULL;
	pImgBmFrame= NULL;
	pImgButtonTitle= NULL;
	pImgBmLineSelect= NULL;
	pImgBmLineSelect_bk=NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
CEBookMarkView::~CEBookMarkView()
{
	SDL_SAFE_FREE(pImgButton);
	SDL_SAFE_FREE(pImgBmFrame);
	SDL_SAFE_FREE(pImgButtonTitle);
	SDL_SAFE_FREE(pImgBmLineSelect);
	SDL_SAFE_FREE(pImgBmLineSelect_bk);
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CEBookMarkView::Initialize(bool refresh)
{
	if(!pImgButton) pImgButton = theApp.LoadImage("ebook", "eb_button");
	pImgBmFrame = theApp.LoadImage("ebook", "bm_frame");
	pImgButtonTitle = theApp.LoadImage("ebook", "eb_button_title");
	pImgBmLineSelect = theApp.LoadImage("ebook", "bm_line_select");
	pImgBmLineSelect_bk = theApp.LoadImage("ebook", "bm_line_select_bk");
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CEBookMarkView::Exitialize(bool refresh)
{
	if(refresh) {
		SDL_SAFE_FREE(pImgButton);
		SDL_SAFE_FREE(pImgBmFrame);
		SDL_SAFE_FREE(pImgButtonTitle);
		SDL_SAFE_FREE(pImgBmLineSelect);
		SDL_SAFE_FREE(pImgBmLineSelect_bk);
	} else {
		theApp.menuEBook.SaveBookmark();
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookMarkView::SetPos(SDL_Surface *pScreen, int x, int y)
{
	cursor = (unsigned short *)pScreen->pixels;
	cursor += y*pScreen->w + x;
}
//[*]------------------------------------------------------------------------------------------[*]
int CEBookMarkView::PutChar(SDL_Surface *pScreen, unsigned short code, int limit = 100)
{
	unsigned	short	font[16];
	int	height,width;
	int	i,j;

	GetFont(code,font,&height,&width);
	if(width > limit)
		return 0;

	for	(i=0;i<height;i++)
	{
		unsigned	short	bit=font[i];
		for	(j=0;j<width;j++)
		{
			if (bit&0x8000)
				*cursor++ = font_color;
			else
				cursor++;
			bit<<=1;
		}
		cursor = cursor + pScreen->w - width;
	}
	cursor = cursor - pScreen->w * height + width;

	return width;
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookMarkView::PutString(SDL_Surface *pScreen, int x, int y, char *str)
{
	unsigned short uni[512];
	unsigned short *up = uni;
	theApp.menuEBook.toUnicode(uni, str, strlen(str));

	SetPos(pScreen, x, y);
	while(*up) {
		PutChar(pScreen, *up++);
    }
}

void CEBookMarkView::PutFileName(SDL_Surface *pScreen, int x, int y, char* buf, int limit = 130)
{
	// 유니코드용 저장 버퍼
	unsigned short dest[512];
	unsigned short *up = dest;
	int width;

	// 파일명을 유니코드로 변경
#ifdef WIN32
	toUnicode(NULL, buf, dest, strlen(buf));
#else
	toUnicode("UTF-8", buf, dest, strlen(buf));
#endif

	SetPos(pScreen, x, y);
	while(*up) {
		width = PutChar(pScreen, *up++, limit);
		if(width == 0) break;
		limit -= width;
    }
}

void CEBookMarkView::ShowStatusLine(SDL_Surface *pScreen)
{
	static char *save = "Save";
	static char *load = "Load";
/*
	static char *setting = " A  Setting";
	static char *bookmark = " B  Bookmark";
	static char *change = " A  Select";
	static char *apply = " B  Apply";
*/
	char buf[256];
	SDL_Rect srcrect, rect;
	rect.x = 0;
	rect.y = pScreen->h - BOTTOM_MARGIN;
	rect.w = pScreen->w;
	rect.h = pScreen->h - rect.y;

	SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, STATUS_LINE_BG_R, STATUS_LINE_BG_G, STATUS_LINE_BG_B));
	sprintf(buf,"%d/%d Lines",theApp.menuEBook.getSetting(EBOOK_CURRENT_LINE),theApp.menuEBook.getSetting(EBOOK_TOTAL_LINE));

	font_color = SDL_MapRGB(pScreen->format, STATUS_LINE_FONT_R, STATUS_LINE_FONT_G, STATUS_LINE_FONT_B);
	PutString(pScreen, pScreen->w - 4 - strlen((const char*)buf) * 6, 221, buf);
	PutString(pScreen, 12, 221, "A");
	PutString(pScreen, 30, 221, save);
	PutString(pScreen, 102, 221, "B");
	PutString(pScreen, 120, 221, load);

	srcrect.w = pImgButton->w >> 1;
	srcrect.h = pImgButton->h;
	srcrect.x = 0;srcrect.y = 0;
    rect.x = 6;rect.y = 215;
	SDL_BlitSurface(pImgButton, &srcrect, pScreen, &rect);
	srcrect.x = srcrect.w;
    rect.x = 96;rect.y = 215;
	SDL_BlitSurface(pImgButton, &srcrect, pScreen, &rect);
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookMarkView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_UP :
			if	(pos > 0) {
				pos--;
				OnDraw(pScreen);
			}
			break;
		case VK_DOWN :
			//if	(pos < 24)
			if	(pos < 8)
			{
				pos++;
				OnDraw(pScreen);
			}
			break;
		case VK_LEFT :
			/*
			if	(pos > 4) {
				pos -= 5;
				OnDraw(pScreen);
			}
			*/
			break;
		case VK_RIGHT :
			/*
			if	(pos < 20) {
				pos += 5;
				OnDraw(pScreen);
			}
			*/
			break;
		case VK_FB: {
				if (theApp.menuEBook.RecallBookmark(pos)) {
					theApp.SwapReverse();
					theApp.SetView(EBOOK_TEXT_VIEW);
				}
			}
			break;

		case VK_FX	:
			theApp.SwapReverse();
			theApp.SetView(EBOOK_TEXT_VIEW);
			break;
		case VK_FA	:
			theApp.menuEBook.MarkBookmark(pos);
			theApp.SwapReverse();
			theApp.SetView(EBOOK_TEXT_VIEW);
			break;

		case VK_START :
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;


		case VK_FY	:
			theApp.menuEBook.DeleteBookmark(pos);
			//theApp.SwapReverse();
			//theApp.SetView(EBOOK_TEXT_VIEW);
			OnDraw(pScreen);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookMarkView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	// ...
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookMarkView::OnDraw(SDL_Surface *pScreen)
{
	int	i;
	SDL_Rect rect,dest;
	RGBA color;



	//A 버튼 출력
	rect.x = 0;
	rect.y = 0;
	rect.w = (pImgButton->w) / 4;
	rect.h = (pImgButton->h) / 1;
	dest.x = 41;
	dest.y = 211;
	SDL_BlitSurface(pImgButton,&rect,pImgBmFrame,&dest);


	//SAVE 타이틀 출력
	rect.x = 0;
	rect.y = ((pImgButtonTitle->h) / 6)*0;
	rect.w = (pImgButtonTitle->w);
	rect.h = (pImgButtonTitle->h) / 6;
	dest.x = 64;
	dest.y = 215;
	SDL_BlitSurface(pImgButtonTitle,&rect,pImgBmFrame,&dest);


	//Y 버튼 출력
	rect.x = ((pImgButton->w) / 4)*3;
	rect.y = 0;
	rect.w = (pImgButton->w) / 4;
	rect.h = (pImgButton->h) / 1;
	dest.x = 91;
	dest.y = 211;
	SDL_BlitSurface(pImgButton,&rect,pImgBmFrame,&dest);

	//DELETE 타이틀 출력
	rect.x = 0;
	rect.y = ((pImgButtonTitle->h) / 6)*1;
	rect.w = (pImgButtonTitle->w);
	rect.h = (pImgButtonTitle->h) / 6;
	dest.x = 113;
	dest.y = 215;
	SDL_BlitSurface(pImgButtonTitle,&rect,pImgBmFrame,&dest);


	//B 버튼 출력
	rect.x = ((pImgButton->w) / 4)*1;
	rect.y = 0;
	rect.w = (pImgButton->w) / 4;
	rect.h = (pImgButton->h) / 1;
	dest.x = 148;
	dest.y = 211;
	SDL_BlitSurface(pImgButton,&rect,pImgBmFrame,&dest);

	//LOAD 타이틀 출력
	rect.x = 0;
	rect.y = ((pImgButtonTitle->h) / 6)*2;
	rect.w = (pImgButtonTitle->w);
	rect.h = (pImgButtonTitle->h) / 6;
	dest.x = 171;
	dest.y = 215;
	SDL_BlitSurface(pImgButtonTitle,&rect,pImgBmFrame,&dest);


	//X 버튼 출력
	rect.x = ((pImgButton->w) / 4)*2;
	rect.y = 0;
	rect.w = (pImgButton->w) / 4;
	rect.h = (pImgButton->h) / 1;
	dest.x = 196;
	dest.y = 211;
	SDL_BlitSurface(pImgButton,&rect,pImgBmFrame,&dest);

	//CANCEL 타이틀 출력
	rect.x = 0;
	rect.y = ((pImgButtonTitle->h) / 6)*3;
	rect.w = (pImgButtonTitle->w);
	rect.h = (pImgButtonTitle->h) / 6;
	dest.x = 220;
	dest.y = 215;
	SDL_BlitSurface(pImgButtonTitle,&rect,pImgBmFrame,&dest);

	SDL_Rect bmk_column_pos[9] = {
		{40,14,236,17},
		{40,36,236,17},
		{40,58,236,17},
		{40,80,236,17},
		{40,102,236,17},
		{40,124,236,17},
		{40,146,236,17},
		{40,168,236,17},
		{40,190,236,17}

	};

	//rect.x = 0;
	//rect.y = 0;
	//rect.w = pScreen->w;
	//rect.h = pScreen->h - BOTTOM_MARGIN;
    //int index = theApp.menuEBook.getSetting(EBOOK_BG_COLOR);
	//SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, color_index[index].r, color_index[index].g, color_index[index].b));

	//rect.x = 13;
	//rect.y = 11;
	//rect.w = 303-13;
	//rect.h = 198-11;
	//SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, STATUS_LINE_BG_R, STATUS_LINE_BG_G, STATUS_LINE_BG_B));

	font_color = SDL_MapRGB(pScreen->format, STATUS_LINE_FONT_R, STATUS_LINE_FONT_G, STATUS_LINE_FONT_B);

	//PutString(pScreen, 58, 26, "bookmark");
	//PutString(pScreen, 59, 26, "bookmark");



	printf("pos : %d\n",pos);
	//int no = pos % 5;
	int no = pos % 9;
	//int page = (pos / 5) * 5;
	int page = 0;

	for	(i=0;i<9;i++) {
		char buf[256];
		sprintf(buf,"%d",i+page+1);
		//if (i == no) {color.r=115;color.g=115;color.b=115;}
		//else {color.r=33;color.g=33;color.b=33;}

		//rect=bmk_column_pos[i];
		dest=bmk_column_pos[i];

		font_color = SDL_MapRGB(pScreen->format, STATUS_LINE_FONT_R, STATUS_LINE_FONT_G, STATUS_LINE_FONT_B);
		//PutString(pScreen, rect.x-16 ,rect.y+4, buf);   // 맨 왼쪽의 1,2,3,4,5,6,7,.. 출력
		//SDL_FillRect(pImgBmFrame, &dest, SDL_MapRGB(pScreen->format, 33, 33, 33));


		//미 선택 바(회색바) 출력
		rect.x = 0;
		rect.y = 0;
		rect.w = (pImgBmLineSelect_bk->w);
		rect.h = (pImgBmLineSelect_bk->h);

		SDL_BlitSurface(pImgBmLineSelect_bk,&rect,pImgBmFrame,&dest);


		if (i == pos)
		{
			printf("pos : %d\n",pos);
			printf("i : %d\n",i);
			//선택 바 출력
			rect.x = 0;
			rect.y = 0;
			rect.w = (pImgBmLineSelect->w);
			rect.h = (pImgBmLineSelect->h);

			SDL_BlitSurface(pImgBmLineSelect,&rect,pImgBmFrame,&dest);

		}
		//else
			//


	}


	//pImgBmFrame 북마크 프레임을 메인 프레임에 출력
	rect.x = 0;
	rect.y = 0;
	rect.w = pImgBmFrame->w;
	rect.h = pImgBmFrame->h;

	SDL_Rect dst;
	dst.x = 13;
	dst.y = 0;

	SDL_BlitSurface(pImgBmFrame,&rect,pScreen,&dst);



	//북마크 파일명,라인 수 등을 메인 프레임에 출력
	for	(i=0;i<9;i++)
	{
		char buf[256];
		dest=bmk_column_pos[i];

		char *name = strdup(theApp.menuEBook.getBookmarkName(i+page));
		char *ptr = strrchr(name, '\\');
		if (ptr==NULL) ptr=strrchr(name, '/');
		if (ptr==NULL) ptr=name;
	    else ptr++; // \, / 기호 삭제
		strcpy(buf,ptr);
		free(name);

		font_color = SDL_MapRGB(pScreen->format, 0xFF, 0xFF, 0xFF);
		PutFileName(pScreen, dest.x+12 ,dest.y+4, buf);

		sprintf(buf, "%d/%d Lines", theApp.menuEBook.getBookmarkInfo(i+page, EBOOK_CURRENT_LINE), theApp.menuEBook.getBookmarkInfo(i+page, EBOOK_TOTAL_LINE));
		PutString(pScreen, 284-strlen(buf)*6, dest.y+4, buf);
	}


	//ShowStatusLine(pScreen);
	SDL_UpdateRect(pScreen, 0, 0, 0, 0);
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookMarkView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	int i, w, h, px, py,frame_gap_x,frame_gap_y;
	SDL_Rect dest;

	frame_gap_x=13;
	frame_gap_y=0;

	SDL_Rect bmk_column_pos[9] = {
		{40,14,236,17},
		{40,36,236,17},
		{40,58,236,17},
		{40,80,236,17},
		{40,102,236,17},
		{40,124,236,17},
		{40,146,236,17},
		{40,168,236,17},
		{40,190,236,17}

	};


	printf("pos : %d\n",pos);
	//int no = pos % 5;
	int no = pos % 9;
	//int page = (pos / 5) * 5;
	int page = 0;

	w=pImgBmLineSelect->w;
	h=pImgBmLineSelect->h;

	for	(i=0;i<9;i++)
	{

		dest=bmk_column_pos[i];


		px=dest.x+frame_gap_x;
		py=dest.y+frame_gap_y;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			pos=i;

			OnDraw(pScreen);
		}




	}


	//각 버튼에 반응

	//A 버튼
	w = (pImgButton->w) / 4;
	h = (pImgButton->h) / 1;
	px = 41+frame_gap_x;
	py = 211+frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FA);

	}

	//SAVE 타이틀
	w = (pImgButtonTitle->w);
	h = (pImgButtonTitle->h) / 6;
	px = 64+frame_gap_x;
	py = 218+frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FA);

	}




	//Y 버튼
	w = (pImgButton->w) / 4;
	h = (pImgButton->h) / 1;
	px = 91+frame_gap_x;
	py = 211+frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FY);

	}

	//DELETE 타이틀
	w = (pImgButtonTitle->w);
	h = (pImgButtonTitle->h) / 6;
	px = 113+frame_gap_x;
	py = 218+frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FY);

	}






	//B 버튼
	w = (pImgButton->w) / 4;
	h = (pImgButton->h) / 1;
	px = 148+frame_gap_x;
	py = 211+frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FB);
	}

	//LOAD 타이틀
	w = (pImgButtonTitle->w);
	h = (pImgButtonTitle->h) / 6;
	px = 171+frame_gap_x;
	py = 218+frame_gap_y;
	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FB);
	}







	//X 버튼
	w = (pImgButton->w) / 4;
	h = (pImgButton->h) / 1;
	px = 196+frame_gap_x;
	py = 211+frame_gap_y;
	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FX);
	}

	//CANCEL 타이틀
	w = (pImgButtonTitle->w);
	h = (pImgButtonTitle->h) / 6;
	px = 220+frame_gap_x;
	py = 218+frame_gap_y;
	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FX);
	}



}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookMarkView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{


}
//[*]------------------------------------------------------------------------------------------[*]
