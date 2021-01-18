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
#include <math.h>
#include <string.h>
#include <fcntl.h>
#ifndef WIN32
#include <sys/mman.h>
#endif
#include <inifile.h>
#include <SDL/SDL_image.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "photoviewer.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
int PhotoViewer::FileLoadThread(void *ptr)
{
	PhotoViewer *view = (PhotoViewer *)ptr;

	while(!view->Terminated) {
		if(view->BufferChanged) {
			if(view->ImageViewMode == PHOTO_FIT_VIEW) {
				int pos = view->itemIndex;
				if(pos < 0) {
					view->BufferChanged = false;
					continue;
				}

				if((view->DoFullLoading) && (view->BufferedType != BUF_SCALE)) {
					view->LoadFullImage();
					if(view->BufferedType == BUF_SCALE) {
						view->ImageScale = -1;
						printf("Scale Image End...\n");
					}
					view->DoFullLoading = false;
				}

				if(view->DoNextLoading) {
					int NextBuffer = (view->ImageViewBuffer + 1) % PHOTO_BUFFER_COUNT;
					int nextpos = pos + 1;

					while(view->DoNextLoading && (view->ImageLoadPosition[NextBuffer] == -99) && (nextpos < view->fileList.size())) {
						view->LoadFitImage(nextpos, NextBuffer);
						if(view->ImageLoadPosition[NextBuffer] == -99)
							printf("Next Image %s failed.\n", view->fileList[nextpos].first.c_str());
						nextpos++;
					}

					if(view->ImageLoadPosition[NextBuffer] == -99) {
						printf("Next Image false.\n");
						view->ImageLoadPosition[NextBuffer] = -1;
					}
				}

				if(view->DoPrevLoading) {
					int PrevBuffer = (view->ImageViewBuffer + PHOTO_BUFFER_COUNT - 1) % PHOTO_BUFFER_COUNT;
					int prevpos = pos - 1;

					while(view->DoPrevLoading && (view->ImageLoadPosition[PrevBuffer] == -99) && (prevpos >= 0)) {
						view->LoadFitImage(prevpos, PrevBuffer);
						if(view->ImageLoadPosition[PrevBuffer] == -99)
							printf("Prev Image %s failed.\n", view->fileList[prevpos].first.c_str());
						prevpos--;
					}

					if(view->ImageLoadPosition[PrevBuffer] == -99){
						printf("Prev Image false.\n");
						view->ImageLoadPosition[PrevBuffer] = -1;
					}
				}
			} else if(view->BufferedType == BUF_COMIC) {
				int pos = view->ComicLoadPosition[view->ComicViewBuffer];
				if(pos < 0) {
					view->BufferChanged = false;
					continue;
				}

				if(view->DoNextLoading) {
					int NextBuffer = (view->ComicViewBuffer + 1) % PHOTO_BUFFER_COUNT;
					int nextpos = pos + 1;

					while(view->DoNextLoading && (view->ComicLoadPosition[NextBuffer] == -99) && (nextpos < view->fileList.size())) {
						view->LoadComicImage(nextpos, NextBuffer);
						if(view->ComicLoadPosition[NextBuffer] == -99)
							printf("Next Comic %s failed.\n", view->fileList[nextpos].first.c_str());
						nextpos++;
					}

					if(view->ComicLoadPosition[NextBuffer] == -99){
						printf("Next Comic false.\n");
						view->ComicLoadPosition[NextBuffer] = -1;
					}
				}

				if(view->DoPrevLoading) {
					int PrevBuffer = (view->ComicViewBuffer + PHOTO_BUFFER_COUNT - 1) % PHOTO_BUFFER_COUNT;
					int prevpos = pos - 1;

					while(view->DoPrevLoading && (view->ComicLoadPosition[PrevBuffer] == -99) && (prevpos >= 0)) {
						view->LoadComicImage(prevpos, PrevBuffer);
						if(view->ComicLoadPosition[PrevBuffer] == -99)
							printf("Prev Comic %s failed.\n", view->fileList[prevpos].first.c_str());
						prevpos--;
					}

					if(view->ComicLoadPosition[PrevBuffer] == -99) {
						printf("Prev Comic false.\n");
						view->ComicLoadPosition[PrevBuffer] = -1;
					}
				}
			}

			view->BufferChanged = false;
		}

		usleep(1);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
PhotoViewer::PhotoViewer()
{
#ifdef GP2X
    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (memfd < 0) {
		printf("open /dev/mem failed\n");
	}

    ImageBuffer = (Uint16 *)mmap(0, PHOTO_SCALE_SIZE << 1, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0x02000000);
    if (ImageBuffer == NULL)
    {
		close(memfd);
        printf("/dev/mem mmap() failed\n");
    }
#endif

	BufferChanged = false;
	pThread = SDL_CreateThread(FileLoadThread, this);

	pScale = CreateSurface(XRES, YRES);

	ComicBuffer[0] = ImageBuffer;
	ComicResizeWidth = 600;

	int i;
	for(i=0;i < PHOTO_BUFFER_COUNT;i++) {
		pFillH[i] = CreateSurface(XRES, YRES);
		pFillV[i] = CreateSurface(XRES, YRES);
		if(i > 0) ComicBuffer[i] = ComicBuffer[i-1] + PHOTO_COMIC_SIZE;
	}

	LoadLock = SDL_CreateMutex();
	Terminated = false;
	fileList.clear();
}
//[*]------------------------------------------------------------------------------------------[*]
PhotoViewer::~PhotoViewer()
{
	BufferChanged = false;
	DoNextLoading = false;
	DoPrevLoading = false;
	DoFullLoading = false;
	Terminated = true;

	if(pThread)
		SDL_WaitThread(pThread, NULL);

	// We're done!
	SDL_FreeSurface(pScale);

	SaveComicMark();

	int i;
	for(i=0;i<PHOTO_BUFFER_COUNT;i++) {
		SDL_FreeSurface(pFillH[i]);
		SDL_FreeSurface(pFillV[i]);
	}

	SDL_DestroyMutex(LoadLock);

#ifdef GP2X
	munmap(ImageBuffer, PHOTO_SCALE_SIZE << 1);
	close(memfd);
	sync();
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
// 화면에 이미지를 표시한다.
// ImageViewMode 에 따라 화면에 꽉찬 모드(PHOTO_FIT_VIEW), 혹은 임의 배율 모드(PHOTO_SCALE_VIEW)을 설정한다.
//[*]------------------------------------------------------------------------------------------[*]
SDL_Surface *PhotoViewer::CreateSurface(int w, int h)
{
	return SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, COLORBIT, REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);
}
//[*]------------------------------------------------------------------------------------------[*]
SDL_Surface *PhotoViewer::ConvertSurface(SDL_Surface *image)
{
	if(image == NULL) return NULL;
	SDL_Surface *convert = CreateSurface(image->w, image->h);
	if(convert == NULL) return NULL;
	SDL_BlitSurface(image, NULL, convert, NULL);
	return convert;
}
//[*]------------------------------------------------------------------------------------------[*]
// 커다란 이미지를 축소한다.
// 메모리 사용을 줄이기 위해 리사이즈 알고리즘은 단순하게 한다.
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::ResizeBegin(int w, int h)
{
	if(xlines){ free(xlines);xlines = NULL; }
	if(ylines){ free(ylines);ylines = NULL; }
	if(xcount){ free(xcount);xcount = NULL; }
	if(ycount){ free(ycount);ycount = NULL; }
	o_width = w;
	o_height = h;

	ylines = (int*)malloc(h * sizeof(int));

	resizebuf = ImageBuffer;
    if(w * h <= PHOTO_SCALE_SIZE){
		int i;

		r_width = w;
		r_height = h;

		for(i=0;i<h;i++)ylines[i] = i;

		return;
	}

	xlines = (int*)malloc(w * sizeof(int));

	r_width = (int)floor(sqrt(PHOTO_SCALE_SIZE * ((double)w / (double)h)));
	r_height = (int)floor(sqrt(PHOTO_SCALE_SIZE * ((double)h / (double)w)));

	int n, v, i;

	xlines[0] = 0;
	n = 0;v = r_width;
	for(i=1;i<o_width;i++) {
		v += r_width;
		if((v > o_width)&&((n+1) < r_width)) {
			xlines[i] = ++n;
			v -= o_width;
		} else {
			xlines[i] = -1;
		}
	}
	r_width = n+1;

	ylines[0] = 0;
	n = 0;v = r_height;
	for(i=1;i<o_height;i++) {
		v += r_height;
		if((v > o_height)&&((n+1) < r_height)) {
			ylines[i] = ++n;
			v -= o_height;
		} else {
			ylines[i] = -1;
		}
	}
	r_height = n+1;

}
// 만화책 모드용 리사이즈 설정
int PhotoViewer::ResizeComic(int w, int h, int buf)
{
	int limit = 1500;

	if(xlines){ free(xlines);xlines = NULL; }
	if(ylines){ free(ylines);ylines = NULL; }
	if(xcount){ free(xcount);xcount = NULL; }
	if(ycount){ free(ycount);ycount = NULL; }
	o_width = w;
	o_height = h;
    comicbuf = buf;

	ylines = (int*)malloc(h * sizeof(int));

	if(w < h) {
        limit = 800; // 1 페이지짜리 그림(세로가 긴 형태)
        ComicPageType[buf] = COMIC_ONE_PAGE;
    } else {
        ComicPageType[buf] = COMIC_TWO_PAGE;
    }

	resizebuf = ComicBuffer[buf];
	if(w <= limit){
		int i;

		r_width = w;
		r_height = h;

    	if((r_width * r_height) > (PHOTO_COMIC_SIZE * 0.8)) {
        	if(ylines){ free(ylines);ylines = NULL; }

            return 0;
        }

		for(i=0;i<h;i++)ylines[i] = i;

		return 1;
	}

	xlines = (int*)malloc(w * sizeof(int));

	r_width = limit;
	r_height = (limit * h) / w;

	int n, v, i;

	xlines[0] = 0;
	n = 0;v = r_width;
	for(i=1;i<o_width;i++) {
		v += r_width;
		if((v > o_width)&&((n+1) < r_width)) {
			xlines[i] = ++n;
			v -= o_width;
		} else {
			xlines[i] = -1;
		}
	}
	r_width = n+1;

	ylines[0] = 0;
	n = 0;v = r_height;
	for(i=1;i<o_height;i++) {
		v += r_height;
		if((v > o_height)&&((n+1) < r_height)) {
			ylines[i] = ++n;
			v -= o_height;
		} else {
			ylines[i] = -1;
		}
	}
	r_height = n+1;

	if((r_width * r_height) > (PHOTO_COMIC_SIZE * 0.8)) {
    	if(xlines){ free(xlines);xlines = NULL; }
    	if(ylines){ free(ylines);ylines = NULL; }

        return 0;
    }

	xcount = (int*)calloc(r_width, sizeof(int));
	ycount = (int*)calloc(r_height, sizeof(int));

    return 1;
}
// 16비트 이미지의 리사이즈를 위한 함수
void PhotoViewer::Resize16(char *line, int y)
{
	if((r_width == 0)||(r_height == 0)||(y > o_height)||(ylines[y] == -1))
		return;

	unsigned short *dst = &resizebuf[ylines[y] * r_width];
	Uint16 *src = (Uint16 *)line;
	int i;

	if(xlines){
		for(i=0;i<o_width;i++,src++) {
			if(xlines[i] != -1){
				dst[xlines[i]] = *src;
			}
		}
	} else {
		memcpy(dst, src, o_width * 2);
		//for(i=0;i<o_width;i++,src++,dst++) {
		//    *dst = *src;
		//}
	}
}
// 24비트 이미지의 리사이즈를 위한 함수
void PhotoViewer::Resize24(char *line, int y)
{
	if((r_width == 0)||(r_height == 0)||(y > o_height)||(ylines[y] == -1))
		return;

	unsigned short *dst = &resizebuf[ylines[y] * r_width];
	char *src = line;
	int i;

	if(xlines){
		for(i=0;i<o_width;i++,src+=3) {
			if(xlines[i] != -1){
				unsigned char r = src[0];
				unsigned char g = src[1];
				unsigned char b = src[2];

				dst[xlines[i]] = ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) &0x001F);
			}
		}
	} else {
		for(i=0;i<o_width;i++,src+=3,dst++) {
			unsigned char r = src[0];
			unsigned char g = src[1];
			unsigned char b = src[2];

			*dst = ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) &0x001F);
		}
	}
}
/* Not Yet~
// 24비트 이미지의 리사이즈를 위한 함수
void Resize24HQ(char *line, int y)
{
	if((r_width == 0)||(r_height == 0)||(y > o_height)||(ylines[y] == -1))
		return;

	unsigned short *dst = &resizebuf[ylines[y] * r_width];
	char *src = line;
	int i;

	ycount[ylines[y]]++;
	if(xlines){
		for(i=0;i<o_width;i++,src+=3) {
			unsigned char r = src[0];
			unsigned char g = src[1];
			unsigned char b = src[2];

			dst[xlines[i]] = ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) &0x001F);
		}
	} else {
		for(i=0;i<o_width;i++,src+=3,dst++) {
			unsigned char r = src[0];
			unsigned char g = src[1];
			unsigned char b = src[2];

			*dst = ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) &0x001F);
		}
	}
}
*/
// 리사이즈 결과를 정리한다.
void PhotoViewer::ResizeEnd(void)
{
	ImageWidth = r_width;
	ImageHeight = r_height;
	ImageViewX = ImageWidth >> 1;
	ImageViewY = ImageHeight >> 1;

	if(xlines){ free(xlines);xlines = NULL; }
	if(ylines){ free(ylines);ylines = NULL; }
	if(xcount){ free(xcount);xcount = NULL; }
	if(ycount){ free(ycount);ycount = NULL; }
	r_width = r_height = 0;
}
// 리사이즈 결과를 정리한다.
void PhotoViewer::ResizeComicEnd(void)
{
	ComicWidth[comicbuf] = r_width;
	ComicHeight[comicbuf] = r_height;
    ComicResizeBuffer[comicbuf] = NULL;

	if(xlines){ free(xlines);xlines = NULL; }
	if(ylines){ free(ylines);ylines = NULL; }
	if(xcount){ free(xcount);xcount = NULL; }
	if(ycount){ free(ycount);ycount = NULL; }
	r_width = r_height = 0;
}
//[*]------------------------------------------------------------------------------------------[*]
// 스케일링 모드에서의 리사이즈(축소) - 리샘플링
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::ZoomOutResampling(void)
{
	int *rlines, *glines, *blines;
	int *xlines, *ylines, *xcount;
	int n, v, c, i, linelen;

	int x, y;
	Uint16 *src, *dst;
	int SrcDir, SrcPitch;
	int DstPitch;

	int r, g, b, count, ycount;
	int dx, dy;

	int rw = 320;
	int rh = 240;
	int ow = (int)(rw / ImageScale);
	int oh = (int)(rh / ImageScale);

	if(ImageRotate & 1) {
		if(ow > ImageHeight) {
			rw = (int)(ImageHeight * ImageScale);
			ow = ImageHeight;
		}
		if(oh > ImageWidth) {
			rh = (int)(ImageWidth * ImageScale);
			oh = ImageWidth;
		}
	} else {
		if(ow > ImageWidth) {
			rw = (int)(ImageWidth * ImageScale);
			ow = ImageWidth;
		}
		if(oh > ImageHeight) {
			rh = (int)(ImageHeight * ImageScale);
			oh = ImageHeight;
		}
	}

	xlines = (int*)malloc(ow * sizeof(int));
	ylines = (int*)malloc(oh * sizeof(int));

	linelen = rw * sizeof(int);

	rlines = (int*)malloc(linelen);
	glines = (int*)malloc(linelen);
	blines = (int*)malloc(linelen);
	xcount = (int*)malloc(linelen);

	xlines[0] = 0;
	n = 0;v = rw;c = 1;
	for(i=1;i<ow;i++) {
		v += rw;
		if((v > ow)&&((n+1) < rw)) {
			xcount[n] = c;
			xlines[i] = ++n;
			v -= ow;
			c = 1;
		} else {
			xlines[i] = n;
			++c;
		}
	}
	xcount[n] = c;

	ylines[0] = 0;
	n = 0;v = rh;
	for(i=1;i<oh;i++) {
		v += rh;
		if((v > oh)&&((n+1) < rh)) {
			ylines[i] = ++n;
			v -= oh;
		} else {
			ylines[i] = n;
		}
	}

	switch(ImageRotate) {
		case 0:
			SrcDir = 1;
			SrcPitch = ImageWidth - ow;
			break;
		case 1:
			SrcDir = ImageWidth;
			SrcPitch = -(ImageWidth * ow +1);
			break;
		case 2:
			SrcDir = -1;
			SrcPitch = -(ImageWidth - ow);
			break;
		case 3:
			SrcDir = -ImageWidth;
			SrcPitch = ImageWidth * ow +1;
			break;
	}

	SDL_LockSurface(pScreen);
	dst = (Uint16 *)pScreen->pixels;

	if(ImageRotate & 1) {
		x = ImageViewX - (oh >> 1);
		y = ImageViewY - (ow >> 1);
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if((x + oh) > ImageWidth)x = ImageWidth - oh;
		if((y + ow) > ImageHeight)y = ImageHeight - ow;

		ImageViewX = x + (oh >> 1);
		ImageViewY = y + (ow >> 1);
	} else {
		x = ImageViewX - (ow >> 1);
		y = ImageViewY - (oh >> 1);
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if((x + ow) > ImageWidth)x = ImageWidth - ow;
		if((y + oh) > ImageHeight)y = ImageHeight - oh;

		ImageViewX = x + (ow >> 1);
		ImageViewY = y + (oh >> 1);
	}

	dst += ((240 - rh) >> 1) * 320 + ((320 - rw) >> 1);
	DstPitch = 320 - rw;

	switch(ImageRotate) {
		case 0:
			src = &ImageBuffer[y * ImageWidth + x];
			break;
		case 1:
			src = &ImageBuffer[y * ImageWidth + x + oh - 1];
			break;
		case 2:
			src = &ImageBuffer[(y + oh - 1) * ImageWidth + x + ow - 1];
			break;
		case 3:
			src = &ImageBuffer[(y + ow - 1) * ImageWidth + x];
			break;
	}

    n = -1;
	for(dy=0;ImageResizeCont && (dy<=oh);dy++) {
		if ((dy == oh) || (n != ylines[dy])) {
			if(n != -1) {
				for(dx=0;dx<rw;dx++,dst++) {
					count = xcount[dx] * ycount;
					r = rlines[dx] / count;
					g = glines[dx] / count;
					b = blines[dx] / count;

					*dst = ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) &0x001F);
				}
				dst += DstPitch;
				SDL_UpdateRect(pScreen, 0, n, 320, 1);
			}

			if(dy == oh) break;

			memset(rlines, 0, linelen);
			memset(glines, 0, linelen);
			memset(blines, 0, linelen);
			n = ylines[dy];
			ycount = 0;
		}
		for(dx=0;dx<ow;dx++,src+=SrcDir) {
			r = (*src & 0xF800) >> 8;
			g = (*src & 0x07E0) >> 3;
			b = (*src & 0x001F) << 3;
			rlines[xlines[dx]] += r;
			glines[xlines[dx]] += g;
			blines[xlines[dx]] += b;
		}
		src += SrcPitch;
		++ycount;
	}
	SDL_UnlockSurface(pScreen);

	free(xcount);
	free(rlines);free(glines);free(blines);
	free(xlines);free(ylines);

	return;
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::ZoomInOut(void)
{
	static int xcount[320], ycount[320];
	static double lastScale = -1;
	static int lastRotate;
	static int lastWidth, lastHeight;
	static int ow, oh, rw, rh, vw, vh;

	Uint16 *src, *dst;
	int dx, dy;
	int x, y;
	int SrcLine, DstPitch;

	if((lastScale != ImageScale) || (lastRotate != ImageRotate) || (lastWidth != ImageWidth) || (lastHeight != ImageHeight)) {
		if(ImageRotate & 1) { // 90도, 270도 회전
    		rw = 240;
    		rh = 320;
		} else { // 0도, 180도 회전
    		rw = 320;
    		rh = 240;
		}
		ow = (int)(rw / ImageScale);
		oh = (int)(rh / ImageScale);

		if(ow > ImageWidth) {
			rw = (int)(ImageWidth * ImageScale);
			ow = ImageWidth;
		}
		if(oh > ImageHeight) {
			rh = (int)(ImageHeight * ImageScale);
			oh = ImageHeight;
		}

        ImageScaleBreak = (rw < 32) || (rh < 32);

		int n, v, c, i;
		n = 0;v = (rw > ow ? 0 : rw);c = 1;
		for(i=1;i<ow;i++) {
			v += rw;
			if((v > ow)&&((n+1) < rw)) {
				while(v > (ow << 1)) {
					xcount[n] = 0;
					v -= ow;
					++n;
				}
				xcount[n] = c;
				v -= ow;
				++n;
				c = 1;
			} else {
				++c;
			}
		}
		vw = n + 1;
		switch(ImageRotate) {
			case 0: // 0도
			case 3: // 270도
				xcount[n] = c + ImageWidth - ow;
				break;
			case 2: // 180도
			case 1: // 90도
				xcount[n] = c + ImageWidth - ow;
				for(i=0;i<vw;i++)
					xcount[i] *= -1;
				break;
		}

		n = 0;v = (rh > oh ? 0 : rh);c = 0;
		for(i=1;i<oh;i++) {
			v += rh;
			if((v > oh)&&((n+1) < rh)) {
				while(v > (oh << 1)) {
					ycount[n] = -1;
					v -= oh;
					++n;
				}
				ycount[n] = c;
				v -= oh;
				++n;
				c = 0;
			} else {
				++c;
			}
		}
		vh = n + 1;
		ycount[n] = c;

		lastScale = ImageScale;
		lastRotate = ImageRotate;
		lastWidth = ImageWidth;
		lastHeight = ImageHeight;
	}

	if(ImageRotate & 1) { // 90도, 270도 회전
		x = ImageViewX - (ow >> 1);
		y = ImageViewY - (oh >> 1);
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if((x + ow) > ImageWidth)x = ImageWidth - ow;
		if((y + oh) > ImageHeight)y = ImageHeight - oh;

		ImageViewX = x + (ow >> 1);
		ImageViewY = y + (oh >> 1);

    	if((vw < 240)||(vh < 320))
    		SDL_FillRect(pScale, NULL, 0);

    	SDL_LockSurface(pScale);
    	dst = (Uint16 *)pScale->pixels;
    	dst += ((240 - vw) >> 1) * 320 + ((320 - vh) >> 1) + vh - 1;
    	DstPitch = -(320 * vw) -1;

        if(ImageRotate == 1) {
			src = &ImageBuffer[(y + oh - 1) * ImageWidth + x + ow - 1];
			SrcLine = -ImageWidth;
        } else {
			src = &ImageBuffer[y * ImageWidth + x];
			SrcLine = ImageWidth;
    	}

    	int *yc = ycount;
    	for(dy=0;dy<vh;dy++) {
    		int *xc = xcount;
    		for(dx=0;dx<vw;dx++) {
    			*dst = *src;
    			src += *xc++;
    			dst += 320;
    		}
    		src += SrcLine * *yc++;
    		dst += DstPitch;
    	}
    	SDL_UnlockSurface(pScale);
	} else { // 0도, 180도 회전
		x = ImageViewX - (ow >> 1);
		y = ImageViewY - (oh >> 1);
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if((x + ow) > ImageWidth)x = ImageWidth - ow;
		if((y + oh) > ImageHeight)y = ImageHeight - oh;

		ImageViewX = x + (ow >> 1);
		ImageViewY = y + (oh >> 1);

    	if((vw < 320)||(vh < 240))
    		SDL_FillRect(pScale, NULL, 0);

    	SDL_LockSurface(pScale);
    	dst = (Uint16 *)pScale->pixels;
    	dst += ((240 - vh) >> 1) * 320 + ((320 - vw) >> 1);
    	DstPitch = 320 - vw;

        if(ImageRotate == 0) {
			src = &ImageBuffer[y * ImageWidth + x];
			SrcLine = ImageWidth;
        } else {
			src = &ImageBuffer[(y + oh - 1) * ImageWidth + x + ow - 1];
			SrcLine = -ImageWidth;
    	}

    	int *yc = ycount;
    	for(dy=0;dy<vh;dy++) {
    		int *xc = xcount;
    		for(dx=0;dx<vw;dx++) {
    			*dst++ = *src;
    			src += *xc++;
    		}
    		src += SrcLine * *yc++;
    		dst += DstPitch;
    	}
    	SDL_UnlockSurface(pScale);
	}

	return;
}
//[*]------------------------------------------------------------------------------------------[*]
// Screen Fit 모드용 리샘플링
//[*]------------------------------------------------------------------------------------------[*]
SDL_Surface *PhotoViewer::ScreenFitResampling(SDL_Surface *surface, int rw, int rh)
{
	int *rlines, *glines, *blines;
	int *xlines, *ylines, *xcount;
	int n, v, c, i, linelen;
	int ow, oh;
	SDL_Surface *fit;

	Uint16 *src, *dst;
	int SrcPitch, DstPitch;

	int r, g, b, count, ycount;
	int dx, dy;

	ow = surface->w;
	oh = surface->h;

	fit = CreateSurface(rw, rh);

	xlines = (int*)malloc(ow * sizeof(int));
	ylines = (int*)malloc(oh * sizeof(int));

	linelen = rw * sizeof(int);

	rlines = (int*)malloc(linelen);
	glines = (int*)malloc(linelen);
	blines = (int*)malloc(linelen);
	xcount = (int*)malloc(linelen);

	xlines[0] = 0;
	n = 0;v = rw;c = 1;
	for(i=1;i<ow;i++) {
		v += rw;
		if((v > ow)&&((n+1) < rw)) {
			xcount[n] = c;
			xlines[i] = ++n;
			v -= ow;
			c = 1;
		} else {
			xlines[i] = n;
			++c;
		}
	}
	xcount[n] = c;

	ylines[0] = 0;
	n = 0;v = rh;
	for(i=1;i<oh;i++) {
		v += rh;
		if((v > oh)&&((n+1) < rh)) {
			ylines[i] = ++n;
			v -= oh;
		} else {
			ylines[i] = n;
		}
	}

	SDL_LockSurface(fit);
	dst = (Uint16 *)fit->pixels;
	DstPitch = (fit->pitch >> 1) - rw;

	SDL_LockSurface(surface);
	src = (Uint16 *)surface->pixels;
	SrcPitch = (surface->pitch >> 1) - ow;

    n = -1;
	for(dy=0;dy<=oh;dy++) {
		if ((dy == oh) || (n != ylines[dy])) {
			if(n != -1) {
				for(dx=0;dx<rw;dx++,dst++) {
					count = xcount[dx] * ycount;
					r = rlines[dx] / count;
					g = glines[dx] / count;
					b = blines[dx] / count;

					*dst = ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) &0x001F);
				}
				dst += DstPitch;
			}

			if(dy == oh) break;

			memset(rlines, 0, linelen);
			memset(glines, 0, linelen);
			memset(blines, 0, linelen);
			n = ylines[dy];
			ycount = 0;
		}
		for(dx=0;dx<ow;dx++,src++) {
			r = (*src & 0xF800) >> 8;
			g = (*src & 0x07E0) >> 3;
			b = (*src & 0x001F) << 3;
			rlines[xlines[dx]] += r;
			glines[xlines[dx]] += g;
			blines[xlines[dx]] += b;
		}
		src += SrcPitch;
		++ycount;
	}
	SDL_UnlockSurface(surface);
	SDL_UnlockSurface(fit);

	free(xcount);
	free(rlines);free(glines);free(blines);
	free(xlines);free(ylines);

	return fit;
}
//[*]------------------------------------------------------------------------------------------[*]
// 이미지를 LCD 크기에 맞춰서 축소한다.
// 이 과정에서 가로방향 이미지, 세로방향 이미지 두 종류를 동시에 작업한다.
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::ScreenFit(SDL_Surface *surface, int w, int h, int buf)
{
	SDL_Surface *rgb_surface = NULL;
	SDL_Surface *u16_surface = NULL;
	SDL_Surface *src_surface = NULL;
	SDL_Rect rect;
	double wscale, hscale, nscale;
	int viewsize;
	bool src_free = false;

	InfoWidth[buf] = w;
	InfoHeight[buf] = h;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// 배경을 검은색으로 맞춘다.
	SDL_FillRect(pFillH[buf], NULL, 0); // 가로
	SDL_FillRect(pFillV[buf], NULL, 0); // 세로

	if(!surface) return;

	if(surface->format->BitsPerPixel != 16) {
		src_surface = ConvertSurface(surface);
		if(!src_surface) return;
		src_free = true;
	} else {
		src_surface = surface;
	}

	// 가로방향 이미지...

	// 축소를 할 것인지 결정한다.
	// 그림이 LCD보다 클 경우 무조건 줄인다.
	if(surface->w > XRES || surface->h > YRES)
	{
		wscale = (double)XRES/surface->w;
		hscale = (double)YRES/surface->h;
		nscale = (wscale < hscale) ? wscale : hscale;

		///////////////////////////////////////////////////////////////////////////////////////////////
		// 이미지를 화면에 맞춘다.
		rgb_surface = ScreenFitResampling(src_surface, (int)(surface->w * nscale), (int)(surface->h * nscale));

		rect.x = (XRES - rgb_surface->w)/2;
		rect.y = (YRES - rgb_surface->h)/2;

		rect.w = rgb_surface->w;
		rect.h = rgb_surface->h;
		viewsize = rect.w * rect.h;

		SDL_BlitSurface(rgb_surface, NULL, pFillH[buf], &rect);

		if(rgb_surface  != NULL) SDL_FreeSurface(rgb_surface);
		rgb_surface = NULL;
	}
	else // 그림이 LCD 보다 작을 경우 그냥 가운데에 찍는다.
	{
		rect.x = (XRES - surface->w)/2;
		rect.y = (YRES - surface->h)/2;

		rect.w = surface->w;
		rect.h = surface->h;
		viewsize = 0;

		SDL_BlitSurface(surface, NULL, pFillH[buf], &rect);
	}

	// 세로방향 이미지...
	Uint16 *pSurface;
	Uint16 *ptrFillV;
	int x, y, FillVPitch, SurfaceDir, SurfacePitch;

	// 축소를 할 것인지 결정한다.
	// 그림이 LCD보다 클 경우 무조건 줄인다.
	if(surface->w > YRES || surface->h > XRES)
	{
		// 이미지의 사이즈를 현재 LCD 사이즈 만큼 축소,확대할 값을 구한다.
		wscale = (double)XRES/surface->h;
		hscale = (double)YRES/surface->w;
		nscale = (wscale < hscale) ? wscale : hscale;

		///////////////////////////////////////////////////////////////////////////////////////////////
		// 이미지를 화면에 맞춘다.
		rgb_surface = ScreenFitResampling(src_surface, (int)(surface->w * nscale), (int)(surface->h * nscale));
		u16_surface = ConvertSurface(rgb_surface);

		if(rgb_surface != NULL) SDL_UnlockSurface(rgb_surface);
	}
	else	// 그림이 LCD 보다 작을 경우 그냥 가운데에 찍는다.
	{
		u16_surface = ConvertSurface(surface);
	}

	if(src_free)
		SDL_FreeSurface(src_surface);

	if(u16_surface == NULL) return;

	rect.w = u16_surface->h;
	rect.h = u16_surface->w;

	rect.x = (XRES - rect.w)/2;
	rect.y = (YRES - rect.h)/2;

	SDL_LockSurface(pFillV[buf]);
	ptrFillV = (Uint16 *)pFillV[buf]->pixels;
	ptrFillV += (rect.y * 320) + rect.x;
	FillVPitch = 320 - rect.w;

	SDL_LockSurface(u16_surface);
	pSurface = (Uint16 *)u16_surface->pixels;
	pSurface += rect.h - 1;
	SurfaceDir = (u16_surface->pitch >> 1);
	SurfacePitch = -(rect.w * SurfaceDir) -1;

	for(y=0;y<rect.h;y++) {
		for(x=0;x<rect.w;x++,ptrFillV++,pSurface+=SurfaceDir) {
			*ptrFillV = *pSurface;
		}
		ptrFillV += FillVPitch;
		pSurface += SurfacePitch;
	}

	SDL_UnlockSurface(pFillV[buf]);

	SDL_UnlockSurface(u16_surface);
	SDL_FreeSurface(u16_surface);

	// g_ImageRotate = 0;
	// 회전의 기본값을 그림이 가장 크게 보이는 방향으로 할 경우.
	// g_ImageRotate = (((viewsize == 0) || (viewsize >= (rect.w * rect.h))) ? 0 : 3);
}
//[*]------------------------------------------------------------------------------------------[*]
// 코믹 모드에서의 리사이즈(리샘플링)
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::ComicResampling(int buf)
{
    if(BufferedType != BUF_COMIC) return;

	int *rlines, *glines, *blines;
	int *xlines, *ylines, *xcount;
	int n, v, c, i, linelen;
	double scale;

	int x, y;
	Uint16 *src, *dst;
	int SrcDir, SrcPitch;
	int DstPitch;

	int r, g, b, count, ycount;
	int dx, dy;

	int rw, rh;
	int ow = ComicWidth[buf];
	int oh = ComicHeight[buf];


	if(ComicPageType[buf] == COMIC_ONE_PAGE) {
        rw = 320 + ((ComicResizeWidth - 600) >> 1);
    } else {
        rw = ComicResizeWidth;
    }

    scale = (double)rw / ow;
    ComicScale[buf] = scale;
    if(scale >= 1) {
        ComicIsResize[buf] = false;
        return;
    }
    rh = (int)(oh * scale);

	xlines = (int*)malloc(ow * sizeof(int));
	ylines = (int*)malloc(oh * sizeof(int));

	linelen = rw * sizeof(int);

	rlines = (int*)malloc(linelen);
	glines = (int*)malloc(linelen);
	blines = (int*)malloc(linelen);
	xcount = (int*)malloc(linelen);

	xlines[0] = 0;
	n = 0;v = rw;c = 1;
	for(i=1;i<ow;i++) {
		v += rw;
		if((v > ow)&&((n+1) < rw)) {
			xcount[n] = c;
			xlines[i] = ++n;
			v -= ow;
			c = 1;
		} else {
			xlines[i] = n;
			++c;
		}
	}
	xcount[n] = c;

	ylines[0] = 0;
	n = 0;v = rh;
	for(i=1;i<oh;i++) {
		v += rh;
		if((v > oh)&&((n+1) < rh)) {
			ylines[i] = ++n;
			v -= oh;
		} else {
			ylines[i] = n;
		}
	}

	//SrcPitch = ImageWidth - ow;

	src = ComicBuffer[buf];
	dst = ComicBuffer[buf] + (ComicWidth[buf] * ComicHeight[buf]);
	ComicResizeBuffer[buf] = dst;

    n = -1;
	for(dy=0;dy<=oh;dy++) {
		if ((dy == oh) || (n != ylines[dy])) {
			if(n != -1) {
				for(dx=0;dx<rw;dx++,dst++) {
					count = xcount[dx] * ycount;
					r = rlines[dx] / count;
					g = glines[dx] / count;
					b = blines[dx] / count;

					*dst = ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) &0x001F);
				}
			}

			if(dy == oh) break;

			memset(rlines, 0, linelen);
			memset(glines, 0, linelen);
			memset(blines, 0, linelen);
			n = ylines[dy];
			ycount = 0;
		}
		for(dx=0;dx<ow;dx++,src++) {
			r = (*src & 0xF800) >> 8;
			g = (*src & 0x07E0) >> 3;
			b = (*src & 0x001F) << 3;
			rlines[xlines[dx]] += r;
			glines[xlines[dx]] += g;
			blines[xlines[dx]] += b;
		}
		//src += SrcPitch;
		++ycount;
	}

	free(xcount);
	free(rlines);free(glines);free(blines);
	free(xlines);free(ylines);

    ComicIsResize[buf] = true;
	ComicResizedWidth[buf] = rw;
	ComicResizedHeight[buf] = rh;

	//printf("Comic Resize: %d x %d to %d x %d\n", ow, oh, rw, rh);

	return;
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::ViewComic(SDL_Surface *pScreen) {
	Uint16 *pScreenMem, *pImage;
	int i, j, buf;
    int SrcPitch;

	SDL_LockSurface(pScreen);
	pScreenMem = (Uint16 *)pScreen->pixels;

    bool scaled = (ImageScale == 1) && ComicIsResize[ComicViewBuffer] && (ComicResizeBuffer[ComicViewBuffer] != NULL);

    if(scaled) {
    	pImage = ComicResizeBuffer[ComicViewBuffer];

    	if(ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
            if(ComicViewX > ((ComicResizedWidth[ComicViewBuffer] - 320) >> 1))
                ComicViewX = ((ComicResizedWidth[ComicViewBuffer] - 320) >> 1);
        } else {
            if(ComicViewX > ((ComicResizedWidth[ComicViewBuffer] - 600) >> 2))
                ComicViewX = ((ComicResizedWidth[ComicViewBuffer] - 600) >> 2);
        }
        if(ComicViewX < 0)ComicViewX = 0;

        if((ComicViewY + 240) > ComicResizedHeight[ComicViewBuffer])
            ComicViewY = ComicResizedHeight[ComicViewBuffer] - 240;
        if(ComicViewY < 0)ComicViewY = 0;

    	pImage += ComicViewY * ComicResizedWidth[ComicViewBuffer];

        if(ComicViewPage & 1) {
            pImage += ComicResizedWidth[ComicViewBuffer] - 320 - ComicViewX;
        } else {
            pImage += ComicViewX;
        }

        SrcPitch = ComicResizedWidth[ComicViewBuffer] - 320;
    } else {
    	pImage = ComicBuffer[ComicViewBuffer];

    	if(ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
            if((ImageViewX + 320) > ComicWidth[ComicViewBuffer])
                ImageViewX = ComicWidth[ComicViewBuffer] - 320;
        } else {
            if(ImageViewX > (int)(ComicWidth[ComicViewBuffer] * 0.55 - 320))
                ImageViewX = (int)(ComicWidth[ComicViewBuffer] * 0.55 - 320);
        }
        if(ImageViewX < 0)ImageViewX = 0;

        if(ImageViewY < 0)ImageViewY = 0;
        if((ImageViewY + 240) > ComicHeight[ComicViewBuffer])
            ImageViewY = ComicHeight[ComicViewBuffer] - 240;

    	pImage += ImageViewY * ComicWidth[ComicViewBuffer];

        if(ComicViewPage & 1) {
            pImage += ComicWidth[ComicViewBuffer] - 320 - ImageViewX;
        } else {
            pImage += ImageViewX;
        }

        SrcPitch = ComicWidth[ComicViewBuffer] - 320;
    }

	for(i=0;i<240;i++) {
    	for(j=0;j<320;j++) {
    		*pScreenMem++ = *pImage++;
    	}
    	pImage += SrcPitch;
	}

	SDL_UnlockSurface(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// 그림 파일을 불러온다.
// 현재 jpg 의 경우 별도 처리를 하고 있고, 그 외의 경우는 SDL_image를 사용한다.
//[*]------------------------------------------------------------------------------------------[*]
bool PhotoViewer::LoadImage(int index)
{
	if(index < 0) index = itemIndex;
	if((index < 0) || (index >= fileList.size())) return false;

	int i;
	for (i=0;i<PHOTO_BUFFER_COUNT;i++) {
		ImageLoadPosition[i] = -99;
	}

	if(isComicMode()) {
		for(i=0;i<PHOTO_BUFFER_COUNT;i++)
			ComicLoadPosition[i] = -99;
		ComicViewBuffer = 0;
		itemIndex = index;

		LoadComicImage(itemIndex, ComicViewBuffer);
		if(ComicLoadPosition[ComicViewBuffer] < 0)
			return false;
	} else {
		ImageRotate = 0;
		ImageViewMode = PHOTO_FIT_VIEW;
		ImageViewBuffer = 0;
		itemIndex = index;

		LoadFitImage(itemIndex, ImageViewBuffer);
		if(ImageLoadPosition[ImageViewBuffer] < 0)
			return false;

		BufferedType = BUF_EMPTY;
	}

	DoPrevLoading = true;
	DoNextLoading = true;
	DoFullLoading = true;
	BufferChanged = true;

	movex = movey = 0;
	memset(moveDir, 0, sizeof(moveDir));

	return true;
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::LoadFitImage(int index, int buf)
{
	if(ImageLoadPosition[buf] == index) return;

	SDL_LockMutex(LoadLock);

	bool loaded;
	std::string name;
	int attr;

	name = path;
	name += "/";
	name += fileList[index].first;
	attr = fileList[index].second;

	switch(attr) {
		case JPG_FORMAT :
			loaded = pJpeg.Load(name.c_str(), buf);
			break;
		default:
			loaded = false;
			break;
	}

	if(!loaded) {
		SDL_Surface *image = NULL;
		image = IMG_Load(name.c_str()); // 이미지를 로드한다.
		if(image != NULL) {
			ScreenFit(image, image->w, image->h, buf);
			SDL_FreeSurface(image);
			loaded = true;
		}
	}

	if(loaded) {
		ImageLoadPosition[buf] = index;
		ImageLoadName[buf] = name;
	} else {
		ImageLoadPosition[buf] = -99;
	}

	SDL_UnlockMutex(LoadLock);
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::LoadFullImage()
{
	if(BufferedType == BUF_SCALE) return;

	SDL_LockMutex(LoadLock);

	bool loaded;
	std::string name;
	int attr;

	ImageWidth = 0;
	ImageHeight = 0;

	name = path;
	name += "/";
	name += fileList[itemIndex].first;
	attr = fileList[itemIndex].second;

	switch(attr) {
		case JPG_FORMAT :
			loaded = pJpeg.FullLoad(name.c_str());
			break;
		default:
			loaded = false;
	}

	while(!loaded && DoFullLoading) {
		SDL_Surface *image = NULL;
		SDL_Surface *convert = NULL;
		char *pLine;
		int i;

		image = IMG_Load(name.c_str()); // 이미지를 로드한다.
		if(image == NULL) break;
		convert = ConvertSurface(image);
		if(convert == NULL) {
			SDL_FreeSurface(image);
			break;
		}

		SDL_LockSurface(convert);
		pLine = (char *)convert->pixels;
		ResizeBegin(convert->w, convert->h);
		for(i=0;DoFullLoading && (i<convert->h);i++) {
			Resize16(pLine, i);
			pLine += convert->pitch;
		}
		ResizeEnd();
		SDL_UnlockSurface(convert);

		SDL_FreeSurface(convert);
		SDL_FreeSurface(image);

		loaded = true;
	}

	if(loaded && DoFullLoading)
		BufferedType = BUF_SCALE;
	else
		BufferedType = BUF_EMPTY;

	SDL_UnlockMutex(LoadLock);
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::LoadComicImage(int index, int buf)
{
	if((BufferedType == BUF_COMIC) && (ComicLoadPosition[buf] == index))return;

	SDL_LockMutex(LoadLock);
	ComicWidth[buf] = 0;
	ComicHeight[buf] = 0;

	bool loaded;
	std::string name;
	int attr;

	name = path;
	name += "/";
	name += fileList[index].first;
	attr = fileList[index].second;

	switch(attr) {
		case JPG_FORMAT :
			loaded = pJpeg.ComicLoad(name.c_str(), buf);
			break;
		default:
			loaded = false;
	}

	while(!loaded) {
		SDL_Surface *image = NULL;
		SDL_Surface *convert = NULL;
		char *pLine;
		int i;

		image = IMG_Load(name.c_str()); // 이미지를 로드한다.
		if(image == NULL) break;
		if((image->w < 320)||(image->h < 240)) {
			SDL_FreeSurface(image);
			break;
		}
		convert = ConvertSurface(image);
		if(convert == NULL) {
			SDL_FreeSurface(image);
			break;
		}

		if(ResizeComic(convert->w, convert->h, buf)) {
    		SDL_LockSurface(convert);
    		pLine = (char *)convert->pixels;
    		for(i=0;i<convert->h;i++) {
    			Resize16(pLine, i);
    			pLine += convert->pitch;
    		}
    		SDL_UnlockSurface(convert);
    		ResizeComicEnd();

    		loaded = true;
        }

		SDL_FreeSurface(convert);
		SDL_FreeSurface(image);

        break;
	}

	if(loaded) {
		BufferedType = BUF_COMIC;
		ComicResampling(buf);
		ComicLoadPosition[buf] = index;
		ComicLoadName[buf] = name;
	} else {
		ComicLoadPosition[buf] = -99;
	}

	SDL_UnlockMutex(LoadLock);
}
//[*]------------------------------------------------------------------------------------------[*]
int PhotoViewer::getSetting(int type)
{
	switch(type) {
		case PHOTO_VIEW_MODE:
			return ImageViewMode;
		case PHOTO_BUFFER_TYPE:
			return BufferedType;
		case PHOTO_SCALE:
			return int(ImageScale * 100);
	}
	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::setSetting(int type, int value)
{
	switch(type) {
		case PHOTO_VIEW_MODE:
			if(value == ImageViewMode) return;
			ImageViewMode = value;

			switch(ImageViewMode) {
				case PHOTO_FIT_VIEW:
					if(ImageLoadPosition[ImageViewBuffer] != itemIndex) { // Loading Large Image...
						int i;
						for(i=0;i<PHOTO_BUFFER_COUNT;i++)
							ImageLoadPosition[i] = -99;
						ImageViewBuffer = 0;
						LoadFitImage(itemIndex, 0);
						BufferedType = BUF_EMPTY;
					}
					movex = movey = 0;
					break;
				case PHOTO_SCALE_VIEW:
					ImageScale = 1;
					setSetting(PHOTO_SET_MOVE, 9);
					break;
				case PHOTO_COMIC_LEFT_VIEW:
				case PHOTO_COMIC_RIGHT_VIEW:
					if(BufferedType != BUF_COMIC) { // Loading Large Image...
						int i;
						for(i=0;i<PHOTO_BUFFER_COUNT;i++)
							ComicLoadPosition[i] = -99;
						ComicViewBuffer = 0;
						LoadComicImage(itemIndex, 0);
					}

					if((ComicWidth[0] <= 0) || (ComicHeight[0] <= 0)) {
						setSetting(PHOTO_VIEW_MODE, PHOTO_FIT_VIEW);
						return;
					}

					ComicBufferResampling = false;

					ImageScale = 1;
					ComicViewX = 0;
					ComicViewY = 0;
					ImageViewX = 0;
					ImageViewY = 0;
					ComicViewPage = (ImageViewMode== PHOTO_COMIC_LEFT_VIEW) ? 1 : 0;
					if(ComicPageType[0] == COMIC_ONE_PAGE)
						ComicViewPage = 0;

					setSetting(PHOTO_SET_MOVE, 9);
					break;
			}

			DoPrevLoading = true;
			DoNextLoading = true;
			DoFullLoading = true;
			BufferChanged = true;
			break;
		case PHOTO_SCALE:
			if(ImageViewMode == PHOTO_FIT_VIEW) {
				ImageViewMode = PHOTO_SCALE_VIEW;
				ImageScale = -1;
			}

			if(ImageViewMode == PHOTO_SCALE_VIEW) {

				//아래 while 루프문에서 두번째 로딩 시 무한루프로 빠져 버린다
				while(BufferedType != BUF_SCALE) {
					usleep(1);
					break; //break를 안 해주면 로딩 후 메뉴로 나간 후 다시 들어온후 ZOOM_IN(OUT) 시 무한루프로 빠져버린다
				}

				if(ImageScale < 0) {
					double h, v;
					if(ImageRotate & 1) {
						h = 240.0 / ImageWidth;
						v = 320.0 / ImageHeight;
					} else {
						h = 320.0 / ImageWidth;
						v = 240.0 / ImageHeight;
					}
					ImageScale = (h < v ? h : v);
					if(ImageScale > 1) ImageScale = 1;
				};
				if(ImageScale < 0) ImageScale = 1;

				switch(value) {
					case PHOTO_ZOOM_IN:
						if(ImageScale < 0.2) ImageScale += 0.02;
						else if(ImageScale < 0.5) ImageScale += 0.05;
						else if(ImageScale < 2) ImageScale += 0.1;
						else if(ImageScale < 4) ImageScale += 0.2;
						break;
					case PHOTO_ZOOM_OUT:
						if(ImageScaleBreak) break;
						if(ImageScale > 2) ImageScale -= 0.2;
						else if(ImageScale > 0.5) ImageScale -= 0.1;
						else if(ImageScale > 0.2) ImageScale -= 0.05;
						else if(ImageScale > 0.05) ImageScale -= 0.02;
						break;
					case PHOTO_ORIGINAL:
						ImageScale = 1.0;
						break;
					case PHOTO_FIT:
						if(ImageScale == 1.0) {
							setSetting(PHOTO_VIEW_MODE, PHOTO_FIT_VIEW);
							return;
						}
						ImageScale = 1.0;
						break;
				}

				if((ImageScale < 0.53) && (ImageScale > 0.47))
					ImageScale = 0.5;
				else if((ImageScale < 1.1) && (ImageScale > 0.9))
					ImageScale = 1;
				else if((ImageScale < 2.1) && (ImageScale > 1.9))
					ImageScale = 2;

				setSetting(PHOTO_SET_MOVE, 9);
			} else {
				int oldWidth = ComicResizeWidth;
				switch(value) {
					case PHOTO_ZOOM_IN:
						ComicResizeWidth += 10;
						break;
					case PHOTO_ZOOM_OUT:
						ComicResizeWidth -= 10;
						break;
					case PHOTO_ORIGINAL:
					case PHOTO_FIT:
						if(!ComicIsResize[ComicViewBuffer]) {
							ImageScale = 2;
						} else if(ImageScale == 1) {
							ImageScale = 2;
							ImageViewY = (int)((double)(ComicViewY + 120) / ComicScale[ComicViewBuffer]) - 120;
							if(ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
								ImageViewX = (ComicWidth[ComicViewBuffer] >> 1) - 160;
							} else {
								ImageViewX = (int)((double)ComicWidth[ComicViewBuffer] * 0.275) - 160;
							}
						} else {
							ImageScale = 1;
							ComicViewY = (int)((double)(ImageViewY + 120) * ComicScale[ComicViewBuffer]) - 120;
						}
						break;
				}
				if(ComicResizeWidth < 600) ComicResizeWidth = 600;
				if(ComicResizeWidth > 800) ComicResizeWidth = 800;
				if(oldWidth != ComicResizeWidth) {
					ComicResampling(ComicViewBuffer);
					ComicBufferResampling = true;
				}
			}
			break;
		case PHOTO_ROTATE:
			if(value == PHOTO_ROTATE_RIGHT) ImageRotate+=3;
			else ImageRotate++;
			ImageRotate &= 3;
			break;
		case PHOTO_SET_MOVE:
			if(value < 0) {
				movex = movey = 0;
				memset(moveDir, 0, sizeof(moveDir));
				return;
			}

			bool down = true;

			if(value > 9) {
				down = false;
				value -= 10;
			}

			if(value != 9) {
				if(moveDir[value] == down) return;
				moveDir[value] = down;
			}

			int i;
			int arrow = -1;

			for(i=0;i<8;i++) {
				if(moveDir[i]) {
					if(arrow == -1)
						arrow = i;
					else {
						if(arrow == i-1) { // 8방향 -> 16방향
							arrow += 8;
						} else if((arrow == 0) && (i == 7)) {
							arrow = 15;
						} else if(arrow == i-2) { // 4방향 -> 8방향
							arrow += 1;
						} else if((arrow == 0) && (i == 6)) {
							arrow = 7;
						} else {
							arrow = -1;
						}
					}
				}
			}

			switch(arrow){
				case 0: movex = 0;movey = -10; break;
				case 1: movex = -7;movey = -7; break;
				case 2: movex = -10;movey = 0; break;
				case 3: movex = -7;movey = 7; break;
				case 4: movex = 0;movey = 10; break;
				case 5: movex = 7;movey = 7; break;
				case 6: movex = 10;movey = 0; break;
				case 7: movex = 7;movey = -7; break; // 여기까지 8방향
				case 8: movex = -4;movey = -9; break;
				case 9: movex = -9;movey = -4; break;
				case 10: movex = -9;movey = 4; break;
				case 11: movex = -4;movey = 9; break;
				case 12: movex = 4;movey = 9; break;
				case 13: movex = 9;movey = 4; break;
				case 14: movex = 9;movey = -4; break;
				case 15: movex = 4;movey = -9; break; // 여기까지 16방향
				default: movex = movey = 0; break;
			}

			if(ImageViewMode == PHOTO_SCALE_VIEW) {
				if(movex) movex = (int)(movex / ImageScale);
				if(movey) movey = (int)(movey / ImageScale);
			}
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
std::string PhotoViewer::getInfo(int type)
{
	switch(type) {
		case PHOTO_FILE_NAME:
			if((itemIndex >= 0) && (itemIndex < fileList.size()))
				return fileList[itemIndex].first;
			break;
		case PHOTO_INFO_STR:
		    char info[64];
			switch(ImageViewMode){
				case PHOTO_FIT_VIEW:
				case PHOTO_SCALE_VIEW: // 확대 모드에서의 출력
		            sprintf(info, "%d x %d [%d / %d]", InfoWidth[ImageViewBuffer], InfoHeight[ImageViewBuffer],
		                ImageLoadPosition[ImageViewBuffer] - 0 + 1,
		                fileList.size() - 0);
					break;
				case PHOTO_COMIC_LEFT_VIEW:
		            sprintf(info, "Comic(Left) [%d / %d]", ComicLoadPosition[ComicViewBuffer] - 0 + 1,
		                fileList.size() - 0);
					break;
				case PHOTO_COMIC_RIGHT_VIEW:
		            sprintf(info, "Comic(Right) [%d / %d]", ComicLoadPosition[ComicViewBuffer] - 0 + 1,
		                fileList.size() - 0);
					break;
			}
			return info;
	}

	return "None";
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::Draw(SDL_Surface *pScreen)
{
	switch(ImageViewMode) {
		case PHOTO_FIT_VIEW:
			switch(ImageRotate){
				case 0: // 0도(기본값)
					SDL_BlitSurface(pFillH[ImageViewBuffer], NULL, pScreen, NULL);
					break;
				case 1: // 90도
					SDL_BlitSurface(pFillV[ImageViewBuffer], NULL, pScreen, NULL);
					break;
				case 2: // 180도
				case 3: // 270도
					{
						Uint16 *pScreenMem, *pImage;
						int i;
						SDL_LockSurface(pScreen);

						pScreenMem = (Uint16 *)pScreen->pixels;
						if(ImageRotate == 2)
							pImage = (Uint16 *)pFillH[ImageViewBuffer]->pixels;
						else
							pImage = (Uint16 *)pFillV[ImageViewBuffer]->pixels;
						pImage += 320*240-1;

						for(i=320*240;i;--i,++pScreenMem,--pImage) {
							*pScreenMem = *pImage;
						}

						SDL_UnlockSurface(pScreen);
						break;
					}
				default:
					SDL_FillRect(pScreen, NULL, 0);
					break;
			}
			break;
		case PHOTO_SCALE_VIEW:
			ZoomInOut();
			SDL_BlitSurface(pScale, NULL, pScreen, NULL);
			break;
		case PHOTO_COMIC_LEFT_VIEW:
		case PHOTO_COMIC_RIGHT_VIEW:
			ViewComic(pScreen);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
bool PhotoViewer::PrevImage(bool fileMode)
{
	if(isComicMode()) {
		if((!fileMode) && (ComicViewY > 0)) {
			ComicViewY -= 140;
			if(ComicViewY < 40)
				ComicViewY = 0;
			return true;
		}

		bool end = !fileMode;

		int PrevBuffer = ComicViewBuffer - 1;
		if(PrevBuffer < 0) PrevBuffer = PHOTO_BUFFER_COUNT - 1;
		DoNextLoading = false;

		ImageViewY = 0;
		if(end) {
			if(ComicResizeBuffer[ComicViewBuffer])
				ComicViewY = ComicResizedHeight[ComicViewBuffer] - 240;
			ImageViewY = ComicHeight[ComicViewBuffer] - 240;
		} else {
			ComicViewY = 0;
		}
		if (ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
            if (ImageViewMode == PHOTO_COMIC_LEFT_VIEW) {
    			ComicViewPage = 0;
            } else {
    			ComicViewPage = 1;
            }
		} else if (ImageViewMode == PHOTO_COMIC_LEFT_VIEW) {
			if(ComicViewPage == 1) {
				ComicViewPage = 0;
			} else {
				ComicViewPage = 1;
				return true;
			}
		} else if (ImageViewMode == PHOTO_COMIC_RIGHT_VIEW) {
			if(ComicViewPage == 1) {
				ComicViewPage = 0;
				return true;
			} else {
				ComicViewPage = 1;
			}
		}

		while (BufferChanged) {
			usleep(1);
		}

		if(ComicLoadPosition[PrevBuffer] == -1){
            if (end) ComicViewY = ImageViewY = 0;
            if (ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
                ComicViewPage = 0;
            } else if (ImageViewMode == PHOTO_COMIC_LEFT_VIEW) {
				ComicViewPage = 1;
			} else if (ImageViewMode == PHOTO_COMIC_RIGHT_VIEW) {
				ComicViewPage = 0;
			}
			return false;
		}

		ComicViewBuffer = PrevBuffer;
		if (ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
			ComicViewPage = 0;
        }
        
		itemIndex = ComicLoadPosition[ComicViewBuffer];
		//std::ostringstream ostrpath;
		//ostrpath << path << "/" << fileList[itemIndex].first;

		if(ComicBufferResampling) {
			ComicResampling(ComicViewBuffer);
			ComicBufferResampling = false;
		}
		if(end) {
			if(ComicResizeBuffer[ComicViewBuffer])
				ComicViewY = ComicResizedHeight[ComicViewBuffer] - 240;
			ImageViewY = ComicHeight[ComicViewBuffer] - 240;
		}

		PrevBuffer = ComicViewBuffer - 1;
		if(PrevBuffer < 0) PrevBuffer = PHOTO_BUFFER_COUNT - 1;
		ComicLoadPosition[PrevBuffer] = -99;

		DoPrevLoading = true;
		DoNextLoading = true;
		DoFullLoading = true;
		BufferChanged = true;
		return true;
	} else {
		int PrevBuffer = (ImageViewBuffer + PHOTO_BUFFER_COUNT - 1) % PHOTO_BUFFER_COUNT;
		DoNextLoading = false;

		if(ImageLoadPosition[PrevBuffer] != -1){
			DoFullLoading = false;
			BufferedType = BUF_EMPTY;
		}

		while (BufferChanged) {
			usleep(1);
		}

		if(ImageLoadPosition[PrevBuffer] == -1){
			BufferChanged = true;
			return false;
		}

		ImageViewBuffer = PrevBuffer;
		ImageViewMode = PHOTO_FIT_VIEW;
		ImageRotate = 0;
		itemIndex = ImageLoadPosition[ImageViewBuffer];

		PrevBuffer = (ImageViewBuffer + PHOTO_BUFFER_COUNT - 1) % PHOTO_BUFFER_COUNT;
		ImageLoadPosition[PrevBuffer] = -99;

		BufferedType = BUF_EMPTY;

		DoPrevLoading = true;
		DoNextLoading = true;
		DoFullLoading = true;
		BufferChanged = true;
		return true;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
bool PhotoViewer::NextImage(bool fileMode)
{
	if(isComicMode()) {
		if((!fileMode) && ((ComicViewY + 240) < ComicResizedHeight[ComicViewBuffer])) {
			ComicViewY += 140;
			if((ComicViewY + 280) >= ComicResizedHeight[ComicViewBuffer])
				ComicViewY = ComicResizedHeight[ComicViewBuffer] - 240;
			return true;
		}

		int NextBuffer = (ComicViewBuffer + 1) % PHOTO_BUFFER_COUNT;
		DoPrevLoading = false;

		ImageViewY = 0;
		ComicViewY = 0;
		if (ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
            if (ImageViewMode == PHOTO_COMIC_LEFT_VIEW) {
    			ComicViewPage = 1;
            } else {
    			ComicViewPage = 0;
            }
		} else if (ImageViewMode == PHOTO_COMIC_LEFT_VIEW) {
			if(ComicViewPage == 1) {
				ComicViewPage = 0;
				return true;
			} else {
				ComicViewPage = 1;
			}
		} else if (ImageViewMode == PHOTO_COMIC_RIGHT_VIEW) {
			if(ComicViewPage == 1) {
				ComicViewPage = 0;
			} else {
				ComicViewPage = 1;
				return true;
			}
		}

		while (BufferChanged) {
			usleep(1);
		}

		if(ComicLoadPosition[NextBuffer] == -1){
            if (ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
                ComicViewPage = 0;
			} else if (ImageViewMode == PHOTO_COMIC_LEFT_VIEW) {
				ComicViewPage = 0;
			} else if (ImageViewMode == PHOTO_COMIC_RIGHT_VIEW) {
				ComicViewPage = 1;
			}
			return false;
		}

		ComicViewBuffer = NextBuffer;
		if (ComicPageType[ComicViewBuffer] == COMIC_ONE_PAGE) {
			ComicViewPage = 0;
        }

		itemIndex = ComicLoadPosition[ComicViewBuffer];

		if(ComicBufferResampling) {
			ComicResampling(ComicViewBuffer);
			ComicBufferResampling = false;
		}

		NextBuffer = ComicViewBuffer + 1;
		if(NextBuffer >= PHOTO_BUFFER_COUNT) NextBuffer = 0;
		ComicLoadPosition[NextBuffer] = -99;

		DoPrevLoading = true;
		DoNextLoading = true;
		DoFullLoading = true;
		BufferChanged = true;
		return true;
	} else {
		int NextBuffer = (ImageViewBuffer + 1) % PHOTO_BUFFER_COUNT;
		DoFullLoading = false;
		DoPrevLoading = false;

		if(ImageLoadPosition[NextBuffer] != -1){
			DoFullLoading = false;
			BufferedType = BUF_EMPTY;
		}

		while (BufferChanged) {
			usleep(1);
		}

		if(ImageLoadPosition[NextBuffer] == -1){
			BufferChanged = true;
			return false;
		}

		ImageViewBuffer = NextBuffer;
		ImageViewMode = PHOTO_FIT_VIEW;
		ImageRotate = 0;
		itemIndex = ImageLoadPosition[ImageViewBuffer];

		NextBuffer = (ImageViewBuffer + 1) % PHOTO_BUFFER_COUNT;
		ImageLoadPosition[NextBuffer] = -99;

		BufferedType = BUF_EMPTY;

		DoPrevLoading = true;
		DoNextLoading = true;
		DoFullLoading = true;
		BufferChanged = true;
		return true;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
bool PhotoViewer::Moving()
{
	if(((movex == 0) && (movey == 0)) || (ImageViewMode == PHOTO_FIT_VIEW)) return false;

	if(isComicMode()) {
		if((ImageScale == 1) && ComicIsResize[0]) {
			if(moveDir[2] || moveDir[6]) {
				if(ComicViewPage & 1) {
					ComicViewX -= movex / 3;
				} else {
					ComicViewX += movex / 3;
				}
			} else if(moveDir[0] || moveDir[1] || moveDir[7]){
				ComicViewY -= 10;
			} else if(moveDir[3] || moveDir[4] || moveDir[5]){
				ComicViewY += 10;
			}
		} else {
			if(ComicViewPage & 1) {
				ImageViewX -= movex;
			} else {
				ImageViewX += movex;
			}
			ImageViewY += movey;
		}
	} else {
		switch(ImageRotate){
		case 0:
			ImageViewX += movex;
			ImageViewY += movey;
			break;
		case 1:
			ImageViewX -= movey;
			ImageViewY += movex;
			break;
		case 2:
			ImageViewX -= movex;
			ImageViewY -= movey;
			break;
		case 3:
			ImageViewX += movey;
			ImageViewY -= movex;
			break;
		}
	}

	return true;
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::SaveComicMark(void)
{
	std::string mark_path = path + "/gp2xmark.ini";
	INI_Open(mark_path.c_str());
	if(isComicMode()) {
		INI_WriteInt("photo", "index", itemIndex);
		INI_WriteInt("photo", "total", fileList.size());
		INI_WriteInt("photo", "view", ImageViewMode);
		INI_WriteInt("photo", "width", ComicResizeWidth);
		INI_WriteInt("photo", "margin", ComicViewX);
		INI_WriteInt("photo", "page", ComicViewPage);
	} else if(INI_ReadInt("photo", "index", -1) >= 0) {
		INI_WriteInt("photo", "index", -1);
		INI_WriteInt("photo", "total", -1);
		INI_WriteInt("photo", "view", PHOTO_FIT_VIEW);
	}
	INI_Close();
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::LoadComicMark(const char *load_path)
{
	std::string mark_path = load_path + std::string("/gp2xmark.ini");
	INI_Open(mark_path.c_str());
	ImageViewMode = INI_ReadInt("photo", "view", PHOTO_FIT_VIEW);
	if(isComicMode()) {
		ComicResizeWidth = INI_ReadInt("photo", "width", 600);
		if(ComicResizeWidth < 600) ComicResizeWidth = 600;
		if(ComicResizeWidth > 800) ComicResizeWidth = 800;

		ComicViewX = INI_ReadInt("photo", "margin", 0);
        if(ComicViewX < 0) ComicViewX = 0;

		ComicViewPage = INI_ReadInt("photo", "page", (ImageViewMode== PHOTO_COMIC_LEFT_VIEW) ? 1 : 0);
		if((ComicViewPage < 0) || (ComicViewPage > 1))
			ComicViewPage = (ImageViewMode== PHOTO_COMIC_LEFT_VIEW) ? 1 : 0;

		ImageScale = 1;
		ComicViewY = 0;
	} else {
		ImageViewMode = PHOTO_FIT_VIEW;
	}
	INI_Close();
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::Break(void)
{
	DoNextLoading = false;
	DoPrevLoading = false;
	DoFullLoading = false;
	while (BufferChanged)
		usleep(1);
	BufferedType = BUF_EMPTY;

	for (int i=0;i<PHOTO_BUFFER_COUNT;i++)
	{
		ImageLoadPosition[i] = -99;
		ComicLoadPosition[i] = -99;
	}
	SaveComicMark();
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::reset(std::string &value)
{
	fileList.clear();
	itemIndex = 0;
	path = value;
}
//[*]------------------------------------------------------------------------------------------[*]
void PhotoViewer::addList(std::string &name, int attr, bool active)
{
	if(active) itemIndex = fileList.size();
	fileList.push_back(std::make_pair(name, attr));
}
//[*]------------------------------------------------------------------------------------------[*]
