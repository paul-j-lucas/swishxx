/*
**	namespaces.c
**
**	TEST	namespaces
**	FAIL	PJL_NO_NAMESPACES
*/

//
// If the C++ compiler supports namespaces, it will accept the following legal
// declaration.
//
namespace N {
	//
	// Make sure the C++ compiler not only supports namespaces in general,
	// but that it supports templates in namespaces specifically.
	//
	template< class T > class PT;
}
