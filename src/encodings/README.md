# SWISH++ encodings

The files in this directory handle the values for the
`Content-Transfer-Encoding` header (RFC 2045 section 6)
used by the mail indexer,
to wit:
quoted-printable and base64.

The values 7bit and 8bit are easily handled internally as ASCII and ISO 8859-1,
respectively;
the values binary, and x-token are not handled at all.

The reason these files are here in this subdirectory rather than in `mod/mail`
is because, some day, there may be a way to specify non-mail files that are,
say, base-64 encoded to be decoded.
If this ever happens,
then additional non-mail decoders could be added to this subdirectory.
