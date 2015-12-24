/*
**      SWISH++
**      src/exit_codes.h
**
**      Copyright (C) 1998  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef exit_codes_H
#define exit_codes_H

// local
#include "config.h"

// exit(3) status codes
enum {
    // common to all executables
    Exit_Internal_Error             = 127,
    Exit_Success                    = 0,
    Exit_Config_File                = 1,
    Exit_Usage                      = 2,

    // unique to index
    Exit_No_Open_Temp               = 10,
    Exit_No_Write_Index             = 11,
    Exit_No_Write_Temp              = 12,
    Exit_Not_Root                   = 13,

    // unique to extract
    Exit_No_Such_File               = 20,

    // common between index and extract
    Exit_No_Read_Stopwords          = 30,

    // common between index and search
    Exit_No_Read_Index              = 40,

    // unique to search
    Exit_Malformed_Query            = 50,
#ifdef WITH_WORD_POS
    Exit_No_Word_Pos_Data           = 51,
#endif /* WITH_WORD_POS */
#ifdef WITH_SEARCH_DAEMON
    Exit_No_Write_PID               = 60,
    Exit_No_Host_or_IP              = 61,
    Exit_No_TCP_Socket              = 62,
    Exit_No_Unix_Socket             = 63,
    Exit_No_Unlink                  = 64,
    Exit_No_TCP_Bind                = 65,
    Exit_No_Unix_Bind               = 66,
    Exit_No_TCP_Listen              = 67,
    Exit_No_Unix_Listen             = 68,
    Exit_No_Select                  = 69,
    Exit_No_Accept                  = 70,
    Exit_No_Fork                    = 71,
    Exit_No_Change_Dir              = 72,
    Exit_No_Create_Thread           = 73,
    Exit_No_Create_Thread_Key       = 74,
    Exit_No_Detach_Thread           = 75,
    Exit_No_Init_Thread_Condition   = 76,
    Exit_No_Init_Thread_Mutex       = 77,
    Exit_No_User                    = 78,
    Exit_No_Group                   = 79,
#endif /* WITH_SEARCH_DAEMON */

    Exit_End_Enum_Marker
};

#endif /* exit_codes_H */
/* vim:set et sw=4 ts=4: */
