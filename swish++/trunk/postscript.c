/*
**	SWISH++
**	postscript.c
**
**	Copyright (C) 1998  Paul J. Lucas
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// local
#include "postscript.h"

//*****************************************************************************
//
// SYNOPSIS
//
	postscript_comment_set::postscript_comment_set()
//
// DESCRIPTION
//
//	Construct (initialize) a postscript_comment_set.  These comments are
//	used to detect the start of encapsulated PostScript in files so it will
//	not be extracted.
//
//*****************************************************************************
{
	insert( "%%BeginSetup" );
	insert( "%%BoundingBox" );
	insert( "%%Creator" );
	insert( "%%EndComments" );
	insert( "%%Title" );
}

//*****************************************************************************
//
// SYNOPSIS
//
	postscript_operator_set::postscript_operator_set()
//
// DESCRIPTION
//
//	Construct (initialize) a postscript_operator_set.  The set of Level 2
//	PostScript operators that are not also English words are removed from
//	files since, if found, they most likely have encapsulated PostScript
//	(EPS) in them.
//
// SEE ALSO
//
//	Adobe Systems Incorporated.  "PostScript Langauge Reference Manual, 2nd
//	ed."  Addison-Wesley, Reading, MA.  pp. 346-359.
//
//*****************************************************************************
{
	static char const *const postscript_table[] = {
		"aload",
		"anchorsearch",
		"arcn",
		"ashow",
		"astore",
		"awidthshow",
		"banddevice",
		"bitshift",
		"bytesavailable",
		"cachestatus",
		"charpath",
		"cleardictstack",
		"cleartomark",
		"clippath",
		"closefile",
		"closepath",
		"colorimage",
		"concat",
		"concatmatrix",
		"copypage",
		"countdictstack",
		"countexecstack",
		"counttomark",
		"cshow",
		"currentblackgeneration",
		"currentcacheparams",
		"currentcmykcolor",
		"currentcolor",
		"currentcolorrendering",
		"currentcolorscreen",
		"currentcolorspace",
		"currentcolortransfer",
		"currentdash",
		"currentdevparams",
		"currentdict",
		"currentfile",
		"currentflat",
		"currentfont",
		"currentglobal",
		"currentgray",
		"currentgstate",
		"currenthalftone",
		"currenthsbcolor",
		"currentlinecap",
		"currentlinejoin",
		"currentlinewidth",
		"currentmatrix",
		"currentmiterlimit",
		"currentobjectformat",
		"currentoverprint",
		"currentpacking",
		"currentpagedevice",
		"currentpoint",
		"currentrgbcolor",
		"currentscreen",
		"currentstrokeadjust",
		"currentsystemparams",
		"currenttransfer",
		"currentundercolorremoval",
		"currentuserparams",
		"curveto",
		"cvi",
		"cvlit",
		"cvn",
		"cvr",
		"cvrs",
		"cvx",
		"def",
		"defaultmatrix",
		"definefont",
		"defineresource",
		"defineuserobject",
		"deletefile",
		"dict",
		"dictstack",
		"dtransform",
		"dup",
		"eoclip",
		"eofill",
		"erasepage",
		"errordict",
		"exch",
		"exec",
		"execform",
		"execstack",
		"execuserobject",
		"executeonly",
		"filenameforall",
		"fileposition",
		"findencoding",
		"findfont",
		"findresource",
		"flattenpath",
		"flushfile",
		"FontDirectory",
		"forall",
		"framedevice",
		"gcheck",
		"getinterval",
		"globaldict",
		"GlobalFontDirectory",
		"glyphshow",
		"grestore",
		"grestoreall",
		"gsave",
		"gsetstate",
		"gstate",
		"identmatrix",
		"idiv",
		"idtransform",
		"ifelse",
		"imagemask",
		"ineofill",
		"infill",
		"initclip",
		"initgraphics",
		"initmatrix",
		"instroke",
		"inueofill",
		"inufill",
		"inustroke",
		"invertmatrix",
		"ISOLatin1Encoding",
		"itransform",
		"kshow",
		"languagelevel",
		"lineto",
		"makefont",
		"makepattern",
		"maxlength",
		"mod",
		"moveto",
		"mul",
		"neg",
		"newpath",
		"noaccess",
		"nulldevice",
		"packedarray",
		"pathbbox",
		"pathforall",
		"printobject",
		"pstack",
		"putinterval",
		"rand",
		"rcheck",
		"rcurveto",
		"readhexstring",
		"readline",
		"readonly",
		"readstring",
		"realtime",
		"rectclip",
		"rectfill",
		"rectstroke",
		"renamefile",
		"renderbands",
		"resetfile",
		"resourceforall",
		"resourcestatus",
		"reversepath",
		"rlineto",
		"rmoveto",
		"rootfont",
		"rrand",
		"scalefont",
		"selectfont",
		"serialnumber",
		"setbbox",
		"setblackgeneration",
		"setcachedevice",
		"setcachedevice2",
		"setcachelimit",
		"setcharwidth",
		"setcmykcolor",
		"setcolor",
		"setcolorrendering",
		"setcolorscreen",
		"setcolorspace",
		"setcolortransfer"
		"setdash",
		"setdevparams",
		"setfileposition",
		"setflat",
		"setfont",
		"setglobal",
		"setgray",
		"sethalftone",
		"sethsbcolor",
		"setlinecap",
		"setlinejoin",
		"setlinewidth",
		"setmatrix",
		"setmiterlimit",
		"setobjectformat",
		"setoverprint",
		"setpagedevice",
		"setpattern",
		"setrgbcolor",
		"setscreen",
		"setstrokeadjust",
		"setsystemparams",
		"settransfer",
		"setucacheparams",
		"setundercolorremoval",
		"setuserparams",
		"setvmthreshold",
		"showpage",
		"srand",
		"StandardEncoding",
		"startjob",
		"statusdict",
		"stringwidth",
		"strokepath",
		"sub",
		"systemdict",
		"uappend",
		"ucache",
		"ucachestatus",
		"ueofill",
		"ufill",
		"undefinefont",
		"undefineresource",
		"undefineuserobject",
		"userdict",
		"UserObjects",
		"usertime",
		"ustroke",
		"ustrokepath",
		"vmreclaim",
		"vmstatus",
		"wcheck",
		"widthshow",
		"writehexstring",
		"writeobject",
		"writestring",
		"xcheck",
		"xshow",
		"xyshow",

		0
	};

	for ( register char const *const *p = postscript_table; *p; ++p )
		insert( *p );
}
/* vim:set noet sw=8 ts=8: */
