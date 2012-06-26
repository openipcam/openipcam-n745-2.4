#ifndef LLIST_H
#define LLIST_H
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>

/*
 *  llist.h  -  header file for linked list routines
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *
 *  Description:
 *      this file conains the data structure and public function 
 *      prototypes for singly linked list routines
 *
 *  Dependencies:
 *      stdio.h
 *      malloc.h
 *
 *  Comments:
 *      in order to use the linked list routines, the file llist.c is need
 *      
 *      the rountines use XtMalloc(), XtFree() etc. so right now the
 *      routines are usable with X/Motif programs only.
 *
 *  Development History:
 *      who                     when        why
 *      ma_muquit@fccc.edu      30-Mar-1997 first cut
 */

/************************* NOTES *****************************************
Data Structure
--------------
typedef struct _EntryLink
{
    char
        *entry;

    struct _EntryLink
        *next;

} EntryLink;

typedef struct _Slink
{
    int
        nentries;

    char
        **entries;
} Slink;

Functions
---------
int AddtoLinkedList(EntryLink *head,char *string);
    ## add a link containing the specified string into the linked list
    ## pointed to by head
    ## returns 0 on success, -1 on failure
    ########################################################################

void ReleaseLinkedList(EntryLink **head)
    ## frees all links of the linked list pointer to by  head. set head to NULL
    ########################################################################

Slink *CreateArrayList(EntryLink *head)
    ## create an array of pointers to strings from the linked list pointed to
    ## by head. returns a pointer to a struct containing the array and
    ## entry point. this function releases the linked list and sets head to NULL
    ## returns pointer to NULL on failure.
    ########################################################################

int AddtoArrayList(EntryLink *list,char *string)
    ## Adds string to end of an array of strings
    ## returns 0 on success, -1 on failure
    ########################################################################

void DeleteFromArrayList(Slink **list,int n)
    ## delete nth string from an array of strings
    ########################################################################

Slink *CreateNullArrayList(void)
    ## create an empty array of pointers to strings.
    ## returns a pointer to a struct coantaining the array and entry point
    ########################################################################

Slink *DuplicateArrayList(Slink **list)
    ## copy a array list and returns a pointer to it
    ## returns NULL on failure
    ########################################################################

void ReleaseArrayList(Slink **list)
    ## release the memory occupied by the structs returned by
    ## CreateArrayList() and CreateNullArrayList()
    ########################################################################

int processTheFile(int direction,Slink *flist, int (*handler) (void *));
    ## Helper function to call a user supplied function. The user supplied
    ## function must take one argument of type void pointer. and the user 
    ## supplied function must return an int as it's exit status. This helper
    ## functionreturns the exit code of the uer supplied function
    ########################################################################
--
Muhammad A Muquit
documented: March-30-1997 (at home in the Linux box (2.0.27)

*******************************************************************/

#define FILE_NEXT       1
#define FILE_PREV       2
#define FILE_THIS       3
#define FILE_LOOP       4
#define FILE_JUST_ADDED 5


/* singly linked list */
typedef struct _EntryLink
{
    char
        *entry;

    struct _EntryLink
        *next;

} EntryLink;

typedef struct _Slink
{
    int
        nentries;

    char
        **entries;
} Slink;

/* function prototypes */
	
int AddtoLinkedList (EntryLink **,char *);
void ReleaseLinkedList (EntryLink **);
void ReleaseArrayList (Slink *);
int AddtoArrayList (Slink **,char *);
void DeleteFromArrayList (Slink **,int);
Slink *DuplicateArrayList (Slink *);
Slink *CreateArrayList (EntryLink **);
Slink *CreateNullArrayList (void);

/*
** Helper function to call a user supplied function. The user supplied
** function must take one argument of type void pointer. and the user supplied
** function must return an int as it's exit status. This helper function
** returns the exit code of the uer supplied function
*/
int processTheFile(int direction,Slink *flist,int (*handler) (void *));

/*
** the above routine uses a static int file counter, the user can reset it
** by calling the following function
*/
void resetFileCounter(int i);
int getFileCounter(Slink *);
int getRcode(void);


#endif  /* LLIST_H */
