/*
**	socklen_1_socklen_t.c
**
**	TEST	socket length type is socklen_t
**	PASS	PJL_SOCKLEN_TYPE socklen_t
*/

#include <sys/types.h>
#include <sys/socket.h>

void f() {
	//
	// See if this socket implementation takes a socklen_t 3rd argument to
	// accept(2).
	//
	socklen_t len;
	::accept( 1, (struct sockaddr*)0, &len );
}
/* vim:set noet sw=8 ts=8: */
