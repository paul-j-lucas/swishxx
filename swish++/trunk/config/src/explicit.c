/*
**	explicit.c
**
**	TEST	explicit
**	FAIL	PJL_NO_EXPLICIT
*/

struct S {
	//
	// If the C++ compiler supports explicit constructors, it will accept
	// the following legal declaration.
	//
	explicit S( int );
};
/* vim:set noet sw=8 ts=8: */
