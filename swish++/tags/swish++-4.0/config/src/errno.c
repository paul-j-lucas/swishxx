/*
**	errno.c
**
**	TEST	thread-safe errno
**	ERROR	"errno" does not appear to be thread-safe; see the comment for
**	ERROR	SEARCH_DAEMON in config.mk.
*/

#ifdef	SEARCH_DAEMON

#include <cerrno>

#ifndef	errno
	This is an intentional error to get compilation to fail if errno is not
	defined as a macro.
#endif

#endif	/* SEARCH_DAEMON */
