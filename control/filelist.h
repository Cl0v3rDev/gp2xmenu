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
#ifndef __FILELIST_H_
#define __FILELIST_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "view.h"
//[*]------------------------------------------------------------------------------------------[*]
// 파일 리스트
struct SFileList
{
	std::string szName;
	FileFormat nAttribute;
	int nMarkedIndex;
	int nTotalCount;
};
//[*]------------------------------------------------------------------------------------------[*]
class FileList : public ViewForm
{
	private:
		SDL_Surface *pImgIcon;
		SDL_Surface *pImgLine;
		SDL_Surface *pImgExit;
		SDL_Surface *pImgArrow;
		SDL_Surface *pImgStorage;

#ifndef GP2X
		std::string szAbsolutePath;
#endif

		int cursor;

	protected:
		SDL_Surface *pImgBackGround;

		int nStartCount;
		int nEndCount;
		int nPosition;
		int nCount;

		std::string lastPath;
		std::string lastName;
		SFileList *pList;

		bool useStorage;
		int waitMode;
		int r, g, b;
		int (*selectFunc)(const struct dirent *);
		SDL_Rect queryRect[2];
		
		int iconNo, iconWidth;
		
		int search(const std::string &path);

	public:
		FileList();
		~FileList();

		void Initialize(bool refresh = false);
		void Exitialize(bool refresh = false);

		void OnDraw(SDL_Surface *pScreen);

		void OnJoystickDown(SDL_Surface *pScreen, Uint8 button);
		void OnPenDown(SDL_Surface *pScreen, int x, int y);

		void OnCursorUp(SDL_Surface *pScreen);
		void OnCursorDown(SDL_Surface *pScreen);

		int action(int cmd = -1);
		virtual void OnExecute(SDL_Surface *pScreen, int button) {};
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
