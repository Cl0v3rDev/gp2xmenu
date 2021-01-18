#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#include "typed.h"
#include "../common/cmn.h"

int g_scaling;

#ifdef PND
#include "../common/host_fb.h"

const struct KEYMAP {
	SDLKey key;
	Uint8 button;
} keymap[] = {
	{SDLK_UP, VK_UP},
	{SDLK_DOWN, VK_DOWN},
	{SDLK_LEFT, VK_LEFT},
	{SDLK_RIGHT, VK_RIGHT},
	{SDLK_LALT, VK_START},
	{SDLK_LCTRL, VK_SELECT},

	{SDLK_HOME, VK_FA},
	{SDLK_END, VK_FB},
	{SDLK_PAGEDOWN, VK_FX},
	{SDLK_PAGEUP, VK_FY},
	{SDLK_RSHIFT, VK_FL},
	{SDLK_RCTRL, VK_FR},

	{SDLK_COMMA, VK_VOL_DOWN},
	{SDLK_PERIOD, VK_VOL_DOWN},
	
	{SDLK_u, VK_USB_CON},
	{SDLK_r, 0xfe}, // REFRESH},
	{SDLK_q, 0xff}, // TERMINATE},
	{SDLK_ESCAPE, 0xff}, // TERMINATE},
	{SDLK_UNKNOWN, 0}
};

static int initialized;
static void *fbdev_mem;

void set_scaling(int id)
{
	switch (id) {
	case 0:
		system("ofbset -fb $FBDEV -pos 240 120 -size 320 240");
		break;
	case 1:
		system("ofbset -fb $FBDEV -pos 80 0 -size 640 480");
		break;
	case 2:
		system("ofbset -fb $FBDEV -pos 0 0 -size 800 480");
		break;
	}
	g_scaling = id;
}

static void finish(void)
{
	printf("pnd finish..\n");
	INI_Open(INI_PATH"/common.ini");
	INI_WriteInt("pandora", "scaling", g_scaling);
	INI_Close();
}

static void init(void)
{
	int opt_scaling;
	int ret, dummy;

	INI_Open(INI_PATH"/common.ini");
	set_scaling(INI_ReadInt("pandora", "scaling", 1));
	INI_Close();

	ret = host_video_init(&dummy, 0);
	if (ret != 0) {
		fprintf(stderr, "can't init fb\n");
		exit(1);
	}
	fbdev_mem = host_video_flip();
	atexit(finish);
	initialized = 1;
}

void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
	if (!initialized)
		init();

	if (SDL_LockSurface(screen) != 0)
		return;

	// XXX: assuming 320x240 for now..
	memcpy(fbdev_mem, screen->pixels, 320*240*2);
	fbdev_mem = host_video_flip();

	SDL_UnlockSurface(screen);
}

#else

void set_scaling(int id)
{
}

static void finish(void)
{
}

#endif

int execlp(const char *file, const char *arg, ...)
{
	// we know gp2xmenu never passes args, and file == arg
	char *new_args[3];
	char ginge_prep[512];
	int ret;

	ret = make_local_path(ginge_prep, sizeof(ginge_prep), "ginge_prep");
	if (ret < 0)
		goto fail;

	new_args[0] = ginge_prep;
	new_args[1] = file;
	new_args[2] = NULL;

	finish();
	system("echo -n 'pwd: ';pwd");
	printf("exec: %s %s\n", ginge_prep, file);
	execvp(new_args[0], new_args);
	perror("execvp");

fail:
	fprintf(stderr, "failed to exec %s, aborting\n", ginge_prep);
	exit(1);
	return -1;
}
