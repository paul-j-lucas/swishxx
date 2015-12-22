# SWISH++ charsets

The files in this directory handle some values for the charset parameter
of the `Content-Type` header (RFC 2045 section 5) used by the mail indexer,
to wit:
utf7 and utf8.

The values `us-ascii` and `iso8859-1` are easily handled internally;
other values are currently not handled at all.

The reason these files are here in this subdirectory rather than in `mod/mail`
is because, some day, there may be a way to specify non-mail files that are in,
say, the UTF-8 character set to be decoded.
If this ever happens,
then additional non-mail decoders could be added to this subdirectory,
e.g., UTF-16.
