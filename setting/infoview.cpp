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
#include <string>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "infoview.h"
#include "textout.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
//[*]------------------------------------------------------------------------------------------[*]
CInfomationView::CInfomationView()
{
	pImgBackGround = NULL;
}
//[*]------------------------------------------------------------------------------------------[*]
CInfomationView::~CInfomationView()
{
	Exitialize();
}
//[*]------------------------------------------------------------------------------------------[*]
// �ʱ�ȭ �Լ�
void CInfomationView::Initialize(bool refresh)
{
	pImgBackGround = theApp.LoadImage("info", "body", "INFORMATION");
}
//[*]------------------------------------------------------------------------------------------[*]
// ���� �Լ�
void CInfomationView::Exitialize(bool refresh)
{
	SDL_SAFE_FREE(pImgBackGround);
}
//[*]------------------------------------------------------------------------------------------[*]
// Draw �Լ�
void CInfomationView::OnDraw(SDL_Surface *pScreen)
{
	int i = 0;
	unsigned char ch;
	char szdate[35]		= { 0, };
	char szBuffer[34]	= {	0, };
	unsigned short szMark[2];
	int r, g, b;
	FILE *stream = NULL;

	// ȭ���� �Ͼ�� ���
	SDL_BlitSurface(pImgBackGround, NULL, pScreen, NULL);

	theApp.FontColor("info", "text", r, g, b);

	if(theApp.cfgLanguage == "_kor")
	{
		PrintKorMessage(pScreen, 54, 34, "����"     , r, g, b);
		PrintKorMessage(pScreen, 54, 54, "��"     , r, g, b);
		PrintKorMessage(pScreen, 54, 74, "�ø���"   , r, g, b);
	}
	else
	{
		PrintMessage(pScreen,	  54,  34, "VERSION"	, r, g, b);	// ����
		PrintMessage(pScreen, 	  54,  54, "MODEL"		, r, g, b);	// ��
		PrintMessage(pScreen, 	  54,  74, "S/N"		, r, g, b);	// �ø���
	}

	// �߿��� ���� ���
	PrintMessage(pScreen, 109, 34, VERSION, r, g, b);
#ifdef GP2XF200
	PrintMessage(pScreen, 109, 54, "GP2X - F200", r, g, b);
#else
	PrintMessage(pScreen, 109, 54, "GP2X - F100", r, g, b);
#endif

	// �� 0x24D2

	// �� 2004-2006 Gamepark Holdings Co,Ltd. All rights
	// reserved. Gamepark Holdings and the Gamepark
	// Holdings logo are trademarks of Gamepark Holdings
	// Co,Ltd., registered in Korea and other countries.
	// GP2X and GP2X logo are trademarks of Gamepark
	// Holdings Co,Ltd.

	// �� 2004-2008 �߰�����ũȦ����.
	// Gamepark Holdings(������ũȦ����) �� GPH �ΰ��
	// ���ѹα� �� ������ �������� ������ ��ȣ�ް� �ִ�
	// �߰�����ũ Ȧ������ ��ϻ�ǥ�Դϴ�.
	// GP2X �� GP2X �ΰ�� �߰�����ũ Ȧ������ ��ϻ�ǥ
	// �Դϴ�.

	szMark[0] = 0x24D2;
	szMark[1] = '\0';
	DrawText(pScreen, 15, 114, szMark, -1, r, g, b);
	
	if(theApp.cfgLanguage == "_kor")
	{
		PrintKorMessage(pScreen, 27, 114, " 2004-2008 �߰�����ũȦ����."                       , r, g, b);
		PrintKorMessage(pScreen, 15, 129, "Gamepark Holdings(������ũȦ����) �� GPH �ΰ��"    , r, g, b);
		PrintKorMessage(pScreen, 15, 144, "���ѹα� �� ������ �������� ������ ��ȣ�ް� �ִ�"   , r, g, b);
		PrintKorMessage(pScreen, 15, 159, "�߰�����ũ Ȧ������ ��ϻ�ǥ�Դϴ�."                , r, g, b);
		PrintKorMessage(pScreen, 15, 174, "GP2X �� GP2X �ΰ�� �߰�����ũ Ȧ������ ��ϻ�ǥ"   , r, g, b);
		PrintKorMessage(pScreen, 15, 189, "�Դϴ�."                                            , r, g, b);
	}
	else
	{
		PrintMessage(pScreen, 27, 114, " 2004-2008 Gamepark Holdings Co,Ltd. All rights"	, r, g, b);
		PrintMessage(pScreen, 15, 129, "reserved. Gamepark Holdings and the Gamepark"		, r, g, b);
		PrintMessage(pScreen, 15, 144, "Holdings logo are trademarks of Gamepark Holdings"	, r, g, b);
		PrintMessage(pScreen, 15, 159, "Co,Ltd., registered in Korea and other countries."	, r, g, b);
		PrintMessage(pScreen, 15, 174, "GP2X and GP2X logo are trademarks of Gamepark"		, r, g, b);
		PrintMessage(pScreen, 15, 189, "Holdings Co,Ltd."									, r, g, b);
	}

	// �ø��� ������ �����ִ� �κ�

	theApp.ReadSerial((unsigned char*)szBuffer);

	szdate[ 0] = szBuffer[ 0];	// ��
	szdate[ 1] = szBuffer[ 1];
	szdate[ 2] = szBuffer[ 2];
	szdate[ 3] = szBuffer[ 3];
	szdate[ 4] = '.';
	szdate[ 5] = szBuffer[ 4];	// ��
	szdate[ 6] = szBuffer[ 5];
	szdate[ 7] = '.';
	szdate[ 8] = szBuffer[ 6];	// ��
	szdate[ 9] = szBuffer[ 7];
	szdate[10] = ' ';
	szdate[11] = '-';
	szdate[12] = ' ';
	szdate[13] = szBuffer[ 8];	// G
	szdate[14] = szBuffer[ 9];	// P
	szdate[15] = szBuffer[10];	// 2
	szdate[16] = szBuffer[11];	// X
	szdate[17] = szBuffer[12];	// V
	szdate[18] = szBuffer[13];	// 1
	szdate[19] = szBuffer[14];	// 2
	szdate[20] = szBuffer[15];	// 3
	szdate[21] = ' ';
	szdate[22] = '-';
	szdate[23] = ' ';
	szdate[24] = szBuffer[16];	// 1
	szdate[25] = szBuffer[17];	// 2
	szdate[26] = szBuffer[18];	// 3
	szdate[27] = szBuffer[19];	// 4
	szdate[28] = szBuffer[20];	// 5
	szdate[29] = szBuffer[21];	// 6
	szdate[30] = szBuffer[22];	// 7
	szdate[31] = szBuffer[23];	// 8
	szdate[32] = '\0';

	PrintMessage(pScreen, 109, 74, szdate, r, g, b);

	//���͸� ������ �ε� ����
	SDL_Rect rect;
	theApp.getStatusRect(&rect);
	theApp.drawStatusIcon(pScreen, &rect);

	SDL_UpdateRect(pScreen, 0, 0, 0, 0);		// ȭ�� ������Ʈ
}
//[*]------------------------------------------------------------------------------------------[*]
void CInfomationView::OnJoystickDown(SDL_Surface *pScreen, Uint8 button)
{
	switch(button)
	{
		case VK_FX :	// X ��ư 	// ���� �޴��� View�� �̵��Ѵ�.
			theApp.SwapReverse();
			theApp.SetView(SETTING_VIEW);
			break;
		case VK_START :	// X ��ư 	// Main View�� �̵��Ѵ�.
			theApp.SwapReverse();
			theApp.SetView(MAIN_VIEW);
			break;
	}
}
//[*]------------------------------------------------------------------------------------------[*]
void CInfomationView::OnPenDown(SDL_Surface *pScreen, int x, int y)
{
	theApp.SwapReverse();
	theApp.SetView(SETTING_VIEW);
}
//[*]------------------------------------------------------------------------------------------[*]
