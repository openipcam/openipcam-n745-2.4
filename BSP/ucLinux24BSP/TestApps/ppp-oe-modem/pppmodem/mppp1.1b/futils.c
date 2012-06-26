#ifdef HAVE_MOTIF
#include "xhead.h"
#endif

#include "mppp.h"

static char *prevSlash(char *ptr);
static char *nextSlash(char *ptr);
static int compareThruSlash(char *string1, char *string2);
static void copyThruSlash(char **toString, char **fromString);


/*
 *  parseFilename -- 
 *                  decompose a unix filename into a filename and a path 
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      0   on success
 *      -1  on failure
 *
 *  Parameters:
 *      char *fullname      passed filename (may be with path)
 *      char *filename      extracted filename (no path),  returns
 *      char *pathname      extracted path of filename, returns
 *
 *  Side Effects:
 *      filename, pathname are chnaged
 *
 *  Limitations and Comments:
 *      filename and pathname must have atleast MAXPATHLEN space allocated
 *      before passing.
 *
 *      All the routines are taken from nedit 4.0.3 source
 *      (ftp://ftp.fnal.gov/pub/nedit)
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-26-1997    first cut
 */


int parseFilename(char *fullname, char *filename, char *pathname)
{
    int
        full_len,
        i,
        path_len,
        file_len;

    if (*fullname == '\0')
        return (-1);

    full_len=strlen(fullname);

    /* find last slash */
    for (i=full_len-1; i >= 0; i--)
    {
        if (fullname[i] == '/')
            break;
    }

    /* move chars before / into pathname,& after into filename */
    path_len = i + 1;
    file_len = full_len - path_len;
    (void) strncpy(pathname,fullname,path_len);
    pathname[path_len]='\0';
    (void) strncpy(filename,&fullname[path_len],file_len);
    filename[file_len]='\0';

    return (normalizePathname(pathname));
}

int normalizePathname(char *pathname)
{
    char
        old_pathname[MAXPATHLEN],
        wd[MAXPATHLEN];

    /* if this is a relative pathname, prepend current directory */

    if (*pathname != '/')
    {
        /* make a copy of pathname to work from */
        (void) strncpy(old_pathname,pathname,MAXPATHLEN-1);

        /* get the working directory */
        getcwd(wd,MAXPATHLEN);

        /* prepend it to the path */
        (void) strncpy(pathname,wd,MAXPATHLEN-1);
        (void) strncat(pathname,"/",2);
        (void) strncat(pathname,old_pathname,MAXPATHLEN-strlen(pathname)-2);
    }

    /* compress out .. and . */

    return (compressPathname(pathname));
}

/* returns 0 on success -1 on failure */
int compressPathname(char *pathname)
{
    char
        *iptr,
        *optr;

    /* compress out . and .. */
    iptr=(&pathname[1]);          /* start after initial / */
    optr=(&pathname[1]);

    while (1)
    {
        /* if the next component is "../", remove previous component */
        if (compareThruSlash(iptr, "../"))
        {
            /* error if already at beginning of string */
            if (optr == &pathname[1])
                return (-1);

             /* back up outPtr to remove last path name component */
             optr=prevSlash(optr);
             iptr=nextSlash(iptr);
        }
        else if (compareThruSlash(iptr, "./"))
        {
            /* don't copy the component if it's the redundant "./" */
            iptr=nextSlash(iptr);
        }
        else
        {
            /* copy the component to outPtr */
            copyThruSlash(&optr,&iptr);
        }
        if (iptr == NULL)
            return (0);
    }

    return (0);
}


static char *nextSlash(char *ptr)
{
    for(; *ptr!='/'; ptr++) {
        if (*ptr == '\0')
            return NULL;
    }
    return ptr + 1;
}

static char *prevSlash(char *ptr)
{
    for(ptr -= 2; *ptr!='/'; ptr--);
    return ptr + 1;
}


static int compareThruSlash(char *string1, char *string2)
{
    while (1) 
    {
        if (*string1 != *string2)
            return (0);
        if (*string1 =='\0' || *string1=='/')
            return (1);
        string1++;
        string2++;
    }
}


static void copyThruSlash(char **toString, char **fromString)
{
    char *to = *toString;
    char *from = *fromString;

    while (1) {
        *to = *from;
        if (*from =='\0') {
            *fromString = NULL;
            return;
        }
        if (*from=='/') {
            *toString = to + 1;
            *fromString = from + 1;
            return;
        }
        from++;
        to++;
    }
}
