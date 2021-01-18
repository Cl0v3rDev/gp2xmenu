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
#ifndef __MAINMENUAPP_H_
#define __MAINMENUAPP_H_
//[*]------------------------------------------------------------------------------------------[*]
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <string>
#include <map>
//[*]------------------------------------------------------------------------------------------[*]
#include "filelist.h"
#include "view.h"
#include "mainview.h"
#include "updateview.h"
#include "game/gameview.h"
#include "game/gamesdview.h"
#include "game/gamebuiltinview.h"
#include "media/mediaplayer.h"
#include "media/mediaplayview.h"
#include "media/mediasettingview.h"
#include "media/movieview.h"
#include "media/musicview.h"
#include "ebook/ebookview.h"
#include "ebook/ebooktextview.h"
#include "ebook/ebookmarkview.h"
#include "ebook/ebooksettingview.h"
#include "ebook/ebookfontview.h"
#include "ebook/ebookreader.h"
#include "photo/photoview.h"
#include "photo/photoimageview.h"
#include "photo/photoviewer.h"
#include "setting/settingview.h"
#include "setting/infoview.h"
#include "setting/tmodeview.h"
#include "setting/systemview.h"
#include "setting/touchview.h"
#include "setting/tvoutview.h"
#include "setting/usbview.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
typedef struct SKIN_ITEM {
	int pos;
	int size;
#ifndef SKINPACKED
	std::string name;
#endif
} skinItem;
typedef std::map< std::string, std::map<std::string, skinItem> > skinTable;
typedef std::map< std::string, std::map<std::string, int> > colorTable;
//[*]------------------------------------------------------------------------------------------[*]
class CMainMenuApp
{
	protected :
		SDL_Surface 	*m_pScreen;			// SDL Surface
		SDL_Surface     *background;
		SDL_Joystick 	*m_pJoy;
		bool            m_SDLInit;

		bool			m_AudioOpened;
		int				m_AudioVolume;

		bool			swapReverse;
		SDL_Surface		*swapPrev;
		SDL_Surface		*swapNext;

		Uint32			timerDelay;
		Uint32			timerTick;

		skinTable		skinInfo;
		int				skinVersion;
		time_t			skinMTime;
		int				skinSize;

		colorTable		colorSetting;
		langTable		langInfo;

		bool			nandMount;

		int             soundHandle;
		int             soundRate;
		int				soundFormat;
		int				soundChannel;
		int             soundFrag;
		float           soundBPS;
#ifdef SOUND_OSS
		int             soundBufferSize;
#endif
#ifdef SOUND_SDL
		Uint8			soundBuffer[16][0x4000];
		int             soundHead, soundTail;
#endif
		DisplayMode     displayMode;
		int             TVHandle;
		int 			battStatus;			// 배터리 상태
		int             batteryTimer;

		SDL_Surface     *batteryGauge;
		int				adcHandle;

		// Views Instance
		CMainView		viewMain;

		CGameView		viewGame;
		CGameSdView		viewSdGame;
		CGameBuiltinView	viewBuiltinGame;

		CMovieView		viewMovie;
		CMusicView		viewMusic;
		CMediaPlayView	viewMediaPlay;
		CMediaSettingView	viewMediaSet;

		CEBookView		viewEBook;
		CEBookTextView	viewEBookText;
		CEBookMarkView	viewEBookMark;
		CEBookSettingView	viewEBookSet;
		CEBookFontView	viewEBookFont;

		CPhotoView		viewPhoto;
		CPhotoImageView	viewPhotoImage;

		CSettingView	viewSetting;

		CInfomationView	viewInfomation;
		CTestModeView	viewTestMode;
		CSystemView		viewSystem;
		CUSBView		viewUSB;
		CTouchView      viewTouch;
		CTvOutView		viewTvOut;

		CUpdateView		viewUpdate;

	public :
		MediaPlayer		menuMedia;
		EBookReader		menuEBook;
		PhotoViewer		menuPhoto;

	public :
		ViewForm			*pView;					// Active View Pointer
		int				lastClock;
		int				m_SoundDev;
		Uint32			playing;
		Uint8			isPlaying;
		Uint8			*m_Sound[5];
		Uint32			m_SoundLen[5];

		Uint8			cfgSound;
		Uint8			cfgVolumeLimit;

		Uint8			cfgNAND;
		Uint8			cfgNANDOnBoot;
		Uint8			cfgExtFont;

		Uint8			cfgAutoRun;

		Uint8			cfgMenuExt;

		Uint8			cfgUSBHost;

		Uint8			cfgUSBNet;
		Uint32			cfgUSBNetIP;
		Uint32			USBNetApply;

		Uint8			cfgWebServer;
		Uint8			cfgInetServer;
		Uint8			cfgSambaServer;

		Uint8			cfgNFSMount;
		Uint8			cfgSambaMount;

		Uint8			cfgCPUClockAdjust;

		Uint8			cfgSkin;
		std::string		cfgSkinName;
#ifndef SKINPACKED
		Uint8			cfgSkinDither;
#endif

		Uint8			cfgEffect;

		std::string		cfgLanguage;
		std::string		cfgCharset;

		Uint8			cfgLCDTiming;

		std::string		skinAuthor;
		std::string		skinModifier;
		std::vector<std::string> skinDesc;

		bool			firstRun;
		bool			bootFlag;
		bool			updateCancel;
		bool			enableTerminate;
		ViewMode		defaultView;
		PhotoViewStatus photovs;
		TextViewStatus  textvs;
		int             lastTask;

		CMainMenuApp();
		~CMainMenuApp();
		SDL_Surface* SDL_Initialize();		// SDL Init
		void SDL_Exitialize();	// SDL Exit
		void MsgProc();						// Message Loop
		void SetView(ViewMode mode);		// Set Active View
		void ApplyConfig(bool SaveChanged = false);	// Apply Config
		void PlaySound(SoundType sound);	// Play Sound

		void ReadSerial(unsigned char *buf);

		void AddTimer(Uint32 delay);
		void RemoveTimer();

		void SwapReverse(bool value = true);
		SDL_Surface* SwapSurface(int value);
		void SwapScreen();

		SDL_Surface* LoadImage(const char *path, const char *name, const char *title = NULL);
		void FontColor(const char *path, const char *name, int &r, int &g, int &b);

		bool SkinExport();
		void SkinLoad();
		void SkinReload();
		void SkinExtract();
		int ParseColor(const char *str);

		void LangInsert(const char *path, const char *name, const char *value);
		const char* LangString(const char *path, const char *name);

		void Mount(int dev);
		bool isSDMount();

		void GP2XControl(int value);

		bool SoundInit(int rate, int format, int channel);
		void SoundReset();
		int GetSoundSpace();
		float GetSoundDelay();
		int PlaySound(char *stream, int len);
#ifdef SOUND_SDL
		void FillSound(Uint8 *stream, int len);
#endif

		int getBatteryStatus();
		void setBatteryMode(int mode);
		int getRemoconValue();
		int callRemocon();

		int getLCDTiming();
		bool setLCDTiming();
		DisplayMode getDisplayMode();
		bool setDisplayMode(DisplayMode mode);
		void moveTVPosition(int direction);

		void getStatusRect(SDL_Rect *rect);
		void drawStatusIcon(SDL_Surface *surface, SDL_Rect *rect);

		void setBackground(SDL_Surface *surface);
		
		void setUSBConnect(bool);
};
//[*]------------------------------------------------------------------------------------------[*]
#endif
//[*]------------------------------------------------------------------------------------------[*]
