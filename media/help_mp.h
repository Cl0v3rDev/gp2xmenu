// $Revision: 1.154 $
// MASTER FILE. Use this file as base for translations.
// Translated files should be sent to the mplayer-DOCS mailing list or
// to the help messages maintainer, see DOCS/tech/MAINTAINERS.
// The header of the translated file should contain credits and contact
// information. Before major releases we will notify all translators to update
// their files. Please do not simply translate and forget this, outdated
// translations quickly become worthless. To help us spot outdated files put a
// note like "sync'ed with help_mp-en.h XXX" in the header of the translation.
// Do NOT translate the above lines, just follow the instructions.

// ========================= MPlayer help ===========================

#define MSGTR_SamplesWanted "Samples of this format are needed to improve support. Please contact the developers.\n"

// ========================= MPlayer messages ===========================

// mplayer.c:

#define MSGTR_Exiting "\nExiting...\n"
#define MSGTR_ExitingHow "\nExiting... (%s)\n"
#define MSGTR_Exit_quit "Quit"
#define MSGTR_Exit_eof "End of file"
#define MSGTR_Exit_error "Fatal error"
#define MSGTR_IntBySignal "\nMPlayer interrupted by signal %d in module: %s\n"
#define MSGTR_NoHomeDir "Cannot find HOME directory.\n"
#define MSGTR_GetpathProblem "get_path(\"config\") problem\n"
#define MSGTR_CreatingCfgFile "Creating config file: %s\n"
#define MSGTR_InvalidAOdriver "Invalid audio output driver name: %s\nUse '-ao help' to get a list of available audio drivers.\n"
#define MSGTR_CopyCodecsConf "(Copy/link etc/codecs.conf from the MPlayer sources to ~/.mplayer/codecs.conf)\n"
#define MSGTR_BuiltinCodecsConf "Using built-in default codecs.conf.\n"
#define MSGTR_CantLoadFont "Cannot load font: %s\n"
#define MSGTR_CantLoadSub "Cannot load subtitles: %s\n"
#define MSGTR_DumpSelectedStreamMissing "dump: FATAL: Selected stream missing!\n"
#define MSGTR_CantOpenDumpfile "Cannot open dump file.\n"
#define MSGTR_CoreDumped "Core dumped ;)\n"
#define MSGTR_FPSnotspecified "FPS not specified in the header or invalid, use the -fps option.\n"
#define MSGTR_TryForceAudioFmtStr "Trying to force audio codec driver family %s...\n"
#define MSGTR_CantFindAudioCodec "Cannot find codec for audio format 0x%X.\n"
#define MSGTR_RTFMCodecs "Read DOCS/HTML/en/codecs.html!\n"
#define MSGTR_TryForceVideoFmtStr "Trying to force video codec driver family %s...\n"
#define MSGTR_CantFindVideoCodec "Cannot find codec matching selected -vo and video format 0x%X.\n"
#define MSGTR_CannotInitVO "FATAL: Cannot initialize video driver.\n"
#define MSGTR_CannotInitAO "Could not open/initialize audio device -> no sound.\n"
#define MSGTR_StartPlaying "Starting playback...\n"

#define MSGTR_SystemTooSlow "\n\n"\
"           ************************************************\n"\
"           **** Your system is too SLOW to play this!  ****\n"\
"           ************************************************\n\n"\
"Possible reasons, problems, workarounds:\n"\
"- Most common: broken/buggy _audio_ driver\n"\
"  - Try -ao sdl or use the OSS emulation of ALSA.\n"\
"  - Experiment with different values for -autosync, 30 is a good start.\n"\
"- Slow video output\n"\
"  - Try a different -vo driver (-vo help for a list) or try -framedrop!\n"\
"- Slow CPU\n"\
"  - Don't try to play a big DVD/DivX on a slow CPU! Try -hardframedrop.\n"\
"- Broken file\n"\
"  - Try various combinations of -nobps -ni -forceidx -mc 0.\n"\
"- Slow media (NFS/SMB mounts, DVD, VCD etc)\n"\
"  - Try -cache 8192.\n"\
"- Are you using -cache to play a non-interleaved AVI file?\n"\
"  - Try -nocache.\n"\
"Read DOCS/HTML/en/video.html for tuning/speedup tips.\n"\
"If none of this helps you, read DOCS/HTML/en/bugreports.html.\n\n"

#define MSGTR_NoGui "MPlayer was compiled WITHOUT GUI support.\n"
#define MSGTR_GuiNeedsX "MPlayer GUI requires X11.\n"
#define MSGTR_Playing "Playing %s.\n"
#define MSGTR_NoSound "Audio: no sound\n"
#define MSGTR_FPSforced "FPS forced to be %5.3f  (ftime: %5.3f).\n"
#define MSGTR_CompiledWithRuntimeDetection "Compiled with runtime CPU detection - WARNING - this is not optimal!\nTo get best performance, recompile MPlayer with --disable-runtime-cpudetection.\n"
#define MSGTR_CompiledWithCPUExtensions "Compiled for x86 CPU with extensions:"
#define MSGTR_AvailableVideoOutputDrivers "Available video output drivers:\n"
#define MSGTR_AvailableAudioOutputDrivers "Available audio output drivers:\n"
#define MSGTR_AvailableAudioCodecs "Available audio codecs:\n"
#define MSGTR_AvailableVideoCodecs "Available video codecs:\n"
#define MSGTR_AvailableAudioFm "Available (compiled-in) audio codec families/drivers:\n"
#define MSGTR_AvailableVideoFm "Available (compiled-in) video codec families/drivers:\n"
#define MSGTR_AvailableFsType "Available fullscreen layer change modes:\n"
#define MSGTR_UsingRTCTiming "Using Linux hardware RTC timing (%ldHz).\n"
#define MSGTR_CannotReadVideoProperties "Video: Cannot read properties.\n"
#define MSGTR_NoStreamFound "No stream found.\n"
#define MSGTR_ErrorInitializingVODevice "Error opening/initializing the selected video_out (-vo) device.\n"
#define MSGTR_ForcedVideoCodec "Forced video codec: %s\n"
#define MSGTR_ForcedAudioCodec "Forced audio codec: %s\n"
#define MSGTR_Video_NoVideo "Video: no video\n"
#define MSGTR_NotInitializeVOPorVO "\nFATAL: Could not initialize video filters (-vf) or video output (-vo).\n"
#define MSGTR_Paused "\n  =====  PAUSE  =====\r"	// no more than 23 characters (status line for audio files)
#define MSGTR_PlaylistLoadUnable "\nUnable to load playlist %s.\n"
#define MSGTR_Exit_SIGILL_RTCpuSel \
"- MPlayer crashed by an 'Illegal Instruction'.\n"\
"  It may be a bug in our new runtime CPU-detection code...\n"\
"  Please read DOCS/HTML/en/bugreports.html.\n"
#define MSGTR_Exit_SIGILL \
"- MPlayer crashed by an 'Illegal Instruction'.\n"\
"  It usually happens when you run it on a CPU different than the one it was\n"\
"  compiled/optimized for.\n"\
"  Verify this!\n"
#define MSGTR_Exit_SIGSEGV_SIGFPE \
"- MPlayer crashed by bad usage of CPU/FPU/RAM.\n"\
"  Recompile MPlayer with --enable-debug and make a 'gdb' backtrace and\n"\
"  disassembly. Details in DOCS/HTML/en/bugreports_what.html#bugreports_crash.\n"
#define MSGTR_Exit_SIGCRASH \
"- MPlayer crashed. This shouldn't happen.\n"\
"  It can be a bug in the MPlayer code _or_ in your drivers _or_ in your\n"\
"  gcc version. If you think it's MPlayer's fault, please read\n"\
"  DOCS/HTML/en/bugreports.html and follow the instructions there. We can't and\n"\
"  won't help unless you provide this information when reporting a possible bug.\n"
#define MSGTR_LoadingConfig "Loading config '%s'\n"
#define MSGTR_AddedSubtitleFile "SUB: added subtitle file (%d): %s\n"
#define MSGTR_ErrorOpeningOutputFile "Error opening file [%s] for writing!\n"
#define MSGTR_CommandLine "CommandLine:"
#define MSGTR_RTCDeviceNotOpenable "Failed to open %s: %s (it should be readable by the user.)\n"
#define MSGTR_LinuxRTCInitErrorIrqpSet "Linux RTC init error in ioctl (rtc_irqp_set %lu): %s\n"
#define MSGTR_IncreaseRTCMaxUserFreq "Try adding \"echo %lu > /proc/sys/dev/rtc/max-user-freq\" to your system startup scripts.\n"
#define MSGTR_LinuxRTCInitErrorPieOn "Linux RTC init error in ioctl (rtc_pie_on): %s\n"
#define MSGTR_UsingTimingType "Using %s timing.\n"
#define MSGTR_MenuInitialized "Menu inited: %s\n"
#define MSGTR_MenuInitFailed "Menu init failed.\n"
#define MSGTR_Getch2InitializedTwice "WARNING: getch2_init called twice!\n"
#define MSGTR_DumpstreamFdUnavailable "Cannot dump this stream - no 'fd' available.\n"
#define MSGTR_FallingBackOnPlaylist "Falling back on trying to parse playlist %s...\n"
#define MSGTR_CantOpenLibmenuFilterWithThisRootMenu "Can't open libmenu video filter with root menu %s.\n"
#define MSGTR_AudioFilterChainPreinitError "Error at audio filter chain pre-init!\n"
#define MSGTR_LinuxRTCReadError "Linux RTC read error: %s\n"
#define MSGTR_SoftsleepUnderflow "Warning! Softsleep underflow!\n"
#define MSGTR_AnsSubVisibility "ANS_SUB_VISIBILITY=%ld\n"
#define MSGTR_AnsLength "ANS_LENGTH=%ld\n"
#define MSGTR_AnsVoFullscreen "ANS_VO_FULLSCREEN=%ld\n"
#define MSGTR_AnsPercentPos "ANS_PERCENT_POSITION=%ld\n"
#define MSGTR_DvdnavNullEvent "DVDNAV Event NULL?!\n"
#define MSGTR_DvdnavHighlightEventBroken "DVDNAV Event: Highlight event broken\n"
#define MSGTR_DvdnavEvent "DVDNAV Event: %s\n"
#define MSGTR_DvdnavHighlightHide "DVDNAV Event: Highlight Hide\n"
#define MSGTR_DvdnavStillFrame "######################################## DVDNAV Event: Still Frame: %d sec(s)\n"
#define MSGTR_DvdnavNavStop "DVDNAV Event: Nav Stop\n"
#define MSGTR_DvdnavNavNOP "DVDNAV Event: Nav NOP\n"
#define MSGTR_DvdnavNavSpuStreamChangeVerbose "DVDNAV Event: Nav SPU Stream Change: phys: %d/%d/%d logical: %d\n"
#define MSGTR_DvdnavNavSpuStreamChange "DVDNAV Event: Nav SPU Stream Change: phys: %d logical: %d\n"
#define MSGTR_DvdnavNavAudioStreamChange "DVDNAV Event: Nav Audio Stream Change: phys: %d logical: %d\n"
#define MSGTR_DvdnavNavVTSChange "DVDNAV Event: Nav VTS Change\n"
#define MSGTR_DvdnavNavCellChange "DVDNAV Event: Nav Cell Change\n"
#define MSGTR_DvdnavNavSpuClutChange "DVDNAV Event: Nav SPU CLUT Change\n"
#define MSGTR_DvdnavNavSeekDone "DVDNAV Event: Nav Seek Done\n"
#define MSGTR_MenuCall "Menu call\n"

#define MSGTR_EdlCantUseBothModes "Can't use -edl and -edlout at the same time.\n"
#define MSGTR_EdlOutOfMem "Can't allocate enough memory to hold EDL data.\n"
#define MSGTR_EdlRecordsNo "Read %d EDL actions.\n"
#define MSGTR_EdlQueueEmpty "There are no EDL actions to take care of.\n"
#define MSGTR_EdlCantOpenForWrite "Can't open EDL file [%s] for writing.\n"
#define MSGTR_EdlCantOpenForRead "Can't open EDL file [%s] for reading.\n"
#define MSGTR_EdlNOsh_video "Cannot use EDL without video, disabling.\n"
#define MSGTR_EdlNOValidLine "Invalid EDL line: %s\n"
#define MSGTR_EdlBadlyFormattedLine "Badly formatted EDL line [%d] Discarding.\n"
#define MSGTR_EdlBadLineOverlap "Last stop position was [%f]; next start is "\
"[%f]. Entries must be in chronological order, cannot overlap. Discarding.\n"
#define MSGTR_EdlBadLineBadStop "Stop time has to be after start time.\n"

// divx4_vbr.c:
#define MSGTR_OutOfMemory "out of memory"
#define MSGTR_OverridingTooLowBitrate "Specified bitrate is too low for this clip.\n"\
"Minimum possible bitrate for the clip is %.0f kbps. Overriding\n"\
"user-specified value.\n"

// fifo.c
#define MSGTR_CannotMakePipe "Cannot make PIPE!\n"

// m_config.c
#define MSGTR_SaveSlotTooOld "Too old save slot found from lvl %d: %d !!!\n"
#define MSGTR_InvalidCfgfileOption "The %s option can't be used in a config file.\n"
#define MSGTR_InvalidCmdlineOption "The %s option can't be used on the command line.\n"
#define MSGTR_InvalidSuboption "Error: option '%s' has no suboption '%s'.\n"
#define MSGTR_MissingSuboptionParameter "Error: suboption '%s' of '%s' must have a parameter!\n"
#define MSGTR_MissingOptionParameter "Error: option '%s' must have a parameter!\n"
#define MSGTR_OptionListHeader "\n Name                 Type            Min        Max      Global  CL    Cfg\n\n"
#define MSGTR_TotalOptions "\nTotal: %d options\n"

// open.c, stream.c:
#define MSGTR_CdDevNotfound "CD-ROM Device '%s' not found.\n"
#define MSGTR_ErrTrackSelect "Error selecting VCD track."
#define MSGTR_ReadSTDIN "Reading from stdin...\n"
#define MSGTR_UnableOpenURL "Unable to open URL: %s\n"
#define MSGTR_ConnToServer "Connected to server: %s\n"
#define MSGTR_FileNotFound "File not found: '%s'\n"

#define MSGTR_SMBInitError "Cannot init the libsmbclient library: %d\n"
#define MSGTR_SMBFileNotFound "Could not open from LAN: '%s'\n"
#define MSGTR_SMBNotCompiled "MPlayer was not compiled with SMB reading support.\n"

#define MSGTR_CantOpenDVD "Couldn't open DVD device: %s\n"
#define MSGTR_DVDwait "Reading disc structure, please wait...\n"
#define MSGTR_DVDnumTitles "There are %d titles on this DVD.\n"
#define MSGTR_DVDinvalidTitle "Invalid DVD title number: %d\n"
#define MSGTR_DVDnumChapters "There are %d chapters in this DVD title.\n"
#define MSGTR_DVDinvalidChapter "Invalid DVD chapter number: %d\n"
#define MSGTR_DVDnumAngles "There are %d angles in this DVD title.\n"
#define MSGTR_DVDinvalidAngle "Invalid DVD angle number: %d\n"
#define MSGTR_DVDnoIFO "Cannot open the IFO file for DVD title %d.\n"
#define MSGTR_DVDnoVOBs "Cannot open title VOBS (VTS_%02d_1.VOB).\n"
#define MSGTR_DVDopenOk "DVD successfully opened.\n"

// demuxer.c, demux_*.c:
#define MSGTR_AudioStreamRedefined "WARNING: Audio stream header %d redefined.\n"
#define MSGTR_VideoStreamRedefined "WARNING: Video stream header %d redefined.\n"
#define MSGTR_TooManyAudioInBuffer "\nToo many audio packets in the buffer: (%d in %d bytes).\n"
#define MSGTR_TooManyVideoInBuffer "\nToo many video packets in the buffer: (%d in %d bytes).\n"
#define MSGTR_MaybeNI "Maybe you are playing a non-interleaved stream/file or the codec failed?\n" \
		      "For AVI files, try to force non-interleaved mode with the -ni option.\n"
#define MSGTR_SwitchToNi "\nBadly interleaved AVI file detected - switching to -ni mode...\n"
#define MSGTR_Detected_XXX_FileFormat "%s file format detected.\n"
#define MSGTR_DetectedAudiofile "Audio file detected.\n"
#define MSGTR_NotSystemStream "Not MPEG System Stream format... (maybe Transport Stream?)\n"
#define MSGTR_InvalidMPEGES "Invalid MPEG-ES stream??? Contact the author, it may be a bug :(\n"
#define MSGTR_FormatNotRecognized "============ Sorry, this file format is not recognized/supported =============\n"\
				  "=== If this file is an AVI, ASF or MPEG stream, please contact the author! ===\n"
#define MSGTR_MissingVideoStream "No video stream found.\n"
#define MSGTR_MissingAudioStream "No audio stream found -> no sound.\n"
#define MSGTR_MissingVideoStreamBug "Missing video stream!? Contact the author, it may be a bug :(\n"

#define MSGTR_DoesntContainSelectedStream "demux: File doesn't contain the selected audio or video stream.\n"

#define MSGTR_NI_Forced "Forced"
#define MSGTR_NI_Detected "Detected"
#define MSGTR_NI_Message "%s NON-INTERLEAVED AVI file format.\n"

#define MSGTR_UsingNINI "Using NON-INTERLEAVED broken AVI file format.\n"
#define MSGTR_CouldntDetFNo "Could not determine number of frames (for absolute seek).\n"
#define MSGTR_CantSeekRawAVI "Cannot seek in raw AVI streams. (Index required, try with the -idx switch.)\n"
#define MSGTR_CantSeekFile "Cannot seek in this file.\n"

#define MSGTR_EncryptedVOB "Encrypted VOB file! Read DOCS/HTML/en/dvd.html.\n"

#define MSGTR_MOVcomprhdr "MOV: Compressed headers support requires ZLIB!\n"
#define MSGTR_MOVvariableFourCC "MOV: WARNING: Variable FOURCC detected!?\n"
#define MSGTR_MOVtooManyTrk "MOV: WARNING: too many tracks"
#define MSGTR_FoundAudioStream "==> Found audio stream: %d\n"
#define MSGTR_FoundVideoStream "==> Found video stream: %d\n"
#define MSGTR_DetectedTV "TV detected! ;-)\n"
#define MSGTR_ErrorOpeningOGGDemuxer "Unable to open the ogg demuxer.\n"
#define MSGTR_ASFSearchingForAudioStream "ASF: Searching for audio stream (id:%d).\n"
#define MSGTR_CannotOpenAudioStream "Cannot open audio stream: %s\n"
#define MSGTR_CannotOpenSubtitlesStream "Cannot open subtitle stream: %s\n"
#define MSGTR_OpeningAudioDemuxerFailed "Failed to open audio demuxer: %s\n"
#define MSGTR_OpeningSubtitlesDemuxerFailed "Failed to open subtitle demuxer: %s\n"
#define MSGTR_TVInputNotSeekable "TV input is not seekable! (Seeking will probably be for changing channels ;)\n"
#define MSGTR_DemuxerInfoAlreadyPresent "Demuxer info %s already present!\n"
#define MSGTR_ClipInfo "Clip info:\n"

#define MSGTR_LeaveTelecineMode "\ndemux_mpg: 30fps NTSC content detected, switching framerate.\n"
#define MSGTR_EnterTelecineMode "\ndemux_mpg: 24fps progressive NTSC content detected, switching framerate.\n"

// dec_video.c & dec_audio.c:
#define MSGTR_CantOpenCodec "Could not open codec.\n"
#define MSGTR_CantCloseCodec "Could not close codec.\n"

#define MSGTR_MissingDLLcodec "ERROR: Could not open required DirectShow codec %s.\n"
#define MSGTR_ACMiniterror "Could not load/initialize Win32/ACM AUDIO codec (missing DLL file?).\n"
#define MSGTR_MissingLAVCcodec "Cannot find codec '%s' in libavcodec...\n"

#define MSGTR_MpegNoSequHdr "MPEG: FATAL: EOF while searching for sequence header.\n"
#define MSGTR_CannotReadMpegSequHdr "FATAL: Cannot read sequence header.\n"
#define MSGTR_CannotReadMpegSequHdrEx "FATAL: Cannot read sequence header extension.\n"
#define MSGTR_BadMpegSequHdr "MPEG: bad sequence header\n"
#define MSGTR_BadMpegSequHdrEx "MPEG: bad sequence header extension\n"

#define MSGTR_ShMemAllocFail "Cannot allocate shared memory.\n"
#define MSGTR_CantAllocAudioBuf "Cannot allocate audio out buffer.\n"

#define MSGTR_UnknownAudio "Unknown/missing audio format -> no sound\n"

#define MSGTR_UsingExternalPP "[PP] Using external postprocessing filter, max q = %d.\n"
#define MSGTR_UsingCodecPP "[PP] Using codec's postprocessing, max q = %d.\n"
#define MSGTR_VideoAttributeNotSupportedByVO_VD "Video attribute '%s' is not supported by selected vo & vd.\n"
#define MSGTR_VideoCodecFamilyNotAvailableStr "Requested video codec family [%s] (vfm=%s) not available.\nEnable it at compilation.\n"
#define MSGTR_AudioCodecFamilyNotAvailableStr "Requested audio codec family [%s] (afm=%s) not available.\nEnable it at compilation.\n"
#define MSGTR_OpeningVideoDecoder "Opening video decoder: [%s] %s\n"
#define MSGTR_OpeningAudioDecoder "Opening audio decoder: [%s] %s\n"
#define MSGTR_UninitVideoStr "uninit video: %s\n"
#define MSGTR_UninitAudioStr "uninit audio: %s\n"
#define MSGTR_VDecoderInitFailed "VDecoder init failed :(\n"
#define MSGTR_ADecoderInitFailed "ADecoder init failed :(\n"
#define MSGTR_ADecoderPreinitFailed "ADecoder preinit failed :(\n"
#define MSGTR_AllocatingBytesForInputBuffer "dec_audio: Allocating %d bytes for input buffer.\n"
#define MSGTR_AllocatingBytesForOutputBuffer "dec_audio: Allocating %d + %d = %d bytes for output buffer.\n"

// ======================= AO Audio Output drivers ========================

// libao2

// audio_out.c
#define MSGTR_AO_ALSA9_1x_Removed "audio_out: alsa9 and alsa1x modules were removed, use -ao alsa instead.\n"

// ao_oss.c
#define MSGTR_AO_OSS_CantOpenMixer "[AO OSS] audio_setup: Can't open mixer device %s: %s\n"
#define MSGTR_AO_OSS_ChanNotFound "[AO OSS] audio_setup: Audio card mixer does not have channel '%s' using default.\n"
#define MSGTR_AO_OSS_CantOpenDev "[AO OSS] audio_setup: Can't open audio device %s: %s\n"
#define MSGTR_AO_OSS_CantMakeFd "[AO OSS] audio_setup: Can't make filedescriptor blocking: %s\n"
#define MSGTR_AO_OSS_CantSetAC3 "[AO OSS] Can't set audio device %s to AC3 output, trying S16...\n"
#define MSGTR_AO_OSS_CantSetChans "[AO OSS] audio_setup: Failed to set audio device to %d channels.\n"
#define MSGTR_AO_OSS_CantUseGetospace "[AO OSS] audio_setup: driver doesn't support SNDCTL_DSP_GETOSPACE :-(\n"
#define MSGTR_AO_OSS_CantUseSelect "[AO OSS]\n   ***  Your audio driver DOES NOT support select()  ***\n Recompile MPlayer with #undef HAVE_AUDIO_SELECT in config.h !\n\n"
#define MSGTR_AO_OSS_CantReopen "[AO OSS]\nFatal error: *** CANNOT RE-OPEN / RESET AUDIO DEVICE *** %s\n"

// ao_esd.c
#define MSGTR_AO_ESD_CantOpenSound "[AO ESD] esd_open_sound failed: %s\n"
#define MSGTR_AO_ESD_LatencyInfo "[AO ESD] latency: [server: %0.2fs, net: %0.2fs] (adjust %0.2fs)\n"
#define MSGTR_AO_ESD_CantOpenPBStream "[AO ESD] failed to open esd playback stream: %s\n"

// ao_mpegpes.c
#define MSGTR_AO_MPEGPES_CantSetMixer "[AO MPEGPES] DVB audio set mixer failed: %s\n"
#define MSGTR_AO_MPEGPES_UnsupSamplerate "[AO MPEGPES] %d Hz not supported, try to resample...\n"

// ao_null.c
// This one desn't even  have any mp_msg nor printf's?? [CHECK]

// ao_pcm.c
#define MSGTR_AO_PCM_FileInfo "[AO PCM] File: %s (%s)\nPCM: Samplerate: %iHz Channels: %s Format %s\n"
#define MSGTR_AO_PCM_HintInfo "[AO PCM] Info: fastest dumping is achieved with -vc dummy -vo null\nPCM: Info: to write WAVE files use -waveheader (default).\n"
#define MSGTR_AO_PCM_CantOpenOutputFile "[AO PCM] Failed to open %s for writing!\n"

// ao_sdl.c
#define MSGTR_AO_SDL_INFO "[AO SDL] Samplerate: %iHz Channels: %s Format %s\n"
#define MSGTR_AO_SDL_DriverInfo "[AO SDL] using %s audio driver.\n"
#define MSGTR_AO_SDL_UnsupportedAudioFmt "[AO SDL] Unsupported audio format: 0x%x.\n"
#define MSGTR_AO_SDL_CantInit "[AO SDL] Initializing of SDL Audio failed: %s\n"
#define MSGTR_AO_SDL_CantOpenAudio "[AO SDL] Unable to open audio: %s\n"

// ao_alsa5.c
#define MSGTR_AO_ALSA5_InitInfo "[AO ALSA5] alsa-init: requested format: %d Hz, %d channels, %s\n"
#define MSGTR_AO_ALSA5_SoundCardNotFound "[AO ALSA5] alsa-init: no soundcards found.\n"
#define MSGTR_AO_ALSA5_InvalidFormatReq "[AO ALSA5] alsa-init: invalid format (%s) requested - output disabled.\n"
#define MSGTR_AO_ALSA5_PlayBackError "[AO ALSA5] alsa-init: playback open error: %s\n"
#define MSGTR_AO_ALSA5_PcmInfoError "[AO ALSA5] alsa-init: pcm info error: %s\n"
#define MSGTR_AO_ALSA5_SoundcardsFound "[AO ALSA5] alsa-init: %d soundcard(s) found, using: %s\n"
#define MSGTR_AO_ALSA5_PcmChanInfoError "[AO ALSA5] alsa-init: pcm channel info error: %s\n"
#define MSGTR_AO_ALSA5_CantSetParms "[AO ALSA5] alsa-init: error setting parameters: %s\n"
#define MSGTR_AO_ALSA5_CantSetChan "[AO ALSA5] alsa-init: error setting up channel: %s\n"
#define MSGTR_AO_ALSA5_ChanPrepareError "[AO ALSA5] alsa-init: channel prepare error: %s\n"
#define MSGTR_AO_ALSA5_DrainError "[AO ALSA5] alsa-uninit: playback drain error: %s\n"
#define MSGTR_AO_ALSA5_FlushError "[AO ALSA5] alsa-uninit: playback flush error: %s\n"
#define MSGTR_AO_ALSA5_PcmCloseError "[AO ALSA5] alsa-uninit: pcm close error: %s\n"
#define MSGTR_AO_ALSA5_ResetDrainError "[AO ALSA5] alsa-reset: playback drain error: %s\n"
#define MSGTR_AO_ALSA5_ResetFlushError "[AO ALSA5] alsa-reset: playback flush error: %s\n"
#define MSGTR_AO_ALSA5_ResetChanPrepareError "[AO ALSA5] alsa-reset: channel prepare error: %s\n"
#define MSGTR_AO_ALSA5_PauseDrainError "[AO ALSA5] alsa-pause: playback drain error: %s\n"
#define MSGTR_AO_ALSA5_PauseFlushError "[AO ALSA5] alsa-pause: playback flush error: %s\n"
#define MSGTR_AO_ALSA5_ResumePrepareError "[AO ALSA5] alsa-resume: channel prepare error: %s\n"
#define MSGTR_AO_ALSA5_Underrun "[AO ALSA5] alsa-play: alsa underrun, resetting stream.\n"
#define MSGTR_AO_ALSA5_PlaybackPrepareError "[AO ALSA5] alsa-play: playback prepare error: %s\n"
#define MSGTR_AO_ALSA5_WriteErrorAfterReset "[AO ALSA5] alsa-play: write error after reset: %s - giving up.\n"
#define MSGTR_AO_ALSA5_OutPutError "[AO ALSA5] alsa-play: output error: %s\n"
