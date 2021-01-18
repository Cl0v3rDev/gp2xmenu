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
#ifndef FONTDISPLAY_H_
#define FONTDISPLAY_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
int toUnicode(const char *from_code, const char *src, unsigned short *target, int len);
int toUTF8(const char *from_code, const char *src, unsigned char *target, int len);
bool isUTF8(const unsigned char *str);

int DrawFont(SDL_Surface *pScreen, int x, int y, unsigned short code, Uint8 r, Uint8 g, Uint8 b,int size=1);
void SetPixel(SDL_Surface *pScreen, int x, int y, Uint8 r, Uint8 g, Uint8 b,int size=1);
void DrawText(SDL_Surface *pScreen, int x, int y, unsigned short *code, int lencode, Uint8 r, Uint8 g, Uint8 b);
void DrawTextOut(SDL_Surface *pScreen, int x, int y, int scale, unsigned short *code, Uint8 r, Uint8 g, Uint8 b,int size=1);
void DrawTitle(SDL_Surface *pScreen, int x, int y, unsigned short *code, Uint8 r, Uint8 g, Uint8 b);
void PrintMessage(SDL_Surface *pScreen, int x, int y, const char *string, unsigned char r, unsigned char g, unsigned char b);
void PrintKorMessage(SDL_Surface *pScreen, int x, int y, const char *string, unsigned char r, unsigned char g, unsigned char b);
unsigned short* OnUTF8ToUnicode(const char *src, int last);

bool changeFont(const char *file, int ptsize, int index);
int _drawFont(SDL_Surface *pScreen, int x, int y, unsigned short code, unsigned short color, bool useTTF, int *width, int *height);
int getFontSize(unsigned short code, bool useTTF, int *width, int *height);
Uint16 *U8toU16(Uint16 * unicode, Uint8 * utf8, int len);
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
