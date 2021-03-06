Test File Format
================

Test files must be a single line in the following format:

*command* `|` *config* `|` *options* `|` *input* `|` *exit*

that is five fields separated by the pipe (`|`) character
(with optional whitespace)
where:

+ *command* = command to execute (`index`, `search`, or `extract`)
+ *config*  = name of config file to use or blank for none
+ *options* = command-line options or blank for none
+ *input*   = name of file(s) to index or extract **OR** query
+ *exit*    = expected exit status code

Note on Test Names
------------------

Care must be taken when naming files that differ only in case
because of case-insensitive (but preserving) filesystems like HFS+
used on Mac OS X.

For example, tests such as these:

    ad-B2-e1-m.test
    ad-B2-E1-m.test

that are the same test but differ only in 'e' vs. 'E' will work fine
on every other Unix filesystem but cause a collision on HFS+.

One solution (the one used here) is to append a distinct number:

    ad-B2-e1-m_01.test
    ad-B2-E1-m_02.test

thus making the filenames unique regardless of case.
