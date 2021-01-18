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
#ifndef __PHOTOVIEWER_H_
#define __PHOTOVIEWER_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "filelist.h"
#include "fileload.h"
#include "jpegload.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
class PhotoViewer
{
	private :
        SDL_Surface *pScreen;
        SDL_Surface *pScale;

        SDL_Surface *pFillH[PHOTO_BUFFER_COUNT];
        SDL_Surface *pFillV[PHOTO_BUFFER_COUNT];

#ifdef GP2X
		int memfd;
        Uint16 *ImageBuffer;
#else
        Uint16 ImageBuffer[PHOTO_SCALE_SIZE];
#endif
        int InfoWidth[PHOTO_BUFFER_COUNT];
        int InfoHeight[PHOTO_BUFFER_COUNT];
        int ImageWidth;
        int ImageHeight;
        int ImageViewX;
        int ImageViewY;
        int ImageRotate;
        bool ImageScaleBreak;
        bool ImageResizeCont;
        int ImageLoadPosition[PHOTO_BUFFER_COUNT];
        int ImageViewBuffer;
        double ImageScale;
		std::string ImageLoadName[PHOTO_BUFFER_COUNT];

        Uint16 *ComicBuffer[PHOTO_BUFFER_COUNT];
        Uint16 *ComicResizeBuffer[PHOTO_BUFFER_COUNT];
        int ComicWidth[PHOTO_BUFFER_COUNT];
        int ComicHeight[PHOTO_BUFFER_COUNT];
        int ComicResizeWidth;
        int ComicResizedWidth[PHOTO_BUFFER_COUNT];
        int ComicResizedHeight[PHOTO_BUFFER_COUNT];
        int ComicPageType[PHOTO_BUFFER_COUNT];
        bool ComicIsResize[PHOTO_BUFFER_COUNT];
        double ComicScale[PHOTO_BUFFER_COUNT];
        int ComicViewPage;
        int ComicViewX;
        int ComicViewY;
        bool ComicBufferResampling;
        int ComicLoadPosition[PHOTO_BUFFER_COUNT];
        int ComicViewBuffer;
		std::string ComicLoadName[PHOTO_BUFFER_COUNT];

		SDL_Thread *pThread;
		SDL_mutex *LoadLock;
		int Status;
		int itemIndex;
		bool moveDir[8];
		int movex, movey;

		CJpegLoad pJpeg;

		bool Terminated;
		bool DoPrevLoading;
		bool DoNextLoading;

		bool BufferChanged;
        int BufferedType;

		int *xlines;
		int *ylines;
		int *xcount;
		int *ycount;
		Uint16 *resizebuf;
		int o_width, o_height;
		int r_width, r_height;
		int comicbuf;

		int ImageViewMode;

        void LoadFitImage(int index, int buf);
		void LoadFullImage();
		void LoadComicImage(int index, int buf);

		SDL_Surface* CreateSurface(int w, int h);
		SDL_Surface* ConvertSurface(SDL_Surface *image);

		SDL_Surface* ScreenFitResampling(SDL_Surface *surface, int rw, int rh);

		void ComicResampling(int buf);
		void ViewComic(SDL_Surface *);

		void ZoomInOut(void);
		void ZoomOutResampling(void);

		static int FileLoadThread(void *ptr);

		std::vector< std::pair<std::string, int> > fileList;
		std::string path;

    public:
        bool DoFullLoading;

		PhotoViewer();
		~PhotoViewer();

		bool LoadImage(int index = -1);

		void ScreenFit(SDL_Surface *surface, int w, int h, int buf);
		void ResizeBegin(int w, int h); // 일반 이미지 리사이즈
		int ResizeComic(int w, int h, int buf); // 코믹 모드 리사이즈
		void Resize16(char *line, int y); // RGB 565 to Buffer
		void Resize24(char *line, int y); // RGB 888 to Buffer
		void ResizeEnd(void);
		void ResizeComicEnd(void);

		int getSetting(int);
		void setSetting(int, int);
		std::string getInfo(int);

		bool PrevImage(bool = false);
		bool NextImage(bool = false);

		bool Moving();

		bool isComicMode() { return (ImageViewMode == PHOTO_COMIC_LEFT_VIEW) || (ImageViewMode == PHOTO_COMIC_RIGHT_VIEW); };

		void Draw(SDL_Surface *pScreen);

		void SaveComicMark(void);
		void LoadComicMark(const char*);

		void Break(void);

		void reset(std::string &path);
		void addList(std::string &filename, int attr, bool active = false);
};
#endif
//[*]------------------------------------------------------------------------------------------[*]
