/*
**	SWISH++
**	index_header.c
**
**	Copyright (C) 2001  Paul J. Lucas
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

/*
**	Note that this file is #include'd into index.c because it would be
**	annoying to make a function out of it.
*/

#ifdef	WRITE_HEADER
	long const num_dirs           = dir_list.size();
	long const num_files          = file_info::num_files();
	long const num_stop_words     = stop_words->size();
	long const num_meta_names     = meta_names.size();

	off_t *const word_offset      = new off_t[ num_unique_words ];
	off_t *const stop_word_offset = num_stop_words ?
					new off_t[ num_stop_words ] : 0;
	off_t *const dir_offset       = new off_t[ num_dirs ];
	off_t *const file_offset      = new off_t[ num_files ];
	off_t *const meta_name_offset = num_meta_names ?
					new off_t[ num_meta_names ] : 0;

	my_write( o, &num_unique_words, sizeof( num_unique_words ) );
	streampos const word_offset_pos = o.tellp();
	my_write( o, word_offset, num_unique_words * sizeof( word_offset[0] ) );

	my_write( o, &num_stop_words, sizeof( num_stop_words ) );
	streampos const stop_word_offset_pos = o.tellp();
	if ( num_stop_words )
		my_write( o, stop_word_offset,
			num_stop_words * sizeof( stop_word_offset[0] )
		);

	my_write( o, &num_dirs, sizeof( num_dirs ) );
	streampos const dir_offset_pos = o.tellp();
	my_write( o, dir_offset, num_dirs * sizeof( dir_offset[0] ) );

	my_write( o, &num_files, sizeof( num_files ) );
	streampos const file_offset_pos = o.tellp();
	my_write( o, file_offset, num_files * sizeof( file_offset[0] ) );

	my_write( o, &num_meta_names, sizeof( num_meta_names ) );
	streampos const meta_name_offset_pos = o.tellp();
	if ( num_meta_names )
		my_write( o, meta_name_offset,
			num_meta_names * sizeof( meta_name_offset[0] )
		);
#endif
#ifdef	REWRITE_HEADER
	o.seekp( word_offset_pos );
	my_write( o, word_offset, num_unique_words * sizeof( word_offset[0] ) );
	if ( num_stop_words ) {
		o.seekp( stop_word_offset_pos );
		my_write( o, stop_word_offset,
			num_stop_words * sizeof( stop_word_offset[0] )
		);
	}
	o.seekp( dir_offset_pos );
	my_write( o, dir_offset, num_dirs * sizeof( dir_offset[0] ) );
	o.seekp( file_offset_pos );
	my_write( o, file_offset, num_files * sizeof( file_offset[0] ) );
	if ( num_meta_names ) {
		o.seekp( meta_name_offset_pos );
		my_write( o, meta_name_offset,
			num_meta_names * sizeof( meta_name_offset[0] )
		);
	}

	delete[] word_offset;
	delete[] stop_word_offset;
	delete[] dir_offset;
	delete[] file_offset;
	delete[] meta_name_offset;
#endif
