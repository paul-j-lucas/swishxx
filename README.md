# SWISH++: Simple Web Indexing System for Humans: C++ version

SWISH++ is a Unix-based file indexing and searching engine
(typically used to index and search files on web sites).
It was based on SWISH-E although SWISH++ is a complete rewrite.
SWISH++ was developed to circumvent my difficulties
with using the SWISH-E package.

SWISH++ has been ported to compile and run under Microsoft
Windows by Robert J. Lebowitz <lebowitz@finaltouch.com> and
Christoph Conrad <christoph.conrad@gmx.de>.

## Features

### Lightning-fast indexing
SWISH++ attains its speed chiefly by doing two things:
using good algorithms and data structures and by doing fast I/O.

1. SWISH++ uses the C++ Standard Template Library's `map` class
that is typically implemented either as a red-black or AVL tree
for which the worst-case running time is O(lg n).

2. SWISH++ uses the **mmap**(2) Unix system call to read files
instead of using standard I/O.
If you are unfamiliar with **mmap**,
it "maps" a file into memory
using the same virtual memory management mechanism
the operating system itself uses.
When the first character of a file is read,
a page fault occurs and the operating system
maps a page of the file into memory.
It is impossible to get faster access.
Additionally, because the file is in memory,
the characters in it are accessed via pointers
using simple pointer arithmetic
rather than through library function calls
and input buffers.

Other factors contributing to SWISH++'s speed
are that it does very little explicit dynamic memory allocation,
uses function inlining,
and makes very few function calls in inner loops.

### Indexes META elements, ALT, and other attributes
For HTML or XHTML files,
SWISH++ indexes words in `META` element `CONTENT` attributes
and associates them with the `NAME` attributes.
Meta names can later be queried against specifically, e.g.:

	search author = hawking

SWISH++ also indexes the words in `ALT` attributes
(for the `AREA`, `IMG`, and `INPUT` elements),
`STANDBY` attributes (for the `OBJECT` element),
`SUMMARY` attributes (for the `TABLE` element),
and
`TITLE` attributes (for any HTML or XHTML element).

### Selectively not index text within HTML or XHTML elements
Text within HTML or XHTML elements belonging to specified classes
can be not indexed.
This is most useful not to index text in common page headers,
footers, and pop-up menus.

### Intelligently index mail and news files
SWISH++ indexes words in headers
and associates them with the name of the headers as meta names
that can later be queried against specifically, e.g.:

	search subject = big-bang

Similarly, words in vCard fields are associated with the names of the fields
as meta names that can also later be queried against, e.g.:

	search title = professor
	search org = SLAC

Additionally, plain and enriched text, and HTML
in any one of ASCII, ISO-8859-1, UTF-7, or UTF-8 character sets
in any one of 7-bit, 8-bit, quoted-printable, or Base64 encodings
is decoded and converted on-the-fly
thus properly indexing encoded bodies and attachments.

Lastly, attachments having other MIME types can be filtered
on-the-fly before being indexed, e.g., convert Microsoft
Word or PDF attachments to plain text.

### Index Unix manual page files
SWISH++ indexes words in sections and associates them with
the name of the section as meta names that can later be
queried against specifically, e.g.:

	search description = environment
	search author = lucas

SWISH++ can therefore be used as a much better **apropos**(1)
command replacement.

### Index LaTeX and RTF documents 
SWISH++ can ignore LaTeX and RTF markup.
Additionally, for LaTeX documents,
SWISH++ sets the document title
to the content of the `\title{...}` command.

### Index ID3 tags of MP3 files
SWISH++ indexes words in ID3 tags of MP3 files
and associates them with the name of the fields as meta names
than can later be queried against specifically, e.g.:

	search artist = roxette
	search title = dangerous

All ID3 tag versions through 2.4 are supported.
Additionally, text fields in any one of
ASCII, ISO-8859-1, UTF-8, or UTF-16
character sets are supported.

### Index non-text files such as Microsoft Office documents
A separate text-extraction utility `extract` is included to
assist in indexing non-text files.
It is a essentially a more sophisticated version
of the Unix **strings**(1) command,
but employs the same word-determination heuristics used for indexing.

### Apply filters to files on-the-fly prior to indexing
Based on filename patterns,
files can be filtered before being indexed,
e.g.: compressed files uncompressed,
PDF files converted to plain text,
etc.

### Modular indexing architecture
New indexing modules can be written
to index other file formats directly
(without filters).

### Index new files incrementally
New files can be indexed and added to an existing index incrementally.

### Index remote web sites
A separate utility `httpindex` is included that interfaces
SWISH++ to the **wget**(1) command enabling remote web sites to
be indexed.
This is useful to be able to search all the servers
in your local area network simultaneously.

### Handles large collections of files
SWISH++ automatically splits and merges partial indices
for large collections of files as it goes
thereby not bringing your machine to its knees
by exhausting physical memory and causing it to swap like mad.

### Lightning-fast searching
The same **mmap**(2) technique used for indexing is used again for searching.
The generated index file is written to disk
such that it can be mmap'ed back into memory and binary searched immediately,
with no parsing of the data,
also in O(lg n) time.

### Optional word stemming (suffix stripping)
SWISH++ allows stemming to be performed at the time of searches,
not at the time of index generation.
This allows users to decide whether to perform stemming or not.

### Ability to run as a search server
SWISH++'s search engine can run in the background
as a multi-threaded daemon process
to function as a search server accepting query requests
and returning results via a Unix domain or TCP socket or both.
For search-intensive applications,
such as a search engine on a heavily used web site,
this can yield a large performance improvement
since the start-up cost (**fork**(2), **exec**(2), and initialization)
is paid only once.

### Easy-to-parse results format
SWISH++ outputs its search results in the form:

	rank path_name file_size file_title

By placing the `file_title`, which may contain spaces, last,
you can easily parse it, e.g., in Perl:

	($rank,$path,$size,$title) = split( / /, $_, 4 );

### XML results format
Alternatively, SWISH++ can output search results in XML
for increased interoperability with other XML applications.

### Generously commented source code
The source code is clearly written with lots of comments
including references to other works
in case you want to modify it
under the terms of the GNU general public license.

---

## Non-Features

The following is a list of the features SWISH++ does not have
that SWISH-E does.
I wrote SWISH++ to solve my immediate indexing problems;
therefore, I implemented only those features useful to me.
If others can also benefit from the work, great.
I may implement other features as time permits.

### Indexing and searching based on HTML tags
SWISH++ has no equivalent means for searching within specific HTML tags
(the SWISH-E `-t` option).
I didn't have a need for this feature.

### Document properties
This functionality can be achieved by using the `extract_meta()`
function in the included WWW Perl module.

### Crash and burn on files
SWISH++ will not crash while indexing any file.
Period.
If it does, there's a bug and I'll fix it.

---

Copyright (C) 1998-2015 by Paul J. Lucas &lt;<paul@lucasmail.org>&gt;  
SWISH++ is available under the GNU General Public License.
