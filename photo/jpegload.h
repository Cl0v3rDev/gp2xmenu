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

#ifndef _JPEG_LOAD_H_
#define _JPEG_LOAD_H_

#include "fileload.h"

class CJpegLoad : public CFileLoad
{
	private :

    public:
		bool Load(const char *szPath, int buf);
		bool FullLoad(const char *szPath);
		bool ComicLoad(const char *szPath, int buf);
};
//[*]------------------------------------------------------------------------------------------[*]

#endif // _JPEG_LOAD_H_
