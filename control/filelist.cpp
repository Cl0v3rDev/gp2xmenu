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
#include <inifile.h>
#include <SDL/SDL_image.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "filelist.h"
#include "textout.h"
#include "mainmenuapp.h"
//[*]----------------------------------------------------------------------------------------------------[*
#define FILELIST_COUNT 8
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
FileList::FileList()
{
   	pImgBackGround = NULL;
	pImgStorage = NULL;
	pImgIcon = NULL;
	pImgLine = NULL;
	pImgStorage = NULL;
	pImgExit = NULL;
	pImgArrow = NULL;
	useStorage = true;
	selectFunc = NULL;
	lastPath.clear();
	lastName.clear();
	
	iconWidth = 34;
   	nPosition = 0;
	nStartCount = 0;
   	nEndCount = 0;
   	nCount = 0;
   	pList = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
FileList::~FileList()
{
	Exitialize();
	if(pList)
		delete [] pList;
}
//[*]------------------------------------------------------------------------------------------[*]
void FileList::Initialize(bool refresh)
{
	SDL_Surface *icon = theApp.LoadImage("common", "icon_body");
	if(icon != NULL)
	{
		SDL_Rect srcrect, dstrect;
		srcrect.w = icon->w / 3;
		srcrect.h = icon->h / 3;
		srcrect.x = srcrect.w * (iconNo % 3);
		srcrect.y = srcrect.h * (iconNo / 3);
		dstrect.x = 1;
		dstrect.y = 7;
		SDL_BlitSurface(icon, &srcrect, pImgBackGround, &dstrect);
		SDL_SAFE_FREE(icon);
	}

	pImgIcon = theApp.LoadImage("common", "icon");
	pImgLine = theApp.LoadImage("common", "selectbar");
	pImgExit = theApp.LoadImage("common", "close");
	pImgArrow = theApp.LoadImage("common", "arrow");
	pImgStorage = theApp.LoadImage("common", "storage");

	if(!refresh)
	{
		waitMode = WAIT_NONE;
		cursor = 0;

		if( search(lastPath) > -1 )
		{
	    	nStartCount = 0;
			nPosition = 0;
			nEndCount = (nCount > FILELIST_COUNT) ? FILELIST_COUNT : nCount;

			if(lastName.size() == 0) return;

	    	// �ڽ��� ��ġ�� ã�Ƽ� �̵��Ѵ�.
	    	int i;
	    	for(i=0; i<nCount; i++)
	    	{
				if(lastName  == pList[i].szName)
				{
					//���⼭ �����ǰ� ���� �������� ������ ���� �̵� ���Ѿ� �Ѵ�.
					nPosition = i;
					if(nStartCount > i)
					{
						nStartCount = i;
						nEndCount = nStartCount + FILELIST_COUNT;
						if(nEndCount > nCount)	nEndCount = nCount;
					}
					else if(nEndCount <= i)
					{
						nEndCount = i+1;
						nStartCount = nEndCount - FILELIST_COUNT;
						if(nStartCount < 0)	nStartCount = 0;
					}
					break;
	    		}
	    	}
		}
		else
		{
		   	nPosition = 0;
			nStartCount	= 0;
		   	nEndCount = 0;
		   	nCount = 0;
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void FileList::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
	SDL_SAFE_FREE(pImgIcon);
	SDL_SAFE_FREE(pImgLine);
	SDL_SAFE_FREE(pImgExit);
	SDL_SAFE_FREE(pImgArrow);
    SDL_SAFE_FREE(pImgStorage);
}
//[*]------------------------------------------------------------------------------------------[*]
void FileList::OnDraw(SDL_Surface *pScreen)
{
	if(pScreen == NULL) return;

	SDL_Rect srcrect, dstrect;

	SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	if(useStorage)
	{
		// ���丮�� �̹��� ǥ��
		srcrect.w = pImgStorage->w / 2;
		srcrect.h = pImgStorage->h / 2;

		srcrect.x = (cursor == 1) ? srcrect.w : 0;
		srcrect.y = 0;
		dstrect.x = XRES - srcrect.w - 60;
		dstrect.y = 4;
		SDL_BlitSurface(pImgStorage, &srcrect, pScreen, &dstrect);

		srcrect.x = (cursor == 2) ? srcrect.w : 0;
		srcrect.y = srcrect.h;
		dstrect.x = dstrect.x + srcrect.w + 7;
		dstrect.y = dstrect.y;
		SDL_BlitSurface(pImgStorage, &srcrect, pScreen, &dstrect);
	}

	// X �̹��� ǥ�� (main �޴��� ��������)
	srcrect.w = pImgExit->w / 2;
	srcrect.h = pImgExit->h;

	srcrect.x = (cursor == 3) ? srcrect.w : 0;
	srcrect.y = 0;
	dstrect.x = XRES - srcrect.w - 5;
	dstrect.y = 5;
	SDL_BlitSurface(pImgExit, &srcrect, pScreen, &dstrect);

    // ���� ȭ��ǥ ǥ��
    if( (nStartCount != 0) && (pImgArrow != NULL) )
	{
		srcrect.w = pImgArrow->w >> 1;
		srcrect.h = pImgArrow->h >> 1;
		srcrect.x = srcrect.y = 0;
		dstrect.x = 320 - srcrect.w - 2;
		dstrect.y = 67;

		SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);
    }

    // �Ʒ��� ȭ��ǥ ǥ��
    if( (nEndCount != nCount) && (pImgArrow != NULL) )
	{
		srcrect.w = pImgArrow->w >> 1;
		srcrect.h = pImgArrow->h >> 1;
		srcrect.x = srcrect.w;
		srcrect.y = 0;
		dstrect.x = 320 - srcrect.w - 2;
		dstrect.y = 67+7*21;

		SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);
	}

	char lastPath_nmnt[512];
	strcpy(lastPath_nmnt, lastPath.c_str() + strlen(ROOT_PATH) + 1);
	//printf("**********lastPath_nmnt : %s\n",lastPath_nmnt);

	unsigned short temp_unicode[512];
#ifdef WIN32
	toUnicode(NULL, lastPath_nmnt, temp_unicode, strlen(lastPath_nmnt));
#else
	toUnicode("UTF-8", lastPath_nmnt, temp_unicode, strlen(lastPath_nmnt));
#endif

	DrawTextOut(pScreen, iconWidth, 15, 320-39, temp_unicode, r, g, b, 1);
	DrawTextOut(pScreen, iconWidth + 1, 15, 320-40, temp_unicode, r, g, b, 1);

	int i, j, iconGroup, iconPos;

	// UTF-8�� �����ڵ�� �����Ѵ�.
    for(i=nStartCount,j=0; i<nEndCount; i++,j++)
    {
		// ���� ���õ� ������ ������ �׸���.
		if(nPosition == i && pImgLine != NULL && cursor==0)
		{
			dstrect.x = 38;
			dstrect.y = 63+j*21;

			SDL_BlitSurface(pImgLine, NULL, pScreen, &dstrect);
		}

    	// �����ڵ�� ���� ����
    	unsigned short dest[512];
    	char name[256];

    	if(pList[i].nAttribute == INI_FORMAT) //ini���� �̶��
		{
			char gameName[512]={0,};  //���� Ÿ��Ʋ��
			char iconPath[512]={0,};  //��ü ������ ���
			SDL_Surface *pImgIcon_GPE = NULL; //��ü ������

			INI_Open( (lastPath + "/" + pList[i].szName).c_str() );
			strcpy(gameName, INI_ReadText("info", "name", pList[i].szName.c_str()) );
			strcpy(iconPath, INI_ReadText("info", "icon", "") );
			INI_Close();

   			toUnicode("UTF-8", gameName, dest, strlen(gameName));

			srcrect.w = 20;
			srcrect.h = 15;
			dstrect.x = 18;
			dstrect.y = 65+j*21;
			// ��ü �������� ������ ���� �Ѵ�.

			if(access(iconPath, F_OK) == 0)
			{
					pImgIcon_GPE = IMG_Load(iconPath);
					srcrect.x = 0;
					srcrect.y = 0;

					SDL_BlitSurface(pImgIcon_GPE, &srcrect, pScreen, &dstrect);

					SDL_SAFE_FREE(pImgIcon_GPE);
			}
			else //��ü �������� ����(�⺻ ���� ������ ���)
			{

				srcrect.x = 1 * srcrect.w;
				srcrect.y = 4 * srcrect.h;

				SDL_BlitSurface(pImgIcon, &srcrect, pScreen, &dstrect);

			}
		}
    	else	//gpe,gpu,txt,mp3 ���� ���� �̶��
    	{
    		if((pList[i].nAttribute == FOLDER_FORMAT) && (pList[i].nTotalCount > 0))
	    	{
		    	sprintf(name, "%s [%d/%d]", pList[i].szName.c_str(),
				pList[i].nMarkedIndex+1, pList[i].nTotalCount);
			}
			else
			{
		    	strcpy(name, pList[i].szName.c_str());
			}
    		// ���ϸ��� �����ڵ�� ����
#ifdef WIN32
	   		toUnicode(NULL, name, dest, strlen(name));
#else
	   		toUnicode("UTF-8", name, dest, strlen(name));
#endif

    		if( (pList[i].nAttribute == GPE_FORMAT) || (pList[i].nAttribute == GPU_FORMAT) )//gpe,gpu ���� �̶��
    		{
	    		std::string szPngName = lastPath + "/" + pList[i].szName;		// FullPath
				int lenPngName   = strlen(szPngName.c_str());										// FullPath Length
				// png ������ ���ؾ� �ϹǷ� ������ 3�ڸ��� �ٲ۾� �ش�.
				szPngName = szPngName.substr(0, lenPngName-4) + ".png";								// ext png FullPath
			    SDL_Surface *pImgIcon_GPE = NULL; //��ü ������

			    srcrect.w = 20;
				srcrect.h = 15;
			    dstrect.x = 18;
				dstrect.y = 65+j*21;
			    // png������ ������ �װ� ǥ���Ѵ�.
			    if(access(szPngName.c_str(), F_OK) == 0)
			    {
					pImgIcon_GPE = IMG_Load(szPngName.c_str());
					srcrect.x = 0;
					srcrect.y = 0;

			    	SDL_BlitSurface(pImgIcon_GPE, &srcrect, pScreen, &dstrect);

			    	SDL_SAFE_FREE(pImgIcon_GPE);
			    }
				else //��ü �������� ����(�⺻ ���� ������ ���)
				{
					srcrect.x = 1 * srcrect.w;
					srcrect.y = 4 * srcrect.h;

					SDL_BlitSurface(pImgIcon, &srcrect, pScreen, &dstrect);
				}


    		}
    		else //txt,mp3,img ����� ���� �̶��
			{
    	   		// �Ӽ��� Ȯ���ؼ� �������� �����Ѵ�.
				switch(pList[i].nAttribute)
				{
					case FOLDER_FORMAT	: iconGroup = 2;iconPos = 3; break;
					case MP3_FORMAT		: iconGroup = 1;iconPos = 0; break;
					case OGG_FORMAT		: iconGroup = 1;iconPos = 1; break;
					case WAV_FORMAT		: iconGroup = 1;iconPos = 2; break;
					case AVI_FORMAT		: iconGroup = 0;iconPos = 0; break;
					case OGM_FORMAT		: iconGroup = 0;iconPos = 5; break;
					case BMP_FORMAT		:
					case GIF_FORMAT		:
					case PNG_FORMAT		:
					case PCX_FORMAT		:
					case JPG_FORMAT		: iconGroup = 2;iconPos = 0; break;
					case TXT_FORMAT		: iconGroup = 2;iconPos = 1; break;
					//case GPU_FORMAT     :
					//case GPE_FORMAT     : iconGroup = 1;iconPos = 4; break;
					default				: iconGroup = 2;iconPos = 2; break;
				}

				if(pImgIcon != NULL)
				{
					srcrect.w = pImgIcon->w / 3;
					srcrect.h = pImgIcon->h / 6;
					srcrect.x = iconGroup * srcrect.w;
					srcrect.y = iconPos * srcrect.h;

					dstrect.x = 18;
					dstrect.y = 65+j*21;

					SDL_BlitSurface(pImgIcon, &srcrect, pScreen, &dstrect);
				}

			}

		}

		DrawTextOut(pScreen, 38, 67+j*21, 320-38-10, dest, r, g, b);
    }

    SDL_UpdateRect(pScreen, 0, 0, 0, 0);
}
//[*]------------------------------------------------------------------------------------------[*]
int FileList::action(int cmd)
{
	int result = UPDATE_NONE;
	if(cmd == -1)
	    cmd = cursor;
	
	switch (cmd)
	{
		case 0:
			{
				if(nCount == 0)	return UPDATE_NONE;

				if(pList[nPosition].nAttribute != FOLDER_FORMAT)
					return DO_ACTION;

				if(pList[nPosition].szName == "..")
		    	{
					int i = lastPath.find_last_of("/\\");
					std::string name = lastPath.substr(i + 1);
					std::string path = lastPath.substr(0, i);
					if(search(path) < 0)
					{
						printf("search failed...\n");
						return UPDATE_NONE;
					}

					nPosition = 0;
					for(i=0; i < nCount; i++)
					{
						if(pList[i].nAttribute != FOLDER_FORMAT)
						    break;
						    
						if(strcasecmp(name.c_str(), pList[i].szName.c_str()) == 0)
						    nPosition = i;
					}
		    	}
		    	else
				{
					std::string path = lastPath + "/" + pList[nPosition].szName;
				    int index = pList[nPosition].nMarkedIndex;

					if(search(path) < 0)
					{
						printf("search failed...\n");
						return UPDATE_NONE;
					}

					nPosition = 0;
					
					// workaround, fixme!!
					if(index > -1) {
					    theApp.menuPhoto.LoadComicMark(lastPath.c_str());

						while((nPosition < nCount) && (pList[nPosition].nAttribute == FOLDER_FORMAT))
						    ++nPosition;
					    nPosition += index;
					}
				}

			    if(nPosition >= nCount) nPosition = nCount - 1;
				if(nPosition < 0) nPosition = 0;
				
				nEndCount = nPosition + 1;
				nStartCount = nEndCount - FILELIST_COUNT;
				if(nStartCount < 0) nStartCount = 0;
				nEndCount = nStartCount + FILELIST_COUNT;
				if(nEndCount > nCount) nEndCount = nCount;

				result = UPDATE_FULL;
			}
			break;
		case 1:
		case 2:
			{
				bool useSD = (strncmp(lastPath.c_str(), SD_PATH, strlen(SD_PATH)) == 0);
				bool refresh = false;

				if(!useSD && (cmd == 1))
				{
					search(SD_PATH);
					refresh = true;
				}
				else if (useSD && (cmd == 2))
				{
					search(EXT_PATH);
					refresh = true;
				}

				if(refresh)
				{
			    	nStartCount = 0;
					nPosition = 0;
					nEndCount = (nCount > FILELIST_COUNT) ? FILELIST_COUNT : nCount;
				}

				cursor = 0;
				result = UPDATE_FULL;
			}
			break;
		case 3:
			theApp.SwapReverse();

			// �����̳� ������ ������̶�� �̵�� ������ Jump
			if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
				theApp.SetView(MEDIA_PLAY_VIEW);
			else
				theApp.SetView(MAIN_VIEW);
			break;
	}
	return result;
}
//[*]------------------------------------------------------------------------------------------[*]
// ���̽�ƽ �ٿ� ó��
void FileList::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(waitMode)
	{
		case WAIT_ANY_BUTTON:
			switch(button) {
				case VK_FB:
				case VK_FX:
				case VK_TAT:
					OnDraw(pScreen);
					waitMode = WAIT_NONE;
					break;
			}
			return;
		case WAIT_QUERY:
			switch(button) {
				case VK_FB:
				case VK_FX:
				case VK_TAT:
					OnDraw(pScreen);
					OnExecute(pScreen, button);
					break;
			}
			return;
	}

	switch(button)
	{
		case VK_UP 	  : 	// �� ����Ű
			if(cursor == 0)
				OnCursorUp(pScreen);
			break;

		case VK_DOWN  :     // �� ����Ű
			if(cursor == 0)
				OnCursorDown(pScreen);
			break;

		case VK_FX  :     	// XŰ
		    if(useStorage && (nCount > 0) && (pList[0].szName == ".."))
		    {
				nPosition = 0; // upper directory
				if(action(0) == UPDATE_FULL)
					OnDraw(pScreen);
			}
			break;

		case VK_LEFT  :     	// �� ����Ű
			if(cursor > 0)
			{
				--cursor;
				OnDraw(pScreen);
			}
			break;

		case VK_RIGHT :     // �� ����Ű
			if(cursor < 3)
				++cursor;
			else
			    cursor = 0;
			OnDraw(pScreen);
			break;

		case VK_TAT:
		case VK_FB:
			switch(action())
			{
				case UPDATE_FULL:
					OnDraw(pScreen);
					break;
				case DO_ACTION:
					OnExecute(pScreen, button);
					break;
			}
			break;

		case VK_FA:
			OnExecute(pScreen, button);
			break;

		case VK_START :
			theApp.menuMedia.close();
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
// Ŀ���� ���� �̵�
void FileList::OnCursorUp(SDL_Surface *pScreen)
{
	if(nCount == 0)	return;

	// �������� �̵��� �ϰ� �Ǹ� �� ���������� �̵��Ѵ�.
	if(nPosition <= 0)
	{
		nPosition = (nPosition-1+nCount) % nCount;
		nStartCount = nCount-FILELIST_COUNT;
		nEndCount = nCount;

		if(nStartCount < 0)
			nStartCount = 0;
	}
	else
	{
		nPosition--;		// ���� ��ĭ �̵�
		// ���� ��ġ�� �������� ������ �ƴϸ� �������� ������ �����Ѵ�.
		if(nPosition < nStartCount)
		{
			nStartCount--;
			nEndCount--;
		}
	}

	// �ٽ� �׸���.
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// Ŀ���� �Ʒ��� �̵�
void FileList::OnCursorDown(SDL_Surface *pScreen)
{
	if(nCount == 0)	return;

	if(nPosition >= nCount -1)
	{
		nPosition = 0;
		nStartCount = 0;
		nEndCount = nCount;

		if(nEndCount > FILELIST_COUNT)
			nEndCount = FILELIST_COUNT;
	}
	else
	{
		nPosition++;	// ��ĭ �Ʒ��� �̵�
		// ���� ��ġ�� �������� ������ �ƴϸ� �������� ������ �����Ѵ�.
		if(nPosition >= nEndCount)
		{
			nStartCount++;
			nEndCount++;
		}
	}

	// �ٽ� �׸���.
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
void FileList::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	int i, j, w, h, px, py;

	switch (waitMode)
	{
		case WAIT_ANY_BUTTON:
			OnDraw(pScreen);
			waitMode = WAIT_NONE;
			return;
		case WAIT_QUERY:
			for (i=0; i<2; i++)
			{
				if((x >= queryRect[i].x) && (x < (queryRect[i].x + queryRect[i].w)) &&
				    (y >= queryRect[i].y) && (y < (queryRect[i].y + queryRect[i].h)))
				{
					OnJoystickDown(pScreen, (i == 0) ? VK_FB : VK_FX);
				}
			}
			return;
	}

	w = pImgLine->w;
	h = pImgLine->h;
	for(i=nStartCount,j=0; i<nEndCount; i++,j++)
    {

		px = 18;
		py = 63+j*21;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			cursor = 0;
			nPosition = i;
			OnJoystickDown(pScreen, VK_FB);
			return;
		}
	}

	// ���丮�� �̹���(SD)
	w = pImgStorage->w / 2;
	h = pImgStorage->h / 2;
	px = XRES - w - 60;
	py = 4;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		cursor = 1;
		OnJoystickDown(pScreen, VK_FB);
		return;
	}

	// ���丮�� �̹���(EXT)
	px = px + w + 7;
	py = py;
	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		cursor = 2;
		OnJoystickDown(pScreen, VK_FB);
		return;
	}

	// X �̹��� ǥ�� (main �޴��� ��������)
	px = px + w + 10;
	w = pImgExit->w / 2;
	h = pImgExit->h + 10;
	py = 5;

	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		cursor = 3;
		OnJoystickDown(pScreen, VK_FB);
		return;
	}

	//ȭ��ǥ
	w = pImgArrow->w >> 1;
	h = (pImgArrow->h >> 1) + 15;
	px = 320 - w - 2;

	//������ ȭ��ǥ(����)
	if( (nStartCount != 0) && (pImgArrow != NULL) )
	{
		py = 67-15;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			cursor = 0;
			OnCursorUp(pScreen);
		}
	}

	//������ ȭ��ǥ(�Ʒ���)
	if( (nEndCount != nCount) && (pImgArrow != NULL) )
	{
		py = 67+7*21;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			cursor = 0;
			OnCursorDown(pScreen);
		}
	}
}
//[*]------------------------------------------------------------------------------------------[*]
int FileList::search(const std::string &path)
{
    int i = 0;
    int pos = 0;

	std::string szFullPath = "";
	struct dirent **namelist = NULL;
	struct stat statbuf;
	char current_path[PATH_MAX] = { 0,	};
	const char *group = (getMode() == PHOTO_VIEW) ? "photo" : NULL;
	const char *dir = path.c_str();

	if( (path.size() == 0) || (lstat(path.c_str(), &statbuf) != 0) || (!S_ISDIR(statbuf.st_mode)) ) {
	    fprintf(stderr, "stat fail path: %s\n", dir);
	    return -1;
	}

//	if( pList && (lastPath == path) )
//	    return nCount;

	getcwd(current_path, sizeof(current_path));

	if(!theApp.isSDMount() && (strncmp(dir, SD_PATH, strlen(SD_PATH)) == 0))
		theApp.Mount(MOUNT_SD);

#if defined(PND) || defined(WIZ)
	bool isRoot = (strcmp(dir, ROOT_PATH) == 0);
#else
	bool isRoot = (strcmp(dir, SD_PATH) == 0) || (strcmp(dir, EXT_PATH) == 0);
#endif

#ifndef WIN32
	if(chdir(dir) != 0) {
		fprintf(stderr, "chdir fail path: %s\n", dir);
		return -1;
	}
#else
	char working_path[PATH_MAX] = { 0,	};
	strcpy(working_path, current_path);
	strcat(working_path, "/");
	strcat(working_path, dir);

	if(chdir(working_path) != 0)
		return -1;

	szAbsolutePath = working_path;
#endif

#ifndef WIN32
	#define INFO_PATH lastPath.c_str()
#else
	#define INFO_PATH szAbsolutePath.c_str()
#endif

	if(pList)
	{
		delete [] pList;
		pList = NULL;
	}
	nCount = 0;
	lastPath = dir;

    nCount = scandir(INFO_PATH, &namelist, selectFunc, alphasort);

    if(nCount > 0)
    {
    	pList = new SFileList[nCount];

		// ���� ���� �����ϰ� ������ ���߿� �����Ѵ�.
		// (������ �̷��� ���ϸ� ���� ���� �� ������ �Ǿ ���� ���� �����Ƿ�...)
    	for(i=0,pos=0; i<nCount; i++)
    	{
			if(isRoot && (strcmp(namelist[i]->d_name, "..") == 0)) continue;

   			lstat(namelist[i]->d_name, &statbuf);
   			// ������ �����Ѵ�.
   			if( S_ISDIR(statbuf.st_mode) )
   			{
   				pList[pos].szName = namelist[i]->d_name;	// �̸��� �����Ѵ�.
	    		pList[pos].nAttribute = FOLDER_FORMAT; 	// �Ӽ��� �����Ѵ�.

				std::string mark_path = INFO_PATH;
				mark_path += "/" + pList[pos].szName + "/gp2xmark.ini";
				// workaround, fixme!!
	    		if( group && (pList[pos].szName != "..") && (access(mark_path.c_str(), R_OK) == 0) )
	    		{
					INI_Open(mark_path.c_str());
					pList[pos].nMarkedIndex = INI_ReadInt(group, "index", -1);
					pList[pos].nTotalCount = (pList[pos].nMarkedIndex < 0) ? -1 : INI_ReadInt(group, "total", -1);
					INI_Close();
/*
					printf("mark_path : %s\n", mark_path.c_str());
		    		printf("pList[%d].nMarkedIndex : %d\n", pos, pList[pos].nMarkedIndex);
		    		printf("pList[%d].nTotalCount  : %d\n", pos, pList[pos].nTotalCount);
		    		printf("\n");
*/
				} else {
					pList[pos].nMarkedIndex = -1;
					pList[pos].nTotalCount = -1;
				}

	    		pos++;
   			}
    	}

		// ���� ����
    	for(i=0; i<nCount; i++)
    	{
   			lstat(namelist[i]->d_name, &statbuf);
   			// �����̸� �����Ѵ�.
   			if(!S_ISDIR(statbuf.st_mode))
   			{
   				char *ext = NULL;
   				pList[pos].szName = namelist[i]->d_name;	// �̸��� �����Ѵ�.

   				ext = (char*)&(namelist[i]->d_name[strlen(namelist[i]->d_name)-4]);

				// ���� ����
				if(strcasecmp(".mp3", ext) == 0)		pList[pos].nAttribute = MP3_FORMAT;
				else if(strcasecmp(".ogg", ext) == 0)	pList[pos].nAttribute = OGG_FORMAT;
				else if(strcasecmp(".wav", ext) == 0)	pList[pos].nAttribute = WAV_FORMAT;
				// ��ȭ ����
				else if(strcasecmp(".avi", ext) == 0)	pList[pos].nAttribute = AVI_FORMAT;
				else if(strcasecmp(".ogm", ext) == 0)	pList[pos].nAttribute = OGM_FORMAT;
				// �̹��� ����
				else if(strcasecmp(".bmp", ext) == 0)	pList[pos].nAttribute = BMP_FORMAT;
				else if(strcasecmp(".gif", ext) == 0)	pList[pos].nAttribute = GIF_FORMAT;
				else if(strcasecmp(".png", ext) == 0)	pList[pos].nAttribute = PNG_FORMAT;
				else if(strcasecmp(".pcx", ext) == 0)	pList[pos].nAttribute = PCX_FORMAT;
				else if(strcasecmp(".jpg", ext) == 0)	pList[pos].nAttribute = JPG_FORMAT;
				// �ؽ�Ʈ ����
				else if(strcasecmp(".txt", ext) == 0)	pList[pos].nAttribute = TXT_FORMAT;
				// GP2X ���� ��������
				else if(strcasecmp(".gpe", ext) == 0)	pList[pos].nAttribute = GPE_FORMAT;
				// GP2X ���� ��ƿ����
				else if(strcasecmp(".gpu", ext) == 0)	pList[pos].nAttribute = GPU_FORMAT;
				// Ʈ��Ÿ�� ��Ʈ ����
				else if(strcasecmp(".ttf", ext) == 0)	pList[pos].nAttribute = TTF_FORMAT;
				// ini ����(������ӿ� ini����)
				else if(strcasecmp(".ini", ext) == 0)	pList[pos].nAttribute = INI_FORMAT;
				// ��Ÿ ����
				else									pList[pos].nAttribute = FILE_FORMAT;

				if(strlen(namelist[i]->d_name) == 7) {
					if(strcasecmp("COPYING", namelist[i]->d_name) == 0) pList[pos].nAttribute = TXT_FORMAT;
					if(strcasecmp("LICENSE", namelist[i]->d_name) == 0) pList[pos].nAttribute = TXT_FORMAT;
					if(strcasecmp("CREDITS", namelist[i]->d_name) == 0) pList[pos].nAttribute = TXT_FORMAT;
					if(strcasecmp("AUTHORS", namelist[i]->d_name) == 0) pList[pos].nAttribute = TXT_FORMAT;
				}

				pList[pos].nMarkedIndex = -1;
				pList[pos].nTotalCount = -1;
	    		pos++;
   			}
    	}

		for(i=0; i<nCount; i++)
        	free(namelist[i]);
        free(namelist);

        nCount = pos;
    }

	// ���⼭ ���� ��η� ����
#ifdef GP2X
	chdir("/usr/gp2x");
#else
	chdir(current_path);
#endif

    return nCount;
#undef INFO_PATH
}
//[*]------------------------------------------------------------------------------------------[*]
