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
#include "typed.h"
#include "textout.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
RGBA color_index[10] = {
	{255,255,255,0},
	{  0,  0,  0,0},
	{254,242,  0,0},
	{237, 27, 36,0},
	{140,198, 62,0},
	{237,  0,140,0},
	{146, 39,143,0},
	{131,123,  0,0},
	{198,156,108,0},
	{200,204,200,0}
};
//[*]------------------------------------------------------------------------------------------[*]
CEBookTextView::CEBookTextView()
{
	pImgMenuButton=NULL;
	pImgMenuFrame=NULL;
	pImgMenuFrame_cp=NULL;
	pImgDirectGo=NULL;
	pImgMenuFrame_up=NULL;
	pImgMenuFrame_up_cp=NULL;
	pImgProgressBar=NULL;
	pImgHeightBar=NULL;
	pImgSmallFont=NULL;
	pImgOptTitle=NULL;

	ttf_font = NULL;
	ShowStatus = false;
	curPercent = 0.0;
}
//[*]------------------------------------------------------------------------------------------[*]
CEBookTextView::~CEBookTextView()
{
	SDL_SAFE_FREE(pImgMenuButton);
	SDL_SAFE_FREE(pImgMenuFrame);
	SDL_SAFE_FREE(pImgMenuFrame_cp);
	SDL_SAFE_FREE(pImgDirectGo);
	SDL_SAFE_FREE(pImgMenuFrame_up);
	SDL_SAFE_FREE(pImgMenuFrame_up_cp);
	SDL_SAFE_FREE(pImgProgressBar);
	SDL_SAFE_FREE(pImgHeightBar);
	SDL_SAFE_FREE(pImgSmallFont);
	SDL_SAFE_FREE(pImgOptTitle);

	if(ttf_font) TTF_CloseFont(ttf_font);
	ttf_font = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CEBookTextView::Initialize(bool refresh)
{
	if(!pImgMenuButton) pImgMenuButton = theApp.LoadImage("ebook", "eb_menu_button");
	pImgMenuFrame = theApp.LoadImage("ebook", "eb_menu_frame");
	pImgMenuFrame_cp = theApp.LoadImage("ebook", "eb_menu_frame");
	pImgDirectGo = theApp.LoadImage("common", "direct_go");
	pImgMenuFrame_up = theApp.LoadImage("ebook", "eb_menu_frame_up");
	pImgMenuFrame_up_cp = theApp.LoadImage("ebook", "eb_menu_frame_up");
	pImgProgressBar = theApp.LoadImage("ebook", "progressbar");
	pImgHeightBar = theApp.LoadImage("common", "height_bar");
	pImgSmallFont = theApp.LoadImage("ebook", "eb_numeric");
	pImgOptTitle = theApp.LoadImage("ebook", "eb_opt_title");

	theApp.lastTask = 2;

	if(!refresh) {
		ChangeFontProcess();
		SeekMode = false;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CEBookTextView::Exitialize(bool refresh)
{
	if(refresh) {
		SDL_SAFE_FREE(pImgMenuButton);
		SDL_SAFE_FREE(pImgMenuFrame);
		SDL_SAFE_FREE(pImgMenuFrame_cp);
		SDL_SAFE_FREE(pImgDirectGo);
		SDL_SAFE_FREE(pImgMenuFrame_up);
		SDL_SAFE_FREE(pImgMenuFrame_up_cp);
		SDL_SAFE_FREE(pImgProgressBar);
		SDL_SAFE_FREE(pImgHeightBar);
		SDL_SAFE_FREE(pImgSmallFont);
		SDL_SAFE_FREE(pImgOptTitle);
	} else {
		theApp.menuEBook.SaveBookmark();
	}
}
//[*]------------------------------------------------------------------------------------------[*]
#define DEFAULT_PTSIZE	12
//[*]------------------------------------------------------------------------------------------[*]
int	CEBookTextView::LoadFont(const char *fontfile)
{
	if (ttf_font) TTF_CloseFont(ttf_font);
	ttf_font = TTF_OpenFont(fontfile, 12);
	if (ttf_font == NULL) {
#ifdef DEBUG
		printf("Unable to load font: %s(%s)\n", fontfile, TTF_GetError());
#endif
		return	0;
	}
	return	1;
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::ChangeFontProcess()
{
	if (LoadFont(theApp.menuEBook.getFont())) {
		RenderOneLine = EBOOK_RENDER_EXTERNAL;
	} else {
		theApp.menuEBook.setFont("");
		RenderOneLine = EBOOK_RENDER_INTERNAL;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::RenderOneLine_external_font(SDL_Surface *pScreen, int y_pos, const unsigned short *uni)
{
	int	x_pos = theApp.menuEBook.getSetting(EBOOK_LEFT_MARGIN);

	if(uni == NULL) return;

	SDL_Surface *text_surface;

	SDL_Color color;
	int index = theApp.menuEBook.getSetting(EBOOK_TEXT_COLOR);
	color.r = color_index[index].r;
	color.g = color_index[index].g;
	color.b = color_index[index].b;

	if(!(text_surface = TTF_RenderUNICODE_Solid(ttf_font,uni,color))) {
		//handle error here, perhaps print TTF_GetError at least
	} else {
		SDL_Rect dst;
		dst.x = x_pos;
		dst.y = y_pos;
		dst.h = text_surface->h;
		dst.w = text_surface->w;
		SDL_BlitSurface(text_surface,NULL,pScreen,&dst);
		//perhaps we can reuse it, but I assume not for simplicity.
		SDL_FreeSurface(text_surface);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::SetPos(SDL_Surface *pScreen, int x, int y)
{
	cursor = (unsigned short *)pScreen->pixels;
	cursor += y*pScreen->w + x;
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::DrawChar(SDL_Surface *pScreen, unsigned short code)
{
	unsigned	short	font[16];
	int	height,width;
	int font_color;
	int	i,j;

	i = theApp.menuEBook.getSetting(EBOOK_TEXT_COLOR);
	font_color = SDL_MapRGB(pScreen->format, color_index[i].r, color_index[i].g, color_index[i].b);

	GetFont(code,font,&height,&width);

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
	cursor = cursor - pScreen->w * height + width + theApp.menuEBook.getSetting(EBOOK_SPACE_PER_CHAR);
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::RenderOneLine_internal_font(SDL_Surface *pScreen, int y_pos, const unsigned short *uni)
{
	int	x_pos = theApp.menuEBook.getSetting(EBOOK_LEFT_MARGIN);
	const unsigned short *up = uni;
	unsigned short code;
	int	i;

	if(uni == NULL) return;

	SetPos(pScreen, x_pos, y_pos);

	while(*up) {
		code = *up++;
		if(code == 0x0d || code == 0x0a) {
			//continue;
		} else if(code == 0x09) {
			int	k;
			//i += TAB_LENGTH-1; //viewer->tab_length-1;
			for(k = 0; k < TAB_LENGTH; k++)
				DrawChar(pScreen, 0x20);
			//continue;
		} else {
			DrawChar(pScreen, code);
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_FL :
        	if(SeekMode)
				theApp.menuEBook.Scroll(EBOOK_PREV_LONG);
			else
				theApp.menuEBook.Scroll(EBOOK_PREV_PAGE);

			OnDraw(pScreen);
        	break;


        case VK_LEFT :

			if(ShowStatus)
			{

				menuPos -= 1;
				while(menuPos < 0)menuPos += 6;
				menuPos %= 6;
			}
			else
			{
	        	if(SeekMode)
					theApp.menuEBook.Scroll(EBOOK_PREV_LONG);
				else
					theApp.menuEBook.Scroll(EBOOK_PREV_PAGE);
			}

			OnDraw(pScreen);
			break;
		case VK_FR :


			if(SeekMode)
				theApp.menuEBook.Scroll(EBOOK_NEXT_LONG);
			else
				theApp.menuEBook.Scroll(EBOOK_NEXT_PAGE);

			OnDraw(pScreen);
			break;

        case VK_RIGHT :
			if(ShowStatus)
			{

				menuPos += 1;
				while(menuPos < 0)menuPos += 6;
				menuPos %= 6;

			}
			else
			{
				if(SeekMode)
					theApp.menuEBook.Scroll(EBOOK_NEXT_LONG);
				else
					theApp.menuEBook.Scroll(EBOOK_NEXT_PAGE);
			}

			OnDraw(pScreen);
			break;

		case VK_UP :
			theApp.menuEBook.Scroll(EBOOK_PREV_LINE);
			OnDraw(pScreen);
			break;
		case VK_DOWN :
			theApp.menuEBook.Scroll(EBOOK_NEXT_LINE);
			OnDraw(pScreen);
			break;
		case VK_FA 	:
			//theApp.SetView(EBOOK_SET_VIEW);
			SeekMode = true;
			break;
		case VK_FB:
		case VK_TAT:
			//theApp.SetView(EBOOK_MARK_VIEW);
			if(ShowStatus)
			{

				switch(menuPos)
				{
					case 0 :
						theApp.SetView(EBOOK_MARK_VIEW);
						break;

					case 1 :
						theApp.SetView(EBOOK_SET_VIEW);
						break;

					case 2 :
						theApp.SwapReverse();
						theApp.SetView(EBOOK_VIEW);
						break;
	    			case 3 :
						theApp.SwapReverse();
						if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
							theApp.SetView(MEDIA_PLAY_VIEW);
						else
							theApp.SetView(MOVIE_VIEW);
						break;
	    			case 4 :
						theApp.SwapReverse();
						if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
							theApp.SetView(MEDIA_PLAY_VIEW);
						else
							theApp.SetView(MUSIC_VIEW);
						break;
	    			case 5 :
						theApp.textvs = TEXT_OFF_STATUS;
						theApp.SwapReverse();

						//MP3 파일이 실행 중 이라면 mp3 플레이어 로...
						if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
							theApp.SetView(MEDIA_PLAY_VIEW);
						else
							theApp.SetView(MAIN_VIEW);


						break;

	    		}

			}

			break;
		case VK_FX :
			theApp.SwapReverse();
			theApp.SetView(EBOOK_VIEW);
			break;
		case VK_START :
			theApp.textvs = TEXT_OFF_STATUS;
			theApp.SwapReverse();
			//MP3 파일이 실행 중 이라면 mp3 플레이어 로...
			if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
				theApp.SetView(MEDIA_PLAY_VIEW);
			else
				theApp.SetView(MAIN_VIEW);
			break;
		case VK_SELECT :
			theApp.SwapReverse();
			if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
				theApp.SetView(MEDIA_PLAY_VIEW);
			else
				theApp.SetView(MUSIC_VIEW);
			break;
		case VK_VOL_UP :
			//SeekMode = true;
			break;
        case VK_FY :
            ShowStatus = !ShowStatus;
			//theApp.menuEBook.setSetting(EBOOK_LINE_PER_PAGE, (pScreen->h - (ShowStatus ? BOTTOM_MARGIN : 0)) / theApp.menuEBook.getSetting(EBOOK_LINE_HEIGHT));
			theApp.menuEBook.setSetting(EBOOK_LINE_PER_PAGE, (pScreen->h) / theApp.menuEBook.getSetting(EBOOK_LINE_HEIGHT));
		    OnDraw(pScreen);
            break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_SELECT :
			break;
		//case VK_VOL_UP :
		case VK_FA :
			SeekMode = false;
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
#define	STATUS_LINE_BG_R	230
#define	STATUS_LINE_BG_G	230
#define	STATUS_LINE_BG_B	230

#define	STATUS_LINE_FONT_R	0
#define	STATUS_LINE_FONT_G	0
#define	STATUS_LINE_FONT_B	0

void CEBookTextView::PutString(SDL_Surface *pScreen, int x, int y, char *str)
{
	// 유니코드용 저장 버퍼
	unsigned short dest[512];
	unsigned short *up = dest;

	// 파일명을 유니코드로 변경
#ifdef WIN32
	toUnicode(NULL, str, dest, strlen(str));
#else
	toUnicode("UTF-8", str, dest, strlen(str));
#endif

	unsigned short font[16];
	int	height,width;
	int font_color;
	int	i,j;

	//font_color = SDL_MapRGB(pScreen->format, STATUS_LINE_FONT_R, STATUS_LINE_FONT_G, STATUS_LINE_FONT_B);
	font_color = SDL_MapRGB(pScreen->format, 230, 230, 230);

	SetPos(pScreen, x, y);
	//while(*str) {
	while(*up) {
		//GetFont(*str++,font,&height,&width);
		GetFont(*up++,font,&height,&width);
		for	(i=0;i<height;i++)
		{
			unsigned short bit = font[i];
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
    }
}

void CEBookTextView::ShowStatusLine(SDL_Surface *pScreen)
{
	static char *setting = "Setting";
	static char *bookmark = "Bookmark";

	char buf[256];
	SDL_Rect srcrect, dstrect;

	srcrect.w = pImgProgressBar->w;
	srcrect.h = 3;
	srcrect.x = 30;
	srcrect.y = 30;

	//dstrect.x = 30;
	//dstrect.y = 30;

	//SDL_BlitSurface(pImgMenuFrame_up, &srcrect, pScreen, &dstrect);
	SDL_BlitSurface(pImgMenuFrame_up, NULL, pScreen, NULL);

	int current = theApp.menuEBook.getSetting(EBOOK_CURRENT_LINE) + theApp.menuEBook.getSetting(EBOOK_LINE_PER_PAGE);
	int total = theApp.menuEBook.getSetting(EBOOK_TOTAL_LINE);
	if(current > total) current = total;
    srcrect.w = current * (pImgProgressBar->w) / total;

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

	//파일명 출력
	char thisFileName[1024];
	strncpy(thisFileName, theApp.menuEBook.getName(), 1024);
	thisFileName[1023] = 0;
	//printf("thisFileName : %s\n",thisFileName);
	PutString(pScreen, 35, 8, thisFileName);

	//배터리 아이콘 로딩 시작
	theApp.getStatusRect(&dstrect);
	theApp.drawStatusIcon(pScreen, &dstrect);

	static const int menupos[6] = {16, 42, 68, 194, 232, 270};

	int i=0;
	int j=4;
	int k=0;

	for(i=0;i<6;i++)
	{
		if( i >= 3) //pImgDirectGo
		{
			srcrect.w = pImgDirectGo->w / 5;
			srcrect.h = pImgDirectGo->h / 2;
			srcrect.x = srcrect.w * j;
			if(menuPos == i)
			{
				srcrect.y = srcrect.h;

			}
			else
			{
				srcrect.y = 0;
			}

			j--;
		}
		else  //pImgMenuButton
		{

			srcrect.w = 21;
			srcrect.h = 21;
			if(menuPos == i)
			{
				srcrect.x =  0;

			}
			else
			{
				srcrect.x = 21;
			}

			srcrect.y = k * 21;


			k++;
		}

		dstrect.x = menupos[i];


		if(i >= 3)
			dstrect.y = pImgMenuFrame->h - 34; //pImgDirectGo
		else
			dstrect.y = pImgMenuFrame->h - 26; //pImgMenuButton


		if( i >= 3 ) //pImgDirectGo
		{

			SDL_BlitSurface(pImgDirectGo, &srcrect, pImgMenuFrame, &dstrect);

		}
		else
		{

			SDL_BlitSurface(pImgMenuButton, &srcrect, pImgMenuFrame, &dstrect);
		}


	}


	//메뉴 버튼 캡션 아이콘 출력(bookmark ,setting 등등)

	srcrect.w = pImgOptTitle->w;
	srcrect.h = pImgOptTitle->h/3;
	dstrect.x = 114;
	dstrect.y = 22;

	srcrect.x = dstrect.x;
	srcrect.y = dstrect.y;

	SDL_BlitSurface(pImgMenuFrame_cp, &srcrect, pImgMenuFrame, &dstrect); //일단 배경을 검은색(공백) 으로 지운다



	if( menuPos < 3 )
	{
		srcrect.x = 0;
		srcrect.y = menuPos * srcrect.h;

		SDL_BlitSurface(pImgOptTitle, &srcrect, pImgMenuFrame, &dstrect);
	}


	int m_width=0;
	//전체 라인 숫자 표시
	dstrect.x = 255;
	dstrect.y = 10;

	m_width=drawNumber(pImgMenuFrame, theApp.menuEBook.getSetting(EBOOK_TOTAL_LINE), dstrect.x, dstrect.y,1,0); //총 페이지 수가 (270,15)이 끝자락이 되도록 찍음


	//슬래쉬 (/) 표시
	srcrect.x=(pImgSmallFont->w/11)*10;
	srcrect.y=0;
	srcrect.w=(pImgSmallFont->w/11);
	srcrect.h=(pImgSmallFont->h/2);
	dstrect.x = dstrect.x-m_width-6;
	dstrect.y = dstrect.y;
	SDL_BlitSurface(pImgSmallFont, &srcrect, pImgMenuFrame, &dstrect);



	//현재 라인 숫자 표시 전에 표시될 부분 지워줌(업데이트)
	SDL_Rect updateRect;

	srcrect.x=0;
	srcrect.y=10;
	srcrect.w=dstrect.x;
	srcrect.h=(pImgSmallFont->h/2);
	updateRect.x = 0;
	updateRect.y = 10;

	//printf("srcrect.x : %d\n",srcrect.x);
	//printf("srcrect.y : %d\n",srcrect.y);
	//printf("dstrect.x : %d\n",dstrect.x);
	//printf("srcrect.w : %d\n",srcrect.w);
	//printf("srcrect.h : %d\n",srcrect.h);
	//printf("updateRect.x : %d\n",updateRect.x);
	//printf("updateRect.y : %d\n",updateRect.y);

	SDL_BlitSurface(pImgMenuFrame_cp, &srcrect, pImgMenuFrame, &updateRect);



	//현재 라인 숫자 표시
	dstrect.x = dstrect.x-6;
	dstrect.y = 10;

	m_width=drawNumber(pImgMenuFrame, theApp.menuEBook.getSetting(EBOOK_CURRENT_LINE), dstrect.x, dstrect.y,1,1); //현재 페이지 수를 (270-m_width-6-6,15)이 끝자락이 되도록 출력


	//메뉴 프레임을 메인 배경에 띄움(아래 프레임)
	dstrect.x = 10;dstrect.y = 183;
	SDL_BlitSurface(pImgMenuFrame, NULL, pScreen, &dstrect);



	/*
	rect.x = 0;
	rect.y = pScreen->h - BOTTOM_MARGIN;
	rect.w = pScreen->w;
	rect.h = pScreen->h - rect.y;

	SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, STATUS_LINE_BG_R, STATUS_LINE_BG_G, STATUS_LINE_BG_B));
	sprintf(buf,"%d/%d Lines",theApp.menuEBook.getSetting(EBOOK_CURRENT_LINE),theApp.menuEBook.getSetting(EBOOK_TOTAL_LINE));

	PutString(pScreen, pScreen->w - 4 - strlen((const char*)buf) * 6, 221, buf);
	PutString(pScreen, 12, 221, "A");
	PutString(pScreen, 30, 221, setting);
	PutString(pScreen, 102, 221, "B");
	PutString(pScreen, 120, 221, bookmark);

	srcrect.w = pImgButton->w >> 1;
	srcrect.h = pImgButton->h;
	srcrect.x = 0;srcrect.y = 0;
    rect.x = 6;rect.y = 215;
	SDL_BlitSurface(pImgButton, &srcrect, pScreen, &rect);
	srcrect.x = srcrect.w;
    rect.x = 96;rect.y = 215;
	SDL_BlitSurface(pImgButton, &srcrect, pScreen, &rect);
	*/
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::OnDraw(SDL_Surface *pScreen)
{
	int	i, v_pos;
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = pScreen->w;
	rect.h = pScreen->h;

	//if(ShowStatus)
    //	rect.h -= BOTTOM_MARGIN;

    int index = theApp.menuEBook.getSetting(EBOOK_BG_COLOR);

	SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, color_index[index].r, color_index[index].g, color_index[index].b));

	unsigned short *ptr = (unsigned short*)pScreen->pixels;
	int limit = theApp.menuEBook.getSetting(EBOOK_LINE_PER_PAGE);

	for(i=0,v_pos=theApp.menuEBook.getSetting(EBOOK_TOP_MARGIN); i < limit; i++,v_pos+=theApp.menuEBook.getSetting(EBOOK_LINE_HEIGHT)) {
		const unsigned short *line = theApp.menuEBook.readLine(i);
		switch(RenderOneLine) {
			case EBOOK_RENDER_INTERNAL:
				RenderOneLine_internal_font(pScreen, v_pos, line);
				break;
			case EBOOK_RENDER_EXTERNAL:
				RenderOneLine_external_font(pScreen, v_pos, line);
				break;
		}
	}

	if(ShowStatus)
    	ShowStatusLine(pScreen);
	SDL_UpdateRect(pScreen, 0, 0, 0, 0);
}
//[*]------------------------------------------------------------------------------------------[*]

int CEBookTextView::drawNumber(SDL_Surface *pScreen, int value, int x, int y, int len,short use)
{

	int width = 0;
	SDL_Rect srcrect, dstrect;

	srcrect.y = (use==0) ? 0:9;
	srcrect.w = 6;
	srcrect.h = 9;
	dstrect.y = y;

	while ((value > 0) || (len > 0)) {
		width += srcrect.w;
		dstrect.x = x - width;
		srcrect.x = (value % 10) * srcrect.w;
		SDL_BlitSurface(pImgSmallFont, &srcrect, pScreen, &dstrect);
		value /= 10;
		len--;
	}
	return width;
}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	//재생 바 바로가기 대응
	SDL_Rect rect;
	rect.w = pImgProgressBar->w;
	rect.h = 7+3+5;
	rect.y = 31-7;
	rect.x = 30;

	if(!ShowStatus)
	{
		if(x < 120)
		{
			if(SeekMode)
				theApp.menuEBook.Scroll(EBOOK_PREV_LONG);
			else
				theApp.menuEBook.Scroll(EBOOK_PREV_PAGE);
		}
		else if(x > 200)
		{
			if(SeekMode)
				theApp.menuEBook.Scroll(EBOOK_NEXT_LONG);
			else
				theApp.menuEBook.Scroll(EBOOK_NEXT_PAGE);
		}
		else
			ShowStatus = true;
		OnDraw(pScreen);
		return;
	}
	else if((x >= rect.x) && (x < (rect.x + rect.w)) && (y >= rect.y) && (y < (rect.y + rect.h)))
	{
		int total = theApp.menuEBook.getSetting(EBOOK_TOTAL_LINE);
		int jump = (total * (x - rect.x) / rect.w) - theApp.menuEBook.getSetting(EBOOK_LINE_PER_PAGE);
		if (jump < 0) jump = 0;
		theApp.menuEBook.setSetting(EBOOK_CURRENT_LINE, jump);
		OnDraw(pScreen);
		return;
	}


	if(y < (rect.y + rect.h)) return;

	int i, w, h, px, py, frame_gap_x, frame_gap_y, frameW, frameH, frameX, frameY;
	bool pushButton=false;
	//아래 메뉴 프레임 위치
	frameW = pImgMenuFrame->w;
	frameH = pImgMenuFrame->h;
	frame_gap_x=10;
	frame_gap_y=183;

	frameX = frame_gap_x;
	frameY = frame_gap_y;


	if((x >= frameX) && (x < (frameX+frameW)) && (y >= frameY) && (y < (frameY+frameH)))
	{
		//메뉴 아이콘들
		static const int menupos[6] = {16, 42, 68, 194, 232, 270};

		for(i=0;i<6;i++)
		{
			if( i >= 3) //pImgDirectGo
			{
				w = pImgDirectGo->w / 5;
				h = pImgDirectGo->h / 2;


			}
			else  //pImgMenuButton
			{

				w = pImgMenuButton->w / 2;
				h = pImgMenuButton->h / 3;

			}

			px = menupos[i] + frame_gap_x;                   //10을 더하는 이유는 아래 메뉴 프레임이 화면의 (10,183)에 위치 하기 때문이다


			if(i >= 3)
				py = YRES - 34;     //pImgDirectGo   //183을 더하는 이유는 아래 메뉴 프레임이 화면의 (10,183)에 위치 하기 때문이다
			else
				py = YRES - 26;     //pImgMenuButton //183을 더하는 이유는 아래 메뉴 프레임이 화면의 (10,183)에 위치 하기 때문이다


			if( (x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)) && ShowStatus )
			{
				menuPos = i;
				OnDraw(pScreen);
				OnJoystickDown(pScreen, VK_FB);
				pushButton=true;

				break;
			}
		}
	}
	else
	{
		ShowStatus = false;
		OnDraw(pScreen);
		return;
	}

}
//[*]------------------------------------------------------------------------------------------[*]
void CEBookTextView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{


}
//[*]------------------------------------------------------------------------------------------[*]
