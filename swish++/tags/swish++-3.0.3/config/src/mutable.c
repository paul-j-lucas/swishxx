/*
**	mutable.c
**
**	TEST	mutable
**	DEFINE	PJL_NO_MUTABLE
*/

struct S {
	//
	// If the C++ compiler supports the mutable storage-class-specifier,
	// it will accept the following legal declaration.
	//
	mutable int i;
};
