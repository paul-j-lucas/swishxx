/*
**	socklen_unsigned.c
**
**	TEST	socket length type is unsigned
**	PASS	PJL_SOCKLEN_TYPE unsigned
*/

#include <sys/socket.h>

void f() {
	//
	// See if this socket implementation takes an unsigned 3rd argument to
	// accept(2).
	//
	unsigned len;
	::accept( 1, (struct sockaddr*)0, &len );
}
