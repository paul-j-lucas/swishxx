/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#ifndef __dj_include_fnmatch_h_
#define __dj_include_fnmatch_h_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	FNM_NOESCAPE
#undef	FNM_NOESCAPE
#endif
#ifdef	FNM_PATHNAME
#undef	FNM_PATHNAME
#endif
#ifdef	FNM_PERIOD
#undef	FNM_PERIOD
#endif
#ifdef	FNM_NOCASE
#undef	FNM_NOCASE
#endif

#define	FNM_NOESCAPE	0x01
#define	FNM_PATHNAME	0x02
#define	FNM_PERIOD	0x04
#define	FNM_NOCASE	0x08

#ifdef	FNM_NOMATCH
#undef	FNM_NOMATCH
#endif

#define	FNM_NOMATCH	1

int fnmatch( const char *pattern, const char *string, int flags );

#ifdef __cplusplus
}
#endif

#endif /* !__dj_include_fnmatch_h_ */
/* vim:set noet sw=8 ts=8: */
