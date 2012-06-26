/*
 *  routines for singly linked lists
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:36 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      taken from llnl-xftp 
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Return Values:
 *      value   description
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      if there's a problem with memory allocation, the routines will
 *      exit. will change it someday. but it works great with X programs.
 *      as if we fail while trying to allocate a small about of memory,
 *      we have no chance later in the program!
 *
 *      the rountines use XtMalloc(), XtFree() etc. so right now the
 *      routines are usable with X/Motif programs only.
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   23-Oct-95   first cut
 */

#include "llist.h"

#ifndef __FILE__
#define __FILE__    "?"
#endif


#ifndef __LINE__
#define __LINE__    0
#endif

#ifdef DEBUG
#define DEBUG_4(fmt,v1,v2,v3,v4)                             \
    {char * trick = (fmt);                                   \
    (void) fprintf (stderr,"%s(%d): ", __FILE__,__LINE__);   \
    (void) fprintf (stderr,(trick),(v1),(v2),(v3),(v4));     \
    (void) fprintf (stderr,"\n");}
    
#define DEBUG_2(fmt,v1,v2) DEBUG_4((fmt),(v1),(v2),0,0)
#else
#define DEBUG_4(fmt,v1,v2,v3,v4)
#define DEBUG_2(fmt,v1,v2)
#endif

static int
    Srcode=0,
    Sfile_counter=0;

/*
** add a link containing the specified string into the linked list
** pointed to by head
** returns: 0 on success, -1 on failure
*/
int AddtoLinkedList(EntryLink **head,char *s)
{
    EntryLink
        *ptr;

    ptr=(EntryLink *) malloc(sizeof(EntryLink));
    if (ptr == (EntryLink *) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed in AddtoLinkedList()\n");
        return(-1);
    }
    ptr->entry=strdup(s);
    if (ptr->entry == (char *) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed for strdup() in AddtoLinkedList()\n");
        return(-1);
    }
    ptr->next=(*head);
    (*head)=ptr;

    return(0);
}

/*
** free all links of the linked list pointer to by
** head. set head to NULL
*/

void ReleaseLinkedList(EntryLink **head)
{
    EntryLink
        *ptr;

    while (*head)
    {
        ptr=(*head);
        (*head)=(*head)->next;
        (void) free ((char *) ptr->entry);
        (void) free ((char *) ptr);
    }
}

/*
** create an array of pointers to strings from the linked list pointed to
** by head. returns a pointer to a struct containing the array and
** entry point. this funcion releases the lined list and sets head to NULL
**
** returns NULL on failure
*/

Slink *CreateArrayList(EntryLink **head)
{
    Slink
        *list;

    EntryLink
        *ptr;

    int
        nentries=0;

    int
        index=0;

    list=(Slink *) malloc(sizeof(Slink));
    if (list == (Slink *) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed in CreateArrayList()\n");
        return ((Slink *) NULL);
    }
    ptr=(*head);

    while (ptr)
    {
        nentries++;
        ptr=ptr->next;
    }
    list->nentries=nentries;

    list->entries=(char **) malloc(sizeof(char *)*nentries);
    if (list->entries == (char **) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed for list->entries in CreateArrayList()\n");
        (void) free ((char *) list);
        return ((Slink *) NULL);
    }

    index=nentries-1;

    while (*head)
    {
        list->entries[index--] = (*head)->entry;
        ptr=(*head);
        *head=(*head)->next;
        (void) free ((char *) ptr);
    }

return (list);
}

/*
** create an empty array of pointers to strings.
** returns a pointer to a struct coantaining the array and entry point
** returns a NULL pointer on failure
*/

Slink *CreateNullArrayList(void)
{
    Slink
        *list;

    list=(Slink *) malloc(sizeof(Slink));
    if (list == (Slink *) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed in CreateNullArrayList()\n");
        return ((Slink *) NULL);
    }

    list->nentries=0;
    list->entries=(char **) NULL;
return (list);
}


/*
** release the memory occupied by the structs returned by
** CreateArrayList() and CreateNullArrayList()
*/

void ReleaseArrayList(Slink *list)
{
    int
        i;

    for (i=0; i < list->nentries; i++)
        (void) free ((char *) list->entries[i]);

    (void) free ((char *) list->entries);
    (void) free ((char *) list);
}

/*
** adds string to end of an array of strings
** returns 0 on success, -1 on failure
*/

int AddtoArrayList(Slink **list,char *string)
{
    Slink
        *new_list;

    int
        i;

    new_list=(Slink *) malloc(sizeof(Slink));
    if (new_list == (Slink *) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed in AddtoArrayList()");
        return (-1);
    }

    new_list->nentries=(*list)->nentries+1;
    new_list->entries=(char **) malloc(sizeof(char *)*(new_list->nentries));
    if (new_list->entries == (char **) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed for new_list->entries in AddtoArrayList()\n");
        (void) free ((char *) new_list);
        return (-1);
    }


    for (i=0; i < (*list)->nentries; i++)
        new_list->entries[i]=(*list)->entries[i];

    new_list->entries[new_list->nentries-1]=strdup(string);
    if (new_list->entries[new_list->nentries-1] == (char *) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed for strdup in AddtoArrayList()\n");

        (void) free ((char *) new_list->entries);
        (void) free ((char *) new_list);

        return (-1);
    }

    (void) free ((char *) (*list)->entries);
    (void) free ((char *) (*list));

    *list=new_list;
    return (0);
}

/*
** delete nth string from an array of strings
*/

void DeleteFromArrayList(Slink **list,int n)
{
    int
        i;

    if (n > (*list)->nentries)
    {
        (void) fprintf (stderr,
            "Problem in DeleteFromArrayList()\n");
        return;
    }
    (void) free ((*list)->entries[n]);

    for (i=n; i < (*list)->nentries-1; i++)
        (*list)->entries[i]=(*list)->entries[i+1];

    (*list)->nentries--;
}

/*
** copy a array list and returns a pointer to it
** return a pointer to Slink struct.
** returns NULL on failure
** caller is responsible for freeing the memory.
*/

Slink *DuplicateArrayList(Slink *list)
{
    Slink
        *new_list;

    int
        i;

    new_list=(Slink *) malloc(sizeof(Slink));
    if (new_list == (Slink *) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed in AddtoArrayList()");
        return ((Slink *) NULL);
    }

    new_list->entries=(char **) malloc((list->nentries)*sizeof(char *));
    if (new_list->entries == (char **) NULL)
    {
        (void) fprintf (stderr,
            "malloc failed for new_list->entries in AddtoArrayList()\n");
        (void) free ((char *) new_list);
        return ((Slink *) NULL);
    }
    /* initialize with NULL*/
    for (i=0; i < list->nentries; i++)
    {
        new_list->entries[i]=(char *) NULL;
    }

    for (i=0; i < list->nentries; i++)
    {
        new_list->entries[i]=strdup(list->entries[i]);
        if (new_list->entries[i] == (char *) NULL)
        {
            (void) fprintf (stderr,
                "malloc failed for strdup in DuplicateArrayList()\n");
            for (i=0; i < list->nentries; i++)
            {
                if (new_list->entries[i] != (char *) NULL)
                    (void) free ((char *) new_list->entries[i]);
            }
            (void) free ((char *) new_list->entries);
            (void) free ((char *) new_list);
        }
    }
    new_list->nentries=list->nentries;

    return (new_list);
}

/*
** Helper function to call a user supplied function. The user supplied
** function must take one argument of type void pointer. and the user supplied
** function must return an int as it's exit status. This helper function
** returns the exit code of the uer supplied function in normal situation.
** if there'r no more files to go for, it will return -11.
*/
int processTheFile(int direction,Slink *flist,int (*handler) (void *))
{
    DEBUG_2("Sfile_counter=%d",Sfile_counter,0);
    switch(direction)
    {
        case FILE_NEXT:
        {
            DEBUG_2("XXXXXXXXXXXXXXXXXXhere...",0,0);
            Sfile_counter++;
            if (Sfile_counter > flist->nentries)
                Sfile_counter=flist->nentries;

            if (Sfile_counter == flist->nentries)
            {
                 DEBUG_2("No more files to go FORWARD",0,0);

                 Srcode=(-11);
                 return(-11);
            }
            break;
        }

        case FILE_PREV:
        {
            if (flist->nentries > 0)
            {
                if (Sfile_counter == 0)
                {
                    DEBUG_2("No more files to go BACKWARD",0,0);
                    Srcode=(-12);
                    return(-12);
                }

                if ((Sfile_counter > 0) && (Sfile_counter == flist->nentries))
                    Sfile_counter -= 2;
                else
                    Sfile_counter--;

                if (Sfile_counter < 0)
                    Sfile_counter=0;
            }

            break;
        }

        case FILE_THIS:
        {
            break;
        }

    }   /* end switch */

        /* called from MIDI repeat loop */
    if (direction == FILE_LOOP)
    {
        int
            rc;
        if ((Sfile_counter > flist->nentries) || 
            (Sfile_counter == flist->nentries))
            Sfile_counter=0;
        if (Sfile_counter < 0)
            Sfile_counter=0;

        rc=(*handler) (flist->entries[Sfile_counter]);
        Sfile_counter++;
        return(rc);
    }
    else
    {
        return ((*handler) (flist->entries[Sfile_counter]));
    }
}

/* reset the static file counter */
void resetFileCounter(int i)
{
    Sfile_counter=i;
}

/* return the current file counter 
** if the counter is more than the total number of entries, the counter
** is reset to 0, if it is less than 0, it's reset to 0 as well
*/

int  getFileCounter(Slink *flist)
{
    if (Sfile_counter >= flist->nentries)
        Sfile_counter=0;
    else if (Sfile_counter < 0)
        Sfile_counter=0;

    return (Sfile_counter);
}

int getRcode()
{
    return (Srcode);
}
