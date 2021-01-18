#CROSSCOMPILE = /gp2xsdk/Tools/bin/arm-gp2x-linux-
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++

CFLAGS += -I. -Icontrol -Iinifile -IUnicodeFont
CFLAGS += -D_REENTRANT -Wno-write-strings -Os
# -DGP2X
ifeq ($(f200),yes)
#	CFLAGS += -DGP2XF200
else
#	CFLAGS += -DGP2XF100
endif
CXXFLAGS = $(CFLAGS)

LDFLAGS	+= -lSDL -lSDL_image -lpng -lz -ljpeg
LDFLAGS += -lSDL_ttf -lfreetype -lmad
# -lunicodefont -linifile -lmedia -lSDL_mixer

ifndef CROSS_COMPILE
ifeq ($(shell uname -m),x86_64)
CFLAGS += -m32 -ggdb
LDFLAGS += -m32
endif
else
LDFLAGS	+= -s
endif

ifdef PND
CFLAGS += -DPND
SRCS += host_fb.c
endif
ifdef WIZ
CFLAGS += -DWIZ
endif

SRCS += mainmenuapp.cpp mainview.cpp menuapp.cpp updateview.cpp

SRCS += control/filelist.cpp control/textout.cpp control/view.cpp

SRCS += game/gamebuiltinview.cpp game/gamesdview.cpp game/gameview.cpp

SRCS += media/mediacontrol.cpp media/mediaplayer.cpp media/mediaplayview.cpp media/mediasettingview.cpp
SRCS += media/mmsp2_if.cpp media/movieview.cpp media/musicview.cpp media/subtitle.cpp 
#SRCS += dummymedia.cpp

SRCS += ebook/ebookfontview.cpp ebook/ebookmarkview.cpp ebook/ebookreader.cpp
SRCS += ebook/ebooksettingview.cpp ebook/ebooktextview.cpp ebook/ebookview.cpp

SRCS += photo/jpegload.cpp photo/photoimageview.cpp photo/photoview.cpp photo/photoviewer.cpp

SRCS += setting/infoview.cpp setting/settingview.cpp setting/systemview.cpp
SRCS += setting/tmodeview.cpp setting/touchview.cpp setting/tvoutview.cpp setting/usbview.cpp

SRCS += inifile/inifile.c UnicodeFont/UnicodeFont.c UnicodeFont/UnicodeFontCode.c UnicodeFont/UnicodeFontSize.c
SRCS += media/demux_audio.c media/demux_avi.c media/demux_demuxers.c media/demuxer.c
# media/demux_ogg.c
SRCS += media/demux_ogg_dummy.c
SRCS += media/ad_alaw.c  media/ad_dvdpcm.c    media/ad_libmad.c     media/ad_msadpcm.c
SRCS += media/ad.c       media/ad_imaadpcm.c  media/ad_pcm.c
# SRCS += media/ad_libvorbis.c
SRCS += media/aviheader.c media/mp3_hdr.c media/video.c media/aviprint.c media/mpeg_hdr.c
SRCS += media/dec_audio.c media/parse_es.c media/stream.c

SRCS += port.c cmn.c

OBJS := $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

vpath %.c = ../common/

TARGET = gp2xmenu

all:	$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

distclean:
	rm -f *.bak $(OBJS) $(TARGET) *~ .depend

dep:    depend

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend

ifneq ($(wildcard .depend),)
include .depend
endif
