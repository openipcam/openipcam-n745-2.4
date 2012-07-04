
zlib - a data compression library routines that provide a safe, free
	and unpatented implementation of the deflate compression algorithm.

	Written by Mark Adler and Jean-loup Gailly. 


This is the standard code for zlib taken from v1.1.3. It has been passed
through indent and the prototypes have been converted to ANSI.

One source file has been added to the library, unzip.c (and 1 header file,
armzip.h). This is a version of uncompress that uses a less rigorous header
check. It also understands the extra fields so that gzip and pkzip (winzip)
files can be uncompressed. Nothing else has been changed.

Look at the files in Docs for further information or see the zlib home
page for details:

	http://www.info-zip.org/pub/infozip/zlib/


One example which shows usage of zlib is included in the Sources directory:
armload.c. This is a very simple loader which uncompresses the specified
image as it copies it to dynamic memory. It then passes control to the
uncompressed image.

To use armload, compress an ARM executable (such as standalone/tick.axf)
and program it into flash at the specified block using afu. Then use the
debugger to load the semihosted version of armload and then execute. One
point to note is that afu normally removes the file header before flashing
the image. armload attempts to strip the header so that the image is
copied to the correct address.



Copyright notice:

 (C) 1995-1998 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.


 unzip.c, armzip.h and armload.c are

 Copyright ARM Limited 1999 - 2000.  All rights reserved.

