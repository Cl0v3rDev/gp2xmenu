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
#ifndef __EBOOKREADER_H_
#define __EBOOKREADER_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <string>
#include <vector>
//[*]------------------------------------------------------------------------------------------[*]
#define	MAX_LINES	65536
//[*]------------------------------------------------------------------------------------------[*]
typedef struct tagTextStruct
{
	char	*textptr;
	int		total_lines;
	std::vector<char*> lineptr;
	std::vector<int> linelen;
/*
	char	*lineptr[MAX_LINES];
	char	linelen[MAX_LINES];
*/
} TextStruct;
//[*]------------------------------------------------------------------------------------------[*]
typedef struct tagViewerStruct
{
	int	current_lines;
	int	line_count;

	int	text_color_index;
	int	text_width;
	int	text_height;
	int	bg_color_index;

	int	top_margin;
	int	left_margin;
	int	bottom_margin;
	int	space_per_line;
	int	space_per_char;
	int	tab_length;

	int	line_per_page;
	int	char_per_line;
	int	use_variable_width;
	int	orientation;

	char font[256];
} ViewerStruct;
//[*]------------------------------------------------------------------------------------------[*]
typedef struct tagFileIdentify
{
	int lastline;
	int total_lines;
	char name[256];
} TextFile;
//[*]------------------------------------------------------------------------------------------[*]
typedef struct tagBookmarkStruct
{
	int font_color_index;
	int bg_color_index;
	char font[256];

	TextFile lastfile;
	TextFile markedfile[25];
} BookmarkStruct;
//[*]------------------------------------------------------------------------------------------[*]
#define TAB_LENGTH 4
#define	CODE_TAB	0x09
#define	CODE_CARRIAGE 0x0D
#define	CODE_RETURN	0x0A
//[*]------------------------------------------------------------------------------------------[*]
enum CODETYPE
{
	CODE_NONE = -1,
	CODE_SINGLE,
	CODE_HEAD,
	CODE_TAIL
};
//[*]------------------------------------------------------------------------------------------[*]
enum EBOOKSETTING
{
	EBOOK_CURRENT_LINE,
	EBOOK_TOTAL_LINE,
	EBOOK_TEXT_COLOR,
	EBOOK_BG_COLOR,
	EBOOK_LEFT_MARGIN,
	EBOOK_TOP_MARGIN,
	EBOOK_SPACE_PER_CHAR,
	EBOOK_SPACE_PER_LINE,
	EBOOK_FONT_SIZE,
	EBOOK_LINE_HEIGHT,
	EBOOK_LINE_PER_PAGE
};
//[*]------------------------------------------------------------------------------------------[*]
enum EBOOKSCROLL
{
	EBOOK_SEEK_TOP,
	EBOOK_PREV_LONG,
	EBOOK_PREV_PAGE,
	EBOOK_PREV_LINE,
	EBOOK_NEXT_LINE,
	EBOOK_NEXT_PAGE,
	EBOOK_NEXT_LONG,
	EBOOK_SEEK_END
};
//[*]------------------------------------------------------------------------------------------[*]
class EBookReader
{
	private :
		ViewerStruct	viewport;
		TextStruct		text;
		BookmarkStruct	bookmark;

		std::string		path;
		std::string		name;
		std::string		loaded;

		//enum CODETYPE GetCodeType(unsigned char *str, int pos);
		//int CalcOneLine(char *CurrentPtr, int *pLineIncrement); //*pLineIncrement를 사용하면 tab키를 사용한 txt파일에서 뻗음
		int CalcOneLine(char *CurrentPtr, int &LineIncrement);

		void MakeLineInfo();

	public :
		EBookReader();
		~EBookReader();

		void setPath(const char*);
		const char* getPath();
		const char* getName();
		bool TextLoad(const char*);
		void clear();

		const char* getFont();
		void setFont(const char*);

		void toUnicode(unsigned short*, const char*, int);

		const unsigned short *readLine(int);

		int getSetting(int);
		void setSetting(int, int);

		void Scroll(int);

		int	InitBookmark();
		int	LoadBookmark();
		void SaveBookmark();
		bool RecallBookmark(int);
		void MarkBookmark(int);
		const char* getBookmarkName(int);
		int getBookmarkInfo(int, int);
		void DeleteBookmark(int);
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
