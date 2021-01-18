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

#ifndef _MMSP2_IF_H_
#define _MMSP2_IF_H_

/* DUAL CPU */
#define	MP_CMD_NULL			0
#define	MP_CODEC_NULL		-1
#define	MP_CODEC_MP4D		0
#define	MP_CODEC_MP4D_DIVX3	1
#define	MP_CODEC_H263D		2
#define	MP_CODEC_MP2D		3
#define	MP_CODEC_MJPD		4
#define	MP_CODEC_MP4E		5
#define	MP_CODEC_WMVD		6
#define MP_CODEC_MP3D       7
#define	MP_CODEC_MP1D		9
#define	MP_CODEC_MJPE		10
#define	MP_SYSTEM_INIT		8
#define	MP_CMD_INIT					1
#define MP_CMD_INIT_BUF     		16
#define	MP_CMD_RUN					2
#define	MP_CMD_RELEASE				4
#define	MP_CMD_GETINFO				6
#define	MP_CMD_940IDLE				5
#define	MP_CMD_INIT_FOR_SEEK		7
#define	MP_CMD_RELEASE_FOR_SEEK		8
#define MP_CMD_DISPLAY      		17
#define	MP_CMD_ACK					5
#define	MP_CMD_FAIL					15
#define	CODEC_VER_DIVX_311	311
#define	CODEC_VER_DIVX_500	500
#define	CODEC_VER_XVID		900

typedef struct _MP2D_INIT_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
    unsigned short	Width;
    unsigned short	Height;
    unsigned short	CodecVersion;
    unsigned short	RL_L;
    unsigned short	RL_H;
    unsigned short	RL_Length;
    unsigned short	StreamBufferPaddrL;
    unsigned short	StreamBufferPaddrH;
} MP2D_INIT_PARAM;

typedef struct _MP4E_INIT_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
    unsigned short	Width;
    unsigned short	Height;
    unsigned short	CodecVersion;
    unsigned short	Framerate;
    unsigned short	RcPeriod;
    unsigned short	RcReactionPeriod;
    unsigned short	RcReactionRation;
    unsigned short	MaxKeyInterval;
    unsigned short	MaxQuantizer;
    unsigned short	MinQuantizer;
    unsigned short	SearchRrange;
} MP4E_INIT_PARAM;

typedef struct _MJPD_INIT_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
} MJPD_INIT_PARAM;

typedef struct _MP2D_RUN_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
    unsigned short	StreamBufferPaddrL;
    unsigned short	StreamBufferPaddrH;
} MP2D_RUN_PARAM;

typedef struct _MP4E_RUN_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
    unsigned short	StreamBufferPaddrL;
    unsigned short	StreamBufferPaddrH;
    unsigned int	StreamBufferSize;
} MP4E_RUN_PARAM;

typedef struct _MJPD_RUN_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
} MJPD_RUN_PARAM;

typedef struct _MJPD_OFFSET_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
    unsigned short	LU_Offset;
    unsigned short	CB_Offset;
    unsigned short	CR_Offset;
} MJPD_OFFSET_PARAM;

typedef struct _MP2D_REL_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
} MP2D_REL_PARAM;

typedef struct _MP4D_REL_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
} MP4D_REL_PARAM;

typedef struct _MP4E_REL_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
} MP4E_REL_PARAM;

typedef struct _MJPD_REL_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
} MJPD_REL_PARAM;

typedef struct _MP4D_INIT_PARAM_
{
    unsigned short	Command;
    unsigned short	CodecType;
    unsigned short	Width;
    unsigned short	Height;
    unsigned short	CodecVersion;
    unsigned short	RL_L;
    unsigned short	RL_H;
    unsigned short	RL_Length;
    unsigned short	mbp_a_addr_offset;
    unsigned short	mbp_b_addr_offset;
    unsigned short	qmat_addr_h;
    unsigned short	qmat_addr_l;
    unsigned short	pred_q_lb_offset;
    unsigned short	arm940_offset;
    unsigned short	non_bframe_max;
    unsigned short	b_frame_max;
} MP4D_INIT_PARAM;

typedef struct _MP4D_INIT_BUF_PARAM_
{
    unsigned short  Command;
    unsigned short  BufferType;
    unsigned short  BufferIdx;
    unsigned short  Y_Offset;
    unsigned short  CB_Offset;
    unsigned short  CR_Offset;
} MP4D_INIT_BUF_PARAM;

typedef struct _MP4D_RUN_PARAM_
{

    unsigned short	Command;
    unsigned short	CodecType;
    unsigned short	frames;
    unsigned short	stream_length;
    unsigned short	stream_last_marker_h;
    unsigned short	stream_last_marker_l;
    unsigned short	StreamBufferPaddrL;
    unsigned short	StreamBufferPaddrH;

} MP4D_RUN_PARAM;

typedef struct _MP4D_DISPLAY_PARAM_
{
    unsigned short	Command;
    unsigned short	has_bframe;
    unsigned short	display_frames;

    unsigned short	time;
    unsigned short	luma_offset_addr;
    unsigned short	cb_offset_addr;
    unsigned short	cr_offset_addr;

    unsigned short	remain_frame_time;
    unsigned short	remain_frame_luma_offset_addr;
    unsigned short	remain_frame_cb_offset_addr;
    unsigned short	remain_frame_cr_offset_addr;

    unsigned short	remain_frame_time1;
    unsigned short	remain_frame_luma_offset_addr1;
    unsigned short	remain_frame_cb_offset_addr1;
    unsigned short	remain_frame_cr_offset_addr1;
    unsigned short	dummy;

} MP4D_DISPLAY_PARAM;

/* FDC */
#    define FDC_CHROMA_420  0
#    define FDC_CHROMA_422  1
#    define FDC_CHROMA_444  2

#    define FDC_ROT_000 0
#    define FDC_ROT_090 1
#    define FDC_ROT_180 2
#    define FDC_ROT_270 3

typedef struct _fdc_info {
	unsigned short VSync;
	unsigned short SrcField;	// 0:Frame, 1:Field
	unsigned short Chroma;
	unsigned short Rotate;
	unsigned short DstField;	// 0:Frame, 1:Field
	unsigned short ToScaler;	// 1:To Scaler
	unsigned short Width;
	unsigned short Height;
	unsigned short Y_Offset;
	unsigned short CB_Offset;
	unsigned short CR_Offset;
	unsigned long DstBaseAddr;
} FDC_INFO;

typedef struct _fdc_run_info {
	unsigned short Y_Offset;
	unsigned short CB_Offset;
	unsigned short CR_Offset;
} FDC_RUN_INFO;

/* SCALER */
#    define SC_FRM_MEM  (0x0<<0)
#    define SC_FRM_MLC  (0x1<<0)
#    define SC_FRM_ISP  (0x1<<1)
#    define SC_FRM_FDC  (0x1<<2)

#    define SC_TO_MEM   0
#    define SC_TO_MLC   1

#    define SC_FOREVER  0x0
#    define SC_ONE_TIME 0x1
#    define SC_1_TIME 0x1

#    define SC_YCBYCR   0x0
#    define SC_SEP_F420 0x1
#    define SC_SEP_F422 0x2

#    define SC_WR_FIELD 0x0
#    define SC_WR_FRAME 0x1

#    define SC_DISABLE  0x0
#    define SC_ENABLE   0x1

#    define SC_EVEN_FIELD   0x0
#    define SC_ODD_FIELD    0x1

#    define SC_FID_NORM     0
#    define SC_FID_INV      1

#    define SC_MIRROR_OFF   0x0
#    define SC_MIRROR_ON    0x1

#    define SC_INT_NONE 0x0
#    define SC_INT_FDC  0x1
#    define SC_INT_SC   (0x1<<1)

#   define SC_RUN          0x01
#   define SC_STARTMEM     0x02
#   define SC_STARTDIS     0x03

typedef struct _sc_info {
	unsigned char Src;
	unsigned char Dst;
	unsigned char NumOfFrame;
	unsigned char WriteForm;
	unsigned char WriteField;
	unsigned char hSrcMirror;
	unsigned char vSrcMirror;
	unsigned char hDstMirror;
	unsigned char vDstMirror;
	unsigned short SrcWidth;
	unsigned short SrcHeight;
	unsigned short SrcStride;
	unsigned short DstWidth;
	unsigned short DstHeight;
	unsigned short DstStride;
	unsigned long SrcOddAddr;
	unsigned long SrcEvenAddr;
	unsigned long DstAddr;
	unsigned short Y_Offset;
	unsigned short CB_Offset;
	unsigned short CR_Offset;
} SC_INFO;

/* MLC */
#    define MLC_YUV_PRIO_A  0x0
#    define MLC_YUV_PRIO_B  0x1

#    define MLC_YUV_MIRROR_ON   0x1
#    define MLC_YUV_MIRROR_OFF  0x0

typedef struct _yuv_info {
	unsigned char Src;	// 0: Mem, 1: Scaler(YUVA), FDC(YUVB)
	unsigned char Priority;	// 0: A, 1: B
	unsigned char vMirrorTop;
	unsigned char hMirrorTop;
	unsigned char vMirrorBot;
	unsigned char hMirrorBot;
	unsigned short SrcWidthTop;
	unsigned short SrcHeightTop;
	unsigned short DstWidthTop;
	unsigned short DstHeightTop;
	unsigned short StrideTop;
	unsigned short SrcWidthBot;
	unsigned short SrcHeightBot;
	unsigned short DstWidthBot;
	unsigned short DstHeightBot;
	unsigned short StrideBot;
	unsigned short StartX;
	unsigned short StartY;
	unsigned short EndX;
	unsigned short EndY;
	unsigned short BottomY;
	unsigned long TopAddr;
	unsigned long BotAddr;
} YUV_INFO;

typedef struct _window_info {
	unsigned short InputWidth;
	unsigned short InputHeight;
	unsigned short WindowWidth;
	unsigned short WindowHeight;
	unsigned short DisplayWidth;
	unsigned short DisplayHeight;
} WINDOW_INFO;

/* DUAL CPU IOCTL Commands */
#define IOCTL_DUALCPU_MP4D_INIT     _IOW('d', 0x01, MP4D_INIT_PARAM)
#define IOCTL_DUALCPU_MP4D_INIT_BUF _IOW('d', 0x02, MP4D_INIT_BUF_PARAM)
#define IOCTL_DUALCPU_MP4D_RUN      _IOW('d', 0x03, MP4D_RUN_PARAM)
#define IOCTL_DUALCPU_MP4D_RELEASE  _IOW('d', 0x04, MP4D_REL_PARAM)
#define IOCTL_DUALCPU_MP4D_STATUS   _IOR('d', 0x05, MP4D_DISPLAY_PARAM)

/* FDC IOCTL Commands */
#define IOCTL_MMSP2_SET_FDC     _IOW('v', 0x00, FDC_INFO)
#define IOCTL_MMSP2_START_FDC   _IOW('v', 0x01, FDC_RUN_INFO)
#define IOCTL_MMSP2_FDC_BUSY    _IO('v', 0x02)
#define IOCTL_MMSP2_STOP_FDC    _IO('v', 0x03)

/* SCALER IOCTL Commands */
#define IOCTL_MMSP2_SET_SC      _IOW('v', 0x08, SC_INFO)
#define IOCTL_MMSP2_ENABLE_SC   _IOW('v', 0x09, unsigned long)
#define IOCTL_MMSP2_START_SC    _IOR('v', 0x0A, unsigned char)
#define IOCTL_MMSP2_SC_BUSY     _IO('v', 0x0B)
#define IOCTL_MMSP2_STOP_SC     _IO('v', 0x0C)

/* MLC IOCTL Commands */
#define IOCTL_MMSP2_SET_YUVA    _IOW('v', 0x10, YUV_INFO)
#define IOCTL_MMSP2_SET_YUVB    _IOW('v', 0x11, YUV_INFO)
#define IOCTL_MMSP2_YUVA_ON     _IO('v', 0x12)
#define IOCTL_MMSP2_YUVA_OFF    _IO('v', 0x13)
#define IOCTL_MMSP2_YUVB_ON     _IO('v', 0x14)
#define IOCTL_MMSP2_YUVB_OFF    _IO('v', 0x15)
#define IOCTL_MMSP2_CALC_WINDOW _IOR('v', 0x16, WINDOW_INFO)
#define IOCTL_MMSP2_JIFFIES     _IOR('v', 0x18, unsigned long)
#define IOCTL_MMSP2_RGB_ON      _IOR('v', 0x20, unsigned long)
#define IOCTL_MMSP2_RGB_OFF     _IOR('v', 0x21, unsigned long)

/* DPC VSYNC IOCTL Commands */
#define IOCTL_MMSP2_DPC_VSYNC_INTERRUPT_ENABLE  _IO('v', 0x30)
#define IOCTL_MMSP2_DPC_VSYNC_INTERRUPT_DISABLE _IO('v', 0x31)

extern unsigned char *dbuf;
extern MP4D_DISPLAY_PARAM mp4d_disp;

/* MMSP2 Interface */
int mmsp2_940_interface_buffer_init(void);
int mmsp2_setup_940_interface_buffer(int width, int height, unsigned int vformat_fourcc);
int open_dualcpu_vpp(void);
void CloseMMSP2Decoder(void);
int set_FDC_YUVB_plane(int width, int height);
int change_resize_mode();

int RunMMSP2Decoder(void);
void RunMMSP2Display(unsigned short yoffset, unsigned short cboffset, unsigned short croffset);

int YUVLayerZoomMode(int mode);
void RGBLayerInit();
void RGBLayerClose();
void RGBLayerMenu(bool on);

#endif
