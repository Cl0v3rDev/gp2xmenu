/*
   ad.c - audio decoder interface
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "stream_internal.h"
#include "demuxer.h"
#include "stheader.h"
#include "ad.h"

/* Missed vorbis, mad, dshow */

extern ad_functions_t mpcodecs_ad_pcm;
extern ad_functions_t mpcodecs_ad_dvdpcm;
extern ad_functions_t mpcodecs_ad_alaw;
extern ad_functions_t mpcodecs_ad_imaadpcm;
extern ad_functions_t mpcodecs_ad_msadpcm;
extern ad_functions_t mpcodecs_ad_libvorbis;
extern ad_functions_t mpcodecs_ad_libmad;

ad_functions_t *mpcodecs_ad_drivers[] = {
	&mpcodecs_ad_pcm,
	&mpcodecs_ad_dvdpcm,
	&mpcodecs_ad_alaw,
	&mpcodecs_ad_imaadpcm,
	&mpcodecs_ad_msadpcm,
	&mpcodecs_ad_libvorbis,
	&mpcodecs_ad_libmad,
	NULL
};
