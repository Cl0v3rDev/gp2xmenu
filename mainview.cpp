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
#include <sys/stat.h>
#include <SDL/SDL_image.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainview.h"
#include "mainmenuapp.h"
#include "typed.h"
#include "inifile.h"
//[*]------------------------------------------------------------------------------------------[*]
static char Order[] = {0, 1, 2, 3, 4, 5, 6};
//[*]------------------------------------------------------------------------------------------[*]
int thread_init(void *app)
{
	CMainMenuApp *theApp = (CMainMenuApp *)app;

	if(theApp->bootFlag)
	{
		system("/root/start.sh boot");
	}

	theApp->ApplyConfig(false);
	theApp->bootFlag = false;

    return(0);
}
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CMainView::CMainView()
{
	pImgBackGround = NULL;
	//pImgButton = NULL;
	pImgIcon_small=NULL;
	pImgIcon_middle=NULL;
	pImgIcon_big=NULL;
	pImgIcon_title=NULL;
	pImgArrow = NULL;
	pImgBattery = NULL;

	threadBoot = NULL;
	posCursor = 0;			// Ŀ�� ��ġ
	useSDIcon = false;

	//pImgBackGround_game=NULL;	// ���ӹ�� Surface
	//pImgBackGround_movie=NULL;	// ������ Surface
	//pImgBackGround_music=NULL;	// ������� Surface
	//pImgBackGround_photo=NULL;	// ������ Surface
	//pImgBackGround_ebook=NULL;	// �̺Ϲ�� Surface
	//pImgBackGround_setting=NULL;// ���ù�� Surface
	pImgBackGround_SD = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
CMainView::~CMainView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// �ʱ�ȭ �Լ�
void CMainView::Initialize(bool refresh)
{
	// �̹����� �̸� �ε� �Ѵ�.
	pImgBackGround = theApp.LoadImage("main", "body", "PERSONAL ENTERTAINMENT PLAYER");
	//pImgButton = theApp.LoadImage("main", "button");
	pImgIcon_small=theApp.LoadImage("main", "icon_small");
	pImgIcon_middle=theApp.LoadImage("main", "icon_middle");
	pImgIcon_big=theApp.LoadImage("main", "icon_big");
	pImgIcon_title=theApp.LoadImage("main", "icon_title");
	pImgArrow = theApp.LoadImage("common", "arrow");
	pImgBattery = theApp.LoadImage("common", "batt_status");

	//���� �߰��� �κ�
	//pImgBackGround_game = theApp.LoadImage("game", "body", "PERSONAL ENTERTAINMENT PLAYER");
	//pImgBackGround_movie = theApp.LoadImage("movie", "body", "PERSONAL ENTERTAINMENT PLAYER");
	//pImgBackGround_music = theApp.LoadImage("music", "body", "PERSONAL ENTERTAINMENT PLAYER");
	//pImgBackGround_photo = theApp.LoadImage("photo", "body", "PERSONAL ENTERTAINMENT PLAYER");
	//pImgBackGround_ebook = theApp.LoadImage("ebook", "body", "PERSONAL ENTERTAINMENT PLAYER");
	//pImgBackGround_setting = theApp.LoadImage("setting", "body", "PERSONAL ENTERTAINMENT PLAYER");
	pImgBackGround_SD = NULL;
	//���� �߰��� �κ� ��

	struct stat info;
	if ((stat(SD_PATH"/autorun.ini", &info) == 0) && S_ISREG(info.st_mode))
	{
		char icon[1024] = SD_PATH"/";
		char image[1024] = SD_PATH"/";
		SDL_Surface *pIcon;
		Order[6] = 7;

		INI_Open(SD_PATH"/autorun.ini");
		strcat(icon, INI_ReadText("info", "icon", ""));
		strcat(image, INI_ReadText("info", "image", ""));
		INI_Close();

		pImgBackGround_SD = IMG_Load(image);
		pIcon = IMG_Load(icon);
		if(pIcon != NULL)
		{
			SDL_Rect srcrect, rect;

			srcrect.w = pImgIcon_middle->w / 3;
			srcrect.h = pImgIcon_middle->h / 3;
			rect.x = (srcrect.w * 6%3) + ((srcrect.w - 32) >> 1);
			rect.y = (srcrect.h * 6/3) + ((srcrect.h - 32) >> 1) - 3;
			srcrect.w = 32;
			srcrect.h = 32;
			srcrect.x = 0;
			srcrect.y = 0;
			SDL_BlitSurface(pIcon, &srcrect, pImgIcon_middle, &rect);

			Order[6] = 6;

			SDL_SAFE_FREE(pIcon);
		}
	}
	useSDIcon = (pImgBackGround_SD != NULL);
	SDMounted = theApp.isSDMount();

	iconCount = ICONCOUNT_MAIN + (useSDIcon ? 1 : 0);
	if(theApp.firstRun && useSDIcon) posCursor = 6;

	lastDraw = -1;

	theApp.AddTimer(3000);

	//if(!refresh) {
	//	theApp.AddTimer(theApp.mainFrameDelay);
	//}
}
//[*]------------------------------------------------------------------------------------------[*]
// ���� �Լ�
void CMainView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
	//SDL_SAFE_FREE(pImgButton);
	SDL_SAFE_FREE(pImgIcon_small);
	SDL_SAFE_FREE(pImgIcon_middle);
	SDL_SAFE_FREE(pImgIcon_big);
	SDL_SAFE_FREE(pImgIcon_title);
	SDL_SAFE_FREE(pImgArrow);
	SDL_SAFE_FREE(pImgBattery);

	//SDL_SAFE_FREE(pImgBackGround_game);	// ���ӹ�� Surface
	//SDL_SAFE_FREE(pImgBackGround_movie);	// ������ Surface
	//SDL_SAFE_FREE(pImgBackGround_music);	// ������� Surface
	//SDL_SAFE_FREE(pImgBackGround_photo);	// ������ Surface
	//SDL_SAFE_FREE(pImgBackGround_ebook);	// �̺Ϲ�� Surface
	//SDL_SAFE_FREE(pImgBackGround_setting);// ���ù�� Surface
	SDL_SAFE_FREE(pImgBackGround_SD);

	//if(!refresh) {
	//	setClock = 1;
		theApp.RemoveTimer();
	//}
}
//[*]------------------------------------------------------------------------------------------[*]
// Ÿ�̸� ó�� �Լ�
void CMainView::OnTimerProc(SDL_Surface *pScreen)
{
	struct stat info;

	if (SDMounted == theApp.isSDMount()) return;
	SDMounted = theApp.isSDMount();

	printf("mainview�� ON TIMER PROC()\n");

	if (SDMounted && (stat(SD_PATH"/autorun.ini", &info) == 0) && S_ISREG(info.st_mode))
	{
		char icon[1024] = SD_PATH"/";
		char image[1024] = SD_PATH"/";
		SDL_Surface *pIcon;
		Order[6] = 7;

		INI_Open(SD_PATH"/autorun.ini");
		strcat(icon, INI_ReadText("info", "icon", ""));
		strcat(image, INI_ReadText("info", "image", ""));
		INI_Close();

		pImgBackGround_SD = IMG_Load(image);
		pIcon = IMG_Load(icon);
		if(pIcon != NULL)
		{
			SDL_Rect srcrect, rect;

			SDL_SAFE_FREE(pImgIcon_middle);
			pImgIcon_middle=theApp.LoadImage("main", "icon_middle");
			srcrect.w = pImgIcon_middle->w / 3;
			srcrect.h = pImgIcon_middle->h / 3;
			rect.x = (srcrect.w * 6%3) + ((srcrect.w - 32) >> 1);
			rect.y = (srcrect.h * 6/3) + ((srcrect.h - 32) >> 1) - 3;
			srcrect.w = 32;
			srcrect.h = 32;
			srcrect.x = 0;
			srcrect.y = 0;
			SDL_BlitSurface(pIcon, &srcrect, pImgIcon_middle, &rect);

			Order[6] = 6;

			SDL_SAFE_FREE(pIcon);
		}
	}
	else
	{
		SDL_SAFE_FREE(pImgBackGround_SD);
	}

	useSDIcon = (pImgBackGround_SD != NULL);
	iconCount = ICONCOUNT_MAIN + (useSDIcon ? 1 : 0);
	if(useSDIcon)
	{
		posCursor = 6;
	}
	else
	{
		posCursor = posCursor % iconCount;
	}
	OnDraw(pScreen);

	//if( (theApp.mainFrame[posCursor] > 1) && (loopIcon != 0) )
	//	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw �Լ�
void CMainView::OnDraw(SDL_Surface *pScreen)
{

	SDL_Rect	srcrect, rect;

	SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);
	if((posCursor == 6) && (pImgBackGround_SD != NULL))
		SDL_BlitSurface(pImgBackGround_SD, NULL, pScreen, NULL);

	//������ Ÿ��Ʋ �ε� ����
	srcrect.w = pImgIcon_title->w / 3;
	srcrect.h = pImgIcon_title->h / 3;
	srcrect.x = srcrect.w * (posCursor%3);
	srcrect.y = srcrect.h * (posCursor/3);
	rect.x    = 10;
	rect.y    = 6;
	SDL_BlitSurface(pImgIcon_title, &srcrect, pScreen, &rect);

	//���͸� ������ �ε� ����
	theApp.getStatusRect(&rect);
	theApp.drawStatusIcon(pScreen, &rect);

	//�� ������ �ε� ����
	srcrect.w = pImgIcon_big->w / 3;
	srcrect.h = pImgIcon_big->h / 3;
	srcrect.x = srcrect.w * (posCursor%3);
	srcrect.y = srcrect.h * (posCursor/3);
	rect.x    = XRES /3;
	rect.y    = YRES /5;
	SDL_BlitSurface(pImgIcon_big, &srcrect, pScreen, &rect);

	//���� ȭ��ǥ
	srcrect.x = 0;
	srcrect.y = pImgArrow->h >> 1;
	srcrect.w = pImgArrow->w >> 1;
	srcrect.h = pImgArrow->h >> 1;
	rect.x = 4;
	rect.y = 198;
	SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &rect);

	//������ ȭ��ǥ
	rect.x = 173;
	rect.y = 198;
	srcrect.w = pImgArrow->w >> 1;
	srcrect.h = pImgArrow->h >> 1;
	srcrect.x = srcrect.w;
	srcrect.y = srcrect.h;
	SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &rect);

	int i, count;
	// �������� �׸���.(���� �ٲ�� F-200�� �޴� �迭)
	for(i=posCursor-1, count = 0; count < 3; i++, count++)
	{

		if(i >= iconCount)
			i = i % iconCount;
		if(i < 0)
			i = iconCount - 1;

		int icon = Order[i];

		if(posCursor == i)
		{
			srcrect.w = pImgIcon_middle->w / 3;
			srcrect.h = pImgIcon_middle->h / 3;
			srcrect.x = srcrect.w * (icon%3);
			srcrect.y = srcrect.h * (icon/3);
		}
		else
		{
			if(icon == 7) icon = 6;
			srcrect.w = pImgIcon_small->w / 3;
			srcrect.h = pImgIcon_small->h / 3;
			srcrect.x = srcrect.w * (icon%3);
			srcrect.y = srcrect.h * (icon/3);
		}

		rect.x = (count % 3) * 67 + ((66 - srcrect.w) >> 1);
		if (posCursor == i)
		{
			rect.x = rect.x-1;
		}
		else
		{
			if( (count%3) == 0 )
				rect.x = rect.x+7;
			else
				rect.x = rect.x-12;

		}

		rect.y = 55 + ((90 - srcrect.h) >> 1) + 100;

		if(posCursor == i)
			SDL_BlitSurface(pImgIcon_middle, &srcrect, pScreen, &rect);
		else
			SDL_BlitSurface(pImgIcon_small , &srcrect, pScreen, &rect);
	}

	SDL_UpdateRect(pScreen, 0, 0, 0, 0);		// ȭ�� ������Ʈ

	if(theApp.firstRun) {
		threadBoot = SDL_CreateThread(thread_init, &theApp);
		theApp.firstRun = false;
	}

	lastDraw = posCursor;
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_UP 		:	OnCursorUp(pScreen);		break;	// �� ����Ű
		case VK_DOWN 	:   OnCursorDown(pScreen);		break;	// �� ����Ű
		case VK_LEFT 	:   OnCursorLeft(pScreen);		break;	// �� ����Ű
		case VK_RIGHT 	:   OnCursorRight(pScreen);		break;	// �� ����Ű
		case VK_TAT	  	:     // TAT ��ư
		case VK_FB		:     // B ��ư
			OnMoveSub_Execute(pScreen);	break;	// B ��ư
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainView::OnJoystickUp(SDL_Surface *pScreen, Uint8 button)
{
	// Nothing...
}
//[*]------------------------------------------------------------------------------------------[*]
#define CHECK(x1, y1, x2, y2) \
	((x1 <= x) && (x2 >= x) && (y1 <= y) && (y2 >= y))

void CMainView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	if(CHECK(5, 180, 65, 220)) { // Left Arrow
		OnCursorLeft(pScreen);
	} else if(CHECK(135, 180, 205, 220)) { // Right Arrow
		OnCursorRight(pScreen);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainView::OnPenUp(SDL_Surface *pScreen, int x, int y)
{
	if(CHECK(75, 180, 130, 220) || CHECK(120, 50, 290, 180)) { // Icon
		OnMoveSub_Execute(pScreen);
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// Ŀ���� ���� �̵�
void CMainView::OnCursorUp(SDL_Surface *pScreen)
{
/*
	if(posCursor-4 >= 0) {
		posCursor -= 4;

		// �ٽ� �׸���.
		theApp.PlaySound(MOVE);
		OnDraw(pScreen);
	}
*/
}
//[*]------------------------------------------------------------------------------------------[*]
// Ŀ���� �Ʒ��� �̵�
void CMainView::OnCursorDown(SDL_Surface *pScreen)
{
/*
	if(posCursor+4 < ICONCOUNT_MAIN) {
		posCursor += 4;

		// �ٽ� �׸���.
		theApp.PlaySound(MOVE);
		OnDraw(pScreen);
	}
*/
}
//[*]------------------------------------------------------------------------------------------[*]
// Ŀ���� ���ʷ� �̵�
void CMainView::OnCursorLeft(SDL_Surface *pScreen)
{
	posCursor = (posCursor-1+iconCount)%iconCount;

	//if(posCursor-1 >= 0)	posCursor -= 1;

	// �ٽ� �׸���.
	theApp.PlaySound(MOVE);
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// Ŀ���� �Ʒ��� �̵�
void CMainView::OnCursorRight(SDL_Surface *pScreen)
{
	posCursor = (posCursor+1+iconCount)%iconCount;
	//if(posCursor+1 < ICONCOUNT_MAIN)	posCursor += 1;

	// �ٽ� �׸���.
	theApp.PlaySound(MOVE);
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
void CMainView::OnMoveSub_Execute(SDL_Surface *pScreen)
{
	char *cmd = NULL;
	bool sdrun = false;
	int clock;

	theApp.PlaySound(RUN);

	switch(posCursor)
	{
		case 0 :	// Game
			theApp.SetView(GAME_VIEW);
			break;

		case 1 :	// Movie
			if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
				theApp.SetView(MEDIA_PLAY_VIEW);
			else
				theApp.SetView(MOVIE_VIEW);
			break;

		case 2 :	// Music
			if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
				theApp.SetView(MEDIA_PLAY_VIEW);
			else
				theApp.SetView(MUSIC_VIEW);
			break;

		case 3 :	// E-Book
			if(theApp.textvs == TEXT_ON_STATUS)
				theApp.SetView(EBOOK_TEXT_VIEW);
			else
				theApp.SetView(EBOOK_VIEW);
			break;

		case 4 :	// Photo
			if(theApp.photovs == ON_STATUS)
				theApp.SetView(PHOTO_IMAGE_VIEW);
			else
				theApp.SetView(PHOTO_VIEW);
			break;

		case 5 :	// Setting
			theApp.SetView(SETTING_VIEW);
			break;

		case 6:
			sdrun = true;
			break;
	}

	if (!sdrun) return;

	//���õ� ini������ ��θ� �о�´�
	char gamePath[512] = SD_PATH"/";
	INI_Open( SD_PATH"/autorun.ini" );
	strcat(gamePath, INI_ReadText("info", "path", "") );
	INI_Close();
	if (access(gamePath, X_OK) != 0) return;

#ifndef WIN32
	// �켱 ȭ���� �����.
	SDL_Rect rect = { 0, 0, 320, 240 };
	SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, 0x00, 0x00, 0x00));
	SDL_UpdateRect(pScreen, 0, 0, 320, 240);		// ȭ�� ������Ʈ

	// ���� ������ ��θ� �����.
	char cutname[512];
	char cutpath[512];

	char *cut = strrchr(gamePath, '/');
	if(cut == NULL) cut = strrchr(gamePath, '\\');
	if(cut == NULL)
	{
		strcpy(cutname, gamePath);
		strcpy(cutpath, "");
	}
	else
	{
		strcpy(cutname, cut + 1);
		strncpy(cutpath, gamePath, cut - gamePath);
		cutpath[cut - gamePath] = 0;
	}

	chdir(cutpath);		// ������ �ִ� ���丮�� �̵�

	std::string szExecFile = std::string("./");
	szExecFile += cutname;

	if(threadBoot != NULL) { // ��Ʈ ������ ����ñ��� ���
		SDL_WaitThread(threadBoot, NULL);
		threadBoot = NULL;
	}

	Exitialize(); 	// ���ҽ� ����
	theApp.SDL_Exitialize();

	execlp(szExecFile.c_str(), szExecFile.c_str(), NULL, 0);		// ���� ����
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
// ����
void CMainView::OnExecute(SDL_Surface *pScreen)
{
}
//[*]------------------------------------------------------------------------------------------[*]
