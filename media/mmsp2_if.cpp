/*
    GP2X MMSP2 Controler
    Copyright (C) 2004 godori <ghcstop>, www.aesop-embedded.org
	Copyright (C) 2004 Magiceyes. <www.mesdigital.com>
	Copyright (C) 2005 DIGNSYS Inc. <www.dignsys.com>
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
#include "../config.h"
//[*]------------------------------------------------------------------------------------------[*]
// Scale-Down -> Screen Shake  [ use path : FDC -> 1D-MEM -> MLC -> DPC ]
//[*]------------------------------------------------------------------------------------------[*]
#ifdef GP2X
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#endif
#include <string.h>
//[*]------------------------------------------------------------------------------------------[*]
#include "mainmenuapp.h"
#include "mmsp2_if.h"
#include "typed.h"
//[*]------------------------------------------------------------------------------------------[*]
extern CMainMenuApp theApp;
// device open & close
static int mem_fd = -1;			// /dev/mem fd
static int mp4_fd = -1;			// dualcpu driver fd
static int vpp_fd = -1;			// vpp driver fd
static int init_flag = 0;		// arm940 setup init flag
static int frame_decode_count;	// hhsong add.. when fill 3 frames because of three swap buffer, YUVA layer on...
static int movie_width = 0;
static int movie_height = 0;
static int current_mode = 0;
static int disp_mode = LCD_MODE;
static int resize_mode = 0;
//[*]------------------------------------------------------------------------------------------[*]
unsigned char *dbuf;		// compressed frame data buffer
MP4D_DISPLAY_PARAM mp4d_disp;
//[*]------------------------------------------------------------------------------------------[*]
static MP4D_INIT_PARAM mp4d_init;	// 940 init for mpeg4 decoder(aka divx)
static MP4D_INIT_BUF_PARAM mp4d_init_buf;	// setup 940 interface 2D memory
static MP4D_RUN_PARAM mp4d_run;	// 940 video decode run command
static MP4D_REL_PARAM mp4d_rel;	// release 940 mpeg4 decoder setting
//[*]------------------------------------------------------------------------------------------[*]
static void *arm940_mem_start = NULL;	// 940 decode interfacing memory, 0x0200_0000(32M start)
//[*]------------------------------------------------------------------------------------------[*]
static unsigned char *rl_buf = NULL;	// run length decoding buffer
static unsigned short y_offset[6];	// decode된 data가 들어갈 buffer, 끝의 2개는 b frame용으로 세팅함
static unsigned short cb_offset[6];
static unsigned short cr_offset[6];
//[*]------------------------------------------------------------------------------------------[*]
static FDC_INFO fdc_info;
static YUV_INFO yuv_info;
static SC_INFO sc_info;
static FDC_RUN_INFO fri;
//[*]------------------------------------------------------------------------------------------[*]
#define CODEC_BUFF_ADDR 0x02000000
//[*]------------------------------------------------------------------------------------------[*]
#define PATH_DEV_DUALCPU              "/dev/dualcpu"
#define PATH_DEV_DUALCPU_WITH_DEVFS   "/dev/misc/dualcpu"
#define PATH_DEV_VPP                  "/dev/vpp"
#define PATH_DEV_VPP_WITH_DEVFS       "/dev/misc/vpp"
//[*]------------------------------------------------------------------------------------------[*]
#define MAX_WIDTH  1024
#define MAX_HEIGHT 512
#define LCD_WIDTH 	320
#define LCD_HEIGHT 	240
#define TV_NTSC_WIDTH 	720
#define TV_NTSC_HEIGHT 	480
#define TV_NTSC_ENC_WIDTH 	670
#define TV_NTSC_ENC_HEIGHT 	(232 * 2)
#define TV_PAL_WIDTH 	720
#define TV_PAL_HEIGHT 	576
#define TV_PAL_ENC_WIDTH 	670
#define TV_PAL_ENC_HEIGHT 	(280 * 2)
//[*]------------------------------------------------------------------------------------------[*]
#define MP4D_CODEC_INFO_SIZE 10
//[*]------------------------------------------------------------------------------------------[*]
#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                \
	( (unsigned int)(char)(ch0) | ( (unsigned int)(char)(ch1) << 8 ) |  \
	( (unsigned int)(char)(ch2) << 16 ) | ( (unsigned int)(char)(ch3) << 24 ) )
#endif
//[*]------------------------------------------------------------------------------------------[*]
const struct mp4d_codec_type_t {
	unsigned int format;
	short number;
} CodecInfo[MP4D_CODEC_INFO_SIZE] = {
	{mmioFOURCC('D', 'I', 'V', '3'), CODEC_VER_DIVX_311},	// Divx3.11
	{mmioFOURCC('d', 'i', 'v', '3'), CODEC_VER_DIVX_311},
	{mmioFOURCC('D', 'I', 'V', 'X'), CODEC_VER_DIVX_500},	// Divx4.x
	{mmioFOURCC('d', 'i', 'v', 'x'), CODEC_VER_DIVX_500},	// Divx4.x
	{mmioFOURCC('D', 'X', '5', '0'), CODEC_VER_DIVX_500},	// Divx5.x
	{mmioFOURCC('d', 'x', '5', '0'), CODEC_VER_DIVX_500},	// Divx5.x
	{mmioFOURCC('X', 'V', 'I', 'D'), CODEC_VER_XVID},	// Xvid
	{mmioFOURCC('x', 'v', 'i', 'd'), CODEC_VER_XVID},	// Xvid
	{mmioFOURCC('F', 'F', 'D', 'S'), CODEC_VER_DIVX_500},	// Divx5.x
	{mmioFOURCC('f', 'f', 'd', 's'), CODEC_VER_DIVX_500},	// Divx5.x
};
//[*]------------------------------------------------------------------------------------------[*]
inline int read_mp4d_940_status(void);
//[*]------------------------------------------------------------------------------------------[*]
int mmsp2_940_interface_buffer_init(void)
{
#ifdef GP2X
	unsigned char *addr;

	// /dev/mem을 사용하기 위해 open
	mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (mem_fd < 0) {
		printf("open /dev/mem failed\n");
		return -1;
	}

	arm940_mem_start = mmap(0, 0x01000000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, CODEC_BUFF_ADDR);
	if (arm940_mem_start == NULL) {
		close(mem_fd);
		mem_fd = -1;
		printf("/dev/mem mmap() failed\n");
		return -1;
	}

	dbuf = (unsigned char *) arm940_mem_start;	// : offset서부터 0.5M
	rl_buf = (unsigned char *) arm940_mem_start + 0x280000;	// 2.5M offset,

	y_offset[0] = 0x0240;	// screen offset(memory offset): 0x02(offset 32M), y좌표는 4, x좌표는 0 ~ 3까지의 영역이므로 256 x 4 = 1024, 즉, x는 1024를 넘지 못한다.
	cb_offset[0] = 0x0244;	// U의 좌표, x좌표가 4 ~ 5를 잡아먹음
	cr_offset[0] = 0x0246;
	y_offset[1] = 0x0260;	// 첫번째 프레임과 y좌표가 2밖에 차이 안나므로, height는 512이상을 넘으면 안됨
	cb_offset[1] = 0x0264;
	cr_offset[1] = 0x0266;
	y_offset[2] = 0x0280;
	cb_offset[2] = 0x0284;
	cr_offset[2] = 0x0286;
	y_offset[3] = 0x02A0;
	cb_offset[3] = 0x02A4;
	cr_offset[3] = 0x02A6;
	y_offset[4] = 0x02C0;
	cb_offset[4] = 0x02C6;
	cr_offset[4] = 0x02C8;
	y_offset[5] = 0x0248;
	cb_offset[5] = 0x024C;
	cr_offset[5] = 0x024E;

	addr = (unsigned char *) arm940_mem_start + 0x00400000;	// 4M offset = 36M서부터 즉, YUV의 databuffer의 시작지점서부터
	memset(addr, 0x10, 0x200000);	// 2M를 0x10으로 채운다. 근데 머하는짓이냐? 쓸따리없이.
	addr = (unsigned char *) arm940_mem_start;	// 0x0200_0000(시스템 메모리의 32M offset)
	memset(addr, 0, 0x200000);	// 2M를 지운다...머하는 짓인감?, encoded frame data buffer clear하는 것이군.
#endif
	return 1;
}
//[*]------------------------------------------------------------------------------------------[*]
// main에서 vformat_fourcc를 찾아서 format을 건네주면 함수내에서 비교해서 찾는 루틴 넣을 것
// vformat_fourcc를 mmioFOURCC( compressor[0], compressor[1], compressor[2], compressor[3])해서 입력을 해줄 것
int mmsp2_setup_940_interface_buffer(int width, int height, unsigned int vformat_fourcc)
{
#ifdef GP2X
	int j;
	unsigned int rlbuf_ptr;

	if ((width > MAX_WIDTH) || (height > MAX_HEIGHT)) {
		printf("width and height are not supported\n");
		return -1;
	}

	mp4d_init.Command = MP_CMD_INIT;
	mp4d_init.CodecType = MP_CODEC_MP4D;
	mp4d_init.Width = width;
	mp4d_init.Height = height;
	// codec type을 입력된 format을 가지고 이미 정의된 넘에서 fourcc를 통해 찾는다.
	for (j = 0; j < MP4D_CODEC_INFO_SIZE; j++) {
		if (CodecInfo[j].format == vformat_fourcc) {
			printf("codec found = %d\n", j);
			break;
		}
	}

	if (j == MP4D_CODEC_INFO_SIZE) {
		// 미스콜이아 : 수정
		// 여기서 10개의 코덱중에 없으면 지원하지 않는 코덱이다.
		// 여기서 표시를 하고 리턴 값으로 검사 해야 될 것 같다.
		printf("AVI compressor(fourcc) is not supported in current mmsp2 firmware!\n");
		return -1;
	}
	//  : vformat_fourcc가 가리키는대로 codec버전을 찾아서 넣어줘야 한다.
	mp4d_init.CodecVersion = CodecInfo[j].number;

	if (mp4d_init.CodecVersion == CODEC_VER_DIVX_311)
		mp4d_init.CodecType = MP_CODEC_MP4D_DIVX3;

	mp4d_init.non_bframe_max = 4;
	mp4d_init.b_frame_max = 2;

	rlbuf_ptr = (unsigned long)rl_buf - (unsigned long)arm940_mem_start + CODEC_BUFF_ADDR;

	// run length code용 버퍼는 1D address형태로 지정한다.
	mp4d_init.RL_L = (unsigned short) (rlbuf_ptr & 0x0000FFFF);
	mp4d_init.RL_H = (unsigned short) (rlbuf_ptr >> 16);
	mp4d_init.RL_Length = 512;	// KB

	// macro block parameter와 quantization table용 버퍼는 2D 스타일의 offset을 지정한다.
	mp4d_init.mbp_a_addr_offset = 0x02F0;	// 47M서부터
	mp4d_init.mbp_b_addr_offset = 0x02E0;	// 46M서부터
	mp4d_init.qmat_addr_h = 0x0220;	// 34M서부터 즉, rlbuf_ptr앞의 0.5M 부분
	mp4d_init.qmat_addr_l = 0x0000;	// 근데 이건 왜 0이야?
	mp4d_init.pred_q_lb_offset = 0x0230;	// 35M서부터
	mp4d_init.arm940_offset = 0x0300;	// 0x0300_0000, 940 firmware가 올라가는 address

	for (j = 0; j < 10; j++) {
		if (ioctl(mp4_fd, IOCTL_DUALCPU_MP4D_INIT, &mp4d_init) == 1)
			break;

		printf("reopen dualcpu...\n");
		usleep(300000);	// wait 0.3 sec.
		close(mp4_fd);

		mp4_fd = open(PATH_DEV_DUALCPU, O_RDWR);
		if (mp4_fd < 0) {
			mp4_fd = open(PATH_DEV_DUALCPU_WITH_DEVFS, O_RDWR);
			if (mp4_fd < 0) {
				printf("IOCTL_DUALCPU_MP4D_INIT failed\n");
				return -1;
			}
		}
	}

	if (j == 10) {
		printf("IOCTL_DUALCPU_MP4D_INIT failed\n");
		return -1;
	}

	if (read_mp4d_940_status() < 0) {
		printf("MP_CMD_INIT failed => goto quit\n");
		return -1;
	}

	// decode buffer초기화
	for (j = 0; j < 6; j++) {
		mp4d_init_buf.Command = MP_CMD_INIT_BUF;
		//  _caution: buffer type이 1이랑 0이랑 차이가 머냐?, 0이면 b frame용이냐? 맞다 0이면 b frame용이다.
		mp4d_init_buf.BufferType = ((j < 4) ? 1 : 0);
		//  _caution: buffer type에 따라 buffer idx를 처음부터 다시 시작해서 설정한다.
		mp4d_init_buf.BufferIdx = ((j < 4) ? j : (j - 4));
		mp4d_init_buf.Y_Offset = y_offset[j];
		mp4d_init_buf.CB_Offset = cb_offset[j];
		mp4d_init_buf.CR_Offset = cr_offset[j];

		if (ioctl(mp4_fd, IOCTL_DUALCPU_MP4D_INIT_BUF, &mp4d_init_buf) != 1) {
			printf("IOCTL_DUALCPU_MP4D_INIT_BUF failed\n");
			return -1;
		}
		// buffer setting하고, status를 읽을때까정 기둘림.
		if (read_mp4d_940_status() < 0) {
			printf("IOCTL_DUALCPU_MP4D_INIT_BUF timeout\n");
			return -1;
		}
	}
#endif
	init_flag = 1;		// 940 engine init

	return 1;
}

//[*]------------------------------------------------------------------------------------------[*]
int open_dualcpu_vpp(void)
{
#ifdef GP2X
	int retry = 1;

	while (retry) {
		// devfs를 사용한다. dualcpu misc driver를 open한다.
		mp4_fd = open(PATH_DEV_DUALCPU, O_RDWR);
		if (mp4_fd >= 0)
			break;

		mp4_fd = open(PATH_DEV_DUALCPU_WITH_DEVFS, O_RDWR);
		if (mp4_fd >= 0)
			break;

		if (retry) {
			// dualcpu 모듈을 추가한다.
			printf("insert dualcpu module...\n");
			system("/sbin/insmod dualcpu");
			retry = 0;
		} else {
			printf("open dualcpu driver failed\n");
			return -1;
		}
	}

	vpp_fd = open(PATH_DEV_VPP, O_RDWR);
	if (vpp_fd < 0) {
		vpp_fd = open(PATH_DEV_VPP_WITH_DEVFS, O_RDWR);
		if (vpp_fd < 0) {
			printf("open video post processor driver failed\n");
			return -1;
		}
	}

	if ((ioctl(vpp_fd, IOCTL_MMSP2_DPC_VSYNC_INTERRUPT_ENABLE, NULL)) < 0)	// hhsong add for vsync interrupt
	{
		close(vpp_fd);
		return -1;
	}

#endif
	return 1;
}

//[*]------------------------------------------------------------------------------------------[*]
void CloseMMSP2Decoder(void)
{
#ifdef GP2X
	if (mp4_fd > 0) {
		if (init_flag == 1) {
			mp4d_rel.Command = MP_CMD_RELEASE;
			if (mp4d_init.CodecVersion == CODEC_VER_DIVX_311)
				mp4d_rel.CodecType = MP_CODEC_MP4D_DIVX3;
			else
				mp4d_rel.CodecType = MP_CODEC_MP4D;

			int i = 1000;
			do {
				if (ioctl(mp4_fd, IOCTL_DUALCPU_MP4D_RELEASE, &mp4d_rel) == 1)
					break;
				usleep(10000);
				i--;
			}
			while (i > 0);

			if (i == 0)	// release가 fail일 경우는 그냥 fail처리를 하고 빠진다.
			{
				printf("IOCTL_DUALCPU_MP4D_RELEASE failed\n");
			} else	// fail이 아닐 경우는 status를 읽는다.
			{
				if (read_mp4d_940_status() < 0) {
					printf("IOCTL_DUALCPU_MP4D_RELEASE timeout\n");
				}
			}
		}
		close(mp4_fd);
		mp4_fd = -1;
	}

	RGBLayerClose();

	// YUVB off & RGB on
	if (vpp_fd > 0) {
		ioctl(vpp_fd, IOCTL_MMSP2_STOP_SC, NULL);
		ioctl(vpp_fd, IOCTL_MMSP2_YUVA_OFF, NULL);
		ioctl(vpp_fd, IOCTL_MMSP2_RGB_OFF, 1);
		ioctl(vpp_fd, IOCTL_MMSP2_DPC_VSYNC_INTERRUPT_DISABLE, NULL);

		close(vpp_fd);
		vpp_fd = -1;
	}

	if (arm940_mem_start)
	{
		munmap(arm940_mem_start, 0x01000000);
		arm940_mem_start = NULL;
	}

	if (mem_fd > 0) {
		close(mem_fd);
		mem_fd = -1;
	}

	theApp.setBatteryMode(MENU_MODE);
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
inline int read_mp4d_940_status()
{
#ifdef GP2X
	if (read(mp4_fd, &mp4d_disp, sizeof(MP4D_DISPLAY_PARAM)) <= 0) {
		printf("dualcpu read error");
		return -1;
	}
#endif
	return 0;
}
//[*]------------------------------------------------------------------------------------------[*]
int set_FDC_YUVB_plane(int width, int height)
{
	movie_width = width;
	movie_height = height;
	disp_mode = theApp.getDisplayMode();

#ifdef GP2X
   	fdc_info.VSync = 0;
    fdc_info.SrcField = 0;
    fdc_info.Chroma = FDC_CHROMA_420;
    fdc_info.Rotate = FDC_ROT_000;

    fdc_info.DstField = 0;

	fdc_info.ToScaler = 1;

	fdc_info.Width = width;
	fdc_info.Height = height;

    fdc_info.Y_Offset = y_offset[0];
    fdc_info.CB_Offset = cb_offset[0];
    fdc_info.CR_Offset = cr_offset[0];
    fdc_info.DstBaseAddr = 0;

	ioctl(vpp_fd, IOCTL_MMSP2_SET_FDC, &fdc_info);

	current_mode = -1;
#endif

	return change_resize_mode();
}
//[*]------------------------------------------------------------------------------------------[*]
int change_resize_mode()
{
#ifdef GP2X
	unsigned int width=movie_width;
	unsigned int height=movie_height;
	unsigned int cal_width=width;
	unsigned int cal_height=height;
	unsigned int disp_width, disp_height;
	unsigned int view_width, view_height;
	float ratio1=1.0, ratio2=1.0, cal_ratio=1.0;
	int skip=0;

	frame_decode_count = 0;
	while(resize_mode < 0) resize_mode += 4;
	resize_mode %= 4;
	if((disp_mode != LCD_MODE) && (resize_mode != 0)) resize_mode = 3;

	if (disp_mode == LCD_MODE) {
		disp_width = LCD_WIDTH;
		disp_height = LCD_HEIGHT;
		view_width = disp_width;
		view_height = disp_height;
	} else if (disp_mode == TV_NTSC_MODE) {
		disp_width = TV_NTSC_WIDTH;
		disp_height = TV_NTSC_HEIGHT;
		view_width = TV_NTSC_ENC_WIDTH;
		view_height = TV_NTSC_ENC_HEIGHT;
	} else {
		disp_width = TV_PAL_WIDTH;
		disp_height = TV_PAL_HEIGHT;
		view_width = TV_PAL_ENC_WIDTH;
		view_height = TV_PAL_ENC_HEIGHT;
	}

	ratio1 = (float)width/(float)view_width;
	ratio2 = (float)height/(float)view_height;

	if(ratio1 == ratio2) resize_mode = 0;

	if(current_mode == resize_mode) return 0;
	current_mode = resize_mode;

	switch(current_mode) {
		case 0:
			cal_ratio = (ratio1 > ratio2) ? ratio1 : ratio2;
			break;
		case 1:
			cal_ratio = (ratio1 + ratio2) / 2;
			break;
		case 2:
			cal_ratio = (ratio1 < ratio2) ? ratio1 : ratio2;;
			break;
		case 3:
			cal_ratio = 0;
			break;
	}

	if(cal_ratio > 0) {
		cal_width = ((unsigned short)(width/cal_ratio+0.5) + 1) & ~1;
		cal_height = (unsigned short)(height/cal_ratio+0.5);
	} else {
		cal_width = view_width;
		cal_height = view_height;
	}

    ioctl(vpp_fd, IOCTL_MMSP2_YUVA_OFF, NULL);
    ioctl(vpp_fd, IOCTL_MMSP2_YUVB_OFF, NULL);

	sc_info.Src = SC_FRM_FDC;
    sc_info.Dst = SC_TO_MEM;
    sc_info.NumOfFrame = SC_1_TIME;
    sc_info.WriteForm = SC_YCBYCR;
    sc_info.WriteField = SC_WR_FRAME;
    sc_info.hSrcMirror = 0;
    sc_info.vSrcMirror = 0;
    sc_info.hDstMirror = 0;
    sc_info.vDstMirror = 0;
    sc_info.SrcOddAddr = 0;
    sc_info.SrcEvenAddr = 0;
    sc_info.DstAddr = 0x03700000;
    sc_info.Y_Offset = 0;
    sc_info.CB_Offset = 0;
    sc_info.CR_Offset = 0;

	sc_info.SrcWidth	=	width;
	sc_info.SrcHeight	=	height;
	sc_info.SrcStride	=	width*2;
	sc_info.DstWidth	=	cal_width;
	sc_info.DstHeight	=	cal_height;
	sc_info.DstStride	=	cal_width*2;

	ioctl(vpp_fd, IOCTL_MMSP2_SET_SC, &sc_info);

 	yuv_info.Src = 0;
    yuv_info.Priority = MLC_YUV_PRIO_A;
    yuv_info.vMirrorTop = MLC_YUV_MIRROR_OFF;
    yuv_info.hMirrorTop = MLC_YUV_MIRROR_OFF;
    yuv_info.vMirrorBot = MLC_YUV_MIRROR_OFF;
    yuv_info.hMirrorBot = MLC_YUV_MIRROR_OFF;

	yuv_info.SrcWidthTop 	= (view_width > cal_width) ? cal_width : view_width;
	yuv_info.SrcHeightTop 	= cal_height;
	yuv_info.DstWidthTop 	= (view_width > cal_width) ? cal_width : view_width;
	yuv_info.DstHeightTop 	= cal_height;
	yuv_info.StrideTop 		= cal_width*2;

	if(disp_width < cal_width)
	{
		skip = (cal_width - disp_width) & ~1;
		yuv_info.StartX = 0;
	}
	else if(disp_width > cal_width)
	{
		yuv_info.StartX = (disp_width-cal_width) >> 1;
	}

	if(disp_mode != LCD_MODE)
		yuv_info.StartX = 15;

	yuv_info.StartY = (disp_height - cal_height) >> 1;

	yuv_info.EndX = yuv_info.StartX + ((view_width > cal_width) ? cal_width : view_width) - 1;
    yuv_info.EndY = yuv_info.StartY + cal_height - 1;
    yuv_info.BottomY = yuv_info.EndY;
	yuv_info.TopAddr = 0x03800000 + skip;
	yuv_info.BotAddr = 0x03800000 + skip;

    ioctl(vpp_fd, IOCTL_MMSP2_SET_YUVA, &yuv_info);

	printf("mode = %d, ratio = %.3f, width = %d, height = %d\n", current_mode, cal_ratio, cal_width, cal_height);
	printf("startx = %d, endx = %d, skip = %d\n", yuv_info.StartX, yuv_info.EndX, skip);
	printf("startx = %d, endx = %d\n", yuv_info.StartY ,yuv_info.EndY);
#endif
    return 1;
}
//[*]------------------------------------------------------------------------------------------[*]
int YUVLayerZoomMode(int mode)
{
	if(mode == -1)
		resize_mode++;
	else if(mode >= 0)
	    resize_mode = mode;
    change_resize_mode();
	return current_mode;
}
//[*]------------------------------------------------------------------------------------------[*]
int RunMMSP2Decoder(void)
{
#ifdef GP2X
	unsigned long bufaddr;

	// Setup MP4D data structure and call IOCTL functions of DUALCPU
	mp4d_run.Command = MP_CMD_RUN;
	if (mp4d_init.CodecVersion == CODEC_VER_DIVX_311)
		mp4d_run.CodecType = MP_CODEC_MP4D_DIVX3;
	else
		mp4d_run.CodecType = MP_CODEC_MP4D;

	bufaddr = (unsigned long) dbuf - (unsigned long) arm940_mem_start + CODEC_BUFF_ADDR;
	mp4d_run.StreamBufferPaddrL = (unsigned short) (bufaddr & 0x0000FFFF);
	mp4d_run.StreamBufferPaddrH = (unsigned short) (bufaddr >> 16);

	if (ioctl(mp4_fd, IOCTL_DUALCPU_MP4D_RUN, &mp4d_run) != 1) {
		printf("IOCTL_DUALCPU_MP4D_RUN failed\n");
		return -1;
	}

	return read_mp4d_940_status();
#else
	return 0;
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
void RunMMSP2Display(unsigned short yoffset, unsigned short cboffset, unsigned short croffset)
{
#ifdef GP2X
	fri.Y_Offset = yoffset;
	fri.CB_Offset = cboffset;
	fri.CR_Offset = croffset;
	ioctl(vpp_fd, IOCTL_MMSP2_START_FDC, &fri);
	ioctl(vpp_fd, IOCTL_MMSP2_START_SC, SC_STARTMEM);

	while (ioctl(vpp_fd, IOCTL_MMSP2_SC_BUSY, NULL));

	if (frame_decode_count < 4)
	{
		if (++frame_decode_count == 3)
		{
			ioctl(vpp_fd, IOCTL_MMSP2_YUVA_ON, NULL);
			RGBLayerInit();
		}
	}
#endif
}
//[*]------------------------------------------------------------------------------------------[*]
#define COLORKEY_RED 0xC8
#define COLORKEY_GREEN 0xCC
#define COLORKEY_BLUE 0xC8
static bool RGBLayerInited = false;
static bool RGBLayerMenuMode = false;
//[*]------------------------------------------------------------------------------------------[*]
void RGBLayerMenu(bool on)
{
	if(RGBLayerMenuMode == on) return;
#ifdef GP2X
	if (mem_fd < 0) return;
	unsigned short *gp2xregs = (unsigned short *) mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0xC0000000);

	if (gp2xregs != NULL) {
		/*
		   int i;
		   unsigned short mixmux = gp2xregs[0x28DC >> 1] & ~3;
		   unsigned short ctrl;
		   if(on) mixmux |= 1;
		   gp2xregs[0x28DC >> 1] = mixmux;
		 */
		gp2xregs[0x28DE >> 1] = on ? 0x0C : 0x0F;
		//printf("RGBLayer menu mode %s\n", (gp2xregs[0x28DE >> 1] & 0x0F) != 0x0F ? "On" : "Off");
		/*
		   mixmux = gp2xregs[0x28DC >> 1];
		   ctrl = gp2xregs[0x28DA >> 1];
		   for (i=0; i<5; i++)
		   {
		   int onoff = (ctrl & (1 << (i * 2)));
		   int mode = (mixmux & (3 << (i * 2)));
		   printf("Layer %d %s, %s mode\n", i, onoff ? "On" : "Off", (mode == 0) ? "STL" : ((mode == 1) ? "Color key" : "Alpha blend"));
		   }
		 */
		munmap((void *) gp2xregs, 0x10000);
	}
#endif
    RGBLayerMenuMode = on;
}
//[*]------------------------------------------------------------------------------------------[*]
void RGBLayerInit()
{
	if(RGBLayerInited) return;
#ifdef GP2X
	if (mem_fd < 0) return;
	unsigned short *gp2xregs = (unsigned short *) mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0xC0000000);

	if (gp2xregs != NULL) {
		int i;
		unsigned short mixmux = (gp2xregs[0x28DC >> 1] & 0xFFF0) | 1;
		unsigned short ctrl;
		unsigned short alpha[2];

		gp2xregs[0x2902 >> 1] = (COLORKEY_GREEN << 8) | COLORKEY_BLUE;
		gp2xregs[0x2904 >> 1] = COLORKEY_RED;
		gp2xregs[0x28DC >> 1] = mixmux;
		gp2xregs[0x28DE >> 1] = 0x0F;
		//gp2xregs[0x28DA >> 1] = (2 << 9) | (1 << 0); // (1 << 2) |
		/*
		   printf("Color Key : #%02X%04X\n", gp2xregs[0x2904 >> 1], gp2xregs[0x2902 >> 1]);
		   mixmux = gp2xregs[0x28DC >> 1];
		   ctrl = gp2xregs[0x28DA >> 1];
		   alpha[0] = gp2xregs[0x28DE >> 1];
		   alpha[1] = gp2xregs[0x28E0 >> 1];
		   for (i=0; i<5; i++)
		   {
		   int onoff = (ctrl & (1 << (i * 2)));
		   int mode = (mixmux & (3 << (i * 2)));
		   int a;
		   if(i < 3) {
		   a = alpha[0] & (0x0F << (i * 4));
		   } else {
		   a = alpha[1] & (0x0F << ((i - 3) * 4));
		   }
		   printf("Layer %d %s, %s mode, alpha is %d\n", i, onoff ? "On" : "Off", (mode == 0) ? "STL" : ((mode == 1) ? "Color key" : "Alpha blend"), a);
		   }
		 */

		munmap((void *) gp2xregs, 0x10000);
	}
#endif
	RGBLayerInited = true;
	RGBLayerMenuMode = false;

	printf("RGB Layer initialize...\n");
}
//[*]------------------------------------------------------------------------------------------[*]
void RGBLayerClose()
{
	if(!RGBLayerInited) return;
#ifdef GP2X
	if (mem_fd < 0) return;
	unsigned short *gp2xregs = (unsigned short *) mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0xC0000000);

	if (gp2xregs != NULL) {
		int i;
		unsigned short mixmux = (gp2xregs[0x28DC >> 1] & 0xFFF0);
		unsigned short ctrl;
		unsigned short alpha[2];

		gp2xregs[0x28DC >> 1] = mixmux;
		gp2xregs[0x28DE >> 1] = 0x0F;
		//gp2xregs[0x28DA >> 1] = (2 << 9) | (1 << 0);
		/*
		   printf("Color Key : #%02X%04X\n", gp2xregs[0x2904 >> 1], gp2xregs[0x2902 >> 1]);
		   mixmux = gp2xregs[0x28DC >> 1];
		   ctrl = gp2xregs[0x28DA >> 1];
		   alpha[0] = gp2xregs[0x28DE >> 1];
		   alpha[1] = gp2xregs[0x28E0 >> 1];
		   for (i=0; i<5; i++)
		   {
		   int onoff = (ctrl & (1 << (i * 2)));
		   int mode = (mixmux & (3 << (i * 2)));
		   int a;
		   if(i < 3) {
		   a = alpha[0] & (0x0F << (i * 4));
		   } else {
		   a = alpha[1] & (0x0F << ((i - 3) * 4));
		   }
		   printf("Layer %d %s, %s mode, alpha is %d\n", i, onoff ? "On" : "Off", (mode == 0) ? "STL" : ((mode == 1) ? "Color key" : "Alpha blend"), a);
		   }
		 */

		munmap((void *) gp2xregs, 0x10000);
	}
#endif
	RGBLayerInited = false;
	RGBLayerMenuMode = false;

	printf("RGB Layer uninitialize...\n");
}
//[*]------------------------------------------------------------------------------------------[*]
