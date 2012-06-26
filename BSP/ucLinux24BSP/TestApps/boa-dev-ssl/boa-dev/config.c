/* vi:set tabstop=2 cindent shiftwidth=2: */
/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Some changes (C) 1998 Martin Hinner <martin@tdp.cz>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* boa: config.c */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

#include "boa.h"

static char g_MimeTypeDef[] =
	"text/plain asc txt\377"
	"video/x-msvideo avi\377"
	"image/gif gif\377"
	"text/html html htm\377"
	"image/jpeg jpe jpeg jpg\377"
	"\377";
/*
	"application/postscript ps eps\377"
	"application/pgp pgp\377"
	"audio/x-aiff aif aifc aiff\377"
	"text/plain asc txt\377"
	"audio/ulaw au\377"
	"video/x-msvideo avi\377"
	"application/x-bcpio bcpio\377"
	"application/octet-stream bin\377"
	"application/x-netcdf cdf\377"
	"application/x-cpio cpio\377"
	"application/x-csh csh\377"
	"application/x-dvi dvi\377"
	"text/x-setext etx\377"
	"application/andrew-inset ez\377"
	"image/gif gif\377"
	"application/x-gtar gtar\377"
	"application/x-gunzip gz\377"
	"application/x-hdf hdf\377"
	"text/html html htm\377"
	"image/ief ief\377"
	"image/jpeg jpe jpeg jpg\377"
	"application/x-latex latex\377"
	"application/x-troff-man man\377"
	"application/x-troff-me me\377"
	"application/x-mif mif\377"
	"video/quicktime mov qt\377"
	"video/x-sgi-movie movie\377"
	"video/mpeg mp2 mpe mpeg mpg\377"
	"application/x-troff-ms ms\377"
	"application/x-netcdf nc\377"
	"application/oda oda\377"
	"image/x-portable-bitmap pbm\377"
	"application/pdf pdf\377"
	"image/x-portable-graymap pgm\377"
	"application/x-chess-pgn pgn\377"
	"image/x-portable-anymap pnm\377"
	"image/x-portable-pixmap ppm\377"
	"image/x-cmu-raster ras\377"
	"image/x-rgb rgb\377"
	"application/x-troff roff\377"
	"application/rtf rtf\377"
	"text/richtext rtx\377"
	"application/x-sh sh\377"
	"application/x-shar shar\377"
	"audio/basic snd\377"
	"application/x-wais-source src\377"
	"application/x-sv4cpio sv4cpio\377"
	"application/x-sv4crc sv4crc\377"
	"application/x-troff t tr\377"
	"application/x-tar tar\377"
	"application/x-tcl tcl\377"
	"application/x-tex tex\377"
	"application/x-texinfo texi texinfo\377"
	"image/tiff tif tiff\377"
	"text/tab-separated-values tsv\377"
	"application/x-ustar ustar\377"
	"audio/x-wav wav\377"
	"image/x-xbitmap xbm\377"
	"image/x-xpixmap xpm\377"
	"image/x-xwindowdump xwd\377"
	"application/zip zip\377"
	"\377";
*/

/* these came from config.c */
char *document_root = "./";

char *directory_index = "index.htm";
char *default_type = "application/octet-stream";

int ka_timeout = 0;
int ka_max = 0;

/* Need to be able to limit connections */
int max_connections = -1; /* -1 = unlimited */

/* These come from boa_grammar.y */
void add_mime_type(char * extension, char * type);

/*
 * Name: read_config_files
 *
 * Description: Reads config files via yyparse, then makes sure that
 * all required variables were set properly.
 */
void read_config_files(void)
{
	char *pcMimeType = NULL;
	char *pcExtType = NULL;
	char *pc;

	for (pcMimeType = pc = g_MimeTypeDef; ; pc++)
	{
		if (*pcMimeType == '\377') break;
		if (*pc == '\40')
		{
			*pc = '\0';
			if (pcExtType != NULL)
				add_mime_type(pcExtType, pcMimeType);
			pcExtType = pc + 1;
		}
		else if (*pc == '\377')
		{
			*pc = '\0';
			if (pcExtType != NULL)
				add_mime_type(pcExtType, pcMimeType);
			pcExtType = NULL;
			pcMimeType = pc + 1;
		}
	}
}

