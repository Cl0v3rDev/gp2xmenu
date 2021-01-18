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

	    	// 자신의 위치를 찾아서 이동한다.
	    	int i;
	    	for(i=0; i<nCount; i++)
	    	{
				if(lastName  == pList[i].szName)
				{
					//여기서 포지션과 파일 보여지는 영역을 같이 이동 시켜야 한다.
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
		// 스토리지 이미지 표시
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

	// X 이미지 표시 (main 메뉴로 빠져나감)
	srcrect.w = pImgExit->w / 2;
	srcrect.h = pImgExit->h;

	srcrect.x = (cursor == 3) ? srcrect.w : 0;
	srcrect.y = 0;
	dstrect.x = XRES - srcrect.w - 5;
	dstrect.y = 5;
	SDL_BlitSurface(pImgExit, &srcrect, pScreen, &dstrect);

    // 위쪽 화살표 표시
    if( (nStartCount != 0) && (pImgArrow != NULL) )
	{
		srcrect.w = pImgArrow->w >> 1;
		srcrect.h = pImgArrow->h >> 1;
		srcrect.x = srcrect.y = 0;
		dstrect.x = 320 - srcrect.w - 2;
		dstrect.y = 67;

		SDL_BlitSurface(pImgArrow, &srcrect, pScreen, &dstrect);
    }

    // 아래쪽 화살표 표시
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

	// UTF-8을 유니코드로 변경한다.
    for(i=nStartCount,j=0; i<nEndCount; i++,j++)
    {
		// 현재 선택된 파일은 라인을 그린다.
		if(nPosition == i && pImgLine != NULL && cursor==0)
		{
			dstrect.x = 38;
			dstrect.y = 63+j*21;

			SDL_BlitSurface(pImgLine, NULL, pScreen, &dstrect);
		}

    	// 유니코드용 저장 버퍼
    	unsigned short dest[512];
    	char name[256];

    	if(pList[i].nAttribute == INI_FORMAT) //ini파일 이라면
		{
			char gameName[512]={0,};  //게임 타이틀명
			char iconPath[512]={0,};  //자체 아이콘 경로
			SDL_Surface *pImgIcon_GPE = NULL; //자체 아이콘

			INI_Open( (lastPath + "/" + pList[i].szName).c_str() );
			strcpy(gameName, INI_ReadText("info", "name", pList[i].szName.c_str()) );
			strcpy(iconPath, INI_ReadText("info", "icon", "") );
			INI_Close();

   			toUnicode("UTF-8", gameName, dest, strlen(gameName));

			srcrect.w = 20;
			srcrect.h = 15;
			dstrect.x = 18;
			dstrect.y = 65+j*21;
			// 자체 아이콘이 있으면 선택 한다.

			if(access(iconPath, F_OK) == 0)
			{
					pImgIcon_GPE = IMG_Load(iconPath);
					srcrect.x = 0;
					srcrect.y = 0;

					SDL_BlitSurface(pImgIcon_GPE, &srcrect, pScreen, &dstrect);

					SDL_SAFE_FREE(pImgIcon_GPE);
			}
			else //자체 아이콘이 없다(기본 게임 아이콘 사용)
			{

				srcrect.x = 1 * srcrect.w;
				srcrect.y = 4 * srcrect.h;

				SDL_BlitSurface(pImgIcon, &srcrect, pScreen, &dstrect);

			}
		}
    	else	//gpe,gpu,txt,mp3 등의 파일 이라면
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
    		// 파일명을 유니코드로 변경
#ifdef WIN32
	   		toUnicode(NULL, name, dest, strlen(name));
#else
	   		toUnicode("UTF-8", name, dest, strlen(name));
#endif

    		if( (pList[i].nAttribute == GPE_FORMAT) || (pList[i].nAttribute == GPU_FORMAT) )//gpe,gpu 파일 이라면
    		{
	    		std::string szPngName = lastPath + "/" + pList[i].szName;		// FullPath
				int lenPngName   = strlen(szPngName.c_str());										// FullPath Length
				// png 파일을 구해야 하므로 마지막 3자리를 바꾼어 준다.
				szPngName = szPngName.substr(0, lenPngName-4) + ".png";								// ext png FullPath
			    SDL_Surface *pImgIcon_GPE = NULL; //자체 아이콘

			    srcrect.w = 20;
				srcrect.h = 15;
			    dstrect.x = 18;
				dstrect.y = 65+j*21;
			    // png파일이 있으면 그걸 표시한다.
			    if(access(szPngName.c_str(), F_OK) == 0)
			    {
					pImgIcon_GPE = IMG_Load(szPngName.c_str());
					srcrect.x = 0;
					srcrect.y = 0;

			    	SDL_BlitSurface(pImgIcon_GPE, &srcrect, pScreen, &dstrect);

			    	SDL_SAFE_FREE(pImgIcon_GPE);
			    }
				else //자체 아이콘이 없다(기본 게임 아이콘 사용)
				{
					srcrect.x = 1 * srcrect.w;
					srcrect.y = 4 * srcrect.h;

					SDL_BlitSurface(pImgIcon, &srcrect, pScreen, &dstrect);
				}


    		}
    		else //txt,mp3,img 등등의 파일 이라면
			{
    	   		// 속성을 확인해서 아이콘을 선택한다.
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

			// 음악이나 동영상 재생중이라면 미디어 재생기로 Jump
			if(theApp.menuMedia.getStatus(MEDIA_PLAY_STATUS) != MEDIA_STOP)
				theApp.SetView(MEDIA_PLAY_VIEW);
			else
				theApp.SetView(MAIN_VIEW);
			break;
	}
	return result;
}
//[*]------------------------------------------------------------------------------------------[*]
// 조이스틱 다운 처리
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
		case VK_UP 	  : 	// 상 방향키
			if(cursor == 0)
				OnCursorUp(pScreen);
			break;

		case VK_DOWN  :     // 하 방향키
			if(cursor == 0)
				OnCursorDown(pScreen);
			break;

		case VK_FX  :     	// X키
		    if(useStorage && (nCount > 0) && (pList[0].szName == ".."))
		    {
				nPosition = 0; // upper directory
				if(action(0) == UPDATE_FULL)
					OnDraw(pScreen);
			}
			break;

		case VK_LEFT  :     	// 좌 방향키
			if(cursor > 0)
			{
				--cursor;
				OnDraw(pScreen);
			}
			break;

		case VK_RIGHT :     // 우 방향키
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
// 커서를 위로 이동
void FileList::OnCursorUp(SDL_Surface *pScreen)
{
	if(nCount == 0)	return;

	// 맨위에서 이동을 하게 되면 맨 마지막으로 이동한다.
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
		nPosition--;		// 위로 한칸 이동
		// 현재 위치가 보여지는 영역이 아니면 보여지는 영역을 조정한다.
		if(nPosition < nStartCount)
		{
			nStartCount--;
			nEndCount--;
		}
	}

	// 다시 그린다.
	OnDraw(pScreen);
}
//[*]------------------------------------------------------------------------------------------[*]
// 커서를 아래로 이동
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
		nPosition++;	// 한칸 아래로 이동
		// 현재 위치가 보여지는 영역이 아니면 보여지는 영역을 조정한다.
		if(nPosition >= nEndCount)
		{
			nStartCount++;
			nEndCount++;
		}
	}

	// 다시 그린다.
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

	// 스토리지 이미지(SD)
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

	// 스토리지 이미지(EXT)
	px = px + w + 7;
	py = py;
	if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
	{
		cursor = 2;
		OnJoystickDown(pScreen, VK_FB);
		return;
	}

	// X 이미지 표시 (main 메뉴로 빠져나감)
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

	//화살표
	w = pImgArrow->w >> 1;
	h = (pImgArrow->h >> 1) + 15;
	px = 320 - w - 2;

	//우측에 화살표(위쪽)
	if( (nStartCount != 0) && (pImgArrow != NULL) )
	{
		py = 67-15;

		if((x >= px) && (x < (px+w)) && (y >= py) && (y < (py+h)))
		{
			cursor = 0;
			OnCursorUp(pScreen);
		}
	}

	//우측에 화살표(아래쪽)
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

		// 폴더 먼저 생성하고 파일은 나중에 생성한다.
		// (이유는 이렇게 안하면 파일 폴더 로 정리가 되어서 나올 수도 있으므로...)
    	for(i=0,pos=0; i<nCount; i++)
    	{
			if(isRoot && (strcmp(namelist[i]->d_name, "..") == 0)) continue;

   			lstat(namelist[i]->d_name, &statbuf);
   			// 폴더면 저장한다.
   			if( S_ISDIR(statbuf.st_mode) )
   			{
   				pList[pos].szName = namelist[i]->d_name;	// 이름을 저장한다.
	    		pList[pos].nAttribute = FOLDER_FORMAT; 	// 속성을 설정한다.

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

		// 파일 저장
    	for(i=0; i<nCount; i++)
    	{
   			lstat(namelist[i]->d_name, &statbuf);
   			// 파일이면 저장한다.
   			if(!S_ISDIR(statbuf.st_mode))
   			{
   				char *ext = NULL;
   				pList[pos].szName = namelist[i]->d_name;	// 이름을 저장한다.

   				ext = (char*)&(namelist[i]->d_name[strlen(namelist[i]->d_name)-4]);

				// 음악 파일
				if(strcasecmp(".mp3", ext) == 0)		pList[pos].nAttribute = MP3_FORMAT;
				else if(strcasecmp(".ogg", ext) == 0)	pList[pos].nAttribute = OGG_FORMAT;
				else if(strcasecmp(".wav", ext) == 0)	pList[pos].nAttribute = WAV_FORMAT;
				// 영화 파일
				else if(strcasecmp(".avi", ext) == 0)	pList[pos].nAttribute = AVI_FORMAT;
				else if(strcasecmp(".ogm", ext) == 0)	pList[pos].nAttribute = OGM_FORMAT;
				// 이미지 파일
				else if(strcasecmp(".bmp", ext) == 0)	pList[pos].nAttribute = BMP_FORMAT;
				else if(strcasecmp(".gif", ext) == 0)	pList[pos].nAttribute = GIF_FORMAT;
				else if(strcasecmp(".png", ext) == 0)	pList[pos].nAttribute = PNG_FORMAT;
				else if(strcasecmp(".pcx", ext) == 0)	pList[pos].nAttribute = PCX_FORMAT;
				else if(strcasecmp(".jpg", ext) == 0)	pList[pos].nAttribute = JPG_FORMAT;
				// 텍스트 파일
				else if(strcasecmp(".txt", ext) == 0)	pList[pos].nAttribute = TXT_FORMAT;
				// GP2X 게임 실행파일
				else if(strcasecmp(".gpe", ext) == 0)	pList[pos].nAttribute = GPE_FORMAT;
				// GP2X 게임 유틸파일
				else if(strcasecmp(".gpu", ext) == 0)	pList[pos].nAttribute = GPU_FORMAT;
				// 트루타입 폰트 파일
				else if(strcasecmp(".ttf", ext) == 0)	pList[pos].nAttribute = TTF_FORMAT;
				// ini 파일(내장게임용 ini파일)
				else if(strcasecmp(".ini", ext) == 0)	pList[pos].nAttribute = INI_FORMAT;
				// 기타 파일
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

	// 여기서 이전 경로로 변경
#ifdef GP2X
	chdir("/usr/gp2x");
#else
	chdir(current_path);
#endif

    return nCount;
#undef INFO_PATH
}
//[*]------------------------------------------------------------------------------------------[*]
