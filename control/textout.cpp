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
#include <iconv.h>
#include <string.h>
#include <UnicodeFont.h>
#include <SDL/SDL_ttf.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "textout.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
int toUnicode(const char *from_code, const char *src, unsigned short *target, int len)
{
	iconv_t cd 		= 0;
	char 	*from	= NULL;
	char 	*to		= NULL;
	int 	flen	= 0;
	int 	tlen	= 0;
	size_t 	result 	= 0;

	// iconv_open
	if(from_code == NULL) {
		cd = iconv_open("UNICODELITTLE", theApp.cfgCharset.c_str());
	} else {
		cd = iconv_open("UNICODELITTLE", from_code);
	}
	if(cd) {
		flen 	= len;
		tlen 	= len * 2;
    	to 		= (char *)target;
#ifdef WIN32
		result 	= iconv(cd, &src, (size_t*)&flen, &to, (size_t*)&tlen);
#else
        from = (char*)src;
		result 	= iconv(cd, &from, (size_t*)&flen, &to, (size_t*)&tlen);
#endif
		*(unsigned short*)to='\0';

		iconv_close(cd);

		return len-tlen;
    } else {
		*target = '\0';
        return 0;
    }
}
//[*]------------------------------------------------------------------------------------------[*]
int toUTF8(const char *from_code, const char *src, unsigned char *target, int len)
{
	iconv_t cd 		= 0;
	char 	*from	= NULL;
	char 	*to		= NULL;
	int 	flen	= 0;
	int 	tlen	= 0;
	size_t 	result 	= 0;

	// iconv_open
	if(from_code == NULL) {
		cd = iconv_open("UTF-8", theApp.cfgCharset.c_str());
	} else {
		cd = iconv_open("UTF-8", from_code);
	}
	if(cd) {
		flen 	= len;
		tlen 	= len * 4;
    	to 		= (char *)target;
#ifdef WIN32
		result 	= iconv(cd, &src, (size_t*)&flen, &to, (size_t*)&tlen);
#else
        from = (char*)src;
		result 	= iconv(cd, &from, (size_t*)&flen, &to, (size_t*)&tlen);
#endif
		*to = '\0';

		iconv_close(cd);

		return len-tlen;
    } else {
        return 0;
    }
}
//[*]------------------------------------------------------------------------------------------[*]
// UTF8을 UNICODE로 변경한다.
unsigned short* OnUTF8ToUnicode(const char *src, int last)
{
	int len = 0;	// 길이
	unsigned short *dest = NULL;

	// 유니 코드는 무조건 2자리 이므로 UTF-8의 2배를 구해야 안전하다.
	len	= (strlen(src)+1)*2;
	dest = (unsigned short*)malloc(len);
	memset((char*)dest, 0, len);			// 0으로 반드시 초기화 해 줘야 한다.

	// UTF8을 UNICODE로 변경한다.
	toUnicode("UTF-8", src, dest, strlen(src)-last);

	return dest;
}
//[*]------------------------------------------------------------------------------------------[*]
void SetPixel(SDL_Surface *pScreen, int x, int y, Uint8 r, Uint8 g, Uint8 b,int size)
{
	Uint8 *ubuff8;
	Uint16 *ubuff16;
	Uint32 *ubuff32;
	Uint32 color;

	Uint8 c1;
	Uint8 c2;
	Uint8 c3;

	color = SDL_MapRGB(pScreen->format, r, g, b );

	/* How we draw the pixel depends on the bitdepth */
	switch(pScreen->format->BytesPerPixel)
	{
		case 1:
			ubuff8 = (Uint8*)pScreen->pixels;
			//ubuff8 += (y * pScreen->pitch) + x;
			ubuff8 += (y * pScreen->pitch) + (x*size);
			*ubuff8 = (Uint8)color;
			break;

		case 2:
			ubuff8 = (Uint8*)pScreen->pixels;
			//ubuff8 += (y * pScreen->pitch) + (x*2);
			ubuff8 += (y * pScreen->pitch) + (x*size*2);

			ubuff16 = (Uint16*)ubuff8;
			*ubuff16 = (Uint16)color;
			break;

		case 3:
			ubuff8 = (Uint8*)pScreen->pixels;
			//ubuff8 += (y * pScreen->pitch) + (x*3);
			ubuff8 += (y * pScreen->pitch) + (x*size*3);

			if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
			{
				c1 = (color & 0xFF0000) >> 16;
				c2 = (color & 0x00FF00) >> 8;
				c3 = (color & 0x0000FF);
			}
			else
			{
				c3 = (color & 0xFF0000) >> 16;
				c2 = (color & 0x00FF00) >> 8;
				c1 = (color & 0x0000FF);
			}

			ubuff8[0] = c3;
			ubuff8[1] = c2;
			ubuff8[2] = c1;
			break;

		case 4:
			ubuff8 = (Uint8*)pScreen->pixels;
			//ubuff8 = (Uint8*)((y*pScreen->pitch) + (x*4));
			ubuff8 = (Uint8*)((y*pScreen->pitch) + (x*size*4));
			ubuff32 = (Uint32*)ubuff8;
			*ubuff32 = color;
			break;

		default:
			fprintf(stderr, "Error: Unknown bitdepth!\n");
	}
}
//[*]------------------------------------------------------------------------------------------[*]
bool isUTF8(const unsigned char *str)
{
	if((str[0] == 0xEF) && (str[1] == 0xBB) && (str[2] == 0xBF))
	    return true;

	const unsigned char *p = str;
	int i, n;
	int weight = 0;

	while(*p && (weight < 100))
	{
		if((*p == 0xC0) || (*p == 0xC1) || (*p >= 0xF5))
		    return false;

		if(*p < 0x80) n = 0; // 0bbbbbbb
		else if((*p & 0xE0) == 0xC0) n = 1; // 110bbbbb
		else if((*p & 0xF0) == 0xE0) n = 2; // 1110bbbb
		else if((*p & 0xF8) == 0xF0) n = 3; // 11110bbb
		else if((*p & 0xFC) == 0xF8) n = 4; // 111110bb
		else if((*p & 0xFE) == 0xFC) n = 5; // 1111110b
		else
		    return false;

		++p;
		for (i = 0; i < n; ++i) // n bytes matching 10bbbbbb follow ?
		{
			if (!*p || ((*p & 0xC0) != 0x80))
			    return false;
			++p;
		}

		if(n > 2) weight += (n - 2);
	}

	return true;
}
//[*]------------------------------------------------------------------------------------------[*]
int DrawFont(SDL_Surface *pScreen, int x, int y, unsigned short code, Uint8 r, Uint8 g, Uint8 b, int size)
{
	unsigned short fontdata[12];
	int height = 0, width = 0;

	GetFont(code, fontdata, &height, &width);

	for(int cx=0; cx<width; cx++)
	{
		for(int cy=0; cy<height; cy++)
		{
			if(fontdata[cy] & (0x8000 >> cx))
			{
				SetPixel(pScreen, cx + x, cy + y, r, g, b, size);
			}
		}
	}

	return width;
}
//[*]------------------------------------------------------------------------------------------[*]
void DrawText(SDL_Surface *pScreen, int x, int y, unsigned short *code, int lencode, Uint8 r, Uint8 g, Uint8 b)
{
	int pos = 0;

	// NULL일떄 까지 인쇄한다.
	if(lencode == -1)
	{
		for(int i=0; code[i]; i++)
			pos += DrawFont(pScreen, x+pos, y, code[i], r, g, b);
	}
	// 갯수 만큼 인쇄한다.
	else
	{
		for(int i=0; i<lencode; i++)
			pos += DrawFont(pScreen, x+pos, y, code[i], r, g, b);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void DrawTextOut(SDL_Surface *pScreen, int x, int y, int scale, unsigned short *code, Uint8 r, Uint8 g, Uint8 b,int size)
{
	int pos		= 0;
	int cnt 	= 0;
	int total 	= 0;
	int height 	= 0;
	int width 	= 0;
	unsigned short fontdata[12] = { 0, };

	// 총 길이를 구한다.
	for(int i=0; code[i]; i++)
	{

		GetFont(code[i], fontdata, &height, &width);		// 총길이를 구한다.
		total += width;
		if(total > scale-18)
		{
			if(cnt == 0)
			{
				cnt = i-1;								// 현재 인쇄될 길이 보다 길 경우 위치를 기억해서 그 이전까지만 인쇄한다.

			}
		}
	}
	//printf("total : %d\n",total);
	//printf("scale : %d\n",scale);
	//printf("cnt_tot  : %d\n\n\n",cnt);

	//x=(XRES/2)-(total/2); 정렬방식

	if(total < scale)
	{
		for(int i=0; code[i]; i++)
			pos += DrawFont(pScreen, x+pos, y, code[i], r, g, b, size);
	}
	else
	{
		for(int i=0; i<cnt; i++)
			pos += DrawFont(pScreen, x+pos, y, code[i], r, g, b, size);
		pos += DrawFont(pScreen, x+pos, y, '.', r, g, b, size);
		pos += DrawFont(pScreen, x+pos, y, '.', r, g, b, size);
		pos += DrawFont(pScreen, x+pos, y, '.', r, g, b, size);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void DrawTitle(SDL_Surface *pScreen, int x, int y, unsigned short *code, Uint8 r, Uint8 g, Uint8 b)
{
	int pos		= 0;
	int total 	= 0;
	int height 	= 0;
	int width 	= 0;
	int draw	= 0;
	unsigned short fontdata[12] = { 0, };

	// 총 길이를 구한다.
	for(int i=0; code[i]; i++)
	{
#ifdef GP2X
		GetFont(code[i], NULL, &height, &width);		// 총길이를 구한다.
#else
		GetFont(code[i], fontdata, &height, &width);		// 총길이를 구한다.
#endif
		x -= width;
	}

	if(x < 0) x = 0;

	for(int i=0; code[i]; i++)
		pos += DrawFont(pScreen, x+pos, y, code[i], r, g, b);
}
//[*]------------------------------------------------------------------------------------------[*]
void PrintMessage(SDL_Surface *pScreen, int x, int y, const char *string, unsigned char r, unsigned char g, unsigned char b)
{
   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
	dest = OnUTF8ToUnicode(string, 0);
	DrawText(pScreen, x, y, dest, -1, r, g, b);
	free(dest);
}
//[*]------------------------------------------------------------------------------------------[*]
void PrintKorMessage(SDL_Surface *pScreen, int x, int y, const char *string, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned short dest[512];
	toUnicode("UHC//IGNORE", string, dest, strlen(string));
	DrawText(pScreen, x, y, dest, -1, r, g, b);
	free(dest);
}
//[*]------------------------------------------------------------------------------------------[*]
static TTF_Font *ttf_font = NULL;
static int font_height;
static signed char font_width[0x10000];
//[*]------------------------------------------------------------------------------------------[*]
bool changeFont(const char *file, int ptsize, int index)
{
	if (!TTF_WasInit() && (TTF_Init() == -1)) {
		printf("TTF_Init: %s\n", TTF_GetError());
		return false;
	}

	if (ttf_font != NULL) {
		TTF_CloseFont(ttf_font);
	}

	ttf_font = NULL;
	memset(font_width, 0xFF, sizeof(font_width));

	if (file && (access(file, F_OK) == 0))
		ttf_font = TTF_OpenFontIndex(file, ptsize, index);

	if (ttf_font) {
		//TTF_SetFontStyle(sub_font, TTF_STYLE_BOLD);
		font_height = ptsize;
		printf("Font Num Faces: %d\n", TTF_FontFaces(ttf_font));
		printf("Font Family Name: %s\n", TTF_FontFaceFamilyName(ttf_font));
		printf("Font Style Name: %s\n", TTF_FontFaceStyleName(ttf_font));
	}
	else
	{
		font_height = 12;
	}

	return ttf_font != NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
int getFontIndexCount()
{
	return TTF_FontFaces(ttf_font);
}
//[*]------------------------------------------------------------------------------------------[*]
const char* getFontName()
{
	return TTF_FontFaceFamilyName(ttf_font);
}
//[*]------------------------------------------------------------------------------------------[*]
int _drawFont(SDL_Surface *pScreen, int x, int y, unsigned short code, unsigned short color, bool useTTF, int *width, int *height)
{
	unsigned short *buf = (unsigned short *) pScreen->pixels;
	int cx = 0;
	int cy = 0;
	int xres = pScreen->w;
	buf += x + (y + 1) * xres;

	if (useTTF && ttf_font) {
		unsigned short text[2] = { code, 0 };
		unsigned short *fontptr;
		SDL_Color fg, bg;
		SDL_Surface *font;

		fg.r = (color & 0xf800) >> 8;
		fg.g = (color & 0x07e0) >> 3;
		fg.b = (color & 0x001f) << 3;
		bg.r = 0xC8;
		bg.g = 0xCC;
		bg.b = 0xC8;
		font = TTF_RenderUNICODE_Solid(ttf_font, text, fg);
		//font = TTF_RenderUNICODE_Shaded(ttf_font, text, fg, bg);
		//font = TTF_RenderUNICODE_Blended(ttf_font, text, fg);
		//font = TTF_RenderGlyph_Solid(sub_font, code, fg);
		//font = TTF_RenderGlyph_Shaded(sub_font, code, fg, bg);

		if (!font) {
			*width = 0;
			*height = 0;	//sub_font->height;
			printf("TTF_Render: %s\n", TTF_GetError());
		} else {
			*width = font->w;
			*height = font->h;
		}

		if ((width > 0) && (height > 0)) {
			SDL_Rect rect = {x, y};
			SDL_BlitSurface(font, NULL, pScreen, &rect);
			/*
			SDL_Surface *draw = SDL_ConvertSurface(font, pScreen->format, SDL_SWSURFACE);
			int fontskip = (draw->pitch >> 1) - *width;
			int bufskip = xres - *width;
			if (draw) {
				fontptr = (unsigned short *) draw->pixels;

				for (cy = 0; cy < *height; cy++) {
					for (cx = 0; cx < *width; cx++, fontptr++, buf++) {
						if (*fontptr)
							*buf = *fontptr;
					}
					fontptr += fontskip;
					buf += bufskip;
				}
				SDL_FreeSurface(draw);
			}
			*/
		}

		if (font)
			SDL_FreeSurface(font);
	} else {
		unsigned short fontdata[12];

		GetFont(code, fontdata, height, width);
		if ((width > 0) && (height > 0)) {
			for (cy = 0; cy < *height; cy++) {
				for (cx = 0; cx < *width; cx++) {
					if (fontdata[cy] & (0x8000 >> cx)) {
						buf[cx] = color;
					}
				}
				buf += xres;
			}
		}
	}

	return *width;
}
//[*]------------------------------------------------------------------------------------------[*]
int getFontSize(unsigned short code, bool useTTF, int *width, int *height)
{
	if (useTTF && (font_width[code] != -1))
	{
		*width = font_width[code];
		*height = font_height;
	}
	else if (useTTF && ttf_font)
	{
		unsigned short text[2] = { code, 0 };
		if (TTF_SizeUNICODE(ttf_font, text, width, height) < 0)
			*width = 0;
		font_width[code] = *width;
	}
	else
	{
		unsigned short fontdata[12];
		GetFont(code, fontdata, height, width);
		font_width[code] = *width;
	}

	if(font_height < *height)
	    font_height = *height;

	return *width;
}
//[*]------------------------------------------------------------------------------------------[*]
Uint16 *U8toU16(Uint16 * unicode, Uint8 * utf8, int len)
{
	int i, j;
	Uint16 ch;

	for (i = 0, j = 0; i < len; ++i, ++j) {
		ch = ((const unsigned char *) utf8)[i];
		if (ch >= 0xF0) {
			ch = (Uint16) (utf8[i] & 0x07) << 18;
			ch |= (Uint16) (utf8[++i] & 0x3F) << 12;
			ch |= (Uint16) (utf8[++i] & 0x3F) << 6;
			ch |= (Uint16) (utf8[++i] & 0x3F);
		} else if (ch >= 0xE0) {
			ch = (Uint16) (utf8[i] & 0x0F) << 12;
			ch |= (Uint16) (utf8[++i] & 0x3F) << 6;
			ch |= (Uint16) (utf8[++i] & 0x3F);
		} else if (ch >= 0xC0) {
			ch = (Uint16) (utf8[i] & 0x1F) << 6;
			ch |= (Uint16) (utf8[++i] & 0x3F);
		}
		unicode[j] = ch;
	}
	unicode[j] = 0;

	return unicode;
}
//[*]------------------------------------------------------------------------------------------[*]
