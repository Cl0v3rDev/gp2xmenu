/*
    Subtitle engine for GP2X
    Copyright (C) 2005-2008 Kyeongmin Kim, nightknight <www.mystictales.net>
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

#include <stack>
#include <algorithm>
#include "subtitle.h"
#include "mainmenuapp.h"
#include "textout.h"
#include "typed.h"

extern CMainMenuApp theApp;

Subtitle::Subtitle()
{
	chain = NULL;
	items.clear();
	items.push_back(std::make_pair(0, std::string()));
}

Subtitle::~Subtitle()
{
	if(chain != NULL)
	    delete chain;
	chain = NULL;
}

void Subtitle::addChain(Subtitle *next)
{
	if(chain == NULL)
	    chain = next;
	else
	    chain->addChain(next);
}

void Subtitle::beginUpdate()
{
	items.clear();
}

void Subtitle::addString(int time, std::string str)
{
	if(items.size() > 0)
	{
		std::vector< std::pair<int, std::string> >::iterator last;
		last = items.end();--last;
		if(last->first == time)
		{
			if(last->second.size() == 0)
				last->second = str;
			else
				last->second += "\n" + str;
			return;
		}
	}
	items.push_back(make_pair(time, str));
}

static bool compare(std::pair<int, std::string> a, std::pair<int, std::string> b)
{
	return a.first < b.first;
}

void Subtitle::endUpdate()
{
	sort(items.begin(), items.end(), compare);

	begin = items.begin();
	end = items.end();
	cursor = end;
}

bool Subtitle::setPlayTime(int time)
{
	bool result = false;

	if(chain != NULL)
		result = chain->setPlayTime(time);

    if(begin->first > time)
	{
		if(cursor != end)
		{
		    cursor = end;
		    result = true;
		}
		return result;
	}
	else if(cursor == end)
	{
		cursor = begin;
		result = true;
	}

	while((cursor->first > time) || ((cursor+1 < end) && ((cursor+1)->first <= time)))
	{
		if(cursor->first > time)
			--cursor;
		else
		    ++cursor;
		if(!result) result = true;
	}

	return result;
}

const std::string Subtitle::getString(int rel)
{
    std::vector< std::pair<int, std::string> >::const_iterator item;
	if(cursor == end)
	{
		if(rel <= 0)
			return std::string();
		else
			item = begin + (rel - 1);
	}
	else
		item = cursor + rel;

	if((item < begin) || (item >= end)) return std::string();

	return item->second;
}

int HexToDecW(unsigned short hex)
{
	if ((hex >= '0') && (hex <= '9'))
		return hex - '0';
	else if ((hex >= 'A') && (hex <= 'F'))
		return hex - 'A' + 10;
	else if ((hex >= 'a') && (hex <= 'f'))
		return hex - 'a' + 10;
	else
		return -1;
}

int Subtitle::drawSubtitle(SDL_Surface *surface, SDL_Rect *rect, int rel, bool outline)
{
	std::string item = getString(rel);
	if(item.size() == 0)
	{
		return 0;
	}

	const int xres = surface->w;
	const int yres = surface->h;
	const int BkColor = 0xCE79;
	const int OutlineColor = 0x0000;

	int leftgap = rect->x;
	int rightgap = xres - rect->x - rect->w;
	int topgap = rect->y;
	int bottomgap = yres - rect->y - rect->h;

	int minbottom = xres / 10;
	int subtitle_top = topgap - 1;
	int subtitle_height = YRES - topgap - bottomgap + 2;

	int MaxWidth = xres - leftgap - rightgap;
	int MaxHeight = yres - topgap - bottomgap;
	int BeginWidth = leftgap;
	int TextColor = 0xFFFF;
	int DrawColor = 0xFFFF;
	std::stack<int> FontColor;

	int dw, dx, dy, cx, cy;
	int cont;
	int SubWidth = MaxWidth;
	int SubHeight = 0;

	dw = 0;dy = 1;
	cx = 0;cy = 0;

	SDL_FillRect(surface, NULL, BkColor);

	// Rendering Start
	int i, j, nBegin;
	int FontHeight = 0;

	unsigned short *text = new unsigned short[item.size() + 1];
	U8toU16(text, (Uint8*)item.c_str(), item.size());

	i = 0;
	if (text[i] == 0xFEFF) i++; // skip BOM
	nBegin = i;
	while (i < (item.size() + 1)) {
		cont = 0;
		if (text[i] == '\\') {
			cont = 1;
			switch (text[++i]) {
			case 'o':
				i++;
				break;
			case 'c':
				i += 7;
				break;
			default:
				cont = 0;
			}
		}
		if (cont)
			continue;

		if ((text[i] >= 32))
		{
			getFontSize(text[i], outline, &cx, &cy);
			cy += 2;
		}
		else
			cx = 0;

		if (cy > FontHeight)
			FontHeight = cy;

		if ((text[i] == 0) || (text[i] == 10) || (text[i] == 13) || ((dw + cx) > (MaxWidth - 2))) {
			if ((dy + FontHeight - 1) > MaxHeight)
				break;
			dx = ((MaxWidth - dw) >> 1) + BeginWidth;
			dw = cx;
			for (j = nBegin; j < i; j++) {
				cont = 0;
				if (text[j] == '\\') {
					cont = 1;
					switch (text[++j]) {
					case 'o':
						if (!FontColor.empty()) FontColor.pop();
						if (FontColor.empty())
							DrawColor = TextColor;
						else
							DrawColor = FontColor.top();
						break;
					case 'c':{
							int R = (HexToDecW(text[++j]) << 4) | HexToDecW(text[++j]);
							int G = (HexToDecW(text[++j]) << 4) | HexToDecW(text[++j]);
							int B = (HexToDecW(text[++j]) << 4) | HexToDecW(text[++j]);
							int color = SDL_MapRGB(surface->format, R, G, B);
							if ((color == BkColor) || (color == OutlineColor)) ++color;
							DrawColor = color;
							FontColor.push(color);
							break;
						}
					default:
						cont = 0;
					}
				}
				if (cont)
					continue;

				if ((text[j] >= 32)) {
					_drawFont(surface, dx, dy, text[j], DrawColor, outline, &cx, &cy);
					dx += cx;
				}
			}
			dy += FontHeight;
			if ((text[i] == 0) || ((dy + FontHeight - 1) > MaxHeight))
				break;
			if(dw == 0)
			{
				++i;
				if( (text[i] != text[i - 1]) && ((text[i] == 10) || (text[i] == 13))) ++i;
			}
			nBegin = i;
		} else {
			dw += cx;
			i++;
		}
	}

	delete text;

	SubHeight = dy - 1;

	if(outline || (rel == 0))
	{
		// draw Outline
		int x, y, i;
		unsigned short *src = (unsigned short *)surface->pixels;
		const int OutLine[8] = { -321, -320, -319, -1, 1, 319, 320, 321 };
		src += 320 + leftgap + 1;

		for (y = 0; y < (SubHeight - 2); y++) {
			for (x = 0; x < (SubWidth - 2); x++, src++) {
				if ((*src != OutlineColor)
				    && (*src != BkColor)) {
					for (i = 0; i < 8; i++) {
						if (*(src + OutLine[i]) == BkColor)
							*(src + OutLine[i]) = OutlineColor;
					}
				}
			}
			src += rightgap + leftgap + 2;
		}
	}
/*
	else if(rel == 0)
	{
		// draw Bold
		int x, y, i;
		unsigned short *src = (unsigned short *)surface->pixels;
		src += 320 + leftgap + 1;

		for (y = 0; y < (SubHeight - 2); y++) {
			for (x = 0; x < (SubWidth - 2); x++, src++) {
				if (*src != BkColor) {
					src[-1] = *src;
				}
			}
			src += rightgap + leftgap + 2;
		}
	}
*/
	return SubHeight;
}
//[*]------------------------------------------------------------------------------------------[*]
class LyricSubtitle:public Subtitle
{
	public:
		LyricSubtitle(unsigned char *buf);
};

LyricSubtitle::LyricSubtitle(unsigned char *buf)
{
	char *str;
	char *p = (char*)buf;
	std::stack<int> times;
	// skip BOM
	if((buf[0] == 0xEF) && (buf[1] == 0xBB) && (buf[2] == 0xBF)) p += 3;

	beginUpdate();

	while(*p) {
		str = p;
		while(*p && (*p != 0x0D) && (*p != 0x0A)) ++p;
		std::string line = std::string((char*)str, (int)p - (int)str);
		while(*p && ((*p == 0x0D) || (*p == 0x0A))) ++p;

		while(true)
		{
			int timeBegin = line.find("[");if(timeBegin == -1) break;
			int timeSplit = line.find(":");if(timeSplit == -1) break;
			if(timeBegin > timeSplit) break;
			int timeEnd = line.find("]");if(timeEnd == -1) break;
			if(timeSplit > timeEnd) break;

			int min = atoi(line.substr(timeBegin + 1, timeSplit - timeBegin - 1).c_str());
			int sec = (int)(atof(line.substr(timeSplit + 1, timeEnd - timeSplit - 1).c_str()) * 1000);
			if((min < 0) || (sec < 0)) break;
			sec += min * 60000;

			line.erase(0, timeEnd + 1);
			times.push(sec);
		}
		if(times.size() == 0) continue;

		while(!times.empty())
		{
			addString(times.top(), line);
			times.pop();
		}
	}

	endUpdate();
}
//[*]------------------------------------------------------------------------------------------[*]
class SamiSubtitle:public Subtitle
{
	private:
		std::string id;
		char *getAttr(char *p);
		SamiSubtitle *findClass(std::string);

	public:
		SamiSubtitle();
		SamiSubtitle(unsigned char *buf);
};

SamiSubtitle *SamiSubtitle::findClass(std::string find)
{
	if(id == find)
	    return this;
	else if(getChain() == NULL)
	    return NULL;
	else {
		SamiSubtitle *chain = (SamiSubtitle *)getChain();
		return chain->findClass(find);
	}
}

char *SamiSubtitle::getAttr(char *p)
{
	while((*p != '=') && (*p != '>'))
	    ++p;
	if(*p == '=') {
		++p;
		while((*p == ' ') || (*p == '\t') || (*p == '\r') || (*p == '\n'))
		    ++p;
		if((*p == '"') || (*p == '\''))
		    ++p;
		return p;
	} else {
		return NULL;
	}
}

SamiSubtitle::SamiSubtitle()
{
	id.clear();
	beginUpdate();
}

SamiSubtitle::SamiSubtitle(unsigned char *buf)
{
	char *str;
	char *p = (char*)buf;
	std::string line;
	int time = -1;
	char *tagStart = NULL, *tagEnd = NULL, *tagAttr;
	bool tagOpen = false;
	bool inStyle = false;
	bool inStyleTag = false;
	bool inComment = false;
	bool useBreak = false;
	SamiSubtitle *target = NULL;

	// skip BOM
	if((buf[0] == 0xEF) && (buf[1] == 0xBB) && (buf[2] == 0xBF)) p += 3;

	id.clear();
	beginUpdate();

	while(*p) {
		if(inStyle)
		{
			if(strncasecmp(p, "</style", 7) == 0)
			{
				inStyle = false;
				continue;
			}

			if(*p == '{')
			{
				inStyleTag = true;
			}
			else if(*p == '}')
			{
				inStyleTag = false;
				target = NULL;
			}
			else if(!inStyleTag && (*p == '.'))
			{
				char *styleClass = ++p;
				while((*p != ' ') && (*p != '\t') && (*p != '{')) ++p;
				std::string className = std::string(styleClass, (int)p - (int)styleClass);
				if(id.size() == 0)
				{
					id = className;
					target = this;
				}
				else
					target = findClass(className);
				if(target == NULL)
				{
					target = new SamiSubtitle();
					target->id = className;
					addChain(target);
				}
				continue;
			}
			else if(inStyleTag && target && (strncasecmp(p, "Name:", 5) == 0))
			{
				p += 5;
				char *styleName = p;
				while((*p != ';') && (*p != '}')) ++p;
				target->setName(std::string(styleName, (int)p - (int)styleName));
				continue;
			}
			++p;
		}
		else if(inComment)
		{
			if(strncmp(p, "-->", 3) == 0)
			{
				p += 3;
				inComment = false;
				continue;
			}
			++p;
		}
		else if (*p == '<')
		{
			if(strncmp(p, "<!--", 4) == 0)
			{
				inComment = true;
				p += 4;
			}
			else
			{
				tagStart = ++p;
				tagOpen = true;
				tagAttr = NULL;
				if(tagEnd) {
					line += std::string(tagEnd, (int)p - (int)tagEnd - 1);
				}
			}
		}
		else if(tagOpen && (*p == '>'))
		{
			tagEnd = ++p;
			tagOpen = false;
			if(tagStart == NULL) continue;

			if(tagAttr && (strncasecmp(tagStart, "sync", 4) == 0))
			{
				if(inStyle) inStyle = false;
				if(target && (time != -1))
				{
					if(!useBreak)
					    line.erase(line.find_last_not_of('\n') + 1);
				    line.erase(line.find_last_not_of(' ') + 1);
					target->addString(time, line);
				}
				line.clear();
				time = atoi(tagAttr);
				target = this;
			}
			else if(tagAttr && (strncasecmp(tagStart, "p", 1) == 0))
			{
				char *endAttr = tagAttr + 1;
				while((*endAttr != ' ') && (*endAttr != '"') && (*endAttr != '\'') && (*endAttr != '>')) ++endAttr;
				target = findClass(std::string(tagAttr, (int)endAttr - (int)tagAttr));
				if(target == NULL)
				    target = this;
			}
			else if(tagAttr && (strncasecmp(tagStart, "font", 4) == 0))
			{
				int color = theApp.ParseColor(tagAttr);
				if(color != -1)
				{
					char hex[15];
					sprintf(hex, "\\c%06x", color);
					line += hex;
				}
			}
			else if(strncasecmp(tagStart, "/font", 5) == 0)
			{
				line += "\\o";
			}
			else if(strncasecmp(tagStart, "br", 2) == 0)
			{
                line.erase(line.find_last_not_of(' ') + 1);
				line += "\n";
				useBreak = true;
			}
			else if(strncasecmp(tagStart, "style", 5) == 0)
			{
				inStyle = true;
			}
			else if(strncasecmp(tagStart, "ruby", 4) == 0)
			{
				// ruby open
			}
			else if(strncasecmp(tagStart, "rt", 2) == 0)
			{
				line += "(";
			}
			else if(strncasecmp(tagStart, "/ruby", 5) == 0)
			{
				line += ")";
			}
		}
		else if(!tagOpen && ((*p == '\r') || (*p == '\n')))
		{
			if(tagEnd && (tagEnd != p))
			{
				line += std::string(tagEnd, (int)p - (int)tagEnd);
				char last = line[line.size() - 1];
				if(useBreak && (last != ' '))
					line += " ";
				else if(!useBreak && (last != '\n'))
				{
	                line.erase(line.find_last_not_of(' ') + 1);
				    line += "\n";
				}
			}
			while((*p == '\r') || (*p == '\n')) ++p;
			while((*p == ' ') || (*p == '\t')) ++p;
			tagEnd = p;
		}
		else if(!tagOpen && ((*p == ' ') || (*p == '\t')))
		{
			if(tagEnd && (tagEnd != p))
			{
				line += std::string(tagEnd, (int)p - (int)tagEnd);
				char last = line[line.size() - 1];
				if(last != ' ') line += " ";
			}
			while((*p == ' ') || (*p == '\t')) ++p;
			tagEnd = p;
		}
		else if(!tagOpen && (strncasecmp(p, "&nbsp", 5) == 0))
		{
			if(tagEnd)
				line += std::string(tagEnd, (int)p - (int)tagEnd) + " ";
			p += 5;
			if(*p == ';') ++p;
			// workaround
			if(*p == ';') ++p;
			tagEnd = p;
		}
		else if(tagOpen &&
			((strncasecmp(p, "start", 5) == 0) ||
			(strncasecmp(p, "class", 5) == 0) ||
			(strncasecmp(p, "color", 5) == 0)) )
		{
			p += 5;
			tagAttr = getAttr(p);
			if(tagAttr)
				p = tagAttr + 1;
		}
		else
		{
			++p;
		}
	}

	if(target && (time != -1))
	{
		if(!useBreak)
		    line.erase(line.find_last_not_of('\n') + 1);
	    line.erase(line.find_last_not_of(' ') + 1);
		target->addString(time, line);
	}

	target = this;
	while(target != NULL)
	{
		target->endUpdate();
		target = (SamiSubtitle *)target->getChain();
	}
}
//[*]------------------------------------------------------------------------------------------[*]
Subtitles::Subtitles()
{
	items = NULL;
}

Subtitles::~Subtitles()
{
	clear();
}

void Subtitles::clear()
{
	if(items != NULL)
		delete items;
	items = NULL;
}

bool Subtitles::load(std::string name, int type)
{
    unsigned char *buf;
    int size;

	int ext = name.find_last_of(".");
	if(ext < 0) return false;
	name = name.substr(0, ext);

	switch (type) {
		case SUBTITLE_LRC:
            name += ".lrc";
            break;
		case SUBTITLE_SMI:
            name += ".smi";
            break;
		case SUBTITLE_SAMI:
            name += ".sami";
            break;
	}

	FILE *fp = fopen(name.c_str(), "rb");
	if(fp == NULL) return false;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = new unsigned char[size];
	fread(buf, size, 1, fp);
	fclose(fp);

	if(!isUTF8(buf))
	{
		unsigned char *conv = new unsigned char[size * 4];
		if((buf[0] == 0xFF) && (buf[1] == 0xFE))
		{
			// UTF16 Little Endian
			toUTF8("UTF-16LE", (char*)buf, conv, size);
		}
		else if((buf[0] == 0xFE) && (buf[1] == 0xFF))
		{
			// UTF16 Big Endian
			toUTF8("UTF-16BE", (char*)buf, conv, size);
		}
		else
		{
			// No BOM
			toUTF8(NULL, (char*)buf, conv, size);
		}
		delete buf;
		buf = conv;
	}

	Subtitle *item = NULL;

	switch (type) {
		case SUBTITLE_LRC:
			item = new LyricSubtitle(buf);
			break;
		case SUBTITLE_SMI:
		case SUBTITLE_SAMI:
			item = new SamiSubtitle(buf);
			break;
	}

	if(item != NULL)
	{
		if(items == NULL)
		    items = item;
		else
		    items->addChain(item);
	}

	delete buf;

	return item != NULL;
}

bool Subtitles::reorder()
{
	while(items && (items->getSize() <= 0))
	{
		Subtitle *remove = items;
		items = remove->getChain();
		remove->setChain(NULL);
		delete remove;
	}
	if(items == NULL) return false;

	Subtitle *cursor = items;
	while(cursor->getChain())
	{
		if(cursor->getChain()->getSize() > 0)
		    cursor = cursor->getChain();
		else
		{
			Subtitle *remove = cursor->getChain();
			cursor->setChain(remove->getChain());
			remove->setChain(NULL);
			delete remove;
		}
	}
	return true;
}


bool Subtitles::setPlayTime(int time)
{
	if(items == NULL)
	    return false;
	return items->setPlayTime(time);
}
