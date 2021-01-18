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
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"        // Main Class
//[*]------------------------------------------------------------------------------------------[*]
CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
// main ÇÔ¼ö
int main(int argc, char *argv[])
{
	int i;
	ViewMode view = MAIN_VIEW;
	SDL_Surface *screenSurface = NULL;
	bool run = theApp.cfgAutoRun;

#ifdef GP2XF200
	printf("GP2X-F200 Mode...\n");
#endif

	for(i=1;i<argc;i++) {
		if(strcmp(argv[i], "--disable-autorun") == 0) {
			run = false;
		} else if(strcmp(argv[i], "--force-autorun") == 0) {
			run = true;
		} else if(strcmp(argv[i], "--enable-terminate") == 0) {
			theApp.enableTerminate = true;
		} else if(strcmp(argv[i], "--view-main") == 0) {
			view = MAIN_VIEW;
		} else if(strcmp(argv[i], "--view-game") == 0) {
			view = GAME_VIEW;
		} else if(strcmp(argv[i], "--view-music") == 0) {
			view = MUSIC_VIEW;
		} else if(strcmp(argv[i], "--view-photo") == 0) {
			view = PHOTO_VIEW;
		} else if(strcmp(argv[i], "--view-ebook") == 0) {
			view = EBOOK_VIEW;
		} else if(strcmp(argv[i], "--view-setting") == 0) {
			view = SETTING_VIEW;
		} else if(strcmp(argv[i], "--view-system") == 0) {
			view = SYSTEM_VIEW;
		} else if(strcmp(argv[i], "--boot") == 0) {
			theApp.bootFlag = true;
		}
	}

#ifdef GP2X
	struct stat info;

	// if removed temp directory...
	if( (lstat("/mnt/tmp", &info) != 0) || (!S_ISDIR(info.st_mode)) ) {
		system("rm -rf /mnt/tmp");
		system("mkdir -p /mnt/tmp");
	}
	// clear temp directory...
	system("rm -rf /mnt/tmp/*");
#endif

	theApp.Mount(MOUNT_SD);
	if (theApp.isSDMount())
	{
		std::string cmd;
		bool update = false;
		
		if(theApp.bootFlag && (access(SD_PATH"/gp2xupdate.gpu", X_OK) == 0))
		{
			cmd = SD_PATH"/gp2xupdate.gpu";
			update = true;
		}

		if(run && (cmd.size() == 0) && (access(SD_PATH"/autorun.ini", R_OK) == 0))
		{
			// autorun.ini
			INI_Open(SD_PATH"/autorun.ini");
			bool autorun = INI_ReadBool("autorun", "enable", true);
			bool runonce = INI_ReadBool("autorun", "runonce", true);
			if(autorun && (!runonce || theApp.bootFlag))
				cmd = INI_ReadText("info", "path", "");
			INI_Close();
			cmd = SD_PATH"/" + cmd;
			if(access(cmd.c_str(), X_OK) != 0)
			    cmd.clear();
		}
		
		if(run && theApp.bootFlag && (cmd.size() == 0) && (access(SD_PATH"/autorun.gpu", X_OK) == 0))
		{
			cmd = SD_PATH"/autorun.gpu";
		}

		if((cmd.size() > 0) && theApp.bootFlag)
		{
#ifdef GP2X
			screenSurface = theApp.SDL_Initialize();
			system("/root/start.sh boot");
			if(update) {
				theApp.SetView(UPDATE_VIEW);
				theApp.MsgProc();
				if(theApp.updateCancel) cmd.clear();
			}
#endif
		}

		if(cmd.size() > 0)
		{
			// GP2X Update or AutoRun start...
			if(screenSurface != NULL)
				theApp.SDL_Exitialize();
#ifndef WIN32
			sync();
			std::string path = cmd.substr(0, cmd.find_last_of("/\\"));
			if(chdir(path.c_str()) != 0)
				chdir(SD_PATH);
			execlp(cmd.c_str(), cmd.c_str(), NULL, 0);
#endif
		}
	}

#ifdef GP2X
	char path[4096];
	if(realpath("/proc/self/exe", path) != NULL) {
		char *p = strrchr(path, '/');
		if(p != NULL) *p = '\0';
		chdir(path);
	}
#endif

	if(screenSurface == NULL)
		screenSurface = theApp.SDL_Initialize();
	theApp.SkinLoad();
	theApp.defaultView = view;
	if(view != MAIN_VIEW)
		theApp.bootFlag = false;

//	theApp.SetView(UPDATE_VIEW);
//	theApp.MsgProc();

	theApp.SetView(view);
	theApp.MsgProc();

	theApp.SDL_Exitialize();
	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
