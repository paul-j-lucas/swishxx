/*
**	SWISH++
**	exit_codes.h
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

#ifndef	exit_codes_H
#define	exit_codes_H

// exit(3) status codes
enum {
	// common to all executables
	Exit_Success		= 0,
	Exit_Config_File	= 1,
	Exit_Usage		= 2,

	// unique to index
	Exit_No_Open_Temp	= 10,
	Exit_No_Write_Index	= 11,
	Exit_No_Write_Temp	= 12,

	// common between index and extract
	Exit_No_Read_Stopwords	= 30,

	// unique to search
	Exit_No_Read_Index	= 50,
	Exit_Malformed_Query	= 51,
#ifdef	SEARCH_DAEMON
	Exit_No_Socket		= 52,
	Exit_No_Unlink		= 53,
	Exit_No_Bind		= 54,
	Exit_No_Listen		= 55,
	Exit_No_Accept		= 56,
	Exit_No_Fork		= 57,
	Exit_No_Change_Dir	= 58,
	Exit_No_Create_Thread	= 59,
	Exit_No_Detach_Thread	= 60,
#endif
};

#endif	/* exit_codes_H */
