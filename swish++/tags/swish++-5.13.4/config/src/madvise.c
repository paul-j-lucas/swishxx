/*
**	madvise.c
**
**	TEST	madvise(2)
**	FAIL	PJL_NO_MADVISE
*/

#include <sys/types.h>
#include <sys/mman.h>

void f() {
	//
	// See if madvise(2) exists.
	//
	::madvise( 0, 0, MADV_NORMAL );
}
