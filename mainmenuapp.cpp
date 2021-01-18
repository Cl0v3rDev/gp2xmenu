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
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#ifdef GP2X
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <dirent.h>
#endif
#include <png.h>
#include <inifile.h>
#include <SDL/SDL_image.h>
#include "info.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "media/mmsp2_if.h"
//[*]------------------------------------------------------------------------------------------[*]
#ifdef GP2X
extern "C" void SDL_SYS_JoystickGp2xSys(SDL_Joystick *joystick,int cmd);
extern "C" unsigned char SDL_SYS_JoystickGp2xDoUsbConnectStart(SDL_Joystick *joystick);
extern "C" unsigned char SDL_SYS_JoystickGp2xDoUsbConnectEnd(SDL_Joystick *joystick);
#endif
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
#ifdef SOUND_SDL
void audio_callback(void *userdata, Uint8 *stream, int len)
{
	CMainMenuApp *app = (CMainMenuApp*) userdata;
	app->FillSound(stream, len);
}
#endif
//[*]------------------------------------------------------------------------------------------[*]
CMainMenuApp::CMainMenuApp()
{
	m_pScreen 	= NULL;
	pView 		= NULL;

	m_pJoy			= NULL;

	m_AudioOpened	= false;
	m_AudioVolume	= -1;

	//m_TimerID		= NULL;
	timerDelay		= 0;

	swapReverse		= false;
	swapPrev		= NULL;
	swapNext		= NULL;

	nandMount		= false;

	enableTerminate = false;
	bootFlag = false;
	firstRun = true;

	updateCancel = true;

	char ip[20], *tok;
	Uint32 addr;
	INI_Open(INI_PATH"/common.ini");

	cfgSound = 0;
	cfgVolumeLimit = INI_ReadBool("sound", "limit", 0);

	cfgNAND = INI_ReadBool("nand", "enable", 1);
	cfgNANDOnBoot = INI_ReadBool("nand", "mountonboot", 0);
	cfgExtFont = INI_ReadBool("nand", "extfont", 0);

	cfgAutoRun = INI_ReadBool("main", "autorun", 1);

	cfgMenuExt = INI_ReadBool("main", "menuext", 0);

	cfgUSBHost = INI_ReadInt("usbhost", "enable", 0);

#ifdef GP2XF100
	cfgUSBNet = (INI_ReadBool("usbnet", "enable", 0) ? 2 : 0);
#else
	cfgUSBNet = 0;
#endif
	strcpy(ip, INI_ReadText("usbnet", "ip", "192.168.0.2"));
	addr = 0xffffffff;
	tok = strtok(ip, ".");
	while(tok != NULL) {
		addr = (addr << 8) | atoi(tok);
		tok = strtok(NULL, ".");
	};
	if(addr == 0xffffffff)
		addr = 0xC0A80002;
	cfgUSBNetIP = addr;

	cfgWebServer = (INI_ReadBool("server", "web", 0) ? 2 : 0);
	cfgInetServer = (INI_ReadBool("server", "inet", 0) ? 2 : 0);
	cfgSambaServer = (INI_ReadBool("server", "samba", 0) ? 2 : 0);

	cfgNFSMount = 0;
	cfgSambaMount = 0;

	cfgCPUClockAdjust = INI_ReadBool("power", "cpuadjust", 1);

	cfgSkin = INI_ReadBool("skin", "use", 0);
	if(cfgSkin)
		cfgSkinName = INI_ReadText("skin", "name", "Default.gps");
#ifndef SKINPACKED
	cfgSkinDither = INI_ReadBool("skin", "dither", 1);
#endif

	cfgEffect = INI_ReadInt("skin", "effect", SWAP_OVERLAP);

	cfgLanguage = INI_ReadText("main", "language", "");
	cfgCharset = INI_ReadText("main", "charset", ICONV_CHARSET);

	cfgLCDTiming = INI_ReadInt("display", "lcdtiming", 8);

	int vol = INI_ReadInt("sound", "volume", 80);
	int eq = INI_ReadInt("sound", "eq", NORMAL_EQ);
	int repeat = INI_ReadInt("sound", "repeat", 0);

    menuMedia.setStatus(MEDIA_VOLUME, vol);
    menuMedia.setStatus(MEDIA_EQUALIZER, eq);
    menuMedia.setStatus(MEDIA_REPEAT_MODE, repeat);

	INI_Close();

	skinInfo.clear();

	skinAuthor = "Gamepark Holdings Co,Ltd.";
	skinModifier = "";
	skinDesc.clear();
	skinDesc.push_back("GP2X Default Skin");

	battStatus = 1;
	adcHandle = -1;
	TVHandle = -1;
	batteryTimer = 0;

	background = LoadImage("common", "user");
	if(background == NULL)
		background = LoadImage("common", "body");
	batteryGauge = LoadImage("common", "battery");
}
//[*]------------------------------------------------------------------------------------------[*]
CMainMenuApp::~CMainMenuApp()
{
	pView->Exitialize();
	SDL_SAFE_FREE(background)
	SDL_SAFE_FREE(batteryGauge);
	skinInfo.clear();
	colorSetting.clear();
}
//[*]------------------------------------------------------------------------------------------[*]
SDL_Surface* CMainMenuApp::SDL_Initialize()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER) < 0)
	{
		printf("Unable to init SDL : %s\n",SDL_GetError());
		exit(1);
	}

#if defined(WIN32) && defined(RETAIL)
	freopen("NUL", "w", stdout);
	freopen("NUL", "w", stderr);
#endif

	// Check joystick
	printf("Check joystick...\n");
	if(SDL_NumJoysticks()>0)
	{
		// Open joystick
		m_pJoy = SDL_JoystickOpen(0);
		if(m_pJoy)
		{
#ifdef DEBUG
			printf("Opened Joystick 0\n");
			printf("Name: %s\n", SDL_JoystickName(0));
			printf("Number of Axes: %d\n", SDL_JoystickNumAxes(m_pJoy));
			printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(m_pJoy));
			printf("Number of Balls: %d\n", SDL_JoystickNumBalls(m_pJoy));
#endif
		}
		else
		{
			printf("Couldn't open Joystick 0\n");
		}
	}

#ifdef DEBUG
	printf("Joystick Count: %d\n", SDL_NumJoysticks());
#endif

	// Check for external joystick
	if(SDL_NumJoysticks()>1)
	{
		int i;
		for (i=1;i<SDL_NumJoysticks();i++)
		{
			// Open joystick
			SDL_Joystick *joy = SDL_JoystickOpen(i);
			if(joy)
			{
#ifdef DEBUG
				printf("Opened Joystick %d\n", i);
				printf("Name: %s\n", SDL_JoystickName(i));
				printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
				printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
				printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
#endif
			}
			else
			{
				printf("Couldn't open Joystick %d\n", i);
			}
		}
	}

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

#if defined(GP2X) || defined(WIZ)
	// 커서 표시 해제
	SDL_ShowCursor(SDL_DISABLE);
#else
	SDL_WM_SetCaption("GP2X Skin Maker", NULL);
	SDL_RWops *rw = SDL_RWFromConstMem(winicon, size_winicon);
	SDL_Surface *icon = IMG_Load_RW(rw, 1);
	if(icon != NULL) {
		SDL_WM_SetIcon(icon, NULL);
		SDL_FreeSurface(icon);
	}
#endif

	soundHandle = -1;
	soundRate = -1;

#ifdef PND
	m_pScreen = SDL_SetVideoMode(XRES, YRES, COLORBIT, SDL_SWSURFACE|SDL_FULLSCREEN);
#else
	m_pScreen = SDL_SetVideoMode(XRES, YRES, COLORBIT, SDL_SWSURFACE);		// SDL Video Mode 설정
#endif
	if(m_pScreen == NULL)
	{
		printf("Unable to set %d*%d*%d video : %s\n", XRES, YRES, COLORBIT, SDL_GetError());
		exit(1);
	}

	setLCDTiming(); // 실행시 LCD 타이밍 설정

	m_SDLInit = true;

	TTF_Init();

	//콜백 메서드 호출(주기적으로 배터리 잔량 체크하는 메서드)
	//int nStatus;
	//nStatus=SDL_SetTimer(60000, TimerCallback);	//60초
	adcHandle = open("/dev/mmsp2adc",O_RDONLY);
	setBatteryMode(MENU_MODE);

	return m_pScreen;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::SDL_Exitialize()
{
	menuMedia.close();
	menuPhoto.Break();

	//e-book 파일이 오픈 모드라면
	if(textvs != TEXT_OFF_STATUS)
		textvs = TEXT_OFF_STATUS;

	//photo 파일이 오픈 모드라면
	if(photovs != OFF_STATUS)
		photovs = OFF_STATUS;

	SoundInit(-1, -1, -1);

	TTF_Quit();

	SDL_SAFE_FREE(swapPrev);
	SDL_SAFE_FREE(swapNext);

	close(adcHandle);
	adcHandle = -1;

	printf("SDL Quit...\n");
	SDL_Quit();

	sync();

	printf("EXIT\n");
}
//[*]------------------------------------------------------------------------------------------[*]
enum {
	MOD_USBD = 0,
	MOD_USBNET,
	MOD_USBFILE,
	MOD_USBHOST,
	MOD_USBHID,
	MOD_USBSTORAGE,
	MOD_TERMINATE=-1
};
//[*]------------------------------------------------------------------------------------------[*]
enum {
	MODI_NET2272 = 0,
	MODI_G_ETHER,
	MODI_G_FILE_STORAGE,
	MODI_USBCORE,
	MODI_MMSP2_USBH,
	MODI_USB_OHCI,
	MODI_USB_OHCI_MMSP2,
	MODI_INPUT,
	MODI_HID,
	MODI_KEYBDEV,
	MODI_MOUSEDEV,
	MODI_EVDEV,
	MODI_SCSI_MOD,
	MODI_SD_MOD,
	MODI_USB_STORAGE,
	MODI_TERMINATE
};
//[*]------------------------------------------------------------------------------------------[*]
static const struct MOD_ITEM {
	char *name;
	int type;
} MODLIST[] = {
	// USB Gadget
	{"net2272", MOD_USBD},
	{"g_ether", MOD_USBNET},
	{"g_file_storage", MOD_USBFILE},
	// USB Host
	{"usbcore", MOD_USBHOST},
	{"mmsp2_usbh", MOD_USBHOST},
	{"usb-ohci", MOD_USBHOST},
	{"usb-ohci-mmsp2", MOD_USBHOST},
	// USB HID
	{"input", MOD_USBHID},
	{"hid", MOD_USBHID},
	{"keybdev", MOD_USBHID},
	{"mousedev", MOD_USBHID},
	{"evdev", MOD_USBHID},
	// USB Storage
	{"scsi_mod", MOD_USBSTORAGE},
	{"sd_mod", MOD_USBSTORAGE},
	{"usb-storage", MOD_USBSTORAGE},
	{NULL, MOD_TERMINATE}
};
//[*]------------------------------------------------------------------------------------------[*]
int mod_group[30];
bool mod_items[30];
//[*]------------------------------------------------------------------------------------------[*]
enum {
	PS_SAMBA = 0,
	PS_WEB,
	PS_INET,
	PS_TERMINATE=-1
};
//[*]------------------------------------------------------------------------------------------[*]
enum {
	PSI_SMBD = 0,
	PSI_NMBD,
	PSI_THTTPD,
	PSI_INETD,
	PSI_TERMINATE
};
//[*]------------------------------------------------------------------------------------------[*]
static const struct PS_ITEM {
	char *name;
	int type;
} PSLIST[] = {
	{"/sbin/smbd", PS_SAMBA},
	{"/sbin/nmbd", PS_SAMBA},
	{"/sbin/thttpd", PS_WEB},
	{"/sbin/inetd", PS_INET},
	{NULL, PS_TERMINATE}
};
//[*]------------------------------------------------------------------------------------------[*]
int ps_group[30];
bool ps_items[30];
//[*]------------------------------------------------------------------------------------------[*]
void ModulesCheck(void)
{
#ifdef GP2X
	FILE *file;
	char mod[100];
	const struct MOD_ITEM *mods;
	int len, i, c;

	memset(mod_group, 0, sizeof(mod_group));
	memset(mod_items, 0, sizeof(mod_items));
	file = fopen("/proc/modules", "r");
	if(!file) {
#ifdef DEBUG
		printf("Can't open /proc/modules\n");
#endif
		return;
	}

	while (!feof(file)) {
		fgets(mod, 100, file);

		mods = MODLIST;
		for(i=0;mods->name != NULL;i++, mods++) {
			if(!mod_items[i] && (strncmp(mod, mods->name, strlen(mods->name)) == 0)) {
				mod_items[i] = true;
				mod_group[mods->type]++;
			}
		}
	}
	fclose(file);
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
void ProcessCheck(void)
{
#ifdef GP2X
	DIR *dir;
	FILE *file;
	struct dirent *entry;
	char path[32];
	char cmd[20];
	const struct PS_ITEM *cmds;
	int len, i, c;

	memset(ps_group, 0, sizeof(ps_group));
	memset(ps_items, 0, sizeof(ps_items));
	dir = opendir("/proc");
	if (!dir) {
#ifdef DEBUG
		printf("Can't open /proc\n");
#endif
		return;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (!isdigit(*entry->d_name))
			continue;
		sprintf(path, "/proc/%s/exe", entry->d_name);
		if(readlink(path, cmd, 20) > 0) {
			cmds = PSLIST;
			for(i=0;cmds->name != NULL;i++, cmds++) {
				if(!ps_items[i] && (strncmp(cmd, cmds->name, strlen(cmds->name)) == 0)) {
					ps_items[i] = true;
					ps_group[cmds->type]++;
				}
			}
		}
	}
	closedir(dir);
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
int FindUSBNet(void)
{
#ifdef GP2X
	FILE *file;
	char dev[100];
	char *find, *cut;
	int ret = 0;

	file = fopen("/proc/net/dev", "r");
	if(!file) {
#ifdef DEBUG
		printf("Can't open /proc/net/dev\n");
#endif
		return 0;
	}

	while (!feof(file)) {
		fgets(dev, 100, file);
		find = dev;
		while(*find == ' ') find++;
		if(strncmp(find, "usb", 3) == 0) {
			find += 3;
			cut = find;
			while(*cut != ':') cut++;
			*cut = '\0';
			ret = atoi(find);
			printf("usb net %d...\n", ret);
			fclose(file);

			return ret;
		}
	}
	fclose(file);

	return ret;
#else
	return 0;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
Uint32 FindIP(const char *dev)
{
#ifdef GP2X
	const int MAX_NIC = 10;
	struct ifconf ifc;
	struct ifreq ifr[MAX_NIC];

	int i, s, ret;
	int nNumIFs;

	ifc.ifc_len = sizeof ifr;
	ifc.ifc_ifcu.ifcu_req = ifr;

	if( (s=socket(AF_INET,SOCK_STREAM,0)) < 0) return 0xFFFFFFFF;

	ret = ioctl(s, SIOCGIFCONF, &ifc);
	close(s);
	if(ret < 0) return 0xFFFFFFFF;

	nNumIFs = ifc.ifc_len / sizeof ( struct ifreq );
	for( i=0; i<nNumIFs; i++ )
	{
		struct in_addr addr;
		if(ifc.ifc_ifcu.ifcu_req[i].ifr_ifru.ifru_addr.sa_family != AF_INET) continue;
		if(strcmp(ifc.ifc_ifcu.ifcu_req[i].ifr_ifrn.ifrn_name, dev) != 0) continue;

		addr = ((struct sockaddr_in *) & ifc.ifc_ifcu.ifcu_req[i].ifr_ifru.ifru_addr)->sin_addr;
		return ntohl(addr.s_addr);
	}
#endif
	return 0xFFFFFFFF;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::ApplyConfig(bool SaveChanged)
{
	if(SaveChanged) {
		char ip[20];

		SkinLoad();

		INI_Open(INI_PATH"/common.ini");

		INI_WriteInt("sound", "level", cfgSound);
		INI_WriteBool("sound", "limit", cfgVolumeLimit);

		INI_WriteBool("nand", "enable", cfgNAND);
		INI_WriteBool("nand", "mountonboot", cfgNANDOnBoot);
		INI_WriteBool("nand", "extfont", cfgExtFont);

		INI_WriteBool("main", "autorun", cfgAutoRun);

		INI_WriteBool("main", "menuext", cfgMenuExt);


		INI_WriteInt("usbhost", "enable", cfgUSBHost);

		INI_WriteBool("usbnet", "enable", (cfgUSBNet == 2 ? 1 : 0) );
		sprintf(ip, "%d.%d.%d.%d", (cfgUSBNetIP >> 24) & 0xFF, (cfgUSBNetIP >> 16) & 0xFF, (cfgUSBNetIP >> 8) & 0xFF, cfgUSBNetIP & 0xFF);
		INI_WriteText("usbnet", "ip", ip);

		INI_WriteBool("server", "web", (cfgWebServer == 2 ? 1 : 0) );
		INI_WriteBool("server", "inet", (cfgInetServer == 2 ? 1 : 0) );
		INI_WriteBool("server", "samba", (cfgSambaServer == 2 ? 1 : 0) );

		INI_WriteBool("power", "cpuadjust", cfgCPUClockAdjust);

		INI_WriteBool("skin", "use", cfgSkin);
		if(cfgSkin)
			INI_WriteText("skin", "name", cfgSkinName.c_str());
		INI_WriteInt("skin", "effect", cfgEffect);

		INI_WriteText("main", "language", cfgLanguage.c_str());
		INI_WriteText("main", "charset", cfgCharset.c_str());

		INI_WriteInt("display", "lcdtiming", cfgLCDTiming);

		INI_Close();

		sync();
	}

#ifdef GP2X
	ModulesCheck();
	if(mod_group[MOD_USBFILE])
		cfgUSBNet = 0;

	if(cfgUSBNet == 2) {
		if(!mod_group[MOD_USBD])
			system("/sbin/insmod net2272");
		if(!mod_group[MOD_USBNET])
			system("/sbin/insmod g_ether");
	} else {
		if(SaveChanged) {
			if(cfgUSBNet) {
				if(!mod_group[MOD_USBD])
					system("/sbin/insmod net2272");
				if(!mod_group[MOD_USBNET])
					system("/sbin/insmod g_ether");
			} else {
				if(mod_group[MOD_USBNET]) {
					char ifdown[30];
					sprintf(ifdown, "/sbin/ifconfig usb%d down", FindUSBNet());
					system(ifdown);
					system("/sbin/rmmod g_ether");
				}
				if(mod_group[MOD_USBD] && !mod_group[MOD_USBFILE])
					system("/sbin/rmmod net2272");
			}
		} else {
			cfgUSBNet = mod_group[MOD_USBNET];
		}
	}

	ModulesCheck();
	if(!mod_group[MOD_USBD] || !mod_group[MOD_USBNET]) {
		if(mod_group[MOD_USBD] && !mod_group[MOD_USBFILE])
			system("/sbin/rmmod net2272");
		cfgUSBNet = 0;
	}

	if(cfgUSBNet) {
		char dev[10];
		char ifup[50];

		sprintf(dev, "usb%d", FindUSBNet());
		Uint32 ip = FindIP(dev);

		if(ip != cfgUSBNetIP) {
			sprintf(ifup, "/sbin/ifconfig %s %d.%d.%d.%d", dev, (cfgUSBNetIP >> 24) & 0xFF, (cfgUSBNetIP >> 16) & 0xFF, (cfgUSBNetIP >> 8) & 0xFF, cfgUSBNetIP & 0xFF);
			system(ifup);
		}
	}

	ModulesCheck();
	if(cfgUSBHost && (mod_group[MOD_USBHOST] != 4))
	{
		if(!mod_items[MODI_USBCORE]) system("/sbin/insmod usbcore");
		if(!mod_items[MODI_MMSP2_USBH]) system("/sbin/insmod mmsp2_usbh");
		if(!mod_items[MODI_USB_OHCI]) system("/sbin/insmod usb-ohci");
		if(!mod_items[MODI_USB_OHCI_MMSP2]) system("/sbin/insmod usb-ohci-mmsp2");
	}

	ModulesCheck();
	if(cfgUSBHost && (mod_group[MOD_USBHOST] == 4))
	{
		printf("loading usb host drivers...\n");
		if(!mod_items[MODI_SCSI_MOD]) system("/sbin/insmod scsi_mod");
		if(!mod_items[MODI_SD_MOD]) system("/sbin/insmod sd_mod");
		if(!mod_items[MODI_USB_STORAGE]) system("/sbin/insmod usb-storage");

		if(!mod_items[MODI_INPUT]) system("/sbin/insmod input");
		if(!mod_items[MODI_HID]) system("/sbin/insmod hid");
		if(!mod_items[MODI_KEYBDEV]) system("/sbin/insmod keybdev");
		if(!mod_items[MODI_MOUSEDEV]) system("/sbin/insmod mousedev");
		if(!mod_items[MODI_EVDEV]) system("/sbin/insmod evdev");
	}
	else
	{
		cfgUSBHost = 0;
	}

	ProcessCheck();

	if(cfgInetServer == 2) {
		if(!ps_group[PS_INET])
			system("/sbin/inetd");
	} else {
		if(SaveChanged) {
			if(cfgInetServer) {
				if(!ps_group[PS_INET])
					system("/sbin/inetd");
			} else {
				if(ps_group[PS_INET])
					system("kill `pidof inetd`");
			}
		} else {
			cfgInetServer = (ps_group[PS_INET] ? 1 : 0);
		}
	}

	if(cfgSambaServer == 2) {
		if(!ps_group[PS_SAMBA])
			system("/sbin/smbd && /sbin/nmbd");
	} else {
		if(SaveChanged) {
			if(cfgSambaServer) {
				if(!ps_group[PS_SAMBA])
					system("/sbin/smbd && /sbin/nmbd");
			} else {
				if(ps_group[PS_SAMBA])
					system("kill `pidof smbd nmbd`");
			}
		} else {
			cfgSambaServer = (ps_group[PS_SAMBA] ? 1 : 0);
		}
	}
#endif

	setLCDTiming();
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::MsgProc()
{
	// Joystick Axis to Button Table
	const int nextpos[2][3][9] =
	{
		// X-Axis 0
		{
			// Left
			{1, 1, 2, 3, 3, 3, 2, 1, 2},
			// Center
			{0, 0, 8, 4, 4, 4, 8, 0, 8},
			// Right
			{7, 7, 6, 5, 5, 5, 6, 7, 6}
		},
		{ // Y-Axis
			// Up
			{0, 1, 1, 1, 0, 7, 7, 7, 0},
			// Center
			{8, 2, 2, 2, 8, 6, 6, 6, 8},
			// Down
			{4, 3, 3, 3, 4, 5, 5, 5, 4}
		}
	};

	int done = 0;
	Uint32 now;
	bool usbLoop = true;
	int remoconTime = 0;

	int joyState = JOY_UP;
	int penState = PEN_UP;
	int penXPos, penYPos;
	int nextJoyTime, nextPenTime;

	Uint8 stickState[32];
	Uint8 lastStick;

	SDL_Event event;

	while(!done)
	{
		usleep(0);
		if(m_SDLInit)
		{
			int i;
			for(i=0;i<32;i++)
				stickState[i] = 8;
			joyState = JOY_UP;
			penState = PEN_UP;
			m_SDLInit = false;
		}

		if(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					done = 1;
					break;

			    case EVENT_MEDIA_END:
					if(menuMedia.getStatus(MEDIA_TYPE) == MEDIA_MUSIC)
					{
						switch(menuMedia.getStatus(MEDIA_REPEAT_MODE))
						{
							case 1 :
								theApp.menuMedia.stop();
								theApp.menuMedia.load();
								theApp.menuMedia.play();
								break;
							case 0 :
							case 2 :
				    		case 3 :
								theApp.menuMedia.next();
								break;
				    	}
					}
					else
					{
						if(pView->getMode() == MEDIA_PLAY_VIEW)
						{
							SDL_BlitSurface(background, NULL, m_pScreen, NULL);
							SDL_Surface *loadingImage = theApp.LoadImage("common", "loading");
							if(loadingImage != NULL)
							{
								SDL_Rect rect;
								rect.x = (XRES - loadingImage->w) >> 1;
								rect.y = (YRES - loadingImage->h) >> 1;
								SDL_BlitSurface(loadingImage, NULL, m_pScreen, &rect);
								SDL_UpdateRect(m_pScreen, rect.x, rect.y, loadingImage->w, loadingImage->h);
								SDL_SAFE_FREE(loadingImage);
							}
						}
						theApp.menuMedia.next();
						if(pView->getMode() != MEDIA_PLAY_VIEW)
						    RGBLayerMenu(true);
					}
					printf("next media...\n");
			    	if(pView->getMode() == MEDIA_PLAY_VIEW)
						pView->OnDraw(m_pScreen);
					break;

				case EVENT_BATTERY_CHANGE:
					printf("battery change...\n");
					pView->OnDraw(m_pScreen);
					break;

				case SDL_USEREVENT:
					pView->OnProc(m_pScreen, &event);
					break;

				case SDL_JOYAXISMOTION:
#if defined(GP2X) || defined(WIZ)
					if((event.jaxis.which != 0) && (event.jaxis.axis < 2))
#else
					if(event.jaxis.axis < 2)
#endif
					{
						int val = 1;

						if(event.jaxis.value < -3000) val = 0;
						else if(event.jaxis.value < 3000) val = 1;
						else val = 2;

						int prev = stickState[event.jaxis.which];
						int next = nextpos[event.jaxis.axis][val][prev];

						if(next != prev) {
							stickState[event.jaxis.which] = next;
							event.jbutton.which = 0;
							event.type = SDL_NOEVENT;
							if(next != 8) {
								event.jbutton.button = next;
								event.jbutton.state = SDL_PRESSED;
								event.type = SDL_JOYBUTTONDOWN;
								pView->OnProc(m_pScreen, &event);
							}
							if(prev != 8) {
								event.jbutton.button = prev;
								event.jbutton.state = SDL_RELEASED;
								event.type = SDL_JOYBUTTONUP;
								pView->OnProc(m_pScreen, &event);
							}
						}
						break;
					}
					break;



				case SDL_JOYBUTTONDOWN:
					if( (enableTerminate
#if defined(GP2X) || defined(WIZ)
						|| pView->getMode() == MAIN_VIEW
#endif
						) && (event.type == SDL_JOYBUTTONDOWN) &&
							(event.jbutton.which == 0) && (event.jbutton.button == VK_START))
					{
						done = 1;
						break;
					}

					//USB 연결
					if(event.jbutton.button == VK_USB_CON)
					{
						printf("USB Connected...\n");
						SetView(USB_VIEW);
						break;
					}

				case SDL_JOYBUTTONUP:
#if defined(GP2X) || defined(WIZ)
					if(event.jbutton.which != 0)
#endif
					{
						event.jbutton.which = 0;
						switch(event.jbutton.button) {
							case 0: event.jbutton.button = VK_FA; break;
							case 1: event.jbutton.button = VK_FY; break;
							case 2: event.jbutton.button = VK_FX; break;
							case 3: event.jbutton.button = VK_FB; break;
							case 4: event.jbutton.button = VK_SELECT; break;
							case 5: event.jbutton.button = VK_START; break;
							case 6: event.jbutton.button = VK_FL; break;
							case 7: event.jbutton.button = VK_FR; break;
							case 8: event.jbutton.button = VK_VOL_DOWN; break;
							case 9: event.jbutton.button = VK_VOL_UP; break;
							case 10: event.jbutton.button = VK_TAT; break;
						}
					}

				case SDL_KEYDOWN:
					if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_c) && (event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))) {
						done = 1;
						break;
					}
					if(enableTerminate && (event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_q)) {
						done = 1;
						break;
					}
					if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_F1))
					{
						printf("USB Connected...\n");
						SetView(USB_VIEW);
						break;
					}

				default :
					pView->OnProc(m_pScreen, &event);
					break;
			}

			switch(event.type)
			{
				case SDL_MOUSEBUTTONDOWN:
					penState = PEN_DOWN;
					nextPenTime = SDL_GetTicks() + 500;
				case SDL_MOUSEMOTION:
					penXPos = event.motion.x;
					penYPos = event.motion.y;
					break;
				case SDL_MOUSEBUTTONUP:
					penState = PEN_UP;
					break;
				case SDL_JOYBUTTONDOWN:
					if(event.jbutton.which != 0) break;
					joyState = event.jbutton.button;
					nextJoyTime = SDL_GetTicks() + 500;
#if 0 //def DEBUG
					switch(event.jbutton.button)
					{
						case VK_UP			:	printf("VK_UP			\n");	break;
						case VK_UP_LEFT		:	printf("VK_UP_LEFT		\n");	break;
						case VK_LEFT		:	printf("VK_LEFT		    \n");	break;
						case VK_DOWN_LEFT	:	printf("VK_DOWN_LEFT	\n");	break;
						case VK_DOWN		:	printf("VK_DOWN		    \n");	break;
						case VK_DOWN_RIGHT	:	printf("VK_DOWN_RIGHT	\n");	break;
						case VK_RIGHT		:	printf("VK_RIGHT		\n");	break;
						case VK_UP_RIGHT	:	printf("VK_UP_RIGHT	    \n");	break;
						case VK_START		:	printf("VK_START		\n");	break;
						case VK_SELECT		:	printf("VK_SELECT		\n");	break;
						case VK_FL			:	printf("VK_FL			\n");	break;
						case VK_FR			:	printf("VK_FR			\n");	break;
						case VK_FA			:	printf("VK_FA			\n");	break;
						case VK_FB			:	printf("VK_FB			\n");	break;
						case VK_FX			:	printf("VK_FX			\n");	break;
						case VK_FY			:	printf("VK_FY			\n");	break;
						case VK_VOL_UP		:	printf("VK_VOL_UP		\n");	break;
						case VK_VOL_DOWN	:	printf("VK_VOL_DOWN	    \n");	break;
						case VK_TAT			:	printf("VK_TAT			\n");	break;
					}
#endif
					break;
				case SDL_JOYBUTTONUP:
					if(event.jbutton.which == 0)
						joyState = JOY_UP;
					break;
			}
		}

		now = SDL_GetTicks();

		if(joyState != JOY_UP && nextJoyTime < now)
		{
			nextJoyTime = SDL_GetTicks() + 150;
			switch(joyState)
			{
				case VK_UP 	  :
				case VK_DOWN  :
				case VK_LEFT  :
				case VK_RIGHT :
					pView->OnJoystickDown(m_pScreen, joyState);
					break;
				case VK_UP_LEFT:
				case VK_DOWN_LEFT:
				case VK_DOWN_RIGHT:
				case VK_UP_RIGHT:
					break;
				case VK_FA:
				case VK_FY:
					if(pView->getMode() == SYSTEM_VIEW)
						pView->OnJoystickDown(m_pScreen, joyState);
					break;
				case VK_FL:
				case VK_FR:
					if(pView->getMode() == EBOOK_TEXT_VIEW)
						pView->OnJoystickDown(m_pScreen, joyState);
					break;
				case VK_VOL_UP:
				case VK_VOL_DOWN:
					pView->OnJoystickDown(m_pScreen, joyState);
				case VK_TAT:
				case VK_FB:
					if(pView->getMode() == MEDIA_PLAY_VIEW)
						pView->OnJoystickDown(m_pScreen, joyState);
					break;
			}
		}

		if(penState == PEN_DOWN && nextPenTime < now)
		{
			nextPenTime = SDL_GetTicks() + 150;
			pView->OnPenDown(m_pScreen, penXPos, penYPos);
		}

		if(timerDelay && (timerTick < now))
		{
			pView->OnTimerProc(m_pScreen);
			timerTick += timerDelay;
			if(timerTick < now)
			{
				timerTick = SDL_GetTicks() + timerDelay;
			}
		}

		if(batteryTimer < now)
		{
			int last = battStatus;
			if(last != getBatteryStatus())
			{
				SDL_Event event;
			    event.type = EVENT_BATTERY_CHANGE;
				SDL_PushEvent(&event);
			}
			batteryTimer = now + 60000;
		}

#ifdef GP2XF200
		if( (menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP) && (remoconTime < now) )
		{
			remoconTime = now + 1000;

			if(callRemocon())
			{
				int remoconValue = getRemoconValue();
				printf("remocon: %d\n", remoconValue);

				switch( remoconValue )
				{
					case EQ:
						printf("EQ MODE\n");
						menuMedia.setStatus(MEDIA_EQUALIZER, -1);
						viewMediaPlay.action(10);
						if(pView->getMode() == MEDIA_PLAY_VIEW)
							viewMediaPlay.OnDraw(m_pScreen);
						break;

					case VOLUME_UP:
						printf("VOLUME_UP MODE\n");
						menuMedia.setStatus(MEDIA_VOLUME, MEDIA_VOLUME_UP);
						if(pView->getMode() == MEDIA_PLAY_VIEW)
							viewMediaPlay.OnDraw(m_pScreen);
						break;

					case VOLUME_DOWN:
						printf("VOLUME_DOWN MODE\n");
						menuMedia.setStatus(MEDIA_VOLUME, MEDIA_VOLUME_DOWN);
						if(pView->getMode() == MEDIA_PLAY_VIEW)
							viewMediaPlay.OnDraw(m_pScreen);
						break;

					case PLAY_PAUSE:
						printf("PLAY & PAUSE MODE\n");
						viewMediaPlay.action(5);
						if(pView->getMode() == MEDIA_PLAY_VIEW)
							viewMediaPlay.OnDraw(m_pScreen);
						break;

					case REW:
						printf("previous Song\n");
						viewMediaPlay.action(2);
						if(pView->getMode() == MEDIA_PLAY_VIEW)
							viewMediaPlay.OnDraw(m_pScreen);
						break;

					case FF:
						printf("next Song\n");
						viewMediaPlay.action(3);
						if(pView->getMode() == MEDIA_PLAY_VIEW)
							viewMediaPlay.OnDraw(m_pScreen);
						break;

					case NEXT_SKIP:
						printf("Turn On/Off LCD\n");
						if(pView->getMode() == MEDIA_PLAY_VIEW)
						{
							viewMediaPlay.OnJoystickDown(m_pScreen, VK_FA);
							viewMediaPlay.OnJoystickDown(m_pScreen, VK_FL);
							viewMediaPlay.OnJoystickUp(m_pScreen, VK_FA);
						}
						break;
				}
			}
		}
#endif
	} //while(done) end
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::SetView(ViewMode mode)
{
#if !defined(PND) && !defined(WIZ)
	if((defaultView != MAIN_VIEW) && (mode == MAIN_VIEW)) {
		swapReverse = false;
		return;
	}
#endif

	if(mode == MAIN_VIEW)
	{
		if(menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
			menuMedia.close();
		lastTask = 0;;
	}

	switch(mode)
	{
		case MAIN_VIEW		:	viewMain.ActiveView(&pView);		break;
		case GAME_VIEW		:	viewGame.ActiveView(&pView);		break;
		case SETTING_VIEW	:	viewSetting.ActiveView(&pView);		break;

		case UPDATE_VIEW	:	viewUpdate.ActiveView(&pView);		break;

		case TESTMODE_VIEW	:	viewTestMode.ActiveView(&pView);	break;
		case USB_VIEW		:	viewUSB.ActiveView(&pView);			break;
		case TOUCH_VIEW		:	viewTouch.ActiveView(&pView);		break;
		case TVOUT_VIEW		:	viewTvOut.ActiveView(&pView);		break;
		case INFOMATION_VIEW:	viewInfomation.ActiveView(&pView);	break;
		case SYSTEM_VIEW	:	viewSystem.ActiveView(&pView);	break;

		// 다른 프로그램 런쳐
		case MOVIE_VIEW		:	viewMovie.ActiveView(&pView); break;
		case MUSIC_VIEW		:	viewMusic.ActiveView(&pView); break;
		case PHOTO_VIEW		:	viewPhoto.ActiveView(&pView); break;
		case EBOOK_VIEW		:	viewEBook.ActiveView(&pView); break;

		case MEDIA_PLAY_VIEW : viewMediaPlay.ActiveView(&pView); break;
		case MEDIA_SET_VIEW	 : viewMediaSet.ActiveView(&pView); break;

		case PHOTO_IMAGE_VIEW: viewPhotoImage.ActiveView(&pView); break;

		case EBOOK_TEXT_VIEW : viewEBookText.ActiveView(&pView); break;
		case EBOOK_MARK_VIEW : viewEBookMark.ActiveView(&pView); break;
		case EBOOK_SET_VIEW  : viewEBookSet.ActiveView(&pView); break;
		case EBOOK_FONT_VIEW : viewEBookFont.ActiveView(&pView); break;

		case GAME_SD_VIEW : viewSdGame.ActiveView(&pView); break;
		case GAME_BUILTIN_VIEW : viewBuiltinGame.ActiveView(&pView); break;

	}

	pView->OnDraw(m_pScreen);
	swapReverse = false;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::PlaySound(SoundType sound)
{
	if(!m_AudioOpened || (sound < 0) || (sound > 4)) return;
	if(m_Sound[sound]) {
#if 0
		if (ioctl(m_SoundDev, SNDCTL_DSP_SYNC) == -1)
			printf("ioctl sync error...\n");
		write(m_SoundDev, m_Sound[sound], m_SoundLen[sound]);
#endif
#if 0
		if(!isPlaying)
			SDL_PauseAudio(0);
#endif
		playing = sound;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::ReadSerial(unsigned char *buf)
{
	int i;
	static unsigned char serial[24];
	static bool unread = true;

	if(unread) {
		FILE *fp = fopen("/var/log/serial", "r");
		if(fp != NULL) {
			fread(serial, 1, 24, fp);
			fclose(fp);
		} else {
			memset(serial, 0, 24);
		}
		unread = false;
	}

	memcpy(buf, serial, 24);
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::AddTimer(Uint32 delay)
{
	timerDelay = delay;
	timerTick = SDL_GetTicks() + timerDelay;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::RemoveTimer()
{
	timerDelay = 0;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::SwapReverse(bool value)
{
	swapReverse = value;
}
//[*]------------------------------------------------------------------------------------------[*]
SDL_Surface* CMainMenuApp::SwapSurface(int value)
{
	SDL_Surface **ret;
	switch(value) {
		case SWAP_PREV:
			ret = &swapPrev;
			break;
		case SWAP_NEXT:
			ret = &swapNext;
			break;
		default:
			return NULL;
	}

	if(*ret == NULL)
		*ret = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, COLORBIT, REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);

	return *ret;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::SwapScreen()
{
#ifdef SWAPEFFECT
	if((swapPrev != NULL) && (swapNext != NULL)) {
		int i;
		SDL_Rect srcrect, dstrect;

		switch(cfgEffect) {
			case SWAP_BLINK:
				SDL_FillRect(m_pScreen, NULL, 0);
				SDL_UpdateRect(m_pScreen, 0, 0, 0, 0);
#ifdef GP2X
				SDL_Delay(30);
#else
				SDL_Delay(70);
#endif
				break;
			case SWAP_SCROLL:
				for (i=1; i < 10; i++) {
					if(swapReverse) {
						dstrect.x = i * 32;
						dstrect.y = 0;
						srcrect.x = 0;
						srcrect.y = 0;
						srcrect.w = XRES - dstrect.x;
						srcrect.h = YRES;
						SDL_BlitSurface(swapPrev, &srcrect, m_pScreen, &dstrect);

						dstrect.x = 0;
						dstrect.y = 0;
						srcrect.x = srcrect.w;
						srcrect.y = 0;
						srcrect.w = i * 32;
						srcrect.h = YRES;
						SDL_BlitSurface(swapNext, &srcrect, m_pScreen, &dstrect);
					} else {
						dstrect.x = 0;
						dstrect.y = 0;
						srcrect.x = i * 32;
						srcrect.y = 0;
						srcrect.w = XRES - srcrect.x;
						srcrect.h = YRES;
						SDL_BlitSurface(swapPrev, &srcrect, m_pScreen, &dstrect);

						dstrect.x = srcrect.w;
						dstrect.y = 0;
						srcrect.x = 0;
						srcrect.y = 0;
						srcrect.w = i * 32;
						srcrect.h = YRES;
						SDL_BlitSurface(swapNext, &srcrect, m_pScreen, &dstrect);
					}

					SDL_UpdateRect(m_pScreen, 0, 0, 0, 0);
#ifdef GP2X
					SDL_Delay(1);
#else
					SDL_Delay(10);
#endif
				}
				break;
			case SWAP_OVERLAP:
				for (i=1; i < 5; i++) {
					SDL_BlitSurface(swapPrev, NULL, m_pScreen, NULL);
					SDL_SetAlpha(swapNext, SDL_SRCALPHA, 50 * i);
					SDL_BlitSurface(swapNext, NULL, m_pScreen, NULL);

					SDL_UpdateRect(m_pScreen, 0, 0, 0, 0);
#ifdef GP2X
					SDL_Delay(1);
#else
					SDL_Delay(20);
#endif
				}

				SDL_FillRect(m_pScreen, NULL, 0);
				SDL_SetAlpha(swapNext, SDL_SRCALPHA, 255);
				break;
			case SWAP_WIPE:
				SDL_BlitSurface(swapPrev, NULL, m_pScreen, NULL);
				for (i=1; i < 10; i++) {
					if(swapReverse) {
						dstrect.x = XRES - i * 32;
						dstrect.y = 0;
						srcrect.x = dstrect.x;
						srcrect.y = 0;
						srcrect.w = i * 32;
						srcrect.h = YRES;
						SDL_BlitSurface(swapNext, &srcrect, m_pScreen, &dstrect);
					} else {
						dstrect.x = 0;
						dstrect.y = 0;
						srcrect.x = 0;
						srcrect.y = 0;
						srcrect.w = i * 32;
						srcrect.h = YRES;
						SDL_BlitSurface(swapNext, &srcrect, m_pScreen, &dstrect);
					}

					SDL_UpdateRect(m_pScreen, 0, 0, 0, 0);
#ifdef GP2X
					SDL_Delay(1);
#else
					SDL_Delay(10);
#endif
				}
				break;
			case SWAP_BLIND:
				SDL_BlitSurface(swapPrev, NULL, m_pScreen, NULL);
				for (i=1; i < 10; i++) {
					int j;
					srcrect.y = dstrect.y = 0;
					srcrect.w = (XRES / 160) * i;
					srcrect.h = YRES;
					for(j=0; j < 16; j++) {
						if(swapReverse) {
							dstrect.x = (XRES / 16) * (j + 1) - srcrect.w;
							srcrect.x = dstrect.x;
							SDL_BlitSurface(swapNext, &srcrect, m_pScreen, &dstrect);
						} else {
							dstrect.x = (XRES / 16) * j;
							srcrect.x = dstrect.x;
							SDL_BlitSurface(swapNext, &srcrect, m_pScreen, &dstrect);
						}
					}

					SDL_UpdateRect(m_pScreen, 0, 0, 0, 0);
#ifdef GP2X
					SDL_Delay(1);
#else
					SDL_Delay(10);
#endif
				}
				break;
		}

		SDL_BlitSurface(swapNext, NULL, m_pScreen, NULL);
		SDL_UpdateRect(m_pScreen, 0, 0, 0, 0);
	}

#endif // SWAPEFFECT
	swapReverse = false;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::FontColor(const char *path, const char *name, int &r, int &g, int &b)
{
	int color = colorSetting[path][name];
	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = color & 0xFF;
}
//[*]------------------------------------------------------------------------------------------[*]
SDL_Surface* CMainMenuApp::LoadImage(const char *path, const char *name, const char *title)
{
	char imgname[256];
	SDL_Surface *img = NULL;

	if(path[0] == '/')
	{
		img = IMG_Load(path);
	}
	else if(strcmp(name, "filelist") == 0)
	{
		img = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, COLORBIT, REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);
		SDL_BlitSurface(background, NULL, img, NULL);
		SDL_Surface *bar = LoadImage("common", "bar");
		if(bar != NULL)
		{
			SDL_Rect rect = {38, 30, 0, 0};
			SDL_BlitSurface(bar, NULL, img, &rect);
			SDL_SAFE_FREE(bar);
		}
	}
	else if((strcmp(name, "body") == 0) && (strcmp(path, "common") != 0))
	{
		img = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, COLORBIT, REDMASK, GREENMASK, BLUEMASK, ALPHAMASK);
		SDL_BlitSurface(background, NULL, img, NULL);

		SDL_Surface *layer = NULL;
		SDL_Rect rect = {0, 0, 0, 0};

		if(strcmp(path, "main") == 0)
		{
			layer = LoadImage(path, "bar");
			rect.x = 67;
			rect.y = 202;
		}
		else if(strcmp(path, "setting") == 0)
		{
			layer = LoadImage(path, "layer");
			rect.x = 157;
			rect.y = 114;
		}
		else
			layer = LoadImage(path, "layer");

		if(layer != NULL)
		{
			SDL_BlitSurface(layer, NULL, img, &rect);
			SDL_SAFE_FREE(layer);
		}
	}

	if((img == NULL) && (cfgLanguage.size() > 0))
	{
		const char *postfix = name + strlen(name) - cfgLanguage.size();
		if((strlen(name) <= cfgLanguage.size()) || (strcmp(cfgLanguage.c_str(), postfix) != 0)) {
			char newname[256];
			sprintf(newname, "%s%s", name, cfgLanguage.c_str());
			img = LoadImage(path, newname, title);
		}
	}

	if((img == NULL) & cfgSkin)
	{
		// not implement yet...
		if(skinInfo[path][name].pos > 0) {
			sprintf(imgname, SKIN_PATH"/%s", cfgSkinName.c_str());
			SDL_RWops *rw = SDL_RWFromFile(imgname, "rb");
			if(rw != NULL) {
				SDL_RWseek(rw, skinInfo[path][name].pos, SEEK_SET);
#ifdef GP2X
				int size = skinInfo[path][name].size;
				char *buf = new char[size];

				if( (buf != NULL) && (SDL_RWread(rw, buf, size, 1) < 0) ) {
					cfgSkin = false;
				} else {
					SDL_RWops *imgrw = SDL_RWFromMem(buf, size);
					img = IMG_Load_RW(imgrw, 1);
				}

				delete buf;

				SDL_RWclose(rw);
#else
				img = IMG_Load_RW(rw, 1);
#endif
				//if(img != NULL) return img;
#ifdef SKINPACKED
			} else {
				cfgSkin = false;
#endif
			}
		}

#ifndef SKINPACKED
		if(img == NULL)
		{
			const char *exts[] = {"gif", "png", "jpg", "jpeg", "bmp", "pcx", NULL};
			const char **ext = exts;
			while(*ext != NULL) {
				sprintf(imgname, SKIN_PATH"/%s/img%s/%s.%s", cfgSkinName.c_str(), path, name, *ext);
				img = IMG_Load(imgname);
				if(img != NULL) break;
				ext++;
			}
		}
#endif // SKINPACKED
	}

	// load common body for user skin...
	if( (img == NULL) && (title != NULL) && (strlen(title) > 0) && cfgSkin && (skinInfo["common"]["body"].pos > 0) )
	{
		img = LoadImage("common", "body");
	}

	// load default skin...
	if(img == NULL)
	{
		sprintf(imgname, IMAGE_PATH"%s/%s.png", path, name);

		img = IMG_Load(imgname);
	}

	// load common body for default skin...
	/*
	if( (img == NULL) && (title != NULL) && (strlen(title) > 0) ){
		img = LoadImage("common", "body");
		if(img != NULL) {
		   	unsigned short *dest = NULL;		// 유니코드용 저장 버퍼
			dest = OnUTF8ToUnicode(title, 0);
			DrawTitle(img, 308, 10, dest, 0xFF, 0xFF, 0xFF);
			free(dest);
		}
	}
	*/

	// blit title image...
	if( (img != NULL) && (title != NULL) && (strlen(title) > 0) )
	{
		SDL_Surface *title = NULL;
		if(strcmp(name, "player") == 0)
			title = LoadImage(path, "playertitle");
		else
			title = LoadImage(path, "title");
		if(title == NULL)
			title = LoadImage("common", "title");
		if(title != NULL)
		{
			SDL_BlitSurface(title, NULL, img, NULL);
			SDL_FreeSurface(title);
		}
	}

#if !defined(SKINPACKED)
	#define DECIMAL_TYPE int
	#define DECIMAL_BITS (sizeof(DECIMAL_TYPE)*8-sizeof(char)*8-1)
	static const char mask565[3] = {0xF8,0xFC,0xF8};

	if( (img != NULL) && (title != NULL) )
	{
		if( cfgSkinDither && (img->format->BytesPerPixel == 1) )
		{
	        SDL_Surface *body = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000);
	        SDL_FillRect(body, NULL, 0);
	        SDL_BlitSurface(img, NULL, body, NULL);
	        SDL_FreeSurface(img);
	        img = body;
		}

		if( cfgSkinDither && (img->format->BytesPerPixel > 2) )
		{
			// 16bit dithering part...
			int width = img->w;
			int height = img->h;
			int numBytes = img->format->BytesPerPixel;
			unsigned char *pixels = (unsigned char *)img->pixels;

	        //printf("Dithering with floyd-steinberg at %dbit decimal precision!\n",DECIMAL_BITS);
	        unsigned DECIMAL_TYPE buf[height][width*numBytes];
	        for (int j=0; j < height; j ++) memset(buf[j],0,width*numBytes*sizeof(DECIMAL_TYPE));
	        //printf("Working buffer created, starting dither algorithm\n");
	        for (int j=0, width2 = width*numBytes; j < height; j ++)
	        {
	            for (int i=0; i < width2; i += numBytes)
	            {
					int pos = j*img->pitch + i;
	                if (numBytes == 4 && !pixels[pos+numBytes-1])
	                {
	                    memset(&pixels[pos],3,0);
	                    continue;
	                }
	                for (char k=0; k<3; k++)
	                {
	                    buf[j][i+k] += (unsigned DECIMAL_TYPE)pixels[pos+k]<<DECIMAL_BITS;
	                    if (buf[j][i+k] > (unsigned DECIMAL_TYPE)255<<DECIMAL_BITS)
	                        buf[j][i+k] = (unsigned DECIMAL_TYPE)255<<DECIMAL_BITS;
	                }
	                // This pixel get it's final value
	                for (int k = 0; k < 3; k ++)
	                    pixels[pos+k] = (unsigned char)(buf[j][i+k]>>DECIMAL_BITS) & mask565[k];

	                // Now for the error to spread around
	                unsigned DECIMAL_TYPE error[3] = {
	                    buf[j][i+0] - (((unsigned DECIMAL_TYPE)pixels[pos+0])<<DECIMAL_BITS),
	                    buf[j][i+1] - (((unsigned DECIMAL_TYPE)pixels[pos+1])<<DECIMAL_BITS),
	                    buf[j][i+2] - (((unsigned DECIMAL_TYPE)pixels[pos+2])<<DECIMAL_BITS)};

	                // Floyd-Steinberg dither
	                //   * 7
	                // 3 5 1
	                if (i + numBytes < width2 )
	                {
	                    for (char k=0; k<3; k++)
	                        buf[j][i+numBytes+k] += (error[k] * 7) >> 4;
	                    if (j < (height-1))
	                        for (char k=0; k<3; k++)
	                            buf[j+1][i+numBytes+k] += (error[k] * 1) >> 4;
	                }
	                if (j < (height-1))
	                {
	                    for (char k=0; k<3; k++)
	                        buf[j+1][i+k] += (error[k] * 5) >> 4;
	                    if (i)
	                        for (char k=0; k<3; k++)
	                            buf[j+1][i-numBytes+k] += (error[k] * 3) >> 4;
	                }
	            }
			}
		}

        SDL_Surface *body = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000);
        if(body != NULL) {
	        SDL_FillRect(body, NULL, 0);
	        SDL_BlitSurface(img, NULL, body, NULL);
	        SDL_FreeSurface(img);

	        return body;
		}
	}
#endif // SKINPACKED

	return img;
}
//[*]------------------------------------------------------------------------------------------[*]
bool CMainMenuApp::SkinExport()
{
#ifndef SKINPACKED
	std::string name;
	struct stat statbuf;

	SDL_Surface *img = LoadImage("common", "skinexport");
	if(img != NULL) {
		SDL_Rect dstrect;
		dstrect.x = (XRES - img->w) >> 1;
		dstrect.y = (YRES - img->h) >> 1;
		SDL_BlitSurface(img, NULL, m_pScreen, &dstrect);
		SDL_UpdateRect(m_pScreen, 0, 0, 0, 0);
		SDL_FreeSurface(img);
	}

	if(!cfgSkin) return false;

	name = SKIN_PATH"/" + cfgSkinName;
	if( (lstat(name.c_str(), &statbuf) == 0) && (!S_ISDIR(statbuf.st_mode)) )
		return false;

	name += ".gps";
	if( (lstat(name.c_str(), &statbuf) == 0) && (S_ISDIR(statbuf.st_mode)) )
		return false;

	FILE *fp = fopen(name.c_str(), "wb");
	if(!fp) return false;

	int header = SKINHEADER;
	int version = 1;
	int count = 0;
	int datapos = 0;

	fwrite(&header, 4, 1, fp);
	fwrite(&version, 4, 1, fp);
	fwrite(&count, 4, 1, fp);
	fwrite(&datapos, 4, 1, fp);

	int item = 0;
	while(skinList[item].path) {
		char skinname[256], basename[256];
		const char *exts[] = {"gif", "png", "jpg", "jpeg", "bmp", "pcx", "ini", NULL};
		const char **ext = exts;
		SDL_Surface *skin, *base;
		bool save, ini;
		skinInfo[skinList[item].path][skinList[item].name].pos = 0;
		while(*ext != NULL) {
			skin = NULL;
			ini = false;
			if(strcmp(*ext, "ini") != 0) {
				sprintf(skinname, SKIN_PATH"/%s/img%s/%s.%s", cfgSkinName.c_str(), skinList[item].path, skinList[item].name, *ext);
				skin = IMG_Load(skinname);
			} else if(strcmp(skinList[item].path, "config") == 0) {
				sprintf(skinname, SKIN_PATH"/%s/%s.%s", cfgSkinName.c_str(), skinList[item].name, *ext);
				if( (lstat(skinname, &statbuf) == 0) && (S_ISREG(statbuf.st_mode)) ) ini = true;
			}

			if( (skin != NULL) || ini) {
				save = true;

				if(skin != NULL) {
					sprintf(basename, IMAGE_PATH"%s/%s.png", skinList[item].path, skinList[item].name);
					base = IMG_Load(basename);
					save = true;

					if( (base != NULL) && (base->w == skin->w) && (base->h == skin->h) && (base->pitch == skin->pitch) )
					{
						int i;
						unsigned char *bp, *sp;
						bp = (unsigned char *)base->pixels;
						sp = (unsigned char *)skin->pixels;
						save = false;
						for (i=(base->pitch * base->h) - 1; i >= 0 ; i--) {
							if(bp[i] != sp[i]) {
								save = true;
								break;
							}
						}
					}

					SDL_FreeSurface(base);
					SDL_FreeSurface(skin);
					skin = NULL;
				}

				if(save) {
					count++;
					fwrite(skinList[item].path, strlen(skinList[item].path), 1, fp);
					fwrite(&datapos, 4 - (strlen(skinList[item].path) & 3), 1, fp);
					fwrite(skinList[item].name, strlen(skinList[item].name), 1, fp);
					fwrite(&datapos, 4 - (strlen(skinList[item].name) & 3), 1, fp);
					skinInfo[skinList[item].path][skinList[item].name].name = skinname;
					skinInfo[skinList[item].path][skinList[item].name].pos = ftell(fp);
					fwrite(&datapos, 4, 1, fp);
					fwrite(&datapos, 4, 1, fp);
				}
				break;
			}
			ext++;
		}
		item++;
	}

	datapos = ftell(fp);

	int skinsize = SKINBUFFERSIZE;
	char *skin = new char[skinsize];
	item = 0;
	while(skinList[item].path) {
		if(skinInfo[skinList[item].path][skinList[item].name].pos > 12) {
			bool savefile = true;

			while ( (strcmp(skinList[item].name, "body") == 0) || (strcmp(skinList[item].name, "player") == 0) || (strcmp(skinList[item].name, "font") == 0) ) {
				png_structp png_ptr;
				png_infop info_ptr;
				png_bytep row_pointers[YRES];
				unsigned char *pixels;
				int y;
				int pos, size;

				SDL_Surface *img = LoadImage(skinList[item].path, skinList[item].name, "");
				if (img == NULL) break;
				if (img->h > YRES) { SDL_FreeSurface(img);break; };

				pixels = (unsigned char*)img->pixels;
				for (y=0; y < img->h; y++) {
					row_pointers[y] = pixels;
					pixels += img->pitch;
				}

				/* initialize stuff */
				png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if (!png_ptr) break;

				info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr) break;

				pos = ftell(fp);

				png_init_io(png_ptr, fp);

				png_set_IHDR(png_ptr, info_ptr, img->w, img->h,
					     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
					     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

				png_write_info(png_ptr, info_ptr);
				png_write_image(png_ptr, row_pointers);
				png_write_end(png_ptr, NULL);
				png_destroy_write_struct(&png_ptr, &info_ptr);

				SDL_FreeSurface(img);

				size = ftell(fp) - pos;

				fseek(fp, skinInfo[skinList[item].path][skinList[item].name].pos, SEEK_SET);
				fwrite(&pos, 4, 1, fp);
				fwrite(&size, 4, 1, fp);
				fseek(fp, pos + size, SEEK_SET);

				savefile = false;
				break;
			}

			if (savefile) {
				int pos, size;
				FILE *src = fopen(skinInfo[skinList[item].path][skinList[item].name].name.c_str(), "rb");

				if(src) {
					pos = ftell(fp);
					fseek(src, 0, SEEK_END);
					size = ftell(src);
					fseek(src, 0, SEEK_SET);
					fseek(fp, skinInfo[skinList[item].path][skinList[item].name].pos, SEEK_SET);
					fwrite(&pos, 4, 1, fp);
					fwrite(&size, 4, 1, fp);
					fseek(fp, pos, SEEK_SET);

					if(size > skinsize) {
						delete skin;
						skinsize = size;
						skin = new char[skinsize];
					}

					fread(skin, size, 1, src);
					fwrite(skin, size, 1, fp);

					fclose(src);
				}
			}
		}
		item++;
	}
	delete skin;

	fseek(fp, 8, SEEK_SET);
	fwrite(&count, 4, 1, fp);
	fwrite(&datapos, 4, 1, fp);

	fclose(fp);

	return true;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
int CMainMenuApp::ParseColor(const char *str) {
	const struct WEBCOLOR_ITEM *items = webColor;
	char ret[6];
	int len = 0;

	while(items->name)
	{
		if((strncasecmp(str, items->name, strlen(items->name)) == 0) && (strlen(items->name) > len))
		{
			memcpy(ret, items->rgb, 6);
			len = strlen(items->name);
		}
		++items;
	}

	if(len == 0) {
		if(str[0] == '#')
			memcpy(ret, str + 1, 6);
		else
			memcpy(ret, str, 6);
	}

	int i, color = 0;
	for(i=0;i<6;i++) {
		color <<= 4;
		if( (ret[i] >= '0') && (ret[i] <= '9') ) {
			color |= ret[i] - '0';
		} else if( (ret[i] >= 'A') && (ret[i] <= 'F') ) {
			color |= ret[i] - 'A' + 10;
		} else if( (ret[i] >= 'a') && (ret[i] <= 'f') ) {
			color |= ret[i] - 'a' + 10;
		} else {
			return -1;
		}
	}

	return color;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::SkinLoad()
{
	std::string name;
	struct stat statbuf;

	skinAuthor = "Gamepark Holdings";
	skinModifier = "";
	skinDesc.clear();
	skinDesc.push_back("GP2X Default Skin");

	int item = 0;
	while(skinList[item].path) {
		skinInfo[skinList[item].path][skinList[item].name].pos = 0;
		item++;
	}

	item = 0;
	while(colorList[item].path) {
		colorSetting[colorList[item].path][colorList[item].name] = colorList[item].defval;
		item++;
	}

	viewSystem.loadLang();
//	viewCredits.loadLang();

	if(!cfgSkin) return;

	Mount(MOUNT_NAND);

	name = SKIN_PATH"/" + cfgSkinName;
	if( (lstat(name.c_str(), &statbuf) == 0) && (S_ISDIR(statbuf.st_mode)) ) {
#ifdef SKINPACKED
		cfgSkin = false;
#else
		std::string ininame;

		ininame = name + "/skin.ini";
		INI_Open(ininame.c_str());

		skinAuthor = INI_ReadText("credits", "author", "");
		skinModifier = INI_ReadText("credits", "modifier", "");

		int i;
		char item[32];

		int line = INI_ReadInt("credits", "line", 0);
		skinDesc.clear();
		for(i=0; i<line; i++) {
			sprintf(item, "desc%02d", i + 1);
			skinDesc.push_back(INI_ReadText("credits", item, ""));
		}

		for(i=0; colorList[i].path; i++) {
			sprintf(item, "%scolor", colorList[i].name);
			int color = ParseColor(INI_ReadText(colorList[i].path, item, ""));

			if(color >= 0)
				colorSetting[colorList[i].path][colorList[i].name] = color;
		}

		INI_Close();

		ininame = name + "/system.ini";
		INI_Open(ininame.c_str());
		viewSystem.loadLang();
		INI_Close();

		ininame = name + "/credits.ini";
		INI_Open(ininame.c_str());
//		viewCredits.loadLang();
		INI_Close();

		const char *exts[] = {"gif", "png", "jpg", "jpeg", "bmp", "pcx", NULL};
		const char **ext = exts;
		char imgname[256];
		while(*ext != NULL) {
			sprintf(imgname, SKIN_PATH"/%s/imgcommon/body.%s", cfgSkinName.c_str(), *ext);
			SDL_Surface *img = IMG_Load(imgname);
			if(img != NULL) {
				skinInfo["common"]["body"].pos = 1;
				SDL_FreeSurface(img);
				break;
			}
			ext++;
		}
#endif
		return;
	}

	skinMTime = statbuf.st_mtime;
	skinSize = statbuf.st_size;

	FILE *fp = fopen(name.c_str(), "rb");
	if(!fp) {
		cfgSkin = false;
		return;
	}

	int header, count, datapos;

	fread(&header, 4, 1, fp);
	if(header != SKINHEADER) {
		fclose(fp);
		cfgSkin = false;
		return;
	}

	fread(&skinVersion, 4, 1, fp);
	if(skinVersion != 1) {
		fclose(fp);
		cfgSkin = false;
		return;
	}

	fread(&count, 4, 1, fp);
	fread(&datapos, 4, 1, fp);

	char *table = new char[datapos - 16];
	char *ptr = table;

	fread(table, datapos - 16, 1, fp);

	while(count) {
		char *path = ptr;ptr += (strlen(path) + 4) & ~3;
		if(((int)ptr - (int)table + 16) > datapos) break;
		char *name = ptr;ptr += (strlen(name) + 4) & ~3;
		if(((int)ptr - (int)table + 16) > datapos) break;
		skinInfo[path][name].pos = *(int *)ptr;ptr += 4;
		if(((int)ptr - (int)table + 16) > datapos) break;
		skinInfo[path][name].size = *(int *)ptr;ptr += 4;
		if(((int)ptr - (int)table + 16) > datapos) break;
		count--;
	}

	if(count != 0) cfgSkin = false;

	delete table;

	fclose(fp);

	if(cfgSkin) {
		char imgname[256];
		char *ini = NULL;
		int size;

		if(skinInfo["config"]["skin"].pos > 0) {
			sprintf(imgname, SKIN_PATH"/%s", cfgSkinName.c_str());
			SDL_RWops *rw = SDL_RWFromFile(imgname, "rb");
			if(rw != NULL) {
				size = skinInfo["config"]["skin"].size;
				ini = new char[size];

				SDL_RWseek(rw, skinInfo["config"]["skin"].pos, SEEK_SET);
				if(SDL_RWread(rw, ini, size, 1) < 0) {
					cfgSkin = false;
					delete ini;
					ini = NULL;
				}
				SDL_RWclose(rw);
#ifdef SKINPACKED
			} else {
				cfgSkin = false;
#endif
			}
		}

		if(ini != NULL) {
			INI_Open_Mem(ini, size);
			skinAuthor = INI_ReadText("credits", "author", "");
			skinModifier = INI_ReadText("credits", "modifier", "");

			int i;
			char item[32];

			int line = INI_ReadInt("credits", "line", 0);
			skinDesc.clear();
			for(i=0; i<line; i++) {
				sprintf(item, "desc%02d", i + 1);
				skinDesc.push_back(INI_ReadText("credits", item, ""));
			}

			for(i=0; colorList[i].path; i++) {
				sprintf(item, "%scolor", colorList[i].name);
				int color = ParseColor(INI_ReadText(colorList[i].path, item, ""));

				if(color >= 0)
					colorSetting[colorList[i].path][colorList[i].name] = color;
			}

			INI_Close();

			delete ini;
			ini = NULL;
		} else if (cfgSkin) {
			skinAuthor = "";
			skinDesc.clear();
			skinDesc.push_back(cfgSkinName);
		}

		if(skinInfo["config"]["system"].pos > 0) {
			sprintf(imgname, SKIN_PATH"/%s", cfgSkinName.c_str());
			SDL_RWops *rw = SDL_RWFromFile(imgname, "rb");
			if(rw != NULL) {
				size = skinInfo["config"]["system"].size;
				ini = new char[size];

				SDL_RWseek(rw, skinInfo["config"]["system"].pos, SEEK_SET);
				if(SDL_RWread(rw, ini, size, 1) < 0) {
					cfgSkin = false;
					delete ini;
					ini = NULL;
				}
				SDL_RWclose(rw);
#ifdef SKINPACKED
			} else {
				cfgSkin = false;
#endif
			}
		}

		if(ini != NULL) {
			INI_Open_Mem(ini, size);
			viewSystem.loadLang();
			INI_Close();

			delete ini;
			ini = NULL;
		}

		if(skinInfo["config"]["credits"].pos > 0) {
			sprintf(imgname, SKIN_PATH"/%s", cfgSkinName.c_str());
			SDL_RWops *rw = SDL_RWFromFile(imgname, "rb");
			if(rw != NULL) {
				size = skinInfo["config"]["credits"].size;
				ini = new char[size];

				SDL_RWseek(rw, skinInfo["config"]["credits"].pos, SEEK_SET);
				if(SDL_RWread(rw, ini, size, 1) < 0) {
					cfgSkin = false;
					delete ini;
					ini = NULL;
				}
				SDL_RWclose(rw);
#ifdef SKINPACKED
			} else {
				cfgSkin = false;
#endif
			}
		}

		if(ini != NULL) {
			INI_Open_Mem(ini, size);
//			viewCredits.loadLang();
			INI_Close();

			delete ini;
			ini = NULL;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::SkinReload()
{
	std::string name;
	struct stat statbuf;

	if(!cfgSkin) return;

	name = SKIN_PATH"/" + cfgSkinName;
	if( (lstat(name.c_str(), &statbuf) == 0) && (S_ISDIR(statbuf.st_mode)) ) {
#ifdef SKINPACKED
		cfgSkin = false;
#endif
		return;
	}

	if( (statbuf.st_mtime != skinMTime) || (statbuf.st_size != skinSize) )
		SkinLoad();
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::SkinExtract()
{
#ifdef SKINMAKER
	struct stat statbuf;
	std::string name = SKIN_PATH"/" + cfgSkinName;

	if( (lstat(name.c_str(), &statbuf) == 0) && (S_ISDIR(statbuf.st_mode)) )
		return;

	std::string outpath = name;
	outpath.erase(outpath.find_last_of("."));

	FILE *fp = fopen(name.c_str(), "rb");
	if(!fp) return;

	int header, count, datapos;

	fread(&header, 4, 1, fp);
	if(header != SKINHEADER) {
		fclose(fp);
		return;
	}

	fread(&skinVersion, 4, 1, fp);
	if(skinVersion != 1) {
		fclose(fp);
		return;
	}

	mkdir(outpath.c_str());

	fread(&count, 4, 1, fp);
	fread(&datapos, 4, 1, fp);

	char *table = new char[datapos - 16];
	char *ptr = table;
	char buf[65536];

	fread(table, datapos - 16, 1, fp);

	while(count) {
		char *path = ptr;ptr += (strlen(path) + 4) & ~3;
		if(((int)ptr - (int)table + 16) > datapos) break;
		char *name = ptr;ptr += (strlen(name) + 4) & ~3;
		if(((int)ptr - (int)table + 16) > datapos) break;
		int pos = *(int *)ptr;ptr += 4;
		if(((int)ptr - (int)table + 16) > datapos) break;
		int size = *(int *)ptr;ptr += 4;
		if(((int)ptr - (int)table + 16) > datapos) break;

		std::string out;
		if(strcmp(path, "config") == 0) {
			out = outpath + "/" + name + ".ini";
		} else {
			std::string imgpath = outpath + "/img" + path;
			mkdir(imgpath.c_str());
			out = outpath + "/img" + path + "/" + name + ".png";
		}

		FILE *fpout = fopen(out.c_str(), "wb");
		fseek(fp, pos, SEEK_SET);
		while(size > 0) {
			int bufsize = 65536;
			if(size < bufsize) bufsize = size;
			fread(buf, bufsize, 1, fp);
			fwrite(buf, bufsize, 1, fpout);
			size -= bufsize;
		}
		fclose(fpout);
		count--;
	}

	if(count != 0) cfgSkin = false;

	delete table;

	fclose(fp);
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
bool CMainMenuApp::isSDMount()
{
#ifdef GP2X
	bool mount = false;

	FILE *fp = fopen("/proc/mounts", "r");
	if(!fp) {
		system("mount /proc");
		fp = fopen("/proc/mounts", "r");
	}

	if(fp) {
		char line[100];
		while(!feof(fp)) {
			fgets(line, 100, fp);
			if( (strncmp(line, "/dev/mmcsd/", 11) == 0) || (strncmp(line, "/dev/discs/", 11) == 0) ) {
				mount = true;
				break;
			}
		}
		fclose(fp);
	}

	return mount;
#else
	return true;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::Mount(int dev)
{
#ifdef GP2X
	if(dev == MOUNT_NAND) return;
	//if((dev == MOUNT_NAND) && nandMount) return;
	bool mount = true;

	FILE *fp = fopen("/proc/mounts", "r");
	if(!fp) {
		system("mount /proc");
		fp = fopen("/proc/mounts", "r");
	}

	if(fp) {
		char line[100];
		if(dev == MOUNT_NAND) {
			while(!feof(fp)) {
				fgets(line, 100, fp);
				if(strncmp(line, "/dev/loop/7", 11) == 0) {
					nandMount = true;
					mount = false;
					break;
				}
			}
		} else {
			while(!feof(fp)) {
				fgets(line, 100, fp);
				if( (strncmp(line, "/dev/mmcsd/", 11) == 0) || (strncmp(line, "/dev/discs/", 11) == 0) ) {
					mount = false;
					break;
				}
			}
		}
		fclose(fp);
	}

	if(mount) {
		chdir("/usr/gp2x");
		printf("Mount device...\n");
		//return;
		//if(dev == MOUNT_NAND)
		//	system("/usr/gp2x/nandmount.sh");
		//else
        {
			struct stat statbuf;
			if(lstat("/dev/mmcsd/disc0/part1", &statbuf) == 0)
				system("mount -t vfat -o iocharset=utf8,noatime,async /dev/mmcsd/disc0/part1 /mnt/sd");
			else if(lstat("/dev/discs/disc0/part1", &statbuf) == 0)
				system("mount -t vfat -o iocharset=utf8,noatime,async /dev/discs/disc0/part1 /mnt/sd");
		}
	}
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
bool CMainMenuApp::SoundInit(int rate, int format, int channel)
{
	if((rate == soundRate) && (format == soundFormat) && (channel == soundChannel))
	{
#ifdef SOUND_SDL
		soundHead = -1;
		soundTail = -1;
#endif
	    return true;
	}

	soundRate = rate;
	soundFormat = format;
	soundChannel = channel;

#ifdef SOUND_OSS
	if(soundHandle != -1)
	{
		SoundReset();
		close(soundHandle);
	}
	if(soundRate == -1) {
		soundHandle = -1;
		return true;
	}

	soundHandle = open("/dev/sound/dsp", O_WRONLY);
	if(soundHandle == -1) {
		soundRate = -1;
		return false;
	}

	int stereo = (soundChannel == 2) ? 1 : 0;

	if( (ioctl(soundHandle, SNDCTL_DSP_STEREO, &stereo) == -1) ||
		(ioctl(soundHandle, SNDCTL_DSP_SETFMT, &soundFormat) == -1 ) ||
		(ioctl(soundHandle, SNDCTL_DSP_SPEED, &soundRate) == -1 ) )
	{
		printf("DSP setting failed...\n");
		close(soundHandle);
		soundHandle = -1;
		soundRate = -1;
		return false;
	}

	audio_buf_info bi;
	if (ioctl(soundHandle, SNDCTL_DSP_GETOSPACE, &bi) != -1)
	{
		//printf("fragments: %d, fragsize: %d, bytes: %d\n", bi.fragments, bi.fragsize, bi.bytes);
		soundFrag = bi.fragsize;
		soundBufferSize = bi.bytes;
	}
	else
	{
		printf("Not support SNDCTL_DSP_GETOSPACE ioctl cmd...\n");
		close(soundHandle);
		soundHandle = -1;
		soundRate = -1;
		return false;
	}

	soundBPS = soundChannel;
	soundBPS *= soundRate;
	if((soundFormat != AFMT_U8) && (soundFormat != AFMT_S8))
		soundBPS *= 2;
#endif
#ifdef SOUND_SDL
	if(!SDL_WasInit(SDL_INIT_AUDIO))
		SDL_InitSubSystem(SDL_INIT_AUDIO);

    SDL_CloseAudio();
    if(soundRate == -1) return true;

    int SDLFormat = -1;
    switch(soundFormat){
		case AFMT_U8: SDLFormat = AUDIO_U8; break;
		case AFMT_S8: SDLFormat = AUDIO_S8; break;
		case AFMT_U16_LE: SDLFormat = AUDIO_U16LSB; break;
		case AFMT_S16_LE: SDLFormat = AUDIO_S16LSB; break;
		case AFMT_U16_BE: SDLFormat = AUDIO_U16MSB; break;
		case AFMT_S16_BE: SDLFormat = AUDIO_S16MSB; break;
	}

	if(SDLFormat == -1)
	{
		fprintf(stderr, "Unsupported sound format...\n");
		soundHandle = -1;
		soundRate = -1;
		return false;
	}

	SDL_AudioSpec desired, obtained;
	desired.freq = soundRate;
	desired.format = SDLFormat;
	desired.channels = soundChannel;
	desired.samples = 2048;
	desired.callback = audio_callback;
	desired.userdata = this;

	/* Open the audio device */
	if ( SDL_OpenAudio(&desired, &obtained) < 0 ){
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		soundHandle = -1;
		soundRate = -1;
		return false;
	} else {
		soundHandle = 1;
		SDL_PauseAudio(1);
	}

	soundHead = -1;
	soundTail = -1;

	soundRate = obtained.freq;
	soundChannel = obtained.channels;

	soundFrag = obtained.channels;
	if((obtained.format != AUDIO_U8) && (obtained.format != AUDIO_S8))
	    soundFrag *= 2;
	soundBPS = obtained.freq * soundFrag;
	soundFrag *= obtained.samples;

	if(soundFrag != obtained.size)
	{
		fprintf(stderr, "Sound: wrong fragsize, %d, %d\n", soundFrag, obtained.size);
		SDL_CloseAudio();
		soundHandle = -1;
		soundRate = -1;
		return false;
	}

	if(soundFrag > 0x4000)
	{
		fprintf(stderr, "Sound: too big fragsize, %d\n", soundFrag);
		SDL_CloseAudio();
		soundHandle = -1;
		soundRate = -1;
		return false;
	}
#endif

#ifdef DEBUG
    printf("rate: %d, fmt: 0x%04x, channels: %d, fragsize: %d, bps: %.0f\n", soundRate, soundFormat, soundChannel, soundFrag, soundBPS);
#endif

	return true;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::SoundReset()
{
#ifdef SOUND_OSS
	int fd = open("/dev/sound/mixer", O_RDONLY);
	if (fd >= 0)
	{
		int skipFlag = SKIP_COMMAND;
		ioctl(fd, MIXER_WRITE(SOUND_MIXER_EQ_HW), &skipFlag);
		close(fd);
	}
	ioctl(soundHandle, SNDCTL_DSP_RESET, NULL);
#endif
#ifdef SOUND_SDL
	soundHead = -1;
	soundTail = -1;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
int CMainMenuApp::GetSoundSpace()
{
	int ret = 0;
#ifdef SOUND_OSS
	audio_buf_info bi;
	if (ioctl(soundHandle, SNDCTL_DSP_GETOSPACE, &bi) != -1)
		ret = bi.fragments * bi.fragsize;
	// workaround for GP2X
	if(soundChannel == 1)
		ret >>= 1;
	//printf("Space: %d, %d, %d\n", ret, bi.fragments, bi.bytes);
	if(ret < soundFrag) ret = 0;
#endif
#ifdef SOUND_SDL
	SDL_LockAudio();
	ret = (((soundHead + 16) % 16) == ((soundTail + 16 + 1) % 16)) ? 0 : soundFrag;
	SDL_UnlockAudio();
#endif
	return ret;
}
//[*]------------------------------------------------------------------------------------------[*]
float CMainMenuApp::GetSoundDelay()
{
	int bytes = 0;
#ifdef SOUND_OSS
	audio_buf_info bi;
	if (ioctl(soundHandle, SNDCTL_DSP_GETOSPACE, &bi) != -1)
		bytes = soundBufferSize - bi.bytes;
	// workaround for GP2X
	if(soundChannel == 1)
		bytes >>= 1;
	//printf("Delay: %d, %d, %d\n", bytes, soundBufferSize, bi.bytes);
#endif
#ifdef SOUND_SDL
	SDL_LockAudio();
	if(soundHead == soundTail)
	    bytes = 0;
	else if(soundTail > soundHead)
		bytes = (soundTail - soundHead) * soundFrag;
	else
	    bytes = (soundTail + 16 - soundHead) * soundFrag;
	//printf("%d, %d, %d, %d, %.4f\n", soundTail, soundHead, soundFrag, bytes, bytes / soundBPS);
	SDL_UnlockAudio();
#endif
	return bytes / soundBPS;
}
//[*]------------------------------------------------------------------------------------------[*]
int CMainMenuApp::PlaySound(char *stream, int len)
{
#ifdef SOUND_OSS
	//printf("PlaySound: %d", len);
	len /= soundFrag;
	len = write(soundHandle, stream, len * soundFrag);
	//printf("-> %d\n", len);
#endif
#ifdef SOUND_SDL
	SDL_LockAudio();
	if (((soundHead + 16) % 16) == ((soundTail + 16 + 1) % 16))
	{
		SDL_UnlockAudio();
		return 0;
	}
	soundTail = (soundTail + 16 + 1) % 16;
	if(len > soundFrag) len = soundFrag;
	memcpy(soundBuffer[soundTail], stream, len);
	SDL_UnlockAudio();

	if(SDL_GetAudioStatus() == SDL_AUDIO_PAUSED) {
		SDL_PauseAudio(0);
	}
#endif
	return len;
}
//[*]------------------------------------------------------------------------------------------[*]
#ifdef SOUND_SDL
void CMainMenuApp::FillSound(Uint8 *stream, int len)
{
	//if(len != soundFrag) printf("fragment size error, %d != %d\n", soundFrag, len);
	SDL_LockAudio();
	if(soundHead == soundTail) {
		//printf("buffer empty, audio pause...\n");
		SDL_UnlockAudio();
		SDL_PauseAudio(1);
		return;
	}
	soundHead = (soundHead + 16 + 1) % 16;
	memcpy(stream, soundBuffer[soundHead], len);
	SDL_UnlockAudio();
}
#endif
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::GP2XControl(int value)
{
#ifdef GP2X
	SDL_SYS_JoystickGp2xSys(m_pJoy, value);
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
int CMainMenuApp::getBatteryStatus()
{
#ifdef GP2X
	MMSP2ADC		adcValue;

	if(adcHandle == -1)
	    adcValue.batt = 1;
	else
		read(adcHandle, (char *)&adcValue, sizeof(MMSP2ADC));

	if(adcValue.batt < 4)
	    battStatus = adcValue.batt;
#endif
#ifdef PND
	FILE *f = fopen("/sys/class/power_supply/bq27500-0/capacity", "r");
	if (f != NULL) {
		fscanf(f, "%d", &battStatus);
		fclose(f);
		battStatus /= 32;
		battStatus = 3 - battStatus;
	}
#endif
#ifdef WIZ
	unsigned short magic_val = 0;
	int ret, battdev;

	battdev = open("/dev/pollux_batt", O_RDONLY);
	if (battdev < 0)
		return battStatus;
	ret = read(battdev, &magic_val, sizeof(magic_val));
	close(battdev);
	if (ret != sizeof(magic_val))
		return battStatus;

	battStatus = (int)magic_val - 1;
	if (battStatus < 0)
		battStatus = 0;
#endif

	return battStatus;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::setBatteryMode(int mode)
{
	if(adcHandle < 0) return;
	ioctl(adcHandle,IOCTL_GP2X_MMSP2ADC_CONTROL,BATT_REMOCON_MODE);
	ioctl(adcHandle,IOCTL_GP2X_MMSP2ADC_CONTROL,mode);
}
//[*]------------------------------------------------------------------------------------------[*]
int CMainMenuApp::getRemoconValue()
{
	MMSP2ADC		adcValue;
	
	if(adcHandle == -1)
		adcValue.remocon = 0;
	else
		read(adcHandle, (char *)&adcValue, sizeof(MMSP2ADC));

	// 노이즈 체크 루틴
	if(adcValue.remocon)
	{
		int i = 0, nCnt = 1;
		int tempVal = adcValue.remocon;

		while( (i < 21) && (nCnt < 7) )
		{
			read(adcHandle, (char *)&adcValue, sizeof(MMSP2ADC));
			if(tempVal == adcValue.remocon ) nCnt++;
			else nCnt=0;
			i++;
		}

		if(nCnt > 6)
		    adcValue.remocon = tempVal;
		else
		    adcValue.remocon = 0;
	}

	return adcValue.remocon;
}
//[*]------------------------------------------------------------------------------------------[*]
int CMainMenuApp::callRemocon()
{
	if(adcHandle == -1)
		return 0;
	else
		return ioctl(adcHandle,IOCTL_GP2X_MMSP2ADC_CONTROL,CALL_REMOCON);
}
//[*]------------------------------------------------------------------------------------------[*]
// 타이밍 처리 함수
int CMainMenuApp::getLCDTiming()
{
    int ret;
#ifdef GP2XF100
    Msgdummy dummymsg, *pdmsg;

    // fb0 에서 LCD 타이밍을 체크한다.
    memset((char *) &dummymsg, 0x00, sizeof(Msgdummy));
    pdmsg = &dummymsg;
    MSG(pdmsg) = MMSP2_FB0_GET_LCD_TIMING;
    LEN(pdmsg) = 0;
    ret = ioctl(SDL_videofd, FBMMSP2CTRL, pdmsg);

#ifdef DEBUG
    printf("GET_LCD_TIMING: %d\n", ret);
#endif

    if((ret < 0) || (ret > 13))
        ret = 8;
#endif
    return ret;
}
//[*]------------------------------------------------------------------------------------------[*]
bool CMainMenuApp::setLCDTiming()
{
    int err = 0;

#ifdef GP2XF100
    if(getLCDTiming() != theApp.cfgLCDTiming) {
	    Msgdummy dummymsg, *pdmsg;

	    memset((char *) &dummymsg, 0x00, sizeof(Msgdummy));
	    pdmsg = &dummymsg;
	    MSG(pdmsg) = MMSP2_FB0_SET_LCD_TIMING;
	    LEN(pdmsg) = 4;
		pdmsg->msgdata[0] = theApp.cfgLCDTiming;

	    err = ioctl(SDL_videofd, FBMMSP2CTRL, pdmsg);
#ifdef DEBUG
		printf("SET_LCD_TIMING(%d) : %d\n", theApp.cfgLCDTiming, (err == 0));
#endif
		if(err != 0) return false;
	}
#endif

    return (err == 0);
}
//[*]------------------------------------------------------------------------------------------[*]
DisplayMode CMainMenuApp::getDisplayMode()
{
#ifdef GP2X
	int ret;
    Msgdummy dummymsg, *pdmsg;

    // fb0 에서 TV모드를 체크한다.
    memset((char *) &dummymsg, 0x00, sizeof(Msgdummy));
    pdmsg = &dummymsg;
    MSG(pdmsg) = MMSP2_FB0_TV_LCD_CHECK;
    LEN(pdmsg) = 0;
    ret = ioctl(SDL_videofd, FBMMSP2CTRL, pdmsg);

    if(ret != 1) return LCD_MODE;

    memset((char *) &dummymsg, 0x00, sizeof(Msgdummy));
    pdmsg = &dummymsg;
    MSG(pdmsg) = MMSP2_FB0_GET_TV_MODE;
    LEN(pdmsg) = 0;

    ret = ioctl(SDL_videofd, FBMMSP2CTRL, pdmsg);
    return (ret == TV_MODE_NTSC) ? TV_NTSC_MODE : TV_PAL_MODE;
#else
	return displayMode;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
bool CMainMenuApp::setDisplayMode(DisplayMode mode)
{
#ifdef GP2X
    int err;
    Msgdummy dummymsg, *pdmsg;
    pdmsg = &dummymsg;

	if(mode == LCD_MODE)
	{
		if(TVHandle < 0)
		{
		    memset((char *) &dummymsg, 0x00, sizeof(Msgdummy));
		    MSG(pdmsg) = MMSP2_FB0_GET_TV_HANDLE;
		    LEN(pdmsg) = 0;
		    TVHandle = ioctl(SDL_videofd, FBMMSP2CTRL, pdmsg);
		}

		if(TVHandle < 0)
		    return false;

		ioctl(TVHandle, IOCTL_CX25874_DISPLAY_MODE_SET, (unsigned char)(DISPLAY_LCD&0xf));
		close(TVHandle);
		TVHandle = -1;
	}
	else
	{
		TVHandle = open(PATH_DEV_CX25874, O_RDWR);
		if (TVHandle < 0)
			TVHandle = open(PATH_DEV_CX25874_WITH_DEVFS, O_RDWR);
		if (TVHandle < 0)
            return false;

		if(mode == TV_NTSC_MODE)
		{
			// NTSC
			if(ioctl(TVHandle, IOCTL_CX25874_DISPLAY_MODE_SET, (unsigned char)(DISPLAY_TV&0xf)) < 0)
			{
				close(TVHandle);
				TVHandle = -1;
				return false;
			}
		}
		else
		{
			// PAL
			if(ioctl(TVHandle, IOCTL_CX25874_DISPLAY_MODE_SET, (unsigned char)(DISPLAY_TV_PAL&0xf)) < 0)
			{
				close(TVHandle);
				TVHandle = -1;
				return false;
			}
		}

	}

    memset((char *) &dummymsg, 0x00, sizeof(Msgdummy));
    MSG(pdmsg) = MMSP2_FB0_SET_TV_HANDLE;
    LEN(pdmsg) = 0;
    memcpy(pdmsg->msgdata, &TVHandle, sizeof(int));
	ioctl(SDL_videofd, FBMMSP2CTRL, pdmsg);
#else
	displayMode = mode;
#endif

	return true;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::moveTVPosition(int direction)
{
#ifdef GP2X
	if(TVHandle > 0)
		ioctl(TVHandle, IOCTL_CX25874_TV_MODE_POSITION, direction);
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::getStatusRect(SDL_Rect *rect)
{
	rect->x = 285;
	rect->y = 7;
	rect->w = batteryGauge->w;
	rect->h = batteryGauge->h / 4;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::drawStatusIcon(SDL_Surface *surface, SDL_Rect *rect)
{
	SDL_Rect srcrect;

	srcrect.w = batteryGauge->w;
	srcrect.h = batteryGauge->h / 4;
	srcrect.x = 0;
	srcrect.y = srcrect.h * battStatus;

	SDL_BlitSurface(batteryGauge, &srcrect, surface, rect);
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::setBackground(SDL_Surface *surface)
{
	if(surface == NULL) return;
	SDL_Surface *img = SDL_CreateRGBSurface(SDL_SWSURFACE, XRES, YRES, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000);
	if(img == NULL) return;

	SDL_Surface *applying = theApp.LoadImage("common", "applying");
	if(applying != NULL)
	{
		SDL_Rect rect;
		rect.x = (XRES - applying->w) >> 1;
		rect.y = (YRES - applying->h) >> 1;
		SDL_BlitSurface(applying, NULL, m_pScreen, &rect);
		SDL_UpdateRect(m_pScreen, rect.x, rect.y, rect.w, rect.h);
		SDL_SAFE_FREE(applying);
	}
	
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row_pointers[YRES];
	unsigned char *pixels;
	int y;
	int pos, size;

	SDL_Rect dstrect;
	dstrect.x = XRES - surface->w;
	dstrect.y = YRES - surface->h;
	SDL_FillRect(img, NULL, 0);
	SDL_BlitSurface(surface, NULL, img, &dstrect);

	pixels = (unsigned char*)img->pixels;
	for (y=0; y < img->h; y++) {
		row_pointers[y] = pixels;
		pixels += img->pitch;
	}

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr) info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr)
	{
		char imgname[512];
		FILE *fp = fopen(IMAGE_PATH"common/user.png", "wb");

		png_init_io(png_ptr, fp);
		png_set_IHDR(png_ptr, info_ptr, img->w, img->h,
			     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		png_write_info(png_ptr, info_ptr);
		png_write_image(png_ptr, row_pointers);
		png_write_end(png_ptr, NULL);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
	}

	SDL_FreeSurface(img);

	background = LoadImage("common", "user");
	if(background == NULL)
		background = LoadImage("common", "body");
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainMenuApp::setUSBConnect(bool on)
{
	if(on)
	{
		menuMedia.close();
		menuPhoto.Break();

#ifdef GP2X
		sync();
		if(isSDMount())
			system("umount /mnt/sd");
#endif

#if !defined(GP2X)
		// non GP2X
#elif defined(GP2XF100)
		//theApp.GP2XControl(FCLK_200); // Set clock to 200MHz
		system("/usr/gp2x/usbconnect.sh"); //f-100사용 시 주석 해제
#elif defined(GP2XF200)
		SDL_SYS_JoystickGp2xDoUsbConnectStart(m_pJoy);
#endif
	}
	else
	{
#if !defined(GP2X)
		// non GP2X
#elif defined(GP2XF100)
		system("/usr/gp2x/usbdisconnect.sh");
#elif defined(GP2XF200)
		SDL_SYS_JoystickGp2xDoUsbConnectEnd(m_pJoy);
#endif
	}
}
//[*]------------------------------------------------------------------------------------------[*]
