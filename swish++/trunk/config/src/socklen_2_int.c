/*
**	socklen_int.c
**
**	TEST	socket length type is int
**	PASS	PJL_SOCKLEN_TYPE int
*/

#include <sys/socket.h>

void f() {
	//
	// See if this socket implementation takes an int 3rd argument to
	// accept(2).
	//
	int len;
	::accept( 1, (struct sockaddr*)0, &len );
}
