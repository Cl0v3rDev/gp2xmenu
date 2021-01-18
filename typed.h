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
#ifndef __TYPED_H__
#define __TYPED_H__
//[*]------------------------------------------------------------------------------------------[*]
//[*]------------------------------------------------------------------------------------------[*]
#include "config.h"
//[*]------------------------------------------------------------------------------------------[*]
#ifdef __cplusplus
#include <string>
#include <map>
#endif
#include <SDL/SDL.h>
//[*]------------------------------------------------------------------------------------------[*]
#define XRES		320
#define YRES		240
#define COLORBIT	16
#define REDMASK		0xf800
#define GREENMASK	0x07e0
#define BLUEMASK	0x001f
#define ALPHAMASK	0x0000
//[*]------------------------------------------------------------------------------------------[*]
#define LCD_OFF         0
#define LCD_ON          1
#define FCLK_200		10
#define FCLK_166		11
#define FCLK_133		12
#define FCLK_100		13
#define FCLK_78			14
#define FCLK_64			15
#define FCLK_DEFAULT	16
//[*]------------------------------------------------------------------------------------------[*]
#define ICONCOUNT_MAIN		6
#define ICONCOUNT_SETTING	6
#define ICONCOUNT_GAME	    2
//[*]------------------------------------------------------------------------------------------[*]
typedef struct
{
	unsigned short batt;
	unsigned short remocon;

}MMSP2ADC;
//[*]------------------------------------------------------------------------------------------[*]
enum
{
	VK_UP = 0		,	// 0
	VK_UP_LEFT		,	// 1
	VK_LEFT			,	// 2
	VK_DOWN_LEFT	,	// 3
	VK_DOWN			,	// 4
	VK_DOWN_RIGHT	,	// 5
	VK_RIGHT		,	// 6
	VK_UP_RIGHT		,	// 7
	VK_START		,	// 8
	VK_SELECT		,	// 9
	VK_FL			,	// 10
	VK_FR			,	// 11
	VK_FA			,	// 12
	VK_FB			,	// 13
	VK_FX			,	// 14
	VK_FY			,	// 15
	VK_VOL_UP		,	// 16
	VK_VOL_DOWN		,	// 17
	VK_TAT			,	// 18
	VK_USB_CON 		,	// 19   /*USB가 연결되었는지 체크*/
	JOY_UP = 99
};
//[*]------------------------------------------------------------------------------------------[*]
enum
{
	PEN_DOWN,
	PEN_UP
};
//[*]------------------------------------------------------------------------------------------[*]
// Active View Enum
enum ViewMode
{
	NONE = 0		,
	MAIN_VIEW		,
	GAME_VIEW		,
	MOVIE_VIEW		,
	MUSIC_VIEW		,
	EBOOK_VIEW		,
	PHOTO_VIEW		,
	SETTING_VIEW	,
	UPDATE_VIEW		,

	INFOMATION_VIEW	,
	TESTMODE_VIEW	,
	SYSTEM_VIEW		,
	USB_VIEW		,
	TOUCH_VIEW      ,
	TVOUT_VIEW		,

	GAME_SD_VIEW	,
	GAME_BUILTIN_VIEW,
	MEDIA_PLAY_VIEW	,
	MEDIA_SET_VIEW  ,
	PHOTO_IMAGE_VIEW,
	EBOOK_TEXT_VIEW	,
	EBOOK_MARK_VIEW	,
	EBOOK_SET_VIEW	,
	EBOOK_FONT_VIEW	,

	SKINEXPORT_VIEW	,

	NEXT_TASK_VIEW,

	MAX_VIEW
};
//[*]------------------------------------------------------------------------------------------[*]
enum MainIcon
{
	ICON_GAME = 0,
	ICON_MOVIE,
	ICON_MUSIC,
	ICON_EBOOK,
	ICON_PHOTO,
	ICON_SETTING,
	ICON_FONT,
};
//[*]------------------------------------------------------------------------------------------[*]
enum ValueAdjust
{
	VALUE_UP = -1,
	VALUE_DOWN = -2
};
//[*]------------------------------------------------------------------------------------------[*]
enum SwapEffect
{
	SWAP_NONE,
	SWAP_BLINK,
	SWAP_OVERLAP,
	SWAP_WIPE,
	SWAP_BLIND,
	SWAP_SCROLL
};
//[*]------------------------------------------------------------------------------------------[*]
enum SwapSurfaceType
{
	SWAP_PREV,
	SWAP_NEXT
};
//[*]------------------------------------------------------------------------------------------[*]
enum MountDevice
{
	MOUNT_SD,
	MOUNT_NAND
};
//[*]------------------------------------------------------------------------------------------[*]
enum SelectDevice
{
	DEVICE_SD,
	DEVICE_NAND,
	DEVICE_EXT
};
//[*]------------------------------------------------------------------------------------------[*]
enum RemoconValue
{
	NONE_REMOCON,  //0
	EQ,    //1
	VOLUME_UP,  //2
	VOLUME_DOWN,  //3
	PLAY_PAUSE,  //4
	REW,   //5
	FF,   //6
	NEXT_SKIP, //7
	REMOVE_REMOCON  //8
};
//[*]------------------------------------------------------------------------------------------[*]
// Play Sound Type...
enum SoundType
{
	BEEP = 0,
	MOVE,
	RUN,
	CONNECT,
	ADJUST
};
//[*]------------------------------------------------------------------------------------------[*]
enum MediaInfo
{
	MEDIA_TYPE = 0,
	MEDIA_PLAY_STATUS,
	MEDIA_FILE_NAME,
	MEDIA_PLAY_TIME,
	MEDIA_TOTAL_TIME,
	MEDIA_BIT_RATE,
	MEDIA_SAMPLE_RATE,
	MEDIA_CHANNEL,
	MEDIA_TITLE,
	MEDIA_ALBUM,
	MEDIA_REPEAT_MODE,
	MEDIA_EQUALIZER,
	MEDIA_VOLUME,
	MEDIA_PROGRESS,
};
//[*]------------------------------------------------------------------------------------------[*]
enum MediaStatus
{
	MEDIA_MOVIE,
	MEDIA_MUSIC,
	MEDIA_PLAY,
	MEDIA_STOP,
	MEDIA_PAUSE,
	MEDIA_VOLUME_UP,
	MEDIA_VOLUME_DOWN
};
//[*]------------------------------------------------------------------------------------------[*]
enum SubtitleType
{
	SUBTITLE_LRC,
	SUBTITLE_SMI,
	SUBTITLE_SAMI
};
//[*]------------------------------------------------------------------------------------------[*]
// 포토 뷰어 상태...
enum PhotoViewStatus
{
	OFF_STATUS,
	ON_STATUS
};
//[*]------------------------------------------------------------------------------------------[*]
enum PhotoSetting
{
	PHOTO_BUFFER_TYPE,
	PHOTO_SCALE,
	PHOTO_VIEW_MODE,
	PHOTO_ROTATE,
	PHOTO_SET_MOVE
};
//[*]------------------------------------------------------------------------------------------[*]
enum PhotoInfo
{
	PHOTO_FILE_NAME,
	PHOTO_INFO_STR
};
//[*]------------------------------------------------------------------------------------------[*]
enum PhotoMenu
{
	PHOTO_ZOOM_IN = 0,
	PHOTO_ZOOM_OUT,
	PHOTO_ORIGINAL,
	PHOTO_ROTATE_LEFT,
	PHOTO_ROTATE_RIGHT,
	PREV_FILE,
	NEXT_FILE,
	PHOTO_COMIC_LEFT,
	PHOTO_COMIC_RIGHT,
	OPEN_FILE,
	PHOTO_FIT,
	PHOTO_WALLPAPER,
};
//[*]------------------------------------------------------------------------------------------[*]
enum DirectGoMenu
{
	GOTO_PHOTO = 100,
	GOTO_EBOOK,
	GOTO_EXIT,
	GOTO_MP3,
};
//[*]------------------------------------------------------------------------------------------[*]
// 텍스트뷰어 상태...
enum TextViewStatus
{
	TEXT_OFF_STATUS,
	TEXT_ON_STATUS
};
//[*]------------------------------------------------------------------------------------------[*]
enum ActionReturn
{
	UPDATE_NONE,
	UPDATE_FULL,
	UPDATE_STATUS,
	DO_ACTION
};
//[*]------------------------------------------------------------------------------------------[*]
// 파일 포맷
enum FileFormat
{
	FOLDER_FORMAT	,	// 폴더
	MP3_FORMAT		,	// 음악
	OGG_FORMAT		,
	WAV_FORMAT		,
	AVI_FORMAT		,	// 동영상
	OGM_FORMAT		,
	BMP_FORMAT		,	// 그림
	GIF_FORMAT		,
	PNG_FORMAT		,
	PCX_FORMAT		,
	JPG_FORMAT		,
	TXT_FORMAT		,	// 텍스트 파일
	FILE_FORMAT		,	// 기타 파일
	GPE_FORMAT		,	// GP2X 실행파일
	GPU_FORMAT		,
	TTF_FORMAT      ,
	INI_FORMAT
};
//[*]------------------------------------------------------------------------------------------[*]
enum WaitMode
{
	WAIT_NONE = 0,
	WAIT_ANY_BUTTON,
	WAIT_QUERY,
};
//[*]------------------------------------------------------------------------------------------[*]
#define IOCTL_GP2X_MMSP2ADC_CONTROL    _IOR('v', 0x00 ,unsigned long)
#define IOCTL_GP2X_BATT_CONTROL	_IOR('v', 0x00, unsigned long)
#define BATT_MODE		0
#define REMOCON_MODE		1
#define BATT_REMOCON_MODE	2
#define CALL_REMOCON     	20
#define BATT_INTERRUPT_START  	1
#define MENU_MODE  10
#define MOVIE_MODE 16
//[*]------------------------------------------------------------------------------------------[*]
#define IMG_COMMON_BODY COMMONPATH"body.png"
#ifdef GP2X
#define SD_PATH		ROOT_PATH"/sd"
#define NAND_PATH	ROOT_PATH"/nand"
#define EXT_PATH	ROOT_PATH"/ext"
#define SD_PATH_GAME	SD_PATH"/game"
#define SD_PATH_MOVIE	SD_PATH"/movie"
#define SD_PATH_MUSIC	SD_PATH"/music"
#define SD_PATH_EBOOK	SD_PATH"/ebook"
#define SD_PATH_FONT	SD_PATH"/font"
#define SD_PATH_PHOTO	SD_PATH"/photo"
#define BUILTIN_PATH_GAME	"/usr/games"
#else
#ifdef WIZ
# define SD_PATH	ROOT_PATH"/sd"
#else
# define SD_PATH	ROOT_PATH
#endif
#define NAND_PATH	ROOT_PATH
#define EXT_PATH	ROOT_PATH
#define SD_PATH_GAME	SD_PATH
#define SD_PATH_MOVIE	SD_PATH
#define SD_PATH_MUSIC	SD_PATH
#define SD_PATH_EBOOK	SD_PATH
#define SD_PATH_FONT	SD_PATH
#define SD_PATH_PHOTO	SD_PATH
#define BUILTIN_PATH_GAME	SD_PATH"/game"
#endif
//[*]------------------------------------------------------------------------------------------[*]
#define SDL_SAFE_FREE(x)	if((x)!=NULL) 	{	SDL_FreeSurface((x)); x= NULL;	}
#define SAFE_FREE(x)		if((x)!=NULL)	{	free(x); x=NULL;	}
//[*]------------------------------------------------------------------------------------------[*]
typedef enum
{
	NORMAL_EQ		,
	ROCK_EQ			,
	POP_EQ			,
	JAZZ_EQ			,
	CLASSIC_EQ		,
	DANCE_EQ		,
	HEAVY_EQ		,
	DISCO_EQ		,
	SOFT_EQ			,
	LIVE_3D_ST_EQ	,
	HALL_EQ
} Equalizer;
//[*]------------------------------------------------------------------------------------------[*]
enum DisplayMode
{
	LCD_MODE,
	TV_NTSC_MODE,
	TV_PAL_MODE
};
//[*]------------------------------------------------------------------------------------------[*]
enum CustomEvent
{
	EVENT_UPDATE = 1,
	EVENT_SUBTITLE_CHANGE,
	EVENT_MEDIA_END = (SDL_USEREVENT + 1),
	EVENT_BATTERY_CHANGE
};
//[*]------------------------------------------------------------------------------------------[*]
#define PATH_DEV_CX25874                  "/dev/cx25874"
#define PATH_DEV_CX25874_WITH_DEVFS       "/dev/misc/cx25874"
//[*]------------------------------------------------------------------------------------------[*]
#define MMSP2_FB_RGB_ON             1
#define MMSP2_FB_RGB_OFF            0
//[*]------------------------------------------------------------------------------------------[*]
#define MMSP2_FB0_TV_LCD_CHECK		10
#define MMSP2_FB0_GET_TV_HANDLE		11
#define MMSP2_FB0_SET_TV_HANDLE		12
//[*]------------------------------------------------------------------------------------------[*]
#define MMSP2_FB0_SET_LCD_TIMING	7
#define MMSP2_FB0_GET_LCD_TIMING	8
#define MMSP2_FB0_SET_LCD_CONTRAST	9
//[*]------------------------------------------------------------------------------------------[*]
enum {
	PHOTO_FIT_VIEW,
	PHOTO_SCALE_VIEW,
	PHOTO_COMIC_LEFT_VIEW,
	PHOTO_COMIC_RIGHT_VIEW
};

enum {
	BUF_EMPTY,
	BUF_SCALE,
	BUF_COMIC,
	BUF_COMIC_R,
	BUF_COMIC_NR
};

enum {
    COMIC_ONE_PAGE,
    COMIC_TWO_PAGE
};
//[*]------------------------------------------------------------------------------------------[*]
#define PHOTO_BUFFER_COUNT 3
#define PHOTO_FIT_SIZE (XRES*YRES)
#define PHOTO_SCALE_SIZE (8*1024*1024)
#define PHOTO_COMIC_SIZE (PHOTO_SCALE_SIZE / PHOTO_BUFFER_COUNT)
//[*]------------------------------------------------------------------------------------------[*]
#ifdef __cplusplus
typedef std::map< std::string, std::map<std::string, std::string> > langTable;
#endif
//[*]------------------------------------------------------------------------------------------[*]
#ifndef WIN32
#define mkdir(x) mkdir(x, 755);
#endif
//[*]------------------------------------------------------------------------------------------[*]
#include <sys/ioctl.h>
//[*]------------------------------------------------------------------------------------------[*]
#define FBMMSP2CTRL             0x4619

#ifdef WIN32
typedef unsigned short u_short;
typedef unsigned char u_char;
#endif

typedef struct Ioctl_Msg_Hdr_t
{
    u_short message;
    u_short plane;
    u_short msgleng;
} Msghdr;

typedef struct Ioctl_Dummy_t
{
    u_short message;
    u_short msgleng;
    u_char  msgdata[32];
} Msgdummy, *PMsgdummy;

#define MSG(msg) (((Msghdr *)msg)->message)
#define PLANE(msg) (((Msghdr *)msg)->plane)
#define LEN(msg) (((Msghdr *)msg)->msgleng)

// return msg;
#define MSGOK  1000
#define MSGERR 1001

#define MMSP2_FB_RGB_ON             1
#define MMSP2_FB_RGB_OFF            0
//[*]------------------------------------------------------------------------------------------[*]
#define IOCTL_CX25874_DISPLAY_MODE_SET	_IOW('v', 0x02, unsigned char		)
//[*]------------------------------------------------------------------------------------------[*]
#define CX25874_ID 			0x8A
//[*]------------------------------------------------------------------------------------------[*]
#define DISPLAY_LCD 		0x1
#define DISPLAY_MONITOR 	0x2
#define DISPLAY_TV 			0x3
#define DISPLAY_TV_PAL		0x4

/* fbo ioctl 함수 (ntsc인지 pal 인치 체크 할때 사용) */
#define MMSP2_FB0_GET_TV_MODE		13

/* ioctl call 함수 리턴값 */
#define TV_MODE_NTSC		0
#define TV_MODE_PAL			1

#define IOCTL_CX25874_TV_MODE_POSITION	_IOW('v', 0x0A, unsigned char)
#define TV_POS_LEFT		0
#define TV_POS_RIGHT	1
#define TV_POS_UP		2
#define TV_POS_DOWN		3

//[*]------------------------------------------------------------------------------------------[*]
#define SOUND_MIXER_EQ_HW	32

#define FLAT				0
#define ROCK				1
#define POP					2
#define JAZZ				3
#define	CLASSICAL			4
#define DANCE				5
#define	HEAVY 				6
#define DISCO				7
#define SOFT				8
#define LIVE_3D_ST			9
#define HALL				10

#define VOLUME_LIMIT_OFF	20
#define VOLUME_LIMIT_ON		21
#define SKIP_COMMAND		22
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
