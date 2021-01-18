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
#include <SDL/SDL_ttf.h>
#include <UnicodeFont.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "ebooktextview.h"
#include "ebooksettingview.h"
#include "ebookfontview.h"
#include "typed.h"
#include "textout.h"
//[*]------------------------------------------------------------------------------------------[*]
#define	STATUS_LINE_BG_R	230
#define	STATUS_LINE_BG_G	230
#define	STATUS_LINE_BG_B	230
//#define	STATUS_LINE_FONT_R	0
#define	STATUS_LINE_FONT_R	255
//#define	STATUS_LINE_FONT_G	0
#define	STATUS_LINE_FONT_G	255
//#define	STATUS_LINE_FONT_B	0
#define	STATUS_LINE_FONT_B	255
#define	BOTTOM_MARGIN	26
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CEBookSettingView::CEBookSettingView()
{
	pImgButton = NULL;
	pImgSettingFrame=NULL;
	pImgColorList=NULL;
	pImgColorChoice=NULL;
	pImgSettingTitle=NULL;
	pImgButtonTitle = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
CEBookSettingView::~CEBookSettingView()
{
	SDL_SAFE_FREE(pImgButton);
	SDL_SAFE_FREE(pImgSettingFrame);
	SDL_SAFE_FREE(pImgColorList);
	SDL_SAFE_FREE(pImgColorChoice);
	SDL_SAFE_FREE(pImgSettingTitle);
	SDL_SAFE_FREE(pImgButtonTitle);
}
//[*]------------------------------------------------------------------------------------------[*]
// �ʱ�ȭ �Լ�
void CEBookSettingView::Initialize(bool refresh)
{
	pImgButton = theApp.LoadImage("ebook", "eb_button");
	pImgSettingFrame=theApp.LoadImage("ebook", "setting_frame");
	pImgColorList=theApp.LoadImage("ebook", "color_list");
	pImgColorChoice=theApp.LoadImage("ebook", "color_choice");
	pImgSettingTitle=theApp.LoadImage("ebook", "eb_setting_title");
	pImgButtonTitle = theApp.LoadImage("ebook", "eb_button_title");

	if(!refresh) {
		original_font = theApp.menuEBook.getFont();
		set_font_color = theApp.menuEBook.getSetting(EBOOK_TEXT_COLOR);
		set_bg_color = theApp.menuEBook.getSetting(EBOOK_BG_COLOR);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// ���� �Լ�
void CEBookSettingView::Exitialize(bool refresh)
{
	if(refresh) {
		SDL_SAFE_FREE(pImgButton);
		SDL_SAFE_FREE(pImgSettingFrame);
		SDL_SAFE_FREE(pImgColorList);
		SDL_SAFE_FREE(pImgColorChoice);
		SDL_SAFE_FREE(pImgSettingTitle);
		SDL_SAFE_FREE(pImgButtonTitle);
	} else {
		original_font = "";
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookSettingView::SetPos(SDL_Surface *pScreen, int x, int y)
{
	cursor = (unsigned short *)pScreen->pixels;
	cursor += y*pScreen->w + x;
}
//[*]------------------------------------------------------------------------------------------[*]
int CEBookSettingView::PutChar(SDL_Surface *pScreen, unsigned short code, int limit = 100)
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
void CEBookSettingView::PutString(SDL_Surface *pScreen, int x, int y, const char *str)
{
	unsigned short uni[512];
	unsigned short *up = uni;
	theApp.menuEBook.toUnicode(uni, str, strlen(str));

	SetPos(pScreen, x, y);
	while(*up) {
		PutChar(pScreen, *up++);
    }
}

void CEBookSettingView::PutFileName(SDL_Surface *pScreen, int x, int y, const char* buf, int limit = 130)
{
	// �����ڵ�� ���� ����
	unsigned short dest[512];
	unsigned short *up = dest;
	int width;

	// ���ϸ��� �����ڵ�� ����
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

void CEBookSettingView::ShowStatusLine(SDL_Surface *pScreen)
{
	static char *change = "Select";
	static char *apply = "Apply";
/*
	static char *save = " A  Save";
	static char *load = " B  Load";
	static char *setting = " A  Setting";
	static char *bookmark = " B  Bookmark";
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
	PutString(pScreen, 30, 221, change);
	PutString(pScreen, 102, 221, "B");
	PutString(pScreen, 120, 221, apply);

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
void CEBookSettingView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_UP :
			if (pos > 0) {
				pos--;
				OnDraw(pScreen);
			}
			break;
		case VK_DOWN :
			if (pos < 2) {
				pos++;
				OnDraw(pScreen);
			}
			break;
		case VK_LEFT :
			switch(pos) {
				case 0: if(set_font_color > 0) set_font_color--; break;
				case 1: if(set_bg_color > 0) set_bg_color--; break;
			}
			OnDraw(pScreen);
			break;
		case VK_RIGHT :
			switch(pos) {
				case 0: if(set_font_color < 9) set_font_color++; break;
				case 1: if(set_bg_color < 9) set_bg_color++; break;
			}
			OnDraw(pScreen);
			break;

		case VK_FB :
			//theApp.menuEBook.setSetting(EBOOK_TEXT_COLOR, set_font_color);
			//theApp.menuEBook.setSetting(EBOOK_BG_COLOR, set_bg_color);
			//original_font = theApp.menuEBook.getFont();
			//theApp.SwapReverse();
			//theApp.SetView(EBOOK_TEXT_VIEW);
			break;
		case VK_FA	:
			if (pos == 2)
				theApp.SetView(EBOOK_FONT_VIEW);
			else
			{
				theApp.menuEBook.setSetting(EBOOK_TEXT_COLOR, set_font_color);
				theApp.menuEBook.setSetting(EBOOK_BG_COLOR, set_bg_color);
				original_font = theApp.menuEBook.getFont();
				theApp.SwapReverse();
				theApp.SetView(EBOOK_TEXT_VIEW);
			}


			break;
		case VK_FX :
			theApp.menuEBook.setFont(original_font.c_str());
			theApp.SwapReverse();
			theApp.SetView(EBOOK_TEXT_VIEW);
			break;
		case VK_START :
			theApp.menuEBook.setFont(original_font.c_str());
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookSettingView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	// ...
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookSettingView::OnDraw(SDL_Surface *pScreen)
{
	SDL_Rect		rect,dest;

	//�ٱ��� �ѷ��ΰ� �ִ� �׸� �ڽ�(�⺻ ������)
	//rect.x = 0;
	//rect.y = 0;
	//rect.w = pScreen->w;
	//rect.h = pScreen->h - BOTTOM_MARGIN;
    //int index = theApp.menuEBook.getSetting(EBOOK_BG_COLOR);
	//SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, color_index[index].r, color_index[index].g, color_index[index].b));

	//font_color = SDL_MapRGB(pImgSettingFrame->format, STATUS_LINE_FONT_R, STATUS_LINE_FONT_G, STATUS_LINE_FONT_B);
	//PutString(pImgSettingFrame, 4,10,"display setting");
	//PutString(pImgSettingFrame, 5,10,"display setting");
	//PutString(pImgSettingFrame, 4,20,"font color");
	//PutString(pImgSettingFrame, 4,30," bg  color");
	//PutString(pImgSettingFrame, 4,40,"font select");
	//PutString(25,120,"font size");

	font_color = SDL_MapRGB(pScreen->format, 0xFF, 0xFF, 0xFF);
	static	SDL_Rect	column_pos[4]={
		{13, 20,(pImgSettingTitle->w)/2,(pImgSettingTitle->h)/3},  //font color Ÿ��Ʋ ������ ��ġ
		{13, 45,(pImgSettingTitle->w)/2,(pImgSettingTitle->h)/3},  // bg  color Ÿ��Ʋ ������ ��ġ
		{13, 70,(pImgSettingTitle->w)/2,(pImgSettingTitle->h)/3},  //font selectŸ��Ʋ ������ ��ġ
		{13, 90,290-78,30}                                         //�� ����
	};

	RGBA	color;
	int	textbg_color[2];
	textbg_color[0] = set_font_color;
	textbg_color[1] = set_bg_color;

	int	i;
	for	(i=0;i<3;i++)
	{
		//if (i == pos) {color.r=115;color.g=115;color.b=115;} //��,�Ʒ� ���� �� ���õ� ������ ȸ������ ��µȴ�
		//else {color.r=33;color.g=33;color.b=33;}             //��,�Ʒ� ���� �� ���þȵ� ������ ���������� ��µȴ�
		//SDL_FillRect(pImgSettingFrame, &rect, SDL_MapRGB(pImgSettingFrame->format, color.r, color.g, color.b));

		dest=column_pos[i];

		if (i == pos)
		{
			//i ���� ���� ���� �Ǿ����� �޴� Ÿ��Ʋ ǥ��(ȸ��->��ȫ��)
			rect.x = 0;
		}
		else
		{
			//i ���� ���� ���� �� �Ǿ����� �޴� Ÿ��Ʋ ǥ��(ȸ��)
			rect.x = (pImgSettingTitle->w) / 2;
		}
		rect.y = ((pImgSettingTitle->h) / 3) * i;
		rect.w = (pImgSettingTitle->w) / 2;
		rect.h = (pImgSettingTitle->h) / 3;

		SDL_BlitSurface(pImgSettingTitle,&rect,pImgSettingFrame,&dest);

		if(i<2)
		{
			int	j;
			SDL_Rect	colors;
			colors.x=dest.x+63;
			colors.y=dest.y-5;
			colors.h=15;colors.w=15;

			for(j=0;j<10;j++)
			{
				SDL_Rect src;

				src.x=((pImgColorList->w) / 10) * j;
				src.y=0;
				src.w=(pImgColorList->w) / 10;
				src.h=pImgColorList->h;

				SDL_BlitSurface(pImgColorList,&src,pImgSettingFrame,&colors);

				if(textbg_color[i]==j)
				{
					SDL_Rect	select_color;

					select_color.x=0;
					select_color.y=0;
					select_color.w=pImgColorChoice->w;
					select_color.h=pImgColorChoice->h;

					SDL_BlitSurface(pImgColorChoice,&select_color,pImgSettingFrame,&colors);
				}
				colors.x+=15+6;

			}
		}
		if	(i==2)
		{
			//if (strlen(font_loc)>30) font_loc[30]='\0';
			//PutFileName(pScreen, 30,90, theApp.menuEBook.getFont(),190);
		}

	}


	//A ��ư ���
	rect.x = 0;
	rect.y = 0;
	rect.w = (pImgButton->w) / 4;
	rect.h = (pImgButton->h) / 1;
	dest.x = 83;
	dest.y = 88;
	SDL_BlitSurface(pImgButton,&rect,pImgSettingFrame,&dest);

	//OK Ÿ��Ʋ ���
	rect.x = 0;
	rect.y = ((pImgButtonTitle->h) / 6)*4;
	rect.w = (pImgButtonTitle->w);
	rect.h = (pImgButtonTitle->h) / 6;
	dest.x = 107;
	dest.y = 92;
	SDL_BlitSurface(pImgButtonTitle,&rect,pImgSettingFrame,&dest);

	//X ��ư ���
	rect.x = ((pImgButton->w) / 4)*2;
	rect.y = 0;
	rect.w = (pImgButton->w) / 4;
	rect.h = (pImgButton->h) / 1;
	dest.x = 136;
	dest.y = 88;
	SDL_BlitSurface(pImgButton,&rect,pImgSettingFrame,&dest);

	//CANCEL Ÿ��Ʋ ���
	rect.x = 0;
	rect.y = ((pImgButtonTitle->h) / 6)*3;
	rect.w = (pImgButtonTitle->w);
	rect.h = (pImgButtonTitle->h) / 6;
	dest.x = 158;
	dest.y = 92;
	SDL_BlitSurface(pImgButtonTitle,&rect,pImgSettingFrame,&dest);


	if(pImgSettingFrame!=NULL)
	{

		rect.x = 0;
		rect.y = 0;
		rect.w = pImgSettingFrame->w;
		rect.h = pImgSettingFrame->h;

		SDL_Rect dst;
		dst.x = 15;
		dst.y = 66;

		SDL_BlitSurface(pImgSettingFrame,&rect,pScreen,&dst);

	}

	//�ܺ� ��Ʈ���� ��� ���
	PutFileName(pScreen, 98,134, theApp.menuEBook.getFont(),190);

	//ShowStatusLine(pScreen);
	SDL_UpdateRect(pScreen, 0, 0, 0, 0);
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookSettingView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	int i, w, h, px, py,frame_gap_x,frame_gap_y;
	SDL_Rect dest;

	frame_gap_x=15;
	frame_gap_y=66;

	static	SDL_Rect	column_pos[3]={
		{12+frame_gap_x, 19+frame_gap_y,(pImgSettingTitle->w)/2,(pImgSettingTitle->h)/3},  //font color Ÿ��Ʋ ������ ��ġ
		{12+frame_gap_x, 45+frame_gap_y,(pImgSettingTitle->w)/2,(pImgSettingTitle->h)/3},  // bg  color Ÿ��Ʋ ������ ��ġ
		{12+frame_gap_x, 69+frame_gap_y,(pImgSettingTitle->w)/2,(pImgSettingTitle->h)/3}   //font selectŸ��Ʋ ������ ��ġ

	};

	for	(i=0;i<3;i++)
	{
		dest=column_pos[i];

		if(i<2)
		{
			int	j;

			px=dest.x+63;
			py=dest.y-5;
			w=15;
			h=15;

			for(j=0;j<10;j++)
			{

				if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
				{
					pos=0;

					if (i==0)
						set_font_color=j;
					else if (i==1)
						set_bg_color=j;

					OnDraw(pScreen);
					break;
				}

				px+=15+6;

			}
		}
		if	(i==2)
		{
			px=dest.x+63;
			py=dest.y-5;
			w=100;
			h=15;

			if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
			{
				pos=2;

				OnJoystickDown(pScreen, VK_FA);

			}

		}

	}


	//�� ��ư�� ����

	//A ��ư
	w = (pImgButton->w) / 4;
	h = (pImgButton->h) / 1;
	px = 83+frame_gap_x;
	py = 88+frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FA);

	}

	//OK Ÿ��Ʋ ���
	w = (pImgButtonTitle->w);
	h = (pImgButtonTitle->h) / 6;
	px = 107+frame_gap_x;
	py = 95 +frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FA);

	}

	//X ��ư ���
	w = (pImgButton->w) / 4;
	h = (pImgButton->h) / 1;
	px = 136+frame_gap_x;
	py = 88 +frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FX);

	}

	//CANCEL Ÿ��Ʋ ���
	w = (pImgButtonTitle->w);
	h = (pImgButtonTitle->h) / 6;
	px = 158+frame_gap_x;
	py = 95 +frame_gap_y;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		OnJoystickDown(pScreen, VK_FX);

	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookSettingView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{


}
//[*]------------------------------------------------------------------------------------------[*]
