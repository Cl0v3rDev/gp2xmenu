/* Protect against multiple inclusion */
#ifndef MPLAYER_CONFIG_H
#    define MPLAYER_CONFIG_H 1

/* use setlocale() function */
#    define USE_SETLOCALE 1

/* Toggles debugging informations */
#    undef MP_DEBUG

/* Define this to enable avg. byte/sec-based AVI sync method by default:
   (use -bps or -nobps commandline option for run-time method selection)
   -bps gives better sync for vbr mp3 audio, it is now default */
#    define AVI_SYNC_BPS 1

/* set up max. outburst. use 65536 for ALSA 0.5, for others 16384 is enough */
#    if 1			// ghcstop 041207: kernel driver를 8 * 16k로 만들었음 그래서 32k로 줄인다.
#        define MAX_OUTBURST 65536
#    else
#        define MAX_OUTBURST 32768
#    endif

/* set up audio OUTBURST. Do not change this! */
#    define OUTBURST 512

/* Define this if your system has the header file for the OSS sound interface */
#ifdef GP2X
#    define HAVE_SYS_SOUNDCARD_H 1
#endif

/* Define this if your system has the "malloc.h" header file */
#    define HAVE_MALLOC_H 1

/* memalign is mapped to malloc if unsupported */
#ifdef GP2X
#    define HAVE_MEMALIGN 1
#else
#    undef HAVE_MEMALIGN
#endif
#    ifndef HAVE_MEMALIGN
#        define memalign(a,b) malloc(b)
#        define MEMALIGN_HACK 1
#    endif

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
#    undef WORDS_BIGENDIAN

/* Define this to any prefered value from 386 up to infinity with step 100 */
#ifdef GP2X
#    define __CPU__ arm
#endif

/*----------------------------------------------------------------------------
**
** NOTE: Instead of modifying these definitions here, use the
**       --enable/--disable options of the ./configure script!
**       See ./configure --help for details.
**
*---------------------------------------------------------------------------*/

/* nanosleep support */
#    undef HAVE_NANOSLEEP

/* enable FriBiDi usage */
#    undef USE_FRIBIDI

/* enable ENCA usage */
#    undef HAVE_ENCA

#    define TREMOR
/* enable OggTheora support */
#    undef HAVE_OGGTHEORA

/* enables / disables subtitles sorting */
//#    define USE_SORTSUB 1
#    undef USE_SORTSUB

/* libmpeg2 uses a different feature test macro for mediaLib */
#    ifdef HAVE_MLIB
#        define LIBMPEG2_MLIB 1
#    endif

//#define DEBUG
#        ifdef DEBUG
#            define dprintf(x...) printf(x)
#        else
#            define dprintf(x...)
#        endif

#endif				/* MPLAYER_CONFIG_H */
