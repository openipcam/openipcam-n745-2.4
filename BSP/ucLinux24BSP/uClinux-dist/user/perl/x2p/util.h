/* $RCSfile: util.h,v $$Revision: 1.1.1.1 $$Date: 2006-07-11 09:32:05 $
 *
 *    Copyright (c) 1991-2001, Larry Wall
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 * $Log: util.h,v $
 * Revision 1.1.1.1  2006-07-11 09:32:05  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:32:47  andy
 * W90N745 uCLinux kernel
 *
 */

/* is the string for makedir a directory name or a filename? */

#define fatal Myfatal

#define MD_DIR 0
#define MD_FILE 1

#ifdef SETUIDGID
    int		eaccess();
#endif

char	*getwd();
int	makedir();

char * cpy2 ( char *to, char *from, int delim );
char * cpytill ( char *to, char *from, int delim );
void growstr ( char **strptr, int *curlen, int newlen );
char * instr ( char *big, char *little );
char * safecpy ( char *to, char *from, int len );
char * savestr ( char *str );
void croak ( char *pat, ... );
void fatal ( char *pat, ... );
void warn  ( char *pat, ... );
int prewalk ( int numit, int level, int node, int *numericptr );

Malloc_t safemalloc (MEM_SIZE nbytes);
Malloc_t safecalloc (MEM_SIZE elements, MEM_SIZE size);
Malloc_t saferealloc (Malloc_t where, MEM_SIZE nbytes);
Free_t   safefree (Malloc_t where);
