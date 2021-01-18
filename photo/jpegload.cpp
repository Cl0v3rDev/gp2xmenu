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
#include <setjmp.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "photoviewer.h"
#include "fileload.h"
#include "jpegload.h"
//[*]------------------------------------------------------------------------------------------[*]
extern "C" {
	#include <jpeglib.h>
}
//[*]------------------------------------------------------------------------------------------[*]

#define INPUT_BUFFER_SIZE	32768
#define MAX_WIDTH			10000
typedef struct {
	struct jpeg_source_mgr pub;

	SDL_RWops *ctx;
	Uint8 buffer[INPUT_BUFFER_SIZE];
} my_source_mgr;

static char line[MAX_WIDTH * 3];
static my_source_mgr source_mgr;

extern CMainMenuApp theApp;

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */
static void init_source (j_decompress_ptr cinfo)
{
	return;
}

static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
	my_source_mgr * src = (my_source_mgr *) cinfo->src;
	int nbytes;

	nbytes = SDL_RWread(src->ctx, src->buffer, 1, INPUT_BUFFER_SIZE);
	if (nbytes <= 0) {
		/* Insert a fake EOI marker */
		src->buffer[0] = (Uint8) 0xFF;
		src->buffer[1] = (Uint8) JPEG_EOI;
		nbytes = 2;
	}
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;

	return TRUE;
}

static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	my_source_mgr * src = (my_source_mgr *) cinfo->src;

	/* Just a dumb implementation for now.	Could use fseek() except
	 * it doesn't work on pipes.  Not clear that being smart is worth
	 * any trouble anyway --- large skips are infrequent.
	 */
	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) src->pub.fill_input_buffer(cinfo);
			/* note we assume that fill_input_buffer will never
			 * return FALSE, so suspension need not be handled.
			 */
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

static void term_source (j_decompress_ptr cinfo)
{
	/* We don't actually need to do anything */
	return;
}

static void jpeg_SDL_RW_src (j_decompress_ptr cinfo, SDL_RWops *ctx)
{
  my_source_mgr *src;

  if (cinfo->src == NULL) {	/* first time for this JPEG object? */
    /*
    printf("alloc source mgr struct...\n");
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  sizeof(my_source_mgr));
	*/
    cinfo->src = (struct jpeg_source_mgr *)&source_mgr;
    src = (my_source_mgr *) cinfo->src;
  }
  SDL_RWseek(ctx, 0, SEEK_SET);

  src = (my_source_mgr *) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->ctx = ctx;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}

struct my_error_mgr {
	struct jpeg_error_mgr errmgr;
	jmp_buf escape;
};

static void my_error_exit(j_common_ptr cinfo)
{
	struct my_error_mgr *err = (struct my_error_mgr *)cinfo->err;
	longjmp(err->escape, 1);
}

static void output_no_message(j_common_ptr cinfo)
{
	/* do nothing */
}

/* Load a JPEG type image from an SDL datasource */
bool CJpegLoad::Load(const char *szPath, int buf)
{
	struct jpeg_decompress_struct cinfo;
	JSAMPROW rowptr[1];
	SDL_Surface *volatile surface = NULL;
	struct my_error_mgr jerr;
	int width, height, lineno;
	int i;

	SDL_RWops *src = SDL_RWFromFile(szPath, "rb");
	if (!src) return false;

	/* Create a decompression structure and load the JPEG header */
	cinfo.err = jpeg_std_error(&jerr.errmgr);
	jerr.errmgr.error_exit = my_error_exit;
	jerr.errmgr.output_message = output_no_message;
	if(setjmp(jerr.escape)) {
		/* If we get here, libjpeg found an error */
		jpeg_destroy_decompress(&cinfo);
    	SDL_RWclose(src);
		printf("JPEG loading error...\n");
		SDL_FreeSurface(surface);
		return false;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_SDL_RW_src(&cinfo, src);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_calc_output_dimensions(&cinfo);

	width = cinfo.output_width;
	height = cinfo.output_height;

	/* Set 24-bit RGB output */
	cinfo.out_color_space = JCS_RGB;
	cinfo.quantize_colors = FALSE;
    cinfo.scale_num   = 1;
    for(i=0;i<4;i++){
		if((cinfo.output_width >> i) > MAX_WIDTH) continue;
        if(((cinfo.output_width >> i) * (cinfo.output_height >> i)) < PHOTO_FIT_SIZE) {
			if(i) --i; // 표현 가능한 해상도보다 1단계 크게 찍음(그 뒤로 리사이즈 처리함)
            break;
        }
    }
	cinfo.scale_denom = 1 << i;
   	cinfo.dct_method = JDCT_FASTEST;
   	cinfo.do_fancy_upsampling = FALSE;
   	cinfo.mem->max_memory_to_use = 1024 * 1024;
/*
	if((cinfo.output_width * cinfo.output_height) > (640*480)) {
    } else {
        cinfo.dct_method = JDCT_IFAST;
    	cinfo.do_fancy_upsampling = FALSE;
    }
*/
	jpeg_calc_output_dimensions(&cinfo);

	if(cinfo.output_width >= MAX_WIDTH) {
		printf("Unsupported resolution(%d x %d)...\n", cinfo.output_width, cinfo.output_height);
    	jpeg_destroy_decompress(&cinfo);
    	SDL_RWclose(src);
    	return false;
	}

	/* Allocate an output surface to hold the image */
	surface = SDL_AllocSurface(SDL_SWSURFACE, cinfo.output_width,
		cinfo.output_height, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0);

	if ( surface == NULL ) {
		printf("Out of memory(try Alloc Surface %d x %d)...\n", cinfo.output_width, cinfo.output_height);
    	jpeg_destroy_decompress(&cinfo);
    	SDL_RWclose(src);
    	return false;
	}

	/* Decompress the image */
	jpeg_start_decompress(&cinfo);

	rowptr[0] = (JSAMPROW)line;
	while (cinfo.output_scanline < cinfo.output_height) {
		lineno = cinfo.output_scanline;
		jpeg_read_scanlines(&cinfo, rowptr, (JDIMENSION) 1);
		memcpy((Uint8 *)surface->pixels + lineno * surface->pitch, line, surface->pitch);
	}
	jpeg_finish_decompress(&cinfo);
	//printf("decompress finished...\n");

	/* Clean up and return */
	jpeg_destroy_decompress(&cinfo);
	SDL_RWclose(src);

	theApp.menuPhoto.ScreenFit(surface, width, height, buf);
	SDL_FreeSurface(surface);

	return true;
}

/* Load a JPEG type image from an SDL datasource */
bool CJpegLoad::FullLoad(const char *szPath)
{
	struct jpeg_decompress_struct cinfo;
	JSAMPROW rowptr[1];
	struct my_error_mgr jerr;
	int i;
	//char *line;
	int lineno;

	SDL_RWops *src = SDL_RWFromFile(szPath, "rb");
	if (!src) return false;

	/* Create a decompression structure and load the JPEG header */
	cinfo.err = jpeg_std_error(&jerr.errmgr);
	jerr.errmgr.error_exit = my_error_exit;
	jerr.errmgr.output_message = output_no_message;
	if(setjmp(jerr.escape)) {
		/* If we get here, libjpeg found an error */
		jpeg_destroy_decompress(&cinfo);
    	SDL_RWclose(src);
		printf("JPEG loading error...\n");
		return false;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_SDL_RW_src(&cinfo, src);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_calc_output_dimensions(&cinfo);

	/* Set 24-bit RGB output */
	cinfo.out_color_space = JCS_RGB;
	cinfo.quantize_colors = FALSE;
    cinfo.scale_num   = 1;
    for(i=0;i<4;i++){
		if((cinfo.output_width >> i) > MAX_WIDTH) continue;
        if(((cinfo.output_width >> i) * (cinfo.output_height >> i)) < PHOTO_SCALE_SIZE) {
			if(i) --i; // 표현 가능한 해상도보다 1단계 크게 찍음(그 뒤로 리사이즈 처리함)
            break;
        }
    }
	cinfo.scale_denom = 1 << i;
   	cinfo.dct_method = JDCT_FASTEST;
   	cinfo.do_fancy_upsampling = FALSE;
   	cinfo.mem->max_memory_to_use = 1024 * 1024;
/*
	if((cinfo.output_width * cinfo.output_height) > (640*480)) {
    } else {
        cinfo.dct_method = JDCT_IFAST;
    	cinfo.do_fancy_upsampling = FALSE;
    }
*/
	jpeg_calc_output_dimensions(&cinfo);

	if(cinfo.output_width >= MAX_WIDTH) {
		printf("Unsupported resolution(%d x %d)...\n", cinfo.output_width, cinfo.output_height);
    	jpeg_destroy_decompress(&cinfo);
    	SDL_RWclose(src);
    	return false;
	}

	/* Decompress the image */
	jpeg_start_decompress(&cinfo);

	rowptr[0] = (JSAMPROW)line;
	theApp.menuPhoto.ResizeBegin(cinfo.output_width, cinfo.output_height);
	while (theApp.menuPhoto.DoFullLoading && (cinfo.output_scanline < cinfo.output_height)) {
	    lineno = cinfo.output_scanline;
		jpeg_read_scanlines(&cinfo, rowptr, (JDIMENSION) 1);
		theApp.menuPhoto.Resize24(line, lineno);
	}
	theApp.menuPhoto.ResizeEnd();
	jpeg_finish_decompress(&cinfo);
	//printf("decompress finished...\n");

	/* Clean up and return */
	jpeg_destroy_decompress(&cinfo);
	SDL_RWclose(src);

	return true;
}

/* Load a JPEG type image from an SDL datasource */
bool CJpegLoad::ComicLoad(const char *szPath, int buf)
{
	struct jpeg_decompress_struct cinfo;
	JSAMPROW rowptr[1];
	struct my_error_mgr jerr;
	int i, ret;
	//char *line;
	int lineno;

	SDL_RWops *src = SDL_RWFromFile(szPath, "rb");
	if (!src) return false;

	/* Create a decompression structure and load the JPEG header */
	cinfo.err = jpeg_std_error(&jerr.errmgr);
	jerr.errmgr.error_exit = my_error_exit;
	jerr.errmgr.output_message = output_no_message;
	if(setjmp(jerr.escape)) {
		/* If we get here, libjpeg found an error */
		jpeg_destroy_decompress(&cinfo);
    	SDL_RWclose(src);
		printf("JPEG loading error...\n");
		return false;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_SDL_RW_src(&cinfo, src);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_calc_output_dimensions(&cinfo);

	/* Set 24-bit RGB output */
	cinfo.out_color_space = JCS_RGB;
	cinfo.quantize_colors = FALSE;
    cinfo.scale_num   = 1;
    for(i=0;i<4;i++){
		if((cinfo.output_width >> i) > MAX_WIDTH) continue;
        if(((cinfo.output_width >> i) * (cinfo.output_height >> i)) < PHOTO_COMIC_SIZE) {
			if(i) --i; // 표현 가능한 해상도보다 1단계 크게 찍음(그 뒤로 리사이즈 처리함)
            break;
        }
    }
	cinfo.scale_denom = 1 << i;
   	cinfo.dct_method = JDCT_FASTEST;
   	cinfo.do_fancy_upsampling = FALSE;
   	cinfo.mem->max_memory_to_use = 1024 * 1024;
/*
	if((cinfo.output_width * cinfo.output_height) > (640*480)) {
    } else {
        cinfo.dct_method = JDCT_IFAST;
    	cinfo.do_fancy_upsampling = FALSE;
    }
*/
	jpeg_calc_output_dimensions(&cinfo);

	if((cinfo.output_width > 320) && (cinfo.output_width < MAX_WIDTH) && (cinfo.output_height > 240)
		&& theApp.menuPhoto.ResizeComic(cinfo.output_width, cinfo.output_height, buf)) {
    	/* Decompress the image */
    	jpeg_start_decompress(&cinfo);
		//line = (char *)malloc(cinfo.output_width * 3);
    	rowptr[0] = (JSAMPROW)line;
    	while ( cinfo.output_scanline < cinfo.output_height ) {
            lineno = cinfo.output_scanline;
    		jpeg_read_scanlines(&cinfo, rowptr, (JDIMENSION) 1);
    		theApp.menuPhoto.Resize24(line, lineno);
    	}
    	theApp.menuPhoto.ResizeComicEnd();
		//free(line);
    	jpeg_finish_decompress(&cinfo);
		//printf("decompress finished...\n");
    } else {
		/* If we get here, libjpeg found an error */
		jpeg_destroy_decompress(&cinfo);
    	SDL_RWclose(src);
		printf("Comic Image loading error...\n");
		return false;
    }

	/* Clean up and return */
	jpeg_destroy_decompress(&cinfo);
	SDL_RWclose(src);

	return true;
}
