/*
**	socklen.c
**
**	TEST	socket length type
**	DEFINE	PJL_SOCKLEN_NOT_INT
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
