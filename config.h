//[*]------------------------------------------------------------------------------------------[*]
#ifndef __GP2X_COMMON_CONFIG_H__
#define __GP2X_COMMON_CONFIG_H__
//[*]------------------------------------------------------------------------------------------[*]
//[*] Common use header...                                                                               [*]
//[*]------------------------------------------------------------------------------------------[*]
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//[*]------------------------------------------------------------------------------------------[*]
//[*] Package option...                                                                                  [*]
//[*]------------------------------------------------------------------------------------------[*]
#define VERSION "4.1.0"
#define MULTILANG
#define SWAPEFFECT
/*
#if !defined(GP2X) && !defined(WIN32)
	#define GP2X
#endif
#if !defined(GP2XF100) && !defined(GP2XF200)
	#define GP2XF100
#endif
#if !defined(GP2XF100) && !defined(GP2XF200)
	#define GP2XF200
#endif
*/
//[*]------------------------------------------------------------------------------------------[*]
//[*] Setting GP2X mode...                                                                               [*]
//[*]------------------------------------------------------------------------------------------[*]
#ifdef GP2X
	#define RETAIL
	#undef TESTING
	#undef EXPERIMENTAL
	#define SOUND_OSS
	#define ROOT_PATH	"/mnt"
	#define INI_PATH	"/usr/gp2x"
	#define IMAGE_PATH	"/usr/gp2x/img"
	#define SKIN_PATH	"/mnt/nand/skin"
	#define SKINPACKED
#endif
//[*]------------------------------------------------------------------------------------------[*]
//[*] Setting PC mode...                                                                                 [*]
//[*]------------------------------------------------------------------------------------------[*]
#ifndef GP2X
	#undef RETAIL
	#undef TESTING
	#undef EXPERIMENTAL
	#define SOUND_SDL
#if defined(PND)
	#define ROOT_PATH	"/media"
#elif defined(WIZ)
	#define ROOT_PATH	"/mnt"
#else
	#define ROOT_PATH	"/"
#endif
	#define INI_PATH	"./gp2xmenu_data"
	#define IMAGE_PATH	"./gp2xmenu_data/image/img"
	#define SKIN_PATH	"./gp2xmenu_data/skin"
	#undef SKINPACKED
#endif
//[*]------------------------------------------------------------------------------------------[*]
//[*] Setting retail/debug mode...                                                                       [*]
//[*]------------------------------------------------------------------------------------------[*]
#ifdef RETAIL
	#ifdef DEBUG
		#undef DEBUG
	#endif
	#define printf(args...) ;
#else
	#ifndef DEBUG
		#define DEBUG
	#endif
#endif
//[*]------------------------------------------------------------------------------------------[*]
//[*] Setting testing code...                                                                            [*]
//[*]------------------------------------------------------------------------------------------[*]
#ifdef TESTING
	#define EXITONROOT
	#define SAVESTATUS
	#ifdef SKINPACKED
		#undef SKINMAKER
	#else
		#define SKINMAKER
	#endif
#else
	#undef EXITONROOT
	#undef SAVESTATUS
	#undef SKINMAKER
#endif
//[*]------------------------------------------------------------------------------------------[*]
//[*] Setting experimental code...                                                                       [*]
//[*]------------------------------------------------------------------------------------------[*]
#ifdef EXPERIMENTAL
	#define SWAPEFFECT_LIST
#else
	#undef SWAPEFFECT_LIST
#endif
//[*]------------------------------------------------------------------------------------------[*]
//[*] Setting charset/locale/etc...                                                                      [*]
//[*]------------------------------------------------------------------------------------------[*]
#define ICONV_CHARSET "UHC//IGNORE" // MS Extended Wansung
#define COMMONPATH "./imgcommon/"
//[*]------------------------------------------------------------------------------------------[*]
#endif // __GP2X_COMMON_CONFIG_H__
//[*]------------------------------------------------------------------------------------------[*]
