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

#ifndef __SUBTITLE_H__
#define __SUBTITLE_H__

#include <vector>
#include <string>
#include <SDL/SDL.h>

class Subtitle
{
	private:
		std::string name;
		std::vector< std::pair<int, std::string> > items;
		std::vector< std::pair<int, std::string> >::const_iterator begin, end, cursor;
		Subtitle *chain;
		
	protected:
		void beginUpdate();
		void addString(int time, std::string str);
		void endUpdate();
		void setName(std::string value) { name = value; };

	public:
		Subtitle();
		virtual ~Subtitle();

		void addChain(Subtitle *);
		Subtitle *getChain() { return chain; };
		void setChain(Subtitle *value) { chain = value; };
		const std::string getName() { return name; };
		int getSize() { return items.size(); };

		bool setPlayTime(int time);
		const std::string getString(int rel);
		int drawSubtitle(SDL_Surface *surface, SDL_Rect *rect, int rel = 0, bool outline = true);
};

class Subtitles
{
	private:
		Subtitle *items;

	public:
		Subtitles();
		~Subtitles();

		void clear();
		bool load(std::string filename, int type);
		bool reorder();

		bool setPlayTime(int time);
		Subtitle* getSubtitle() { return items; };
};

#endif // __SUBTITLE_H__
