#ifndef __INIFILE_H__
#define __INIFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
# ifdef BUILD_DLL
#  define DLLIMPORT __declspec (dllexport)
# else
#  define DLLIMPORT __declspec (dllimport)
# endif
#else
# define DLLIMPORT
#endif

DLLIMPORT int INI_Open(const char *file);
DLLIMPORT int INI_Open_Mem(const char *src, int size);
DLLIMPORT void INI_Close(void);

DLLIMPORT int INI_ReadInt(const char *group, const char *entry, const int defval);
DLLIMPORT int INI_ReadBool(const char *group, const char *entry, const int defval);
DLLIMPORT float INI_ReadFloat(const char *group, const char *entry, const float defval);
DLLIMPORT const char *INI_ReadText(const char *group, const char *entry, const char *defval);

DLLIMPORT void INI_WriteInt(const char *group, const char *entry, const int val);
DLLIMPORT void INI_WriteBool(const char *group, const char *entry, const int val);
DLLIMPORT void INI_WriteFloat(const char *group, const char *entry, const float val);
DLLIMPORT void INI_WriteText(const char *group, const char *entry, const char *val);

#ifdef __cplusplus
} /* ends extern "C" */
#endif

#endif
