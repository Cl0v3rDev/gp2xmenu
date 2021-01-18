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
#include <inifile.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "systemview.h"
#include "textout.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
#define MAX_VIEW                8
//[*]------------------------------------------------------------------------------------------[*]
enum {
    BTN_ON = 0,
    BTN_OFF,
    BTN_ALWAYS_OFF,
    BTN_ALWAYS_ON,
    BTN_YES,
    BTN_NO
};
//[*]------------------------------------------------------------------------------------------[*]
enum {
	MENU_SPACE=0,
	MENU_ONOFF,
	MENU_ONOFFALWAYS,
	MENU_IPBOX,
	MENU_BUTTON,
	MENU_YESNO,
	MENU_LEVEL,
	MENU_SOUND,
	MENU_EFFECT,
	MENU_SKIN,
	MENU_LANGUAGE,
	MENU_CHARSET,
	MENU_MENUEXT,
	MENU_LCD,
	MENU_SCALING,
	MENU_TERMINATE=-1
};
//[*]------------------------------------------------------------------------------------------[*]
static const struct MENU_ITEM {
	char *group;
	char *item;
	char *label;
	int type;
	int image;
	unsigned int defval;
} menuList[] = {
#ifdef PND
	{"system", "scaling", "Scaling", MENU_SCALING, 0, 1},
#endif
	//{"system", "title", "SYSTEM", MENU_SPACE, 0},
	//{"sd", "sdmakedir", "Create Folder", MENU_YESNO, 0},
	//{"system", "volumelimit", "Volume Limit", MENU_ONOFF, 0},
#if !defined(PND) && !defined(WIZ)
	{"system", "autorun", "Auto Run", MENU_ONOFF, 0, 1},
#endif
#ifdef SWAPEFFECT
	{"system", "effect", "Effect", MENU_EFFECT, 1, 0},
#endif
	//{"system", "skin", "Skin/Theme", MENU_SKIN, 2, 0},
#ifndef SKINPACKED
	//{"system", "skindither", "Skin Dithering", MENU_ONOFF, 1},
#endif
	{"system", "language", "Language", MENU_LANGUAGE, 2, 0},
	{"system", "charset", "Charecter Set", MENU_CHARSET, 3, 0},
#ifdef GP2XF100
	{"system", "lcdtiming", "LCD Timing", MENU_LCD, 5, 8},
#endif

//	{"nand", "title", "NAND", MENU_SPACE, 0},
//	{"nand", "nandsupport", "NAND Support", MENU_ONOFF, 1},
//	{"nand", "nandonboot", "Mount on Boot", MENU_ONOFF, 0},
//	{"nand", "extfont", "External Font", MENU_ONOFF, 0},
//	{"nand", "nandformat", "Format", MENU_YESNO, 0},
//	{"advance", "title", "ADVANCED", MENU_SPACE, 0},
//	{"advance", "menuext", "Menu Extension", MENU_MENUEXT, 0},
//	{"usbhost", "title", "USB HOST", MENU_SPACE, 0},
//	{"usbhost", "usbhost", "USB Host", MENU_ONOFFALWAYS, 0},
#if !defined(PND) && !defined(WIZ)
	{"usbhost", "usbhost", "USB Host", MENU_ONOFF, 4, 0},
#endif
//	{"usbhost", "usbstorage", "Storage", MENU_ONOFF, 0},
//	{"usbhost", "usbhid", "Human Interface", MENU_ONOFF, 0},
//	{"usbhost", "usblan", "Network", MENU_ONOFF, 0},
//	{"usbnet", "title", "USB NETWORK", MENU_SPACE, 0},
//	{"usbnet", "usbnet", "USB Network", MENU_ONOFFALWAYS, 0},
//	{"usbnet", "usbnetip", "IP", MENU_IPBOX, 0xC0A80002},
//	{"server", "title", "SERVER", MENU_SPACE, 0},
//	{"server", "webserver", "Web Server", MENU_ONOFFALWAYS, 0},
//	{"server", "inetserver", "FTP,Telnet Server", MENU_ONOFFALWAYS, 0},
//	{"server", "sambaserver", "Samba Server", MENU_ONOFFALWAYS, 0},
//	{"Network Mount", MENU_SPACE, 0},
//	{"NFS Mount", MENU_ONOFF, 0},
//	{"Samba Mount", MENU_ONOFF, 0},
//	{"power", "title", "POWER SAVING", MENU_SPACE, 0},
//	{"power", "cpuclock", "CPU Clock Adjust", MENU_ONOFF, 1},
//	{"Setting & Record", MENU_SPACE, 0},
//	{"Backup to SD", MENU_YESNO, 0},
//	{"Backup to NAND", MENU_YESNO, 0},
//	{"Restore from SD", MENU_YESNO, 0},
//	{"Restore from NAND", MENU_YESNO, 0},
	{NULL, NULL, NULL, MENU_TERMINATE, 0}
};
//[*]------------------------------------------------------------------------------------------[*]
extern "C" void set_scaling(int id);
extern "C" int g_scaling;
static const char *scaling_list[] = {
	"Off",
	"2x",
	"fullscreen",
};
//[*]------------------------------------------------------------------------------------------[*]
static const char *soundLevelList[] = {
	"Sound Off",
	"Volume Min",
	"Volume Normal",
	"Volume Max",
	NULL
};
//[*]------------------------------------------------------------------------------------------[*]
static const struct EFFECT_ITEM {
	char *item;
	char *label;
	int type;
} effectList[] = {

	{"none", "None", SWAP_NONE},
	{"blink", "Blink", SWAP_BLINK},
	{"overlap", "Overlap", SWAP_OVERLAP},
	{"wipe", "Wipe", SWAP_WIPE},
	{"blind", "Blind", SWAP_BLIND},
	{"scroll", "Scroll", SWAP_SCROLL},
	{NULL, NULL, SWAP_NONE}
};
//[*]------------------------------------------------------------------------------------------[*]
static const struct CHARSET_ITEM {
	char *label;
	char *charset;
} charsetList[] = {
	{"EUC-KR/UHC", "UHC//IGNORE"},
	{"ISO-8859-1", "ISO-8859-1//IGNORE"},
	{"ISO-8859-2", "ISO-8859-2//IGNORE"},
	{"ISO-8859-3", "ISO-8859-3//IGNORE"},
	{"ISO-8859-4", "ISO-8859-4//IGNORE"},
	{"ISO-8859-5", "ISO-8859-5//IGNORE"},
	{"ISO-8859-6", "ISO-8859-5//IGNORE"},
	{"ISO-8859-7", "ISO-8859-7//IGNORE"},
	{"ISO-8859-8", "ISO-8859-5//IGNORE"},
	{"ISO-8859-9", "ISO-8859-9//IGNORE"},
	{"ISO-8859-10", "ISO-8859-10//IGNORE"},
	{"ISO-8859-13", "ISO-8859-13//IGNORE"},
	{"ISO-8859-14", "ISO-8859-14//IGNORE"},
	{"ISO-8859-15", "ISO-8859-15//IGNORE"},
	{"ISO-8859-16", "ISO-8859-16//IGNORE"},
	{"ASCII", "ASCII//IGNORE"},
	{"UTF-8", "UTF-8//IGNORE"},
	{"EUC-JP", "EUC-JP//IGNORE"},
	{"Shift-JIS", "SJIS//IGNORE"},
	{"GB2312", "GB2312//IGNORE"},
	{"BIG5", "BIG5//IGNORE"},
	{NULL, NULL}
};
//[*]------------------------------------------------------------------------------------------[*]
static const struct LANGUAGE_ITEM {
	char *label;
	char *postfix;
} languageList[] = {
	{"English", ""},
	{"Korean", "_kor"},
	{NULL, NULL}
};
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CSystemView::CSystemView()
{
	m_pImgBackGround = NULL;
	pImgButton = NULL;
	pImgArrow = NULL;
	pImgLeftTitle = NULL;


	m_needScroll = false;
	m_drawBegin = 0;
	m_posCursor = 0;
	while(menuList[m_posCursor].type == MENU_SPACE)
		m_posCursor++;

	const MENU_ITEM *items = menuList;
	int i;

	itemValue.clear();
	for(i=0;items->label != NULL;i++, items++)
		itemValue[items->item] = items->defval;

}
//[*]------------------------------------------------------------------------------------------[*]
CSystemView::~CSystemView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// 초기화 함수
void CSystemView::Initialize(bool refresh)
{
	// background Load
	m_pImgBackGround = theApp.LoadImage("system", "body", "SYSTEM");
	pImgButton = theApp.LoadImage("system", "button");
	pImgArrow = theApp.LoadImage("common", "arrow");
	pImgLeftTitle = theApp.LoadImage("system", "left_title");
	//m_pImgUpArrow = theApp.LoadImage("common", "uparrow");
	//m_pImgDownArrow = theApp.LoadImage("common", "downarrow");
/*
	//m_pImgSelectBar = IMG_Load(IMG_SELECT_BAR);
	m_pImgButton[BTN_ON][0] = IMG_Load(IMG_BTN_PATH"on.png");
	m_pImgButton[BTN_ON][1] = IMG_Load(IMG_BTN_PATH"on_on.png");
	m_pImgButton[BTN_OFF][0] = IMG_Load(IMG_BTN_PATH"off.png");
	m_pImgButton[BTN_OFF][1] = IMG_Load(IMG_BTN_PATH"off_on.png");
	m_pImgButton[BTN_ALWAYS_ON][0] = IMG_Load(IMG_BTN_PATH"aw.png");
	m_pImgButton[BTN_ALWAYS_ON][1] = IMG_Load(IMG_BTN_PATH"aw_on.png");
	m_pImgButton[BTN_YES][0] = IMG_Load(IMG_BTN_PATH"yes.png");
	m_pImgButton[BTN_YES][1] = IMG_Load(IMG_BTN_PATH"yes_on.png");
	m_pImgButton[BTN_NO][0] = IMG_Load(IMG_BTN_PATH"no.png");
	m_pImgButton[BTN_NO][1] = IMG_Load(IMG_BTN_PATH"no_on.png");
*/
	if(!refresh) {
		int i;

		itemValue["volumelimit"] = theApp.cfgVolumeLimit;

	//	itemValue[ITEM_NANDSUPPORT] = theApp.cfgNAND;
		itemValue["nandonboot"] = theApp.cfgNANDOnBoot;
	//	itemValue[ITEM_EXTFONT] = theApp.cfgExtFont;

		itemValue["autorun"] = theApp.cfgAutoRun;

		itemValue["menuext"] = theApp.cfgMenuExt;

		itemValue["usbhost"] = theApp.cfgUSBHost;

		itemValue["usbnet"] = theApp.cfgUSBNet;
		itemValue["usbnetip"] = theApp.cfgUSBNetIP;

		itemValue["webserver"] = theApp.cfgWebServer;
		itemValue["inetserver"] = theApp.cfgInetServer;
		itemValue["sambaserver"] = theApp.cfgSambaServer;

	//	itemValue[ITEM_NFSMOUNT] = theApp.cfgNFSMount;
	//	itemValue[ITEM_SAMBAMOUNT] = theApp.cfgSambaMount;

		itemValue["cpuclock"] = theApp.cfgCPUClockAdjust;

		itemValue["lcdtiming"] = theApp.cfgLCDTiming;

#ifdef SWAPEFFECT
		itemValue["effect"] = 0;
		for(i=0; effectList[i].label; i++) {
			if(effectList[i].type == theApp.cfgEffect) {
				itemValue["effect"] = i;
				break;
			}
		}
#endif

		for (i=0; languageList[i].label; i++) {
			if(strcmp(theApp.cfgLanguage.c_str(), languageList[i].postfix) == 0) {
				itemValue["language"] = i;  // 0 (UHC//IGNORE)
				break;
			}
		}

		for (i=0; charsetList[i].label; i++) {
			if(strcmp(theApp.cfgCharset.c_str(), charsetList[i].charset) == 0) {
				itemValue["charset"] = i;  // 0 (UHC//IGNORE)
				break;
			}
		}

#ifndef SKINPACKED
		itemValue["skindither"] = theApp.cfgSkinDither;
#endif
		itemValue["scaling"] = g_scaling;
	}
/*
	// skin list refresh
	{
		int i;
		std::string find = "";
		theApp.Mount(MOUNT_NAND);
		listDir.GetDirInfo(&skinDir, SKIN_PATH, SKIN_MODE);     //#define SKIN_PATH	"/mnt/nand/skin"
		if(refresh) {
			if(itemValue["skin"] > 0)
				find = skinDir.pList[itemValue["skin"] - 1].szName;
		} else {
			find = theApp.cfgSkinName;
		}
		itemValue["skin"] = 0;
		if( (theApp.cfgSkin) && (find.size() > 0) )
		{
			for (i=0;i<skinDir.nCount;i++)
			{
				if(find == skinDir.pList[i].szName)
				{
					itemValue["skin"] = i + 1;
					break;
				}
			}
		}
	}
*/
	theApp.FontColor("system", "text", r0, g0, b0);
	theApp.FontColor("system", "texton", r1, g1, b1);
}
//[*]------------------------------------------------------------------------------------------[*]
// 정리 함수
void CSystemView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(m_pImgBackGround);
	//SDL_SAFE_FREE(m_pImgUpArrow);
	//SDL_SAFE_FREE(m_pImgDownArrow);
	SDL_SAFE_FREE(pImgButton);
	SDL_SAFE_FREE(pImgArrow);
	SDL_SAFE_FREE(pImgLeftTitle);

	//listDir.DeleteDirInfo(&skinDir);
/*
	int i;
	for(i=0;i<5;i++) {
		SDL_SAFE_FREE(m_pImgButton[i][0]);
		SDL_SAFE_FREE(m_pImgButton[i][1]);
    }
*/
}
//[*]------------------------------------------------------------------------------------------[*]
void CSystemView::OnPrintSetting(SDL_Surface *pScreen, int x, int y, int type, unsigned long value, bool active)
{
    SDL_Rect srcrect, dstrect;
    dstrect.y = y - 7;
	switch(type){
		case MENU_ONOFFALWAYS:
			srcrect.w = 90;
			srcrect.h = 25;

			srcrect.x = 0;
			srcrect.y = (value == 2) ? BTN_ALWAYS_ON * 25 : BTN_ALWAYS_OFF * 25;
            dstrect.x = x + 80;
			SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);
		case MENU_ONOFF:
		case MENU_MENUEXT:
			srcrect.w = 48;
			srcrect.h = 25;

			srcrect.x = ( ((type == MENU_ONOFFALWAYS) && (value == 1)) || ((type != MENU_ONOFFALWAYS) && value) ) ? 48 : 0;
			srcrect.y = BTN_ON * 25;   //BTN_ON 은 0
            dstrect.x = x;
			SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);  //ON 이미지 출력

			srcrect.x = (!value) ? 48 : 0;
			srcrect.y = BTN_OFF * 25;
            dstrect.x = x + 48;
			SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);  //OFF 이미지 출력
			break;
		case MENU_YESNO:
			srcrect.w = 45;
			srcrect.h = 25;

			srcrect.x = (value) ? 45 : 0;
			srcrect.y = BTN_YES * 25;
            dstrect.x = x;
			SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);

			srcrect.x = (!value) ? 45 : 0;
			srcrect.y = BTN_NO * 25;
            dstrect.x = x + 40;
			SDL_BlitSurface(pImgButton, &srcrect, pScreen, &dstrect);
			break;
		case MENU_LEVEL:
			if(value == 0)
				PrintMessage(pScreen, x, y, "OFF", r1, g1, b1);
			else
				PrintMessage(pScreen, x, y, "OFF", r0, g0, b0);
			if(value == 1)
				PrintMessage(pScreen, x + 45, y, "1", r1, g1, b1);
			else
				PrintMessage(pScreen, x + 45, y, "1", r0, g0, b0);
			if(value == 2)
				PrintMessage(pScreen, x + 90, y, "2", r1, g1, b1);
			else
				PrintMessage(pScreen, x + 90, y, "2", r0, g0, b0);
			if(value == 3)
				PrintMessage(pScreen, x + 135, y, "3", r1, g1, b1);
			else
				PrintMessage(pScreen, x + 135, y, "3", r0, g0, b0);
			break;
		case MENU_IPBOX:
			{
				char ip[20];
                int i;
                if(m_itemCursor < 0)m_itemCursor = 3;
                if(m_itemCursor > 3)m_itemCursor = 0;
                for(i=0;i<4;i++) {
                    if(i) {
        				PrintMessage(pScreen, x, y, ".", r0, g0, b0);
        				x += 6;
                    }
                    sprintf(ip, "%3d", (value >> (8*(3-i))) & 0xFF);
                    if(active && (m_itemCursor == i)) {
        				PrintMessage(pScreen, x, y, ip, r1, g1, b1);
                    } else {
						PrintMessage(pScreen, x, y, ip, r0, g0, b0);
					}
                    x += 18;
                }
				//sprintf(ip, "%3d.%3d.%3d.%3d", (value >> 24) & 0xFF, (value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
				//PrintMessage(pScreen, x, y, ip, r0, g0, b0);
			}
			break;
		case MENU_SKIN:
		case MENU_EFFECT:
		case MENU_SOUND:
		case MENU_LANGUAGE:
		case MENU_CHARSET:
		case MENU_LCD:
		case MENU_SCALING:
			{
				std::string str;

				switch(type) {
					case MENU_SKIN:
						//if(value > 0) str = skinDir.pList[value-1].szName;
						//else str = lang["skin"]["default"];

						break;
					case MENU_EFFECT:
						str = lang["effect"][effectList[value].item];

						break;
					case MENU_SOUND:
						str = soundLevelList[value];

						break;
					case MENU_LANGUAGE:
						str = languageList[value].label;

						break;
					case MENU_CHARSET:
						str = charsetList[value].label;

						break;
					case MENU_LCD:
						{
							char val[10];
							sprintf(val, "%d", value);
							str = val;

							break;
						}
					case MENU_SCALING:
						str = scaling_list[value];
						break;
				}

				if(active)
					PrintMessage(pScreen, x + 20, y, str.c_str(), r1, g1, b1);
				else
					PrintMessage(pScreen, x + 20, y, str.c_str(), r0, g0, b0);

				srcrect.w = pImgArrow->w >> 1;
				srcrect.h = pImgArrow->h >> 1;
				srcrect.x = 0;
				srcrect.y = srcrect.h;
				dstrect.x = x;
				dstrect.y = y - ((pImgArrow->h - 20) >> 2);
				//SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);     //왼쪽 화살표 출력

				srcrect.x = srcrect.w;
				srcrect.y = srcrect.h;
				dstrect.x = x + 150;
				dstrect.y = y - ((pImgArrow->h - 20) >> 2);
				//SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);     //오른쪽 화살표 출력

				if(pImgArrow != NULL)
				{
					srcrect.w = pImgArrow->w >> 1;
					srcrect.h = pImgArrow->h >> 1;
					srcrect.x = 0;
					srcrect.y = srcrect.h;
					dstrect.x = x - 20;
					dstrect.y = y - ((pImgArrow->h - 20) >> 2);

					SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);

					srcrect.x = srcrect.w;
					srcrect.y = srcrect.h;
					dstrect.x = x + 110;
					dstrect.y = y - ((pImgArrow->h - 20) >> 2);

					SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);
				}
			}
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw 함수
void CSystemView::OnDraw(SDL_Surface *pScreen)
{
	// 화면을 하얀색 출력
	SDL_BlitSurface(m_pImgBackGround, NULL, pScreen, NULL);

	int i, count;
	SDL_Rect srcrect, dstrect;
	bool drawEnd = false;


	//printf("m_drawBegin : %d\n",m_drawBegin); //0
	//printf("m_posCursor : %d\n",m_posCursor); //1


	srcrect.w=(pImgLeftTitle->w) / 2;
	srcrect.h=(pImgLeftTitle->h) / 6;
	for(i=m_drawBegin, count = 0;count < MAX_VIEW;i++, count++)
	{
		if(menuList[i].type == MENU_TERMINATE)
		{
			drawEnd = true;
			break;
		}
		if((i == m_posCursor) && (menuList[i].type == MENU_SPACE))
			m_posCursor++;
		if(menuList[i].label)
		{
			std::string str = lang[menuList[i].group][menuList[i].item];
			//printf("str.c_str() : %s\n",str.c_str());

			srcrect.y=((pImgLeftTitle->h) / 6) * menuList[i].image;
			dstrect.x=50;
			dstrect.y=43+(26*count);

			if(i == m_posCursor)
			{
				//PrintMessage(pScreen, 15, 55 + count*23, str.c_str(), r1, g1, b1); //선택된 글자를 검은색으로 찍음
				srcrect.x=(pImgLeftTitle->w) / 2;

				/*
				if(m_pImgSelectBar) {
					rect.x = 320 - m_pImgSelectBar->w - 40;
					rect.y = 55 + count*20 + 13;

					SDL_BlitSurface(m_pImgSelectBar, NULL, pScreen, &rect);
				}
				*/
			}
			else if(menuList[i].type == MENU_SPACE)
			{
				//PrintMessage(pScreen, 30, 55 + count*23, str.c_str(), r0, g0, b0); //타이틀은 하얀색으로 찍음
				//PrintMessage(pScreen, 31, 55 + count*23, str.c_str(), r0, g0, b0); //타이틀은 하얀색으로 찍음(두껍게)
			}
			else
			{
				//PrintMessage(pScreen, 15, 55 + count*23, str.c_str(), r0, g0, b0); //선택안된 글자를 하얀색으로 찍음
            	srcrect.x=0;
            }

			if(menuList[i].type != MENU_SPACE)
				SDL_BlitSurface(pImgLeftTitle, &srcrect, pScreen, &dstrect);

		}

		OnPrintSetting(pScreen, 145, 49 + count*27, menuList[i].type, itemValue[menuList[i].item], i == m_posCursor);
		if((menuList[i].type == MENU_MENUEXT) && (!itemValue["menuext"])) {
			drawEnd = true;
			break;
		}
	}

	if(!drawEnd && (menuList[m_drawBegin + MAX_VIEW].type == MENU_TERMINATE))
		drawEnd = true;

	// 위쪽 화살표 표시
	if( (m_drawBegin != 0) && (pImgArrow != NULL) )
	{
		srcrect.w = pImgArrow->w >> 1;
		srcrect.h = pImgArrow->h >> 1;
		srcrect.x = srcrect.y = 0;
		dstrect.x = 320 - srcrect.w - 2;
		dstrect.y = 55;

		SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);
	}

	// 아래쪽 화살표 표시
	if( (!drawEnd) && (pImgArrow != NULL) )
	{
		srcrect.w = pImgArrow->w >> 1;
		srcrect.h = pImgArrow->h >> 1;
		srcrect.x = srcrect.w;
		srcrect.y = 0;
		dstrect.x = 320 - srcrect.w - 2;
		dstrect.y = 55+(MAX_VIEW-1)*23;

		SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);
	}

	//배터리 아이콘 로딩 시작
	theApp.getStatusRect(&dstrect);
	theApp.drawStatusIcon(pScreen, &dstrect);

	SDL_UpdateRect(pScreen, 0, 0, 320, 240);		// 화면 업데이트
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CSystemView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_UP 		:	OnCursorUp(pScreen);		break;	// 상 방향키
		case VK_DOWN 	:   OnCursorDown(pScreen);		break;	// 하 방향키
		case VK_LEFT 	:   OnCursorLeft(pScreen);		break;	// 좌 방향키
		case VK_RIGHT 	:   OnCursorRight(pScreen);		break;	// 우 방향키
		case VK_TAT :	// TAT 버튼 	// 설정값을 적용한다.
		case VK_FB :	// B 버튼 	// 설정값을 적용한다.
			theApp.cfgSkin = (itemValue["skin"] != 0);
			if(theApp.cfgSkin) {
				//theApp.cfgSkinName = skinDir.pList[itemValue["skin"] - 1].szName;
			}
#ifndef SKINPACKED
			theApp.cfgSkinDither = itemValue["skindither"];
#endif

#ifdef SWAPEFFECT
			theApp.cfgEffect = effectList[itemValue["effect"]].type;
#endif
			theApp.cfgVolumeLimit = itemValue["volumelimit"];

			if(theApp.cfgLanguage != languageList[itemValue["language"]].postfix) {
				theApp.cfgLanguage = languageList[itemValue["language"]].postfix;
			}

			if(theApp.cfgCharset != charsetList[itemValue["charset"]].charset) {
				//theApp.menuEBook.clear();   //주석 처리 안 하면 이북 로딩 후 셋팅에서 변경 후 다시 이북 들어가면 애러난다
				theApp.cfgCharset = charsetList[itemValue["charset"]].charset;
			}

//			theApp.cfgNAND = itemValue[ITEM_NANDSUPPORT];
			theApp.cfgNANDOnBoot = itemValue["nandonboot"];
//			theApp.cfgExtFont = itemValue[ITEM_EXTFONT];

			theApp.cfgAutoRun = itemValue["autorun"];

			theApp.cfgMenuExt = itemValue["menuext"];

			theApp.cfgUSBHost = itemValue["usbhost"];

			theApp.cfgUSBNet = itemValue["usbnet"];
			theApp.cfgUSBNetIP = itemValue["usbnetip"];

			theApp.cfgWebServer = itemValue["webserver"];
			theApp.cfgInetServer = itemValue["inetserver"];
			theApp.cfgSambaServer = itemValue["sambaserver"];

//			theApp.cfgNFSMount = itemValue[ITEM_NFSMOUNT];
//			theApp.cfgSambaMount = itemValue[ITEM_SAMBAMOUNT];

			theApp.cfgCPUClockAdjust = itemValue["cpuclock"];

			theApp.cfgLCDTiming = itemValue["lcdtiming"];

			theApp.ApplyConfig(true);
			theApp.SwapReverse();
			theApp.SetView(SETTING_VIEW);
			break;
		case VK_FX :	// X 버튼 	// 이전 메뉴인 View로 이동한다.
			theApp.SwapReverse();
			theApp.SetView(SETTING_VIEW);
			break;
		case VK_FY :	// A,Y 버튼 : IP 설정 변경
		case VK_FA :
            if(menuList[m_posCursor].type == MENU_IPBOX) {
                if((m_itemCursor < 0) || (m_itemCursor > 3))break;
				if(m_joyButton != button) m_joyDown = 0;
				if(m_joyDown == 0) m_joyDown = SDL_GetTicks();
				m_joyButton = button;
                int val = (itemValue[menuList[m_posCursor].item] >> (8*(3-m_itemCursor))) & 0xFF;
				int chg = 1;
				if((m_joyDown + 2000) < SDL_GetTicks()) chg = 10;
                if(button == VK_FY) {
                    val += chg;
					if(val > 0xFF)val = 0;
                } else {
                    val -= chg;
					if(val < 0)val = 0xFF;
                }
                int i, ret = 0;
                for(i=0;i<4;i++) {
                    ret <<= 8;
                    if(m_itemCursor == i)
                        ret |= val;
                    else
                        ret |= (itemValue[menuList[m_posCursor].item] >> (8*(3-i))) & 0xFF;
                }
                itemValue[menuList[m_posCursor].item] = ret;
                OnDraw(pScreen);
            }
			break;
		case VK_START :	// X 버튼 	// Main View로 이동한다.
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
void CSystemView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_FY :	// A,Y 버튼 : IP 설정 변경
		case VK_FA :
			m_joyDown = 0;
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 위로 이동
void CSystemView::OnCursorUp(SDL_Surface *pScreen)
{
	int prev = m_posCursor;
	m_posCursor--;
	while((m_posCursor > 0)&&(menuList[m_posCursor].type == MENU_SPACE))
		m_posCursor--;
	if(m_posCursor < 0)
		m_posCursor = 0;
	if(m_posCursor == prev)
		return;

	if(m_drawBegin > m_posCursor)
		m_drawBegin = m_posCursor;
    m_itemCursor = 0;
	OnDraw(pScreen);	// 다시 그린다.
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 아래로 이동
void CSystemView::OnCursorDown(SDL_Surface *pScreen)
{
	if((menuList[m_posCursor].type == MENU_MENUEXT) && (!itemValue["menuext"]))
		return;
	int prev = m_posCursor;
	m_posCursor++;
	while(menuList[m_posCursor].type == MENU_SPACE)
		m_posCursor++;
	if(menuList[m_posCursor].type == MENU_TERMINATE)
		m_posCursor--;
	if(m_posCursor == prev)
		return;

	if((m_drawBegin + MAX_VIEW) <= m_posCursor)
		m_drawBegin = m_posCursor - MAX_VIEW + 1;
    m_itemCursor = 0;
	OnDraw(pScreen);	// 다시 그린다.
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 왼쪽로 이동
void CSystemView::OnCursorLeft(SDL_Surface *pScreen)
{
	switch(menuList[m_posCursor].type){
		case MENU_ONOFF:
		case MENU_YESNO:
		case MENU_MENUEXT:
			itemValue[menuList[m_posCursor].item] = !itemValue[menuList[m_posCursor].item];
			break;
		case MENU_ONOFFALWAYS:
			if(itemValue[menuList[m_posCursor].item] < 2)
				itemValue[menuList[m_posCursor].item]++;
			else
				itemValue[menuList[m_posCursor].item] = 0;
			break;
		case MENU_SOUND:
		case MENU_LEVEL:
			if(itemValue[menuList[m_posCursor].item] > 0)
				itemValue[menuList[m_posCursor].item]--;
			else
				itemValue[menuList[m_posCursor].item] = 3;
			break;
		case MENU_IPBOX:
            m_itemCursor--;
		case MENU_BUTTON:
			break;
		case MENU_SKIN:
			//if(itemValue[menuList[m_posCursor].item] > 0)
			//	itemValue[menuList[m_posCursor].item]--;
			//else
			//	itemValue[menuList[m_posCursor].item] = skinDir.nCount;
			break;
		case MENU_EFFECT:
			if(itemValue[menuList[m_posCursor].item] > 0)
				itemValue[menuList[m_posCursor].item]--;
			else {
				while(effectList[itemValue[menuList[m_posCursor].item] + 1].label)
					itemValue[menuList[m_posCursor].item]++;
			}
			break;
		case MENU_LANGUAGE:
			if(itemValue[menuList[m_posCursor].item] > 0)
				itemValue[menuList[m_posCursor].item]--;
			else {
				while(languageList[itemValue[menuList[m_posCursor].item] + 1].label)
					itemValue[menuList[m_posCursor].item]++;
			}
			break;
		case MENU_CHARSET:
			if(itemValue[menuList[m_posCursor].item] > 0)
				itemValue[menuList[m_posCursor].item]--;
			else {
				while(charsetList[itemValue[menuList[m_posCursor].item] + 1].label)
					itemValue[menuList[m_posCursor].item]++;
			}
			break;
		case MENU_LCD:
			if(itemValue[menuList[m_posCursor].item] > 0) {
				itemValue[menuList[m_posCursor].item]--;
				theApp.cfgLCDTiming = itemValue["lcdtiming"];
				theApp.setLCDTiming();
			}
			break;
		case MENU_SCALING:
			if(itemValue[menuList[m_posCursor].item] > 0) {
				itemValue[menuList[m_posCursor].item]--;
				set_scaling(itemValue[menuList[m_posCursor].item]);
			}
			break;
	}

	OnDraw(pScreen);	// 다시 그린다.
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 오른쪽으로 이동
void CSystemView::OnCursorRight(SDL_Surface *pScreen)
{
	switch(menuList[m_posCursor].type){
		case MENU_ONOFF:
		case MENU_YESNO:
		case MENU_MENUEXT:
			itemValue[menuList[m_posCursor].item] = !itemValue[menuList[m_posCursor].item];
			break;
		case MENU_ONOFFALWAYS:
			if(itemValue[menuList[m_posCursor].item] > 0)
				itemValue[menuList[m_posCursor].item]--;
			else
				itemValue[menuList[m_posCursor].item] = 2;
			break;
		case MENU_SOUND:
		case MENU_LEVEL:
			if(itemValue[menuList[m_posCursor].item] < 3)
				itemValue[menuList[m_posCursor].item]++;
			else
				itemValue[menuList[m_posCursor].item] = 0;
			break;
		case MENU_IPBOX:
            m_itemCursor++;
		case MENU_BUTTON:
			break;
		case MENU_SKIN:
			//itemValue[menuList[m_posCursor].item]++;
			//if(itemValue[menuList[m_posCursor].item] > skinDir.nCount)
			//	itemValue[menuList[m_posCursor].item] = 0;
			break;
		case MENU_EFFECT:
			itemValue[menuList[m_posCursor].item]++;
			if(!effectList[itemValue[menuList[m_posCursor].item]].label)
				itemValue[menuList[m_posCursor].item] = 0;
			break;
		case MENU_LANGUAGE:
			itemValue[menuList[m_posCursor].item]++;
			if(!languageList[itemValue[menuList[m_posCursor].item]].label)
				itemValue[menuList[m_posCursor].item] = 0;
			break;
		case MENU_CHARSET:
			itemValue[menuList[m_posCursor].item]++;
			if(!charsetList[itemValue[menuList[m_posCursor].item]].label)
				itemValue[menuList[m_posCursor].item] = 0;
			break;
		case MENU_LCD:
			if(itemValue[menuList[m_posCursor].item] < 13) {
				itemValue[menuList[m_posCursor].item]++;
				theApp.cfgLCDTiming = itemValue["lcdtiming"];
				theApp.setLCDTiming();
			}
			break;
		case MENU_SCALING:
			if(itemValue[menuList[m_posCursor].item] < 
					sizeof(scaling_list) / sizeof(scaling_list[0]) - 1) {
				itemValue[menuList[m_posCursor].item]++;
				set_scaling(itemValue[menuList[m_posCursor].item]);
			}
			break;
	}

	OnDraw(pScreen);	// 다시 그린다.
}
//[*]------------------------------------------------------------------------------------------[*]
void CSystemView::OnMoveSub_Execute(SDL_Surface *pScreen)
{
}
//[*]------------------------------------------------------------------------------------------[*]
// 실행
void CSystemView::OnExecute(SDL_Surface *pScreen)
{
}
//[*]------------------------------------------------------------------------------------------[*]
void CSystemView::loadLang()
{
	int i;
	std::string str;

	for(i=0; menuList[i].label; i++) {
		str = INI_ReadText(menuList[i].group, menuList[i].item, "");
		if(str.size() > 0)
			lang[menuList[i].group][menuList[i].item] = str;
		else
			lang[menuList[i].group][menuList[i].item] = menuList[i].label;
	}

	for(i=0; effectList[i].label; i++) {
		str = INI_ReadText("effect", effectList[i].item, "");
		if(str.size() > 0)
			lang["effect"][effectList[i].item] = str;
		else
			lang["effect"][effectList[i].item] = effectList[i].label;
	}

	str = INI_ReadText("skin", "default", "");
	if(str.size() > 0)
		lang["skin"]["default"] = str;
	else
		lang["skin"]["default"] = "Default";
}
//[*]------------------------------------------------------------------------------------------[*]
void CSystemView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	int i, w, h, px, py ,count,postX,postY;

	for(i=m_drawBegin, count = 0;count < MAX_VIEW;i++, count++)
	{
		if((i == m_posCursor) && (menuList[i].type == MENU_SPACE))
			m_posCursor++;

		postX = 145;
    	postY = (43 + count*28);
		switch(menuList[i].type)
		{
			case MENU_ONOFFALWAYS:
				w = 90;
				h = 25;

	            px = postX + 80;
				py = postY;

				if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
				{
					m_posCursor = i;

					OnCursorLeft(pScreen);

					break;
				}
				break;

			case MENU_ONOFF:
			case MENU_MENUEXT:
				w = 48;
				h = 25;

	            px = postX;
				py = postY;

				if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
				{
					m_posCursor = i;

					if(itemValue[menuList[i].item]==0)
						OnCursorLeft(pScreen);
					break;
				}



            	px = postX + 48;
				if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
				{
					m_posCursor = i;

					if(itemValue[menuList[i].item]==1)
						OnCursorRight(pScreen);
					break;
				}

				break;
			case MENU_YESNO:
				w = 45;
				h = 25;

				px = postX;
				py = postY;
				if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
				{
					m_posCursor = i;
					OnCursorLeft(pScreen);
					break;
				}


	            px = postX + 40;
				if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
				{
					m_posCursor = i;
					OnCursorLeft(pScreen);
					break;
				}
				break;

			case MENU_SKIN:
			case MENU_EFFECT:
			case MENU_SOUND:
			case MENU_LANGUAGE:
			case MENU_CHARSET:
			case MENU_LCD:
			case MENU_SCALING:

				w = pImgArrow->w >> 1;
				h = pImgArrow->h >> 1;

				px = 145-16;
				py = postY;
				if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
				{
					m_posCursor = i;

					OnCursorLeft(pScreen);
					break;
				}

				px = px + 126;

				if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
				{
					m_posCursor = i;

					OnCursorRight(pScreen);
					break;
				}

				break;
		}


	}

}
//[*]------------------------------------------------------------------------------------------[*]
void CSystemView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{


}
//[*]------------------------------------------------------------------------------------------[*]
