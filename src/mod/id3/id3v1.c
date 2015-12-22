/*
**      SWISH++
**      src/mod/id3/id3v1.c
**
**      Copyright (C) 2002  Paul J. Lucas
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

#ifdef  MOD_id3

// local
#include "id3v1.h"

extern id3v1_genre const id3v1_genre_table[] = {
    "blues",                 5, // 0
    "classic-rock",         12, // 1
    "country",               7, // 2
    "dance",                 5, // 3
    "disco",                 5, // 4
    "funk",                  4, // 5
    "grunge",                6, // 6
    "hip-hop",               7, // 7
    "jazz",                  4, // 8
    "metal",                 5, // 9
    "new-age",               7, // 10
    "oldies",                6, // 11
    "other",                 5, // 12
    "pop",                   3, // 13
    "r&b",                   3, // 14
    "rap",                   3, // 15
    "reggae",                6, // 16
    "rock",                  4, // 17
    "techno",                6, // 18
    "industrial",           10, // 19
    "alternative",          11, // 20
    "ska",                   3, // 21
    "death-metal",          11, // 22
    "pranks",                6, // 23
    "soundtrack",           10, // 24
    "euro-techno",          11, // 25
    "ambient",               7, // 26
    "trip-hop",              8, // 27
    "vocal",                 5, // 28
    "jazz-funk",             9, // 29
    "fusion",                6, // 30
    "trance",                6, // 31
    "classical",             9, // 32
    "instrumental",         12, // 33
    "acid",                  4, // 34
    "house",                 5, // 35
    "game",                  4, // 36
    "sound-clip",           10, // 37
    "gospel",                6, // 38
    "noise",                 5, // 39
    "altern-rock",          11, // 40
    "bass",                  4, // 41
    "soul",                  4, // 42
    "punk",                  4, // 43
    "space",                 5, // 44
    "meditative",           10, // 45
    "instrumental-pop",     16, // 46
    "instrumental-rock",    17, // 47
    "ethnic",                6, // 48
    "gothic",                6, // 49
    "darkwave",              8, // 50
    "techno-industrial",    17, // 51
    "electronic",           10, // 52
    "pop-folk",              8, // 53
    "eurodance",             9, // 54
    "dream",                 5, // 55
    "southern-rock",        13, // 56
    "comedy",                6, // 57
    "cult",                  4, // 58
    "gangsta",               7, // 59
    "top-40",                6, // 60
    "christian-rap",        13, // 61
    "pop-funk",              8, // 62
    "jungle",                6, // 63
    "native-american",      15, // 64
    "cabaret",               7, // 65
    "new-wave",              8, // 66
    "phychadelic",          11, // 67
    "rave",                  4, // 68
    "showtunes",             9, // 69
    "trailer",               7, // 70
    "lo-fi",                 5, // 71
    "tribal",                6, // 72
    "acid-punk",             9, // 73
    "acid-jazz",             9, // 74
    "polka",                 5, // 75
    "retro",                 5, // 76
    "musical",               7, // 77
    "rock-n-roll",          11, // 78
    "hard-rock",             9, // 79
    //
    // The remaining genres are WinAmp extensions.
    //
    "folk",                  4, // 80
    "folk-rock",             9, // 81
    "national-folk",        13, // 82
    "swing",                 5, // 83
    "fast-fusion",          11, // 84
    "bebob",                 5, // 85
    "latin",                 5, // 86
    "revival",               7, // 87
    "celtic",                6, // 88
    "bluegrass",             9, // 89
    "avantgarde",           10, // 90
    "gothic-rock",          11, // 91
    "progressive-rock",     16, // 92
    "psychedelic-rock",     16, // 93
    "symphonic-rock",       14, // 94
    "slow-rock",             9, // 95
    "big-band",              8, // 96
    "chorus",                6, // 97
    "easy-listening",       14, // 98
    "acoustic",              8, // 99
    "humour",                6, // 100
    "speech",                6, // 101
    "chanson",               7, // 102
    "opera",                 5, // 103
    "chamber-music",        13, // 104
    "sonata",                6, // 105
    "symphony",              8, // 106
    "booty-bass",           10, // 107
    "primus",                6, // 108
    "porn-groove",          11, // 109
    "satire",                6, // 110
    "slow-jam",              7, // 111
    "club",                  4, // 112
    "tango",                 5, // 113
    "samba",                 5, // 114
    "folklore",              8, // 115
    "ballad",                6, // 116
    "power-ballad",         12, // 117
    "rhythmic-soul",        13, // 118
    "freestyle",             9, // 119
    "duet",                  4, // 120
    "punk-rock",             9, // 121
    "drum-solo",             9, // 122
    "a-capella",             9, // 123
    "euro-house",           10, // 124
    "dance-hall",           10, // 125
};

#endif  /* MOD_id3 */
/* vim:set et sw=4 ts=4: */
