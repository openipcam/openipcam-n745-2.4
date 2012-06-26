/* $RCSfile: hash.h,v $$Revision: 1.1.1.1 $$Date: 2006-07-11 09:32:05 $
 *
 *    Copyright (c) 1991-2001, Larry Wall
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 * $Log: hash.h,v $
 * Revision 1.1.1.1  2006-07-11 09:32:05  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:32:47  andy
 * W90N745 uCLinux kernel
 *
 */

#define FILLPCT 60		/* don't make greater than 99 */

#ifdef DOINIT
char coeff[] = {
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1,
		61,59,53,47,43,41,37,31,29,23,17,13,11,7,3,1};
#else
extern char coeff[];
#endif

typedef struct hentry HENT;

struct hentry {
    HENT	*hent_next;
    char	*hent_key;
    STR		*hent_val;
    int		hent_hash;
};

struct htbl {
    HENT	**tbl_array;
    int		tbl_max;
    int		tbl_fill;
    int		tbl_riter;	/* current root of iterator */
    HENT	*tbl_eiter;	/* current entry of iterator */
};

bool hdelete (HASH *tb, char *key);
STR * hfetch ( HASH *tb, char *key );
int hiterinit ( HASH *tb );
char * hiterkey ( HENT *entry );
HENT * hiternext ( HASH *tb );
STR * hiterval ( HENT *entry );
HASH * hnew ( void );
void hsplit ( HASH *tb );
bool hstore ( HASH *tb, char *key, STR *val );
