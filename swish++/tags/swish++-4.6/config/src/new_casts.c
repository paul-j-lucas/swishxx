/*
**	new_casts.c
**
**	TEST	new-style casts
**	FAIL	PJL_NO_NEW_CASTS
*/

void f() {
	//
	// If the C++ compiler supports the new-style cast operators, it will
	// accept the following legal cast.
	//
	int i = static_cast<int>( 1.0 );
}
