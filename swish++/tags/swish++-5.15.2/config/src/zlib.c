/*
**	zlib.c
**
**	TEST	zlib.h
**	PASS	HAVE_ZLIB
*/

#include <zlib.h>

void f() {
	unsigned char dest[ 1024 ], src[ 1024 ];
	unsigned long dest_len;
	uncompress( dest, &dest_len, src, sizeof src );
}
