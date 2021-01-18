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
#include <inifile.h>
#include <iconv.h>
#include <UnicodeFont.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "ebookreader.h"
#include "mainmenuapp.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
EBookReader::EBookReader()
{
	viewport.text_color_index	= 0;
	viewport.bg_color_index		= 1;
	viewport.text_width			= 6;
	viewport.text_height		= 12;
	viewport.orientation		= 0;
	viewport.use_variable_width	= 0;

	viewport.top_margin			= 0;
	viewport.bottom_margin		= 0;
	viewport.left_margin		= 0;
	viewport.space_per_line		= 4;
	viewport.space_per_char		= 0;
	viewport.tab_length			= 4;

	viewport.current_lines		= 0;

	viewport.line_per_page = (240-viewport.bottom_margin) / (viewport.text_height+viewport.space_per_line);

	viewport.char_per_line = (320-viewport.left_margin)/(viewport.text_width+viewport.space_per_char);
	viewport.font[0]='\0';

	if	(LoadBookmark()) {
		viewport.text_color_index=bookmark.font_color_index;
		viewport.bg_color_index=bookmark.bg_color_index;
        strcpy(viewport.font, bookmark.font);
	} else {
        InitBookmark();
    }

    path = name = "";
}
//[*]------------------------------------------------------------------------------------------[*]
EBookReader::~EBookReader()
{
	if(text.textptr != NULL) delete [] text.textptr;
	text.textptr = NULL;
	loaded = "";
	SaveBookmark();
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::setPath(const char *value)
{
	char *str, *sp;
	int len = strlen(value);
	str = new char[len + 1];
	strcpy(str, value);
	sp = str + len - 1;
	while((*sp == '/') || (*sp == '\\')) sp--;
	path = str;
	delete [] str;
}
//[*]------------------------------------------------------------------------------------------[*]
const char* EBookReader::getPath()
{
	return path.c_str();
}
//[*]------------------------------------------------------------------------------------------[*]
const char* EBookReader::getName()
{
	return name.c_str();
}
//[*]------------------------------------------------------------------------------------------[*]
int EBookReader::CalcOneLine(char *CurrentPtr, int &LineIncrement)
{
#ifdef MULTILANG
	int maxwidth = viewport.char_per_line * 6;

	iconv_t cd = iconv_open("UNICODELITTLE", theApp.cfgCharset.c_str());
	unsigned short uni[2];
	char *pBuffer = CurrentPtr;
	int width, height;
	int length = 0;

	size_t slen, ulen, ret;
#ifdef WIN32
	const char *sp;
#else
	char *sp;
#endif
	char *up;
	unsigned short font[16];

	LineIncrement = 0;

	if(cd != (iconv_t)-1) {
		while(*pBuffer != '\0') {
			if((*pBuffer == CODE_RETURN) || (*pBuffer == CODE_CARRIAGE)){
				char code = *pBuffer++;
				LineIncrement++;
				if((code != *pBuffer) && ((*pBuffer == CODE_RETURN) || (*pBuffer == CODE_CARRIAGE))) {
					pBuffer++;
					LineIncrement++;
				}
				break;
			}

			if(*pBuffer == CODE_TAB) {
				pBuffer++;
				LineIncrement++;
				length += 6 * TAB_LENGTH;
				continue;
			}

			sp = (char *)pBuffer;
			up = (char *)uni;
			slen = 16;
			ulen = 2;

			ret = iconv(cd, &sp, &slen, &up, &ulen);
			if(ulen != 0) {
				// ignore one char...
				*pBuffer = '?';
				uni[0] = '?';
				slen = 15;
				// skip line...
				//pBuffer += 1;
				//LineIncrement += 1;
				//break;
			}
#ifdef WIN32
			GetFont(uni[0], font, &height, &width);
#else
			GetFont(uni[0], NULL, &height, &width);
#endif
			if(length + width > maxwidth) break;

			length += width;
			pBuffer += 16 - slen;
			LineIncrement += 16 - slen;
		}

		iconv_close(cd);
    }

    return pBuffer - CurrentPtr;
#else // MULTILANG
	int maxwidth = viewport.char_per_line;

	int Count  = 0;
	int Length = 0;
	int	codetype = CODE_NONE;

	char *pBuffer = CurrentPtr;

	while(*pBuffer != '\0')
	{
		char code = *pBuffer++;
		if (code & 0x80)
			codetype = (codetype != CODE_HEAD) ? CODE_HEAD : CODE_TAIL;
		else
			codetype = CODE_TAIL;

		if(code == CODE_TAB) {
			int Skip = TAB_LENGTH;
			if(Skip > (maxwidth - Length))
				Skip = (maxwidth - Length);

			Length += (viewport.tab_length - 1);
		}
		else if(code == CODE_RETURN) {
			LineIncrement = Length + 1;
			return (Count + 1);
		}

		Count++;
		if(code == CODE_CARRIAGE) continue;
		if(code == CODE_RETURN) continue;
		Length++;

		if(Length > (maxwidth - 1))
		{
			if(codetype == CODE_HEAD)
			{
				Length--;
				Count--;
			}

			break;
		}
	}

	LineIncrement = Length;

	return Count;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::MakeLineInfo()
{
	int	FilePtrIncrement;
	int	LineIncrement;
	char *CurrentPtr=text.textptr;

	text.lineptr.clear();
	text.linelen.clear();
	while(*CurrentPtr!=0)
	{
		FilePtrIncrement = CalcOneLine(CurrentPtr, LineIncrement);
#ifndef GP2X
		printf("Make Line: %x, %d, %d\n", CurrentPtr - text.textptr, LineIncrement, FilePtrIncrement);
#endif
		if(LineIncrement) {
			text.lineptr.push_back(CurrentPtr);
			text.linelen.push_back(LineIncrement);
		}
		CurrentPtr += FilePtrIncrement;
	}
	text.total_lines = text.lineptr.size();
}
//[*]------------------------------------------------------------------------------------------[*]
bool EBookReader::TextLoad(const char *filename)
{
	FILE *fp = NULL;
	char *fileptr = NULL;
	unsigned long filesize = 0;
	char cutname[512];
	char cutpath[512];

	const char *cut = strrchr(filename, '/');
	if(cut == NULL) cut = strrchr(filename, '\\');
	if(cut == NULL) {
		strcpy(cutname, filename);
		strcpy(cutpath, path.c_str());
	} else {
		strcpy(cutname, cut + 1);
		strncpy(cutpath, filename, cut - filename);
		cutpath[cut - filename] = 0;
	}
	
	std::string file = std::string(cutpath);
	file += "/";
	file += cutname;

#ifdef DEBUG
	printf("Load Text : %s\n", file.c_str());
	printf("Path : %s\n", cutpath);
	printf("Name : %s\n", cutname);
#endif
	if (text.textptr && (loaded == file)) {
		//viewport.current_lines = bookmark.lastfile.lastline;
		return true;
	} else {
	 	fp = fopen(file.c_str(), "rb");
		if	(fp!=NULL) {
			if(text.textptr != NULL) {
				delete [] text.textptr;
				text.textptr = NULL;
				loaded = "";
			}

			fseek(fp, 0, SEEK_END);
			filesize = ftell( fp );
			fseek(fp, 0, SEEK_SET);

			text.textptr = new char[filesize+2];
			fread(text.textptr, 1, filesize, fp);

			*(text.textptr+filesize) = '\n';
			*(text.textptr+filesize+1) = '\0';

			fclose(fp);

			MakeLineInfo();

			path = cutpath;
			name = cutname;
			loaded = file;
			if(strcmp(bookmark.lastfile.name, file.c_str()) == 0) {
				viewport.current_lines = bookmark.lastfile.lastline;
			} else {
				strcpy(bookmark.lastfile.name, file.c_str());
				viewport.current_lines = 0;
			}

			return true;
		} else {
			path = cutpath;
			name = "";

			return false;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::clear()
{
	if(text.textptr != NULL) {
		delete [] text.textptr;
		text.textptr = NULL;
		loaded = "";
	}
	SaveBookmark();
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::Scroll(int factor)
{
	switch(factor) {
		case EBOOK_PREV_LONG:
			viewport.current_lines -= 1000;
			break;
		case EBOOK_PREV_PAGE:
			viewport.current_lines -= viewport.line_per_page;
			break;
		case EBOOK_PREV_LINE:
			viewport.current_lines--;
			break;
		case EBOOK_NEXT_LINE:
			viewport.current_lines++;
			break;
		case EBOOK_NEXT_PAGE:
			viewport.current_lines += viewport.line_per_page;
			break;
		case EBOOK_NEXT_LONG:
			viewport.current_lines += 1000;
			break;
	}
	if(viewport.current_lines + viewport.line_per_page >= text.total_lines)
		viewport.current_lines = text.total_lines - viewport.line_per_page;
	if(viewport.current_lines < 0)
		viewport.current_lines = 0;
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::toUnicode(unsigned short *uni, const char *src, int len)
{
	iconv_t cd = iconv_open("UNICODELITTLE", theApp.cfgCharset.c_str());
	uni[0] = 0;

	if(cd != (iconv_t)-1) {
		size_t slen, ulen;
#ifdef WIN32
        const char *sp;
#else
        char *sp;
#endif
		char *up;

		sp = (char *)src;
		up = (char *)uni;
		slen = len;
		ulen = 512;

		if(iconv(cd, &sp, &slen, &up, &ulen) != (size_t) (-1))
			*(unsigned short *)up = 0;
		iconv_close(cd);
    }
}
//[*]------------------------------------------------------------------------------------------[*]
int	EBookReader::InitBookmark()
{
	bookmark.lastfile.name[0]='\0';
	bookmark.lastfile.total_lines=0;
	bookmark.lastfile.lastline=0;

	int	i;
	for	(i=0;i<25;i++);
	{
		bookmark.markedfile[i].name[0]='\0';
		bookmark.markedfile[i].total_lines=0;
		bookmark.markedfile[i].lastline=0;
	}

	strcpy(bookmark.font,viewport.font);
	bookmark.font_color_index=viewport.text_color_index;
	bookmark.bg_color_index=viewport.bg_color_index;

	printf("Bookmark Initialize...\n");

	return	1;
}
//[*]------------------------------------------------------------------------------------------[*]
int	EBookReader::LoadBookmark()
{
	INI_Open(INI_PATH"/ebook.ini");

	bookmark.font_color_index = INI_ReadInt("color", "font", viewport.text_color_index);
	bookmark.bg_color_index = INI_ReadInt("color", "bg", viewport.bg_color_index);
	strcpy(bookmark.font, INI_ReadText("font", "name", viewport.font));

	bookmark.lastfile.lastline = INI_ReadInt("last", "last", 0);
	bookmark.lastfile.total_lines = INI_ReadInt("last", "total", 0);
	strcpy(bookmark.lastfile.name, INI_ReadText("last", "name", ""));

	int i;
	for(i=0;i<25;i++) {
		char chapter[32];
		sprintf(chapter, "mark%d", i);

		bookmark.markedfile[i].lastline = INI_ReadInt(chapter, "last", 0);
		bookmark.markedfile[i].total_lines = INI_ReadInt(chapter, "total", 0);
		strcpy(bookmark.markedfile[i].name, INI_ReadText(chapter, "name", ""));
	}

	INI_Close();

	return 1;
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::SaveBookmark()
{
#ifdef DEBUG
	printf("Bookmark Save: %d, %d, %s\n", viewport.text_color_index, viewport.bg_color_index, viewport.font);
#endif
	INI_Open(INI_PATH"/ebook.ini");
	INI_WriteInt("color", "font", viewport.text_color_index);
	INI_WriteInt("color", "bg", viewport.bg_color_index);
	INI_WriteText("font", "name", viewport.font);

	INI_WriteInt("last", "last", viewport.current_lines);
	INI_WriteInt("last", "total", text.total_lines);
	INI_WriteText("last", "name", bookmark.lastfile.name);
	bookmark.lastfile.lastline = viewport.current_lines;
	bookmark.lastfile.total_lines = text.total_lines;

	int i;
	for(i=0;i<25;i++) {
		char chapter[32];
		sprintf(chapter, "mark%d", i);

		INI_WriteInt(chapter, "last", bookmark.markedfile[i].lastline);
		INI_WriteInt(chapter, "total", bookmark.markedfile[i].total_lines);
		INI_WriteText(chapter, "name", bookmark.markedfile[i].name);
	}

	INI_Close();

	sync();
}
//[*]------------------------------------------------------------------------------------------[*]
bool EBookReader::RecallBookmark(int no)
{
	if(bookmark.markedfile[no].name[0] && TextLoad(bookmark.markedfile[no].name)) {
		viewport.current_lines = bookmark.markedfile[no].lastline;
		return true;
	}
	return false;
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::MarkBookmark(int no)
{
	strcpy(bookmark.markedfile[no].name, bookmark.lastfile.name);
	bookmark.markedfile[no].lastline = viewport.current_lines;
	bookmark.markedfile[no].total_lines = text.total_lines;
	SaveBookmark();
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::DeleteBookmark(int no)
{
	//현재 행만 지움
	//strcpy(bookmark.markedfile[no].name, "");
	//bookmark.markedfile[no].lastline = 0;
	//bookmark.markedfile[no].total_lines = 0;






	//read form file (지운 행 이후 모든 레코드를 하나 전 행으로 이동)
	INI_Open(INI_PATH"/ebook.ini");

	int i;
	for(i=no+1;i<25;i++)
	{
		char chapter[32];
		sprintf(chapter, "mark%d", i);

		bookmark.markedfile[i-1].lastline = INI_ReadInt(chapter, "last", 0);
		bookmark.markedfile[i-1].total_lines = INI_ReadInt(chapter, "total", 0);
		strcpy(bookmark.markedfile[i-1].name, INI_ReadText(chapter, "name", ""));
	}
	INI_Close();

	//맨 마지막 번째 값(기본 24) 0으로 채움
	bookmark.markedfile[24].lastline = 0;
	bookmark.markedfile[24].total_lines = 0;
	strcpy(bookmark.markedfile[24].name, "");





	SaveBookmark();
}
//[*]------------------------------------------------------------------------------------------[*]
const char* EBookReader::getBookmarkName(int no)
{
	return bookmark.markedfile[no].name;
}
//[*]------------------------------------------------------------------------------------------[*]
int EBookReader::getBookmarkInfo(int no, int type)
{
	switch (type) {
		case EBOOK_CURRENT_LINE:
			return bookmark.markedfile[no].lastline;
		case EBOOK_TOTAL_LINE:
			return bookmark.markedfile[no].total_lines;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
const char* EBookReader::getFont()
{
	return viewport.font;
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::setFont(const char *font)
{
	strcpy(viewport.font, font);
}
//[*]------------------------------------------------------------------------------------------[*]
int EBookReader::getSetting(int type)
{
	switch (type) {
		case EBOOK_CURRENT_LINE:
			return viewport.current_lines;
		case EBOOK_TOTAL_LINE:
			return text.total_lines;
		case EBOOK_TEXT_COLOR:
			return viewport.text_color_index;
		case EBOOK_BG_COLOR:
			return viewport.bg_color_index;
		case EBOOK_LEFT_MARGIN:
		case EBOOK_TOP_MARGIN:
		case EBOOK_SPACE_PER_CHAR:
			return viewport.space_per_char;
		case EBOOK_SPACE_PER_LINE:
		case EBOOK_FONT_SIZE:
		case EBOOK_LINE_HEIGHT:
			return viewport.text_height + viewport.space_per_line;
		case EBOOK_LINE_PER_PAGE:
			return viewport.line_per_page;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void EBookReader::setSetting(int type, int value)
{
	switch (type) {
		case EBOOK_CURRENT_LINE:
            viewport.current_lines = value;
			break;
		case EBOOK_TEXT_COLOR:
			viewport.text_color_index = value;
			break;
		case EBOOK_BG_COLOR:
			viewport.bg_color_index = value;
			break;
		case EBOOK_LEFT_MARGIN:
		case EBOOK_TOP_MARGIN:
		case EBOOK_SPACE_PER_CHAR:
		case EBOOK_SPACE_PER_LINE:
		case EBOOK_FONT_SIZE:
			break;
		case EBOOK_LINE_PER_PAGE:
			viewport.line_per_page = value;
			if(viewport.current_lines + viewport.line_per_page >= text.total_lines)
				viewport.current_lines = text.total_lines - viewport.line_per_page;
			if(viewport.current_lines < 0)
				viewport.current_lines = 0;
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
const unsigned short* EBookReader::readLine(int count)
{
	static unsigned short uni[512];
	if ((viewport.current_lines + count >= 0) && (viewport.current_lines + count < text.total_lines)) {
		toUnicode(uni, text.lineptr[viewport.current_lines + count], text.linelen[viewport.current_lines + count]);
	} else {
		uni[0] = 0;
	}
	return (const unsigned short *)uni;
}
//[*]------------------------------------------------------------------------------------------[*]
