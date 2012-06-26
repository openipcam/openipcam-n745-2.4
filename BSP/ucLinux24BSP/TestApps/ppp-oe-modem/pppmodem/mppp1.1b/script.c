#ifdef HAVE_MOTIF
#include "xhead.h"
#endif

#include "mppp.h"

/* private prototypes */
static Script *initScript(void);

/*
 *  initScript() - initializes Script struct
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      pointer to struct Script        if succeeds
 *      NULL                            if fails
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    frist cut
 */

static Script *initScript(void)
{
    Script
        *sc;

    sc=(Script *) malloc(sizeof(Script));
    sc->qualifier=(char *) NULL;
    sc->value=(char *) NULL;
    return (sc);
}


/*
 *  addtoScriptList() - adds send-expect sequence to a linked list
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      0   on success
 *      -1  on failure
 *
 *  Parameters:
 *      Script **head               returns
 *      char   *send_or_expect      "send" or "expect"
 *      char   *value               value of "send" or "expect"
 *      
 *
 *  Side Effects:
 *
 *
 *  Limitations and Comments:
 *      When the very fist time this routine is called, head must be
 *      initialzed to NUL.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */
int addtoScriptList(Script **head,char *send_or_expect,char *value)
{

    static Script
        *sc,
        *tmp;

    Debug2("Send or Expect; %s",send_or_expect,0);
    Debug2("Value=%s",value,0);

    sc=initScript();
    if (sc == (Script *) NULL)
    {
        (void) fprintf(stderr,"malloc failed at addtoScriptList()\n");
        return (-1);
    }
    
    if (strcasecmp(send_or_expect,"send") == 0)
    {
        /* if xstrdup() fails, we'll exit */
        sc->qualifier=xstrdup("send");
        sc->value=xstrdup(value);
    }
    else if (strcasecmp(send_or_expect,"expect") == 0)
    {
        sc->qualifier=xstrdup("expect");
        sc->value=xstrdup(value);
    }

    if ((*head) == (Script *) NULL) /* first time */
    {
        (*head)=sc;
        sc->next=(Script *) NULL;
        tmp=sc;
    }
    else
    {
        tmp->next=sc;
        sc->next=(Script *) NULL;
        tmp=sc;
    }

    return (0);
}



/*
 *  readScript()    - opens and reads the script file
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      0  on success
 *      -1 on failure
 *
 *  Parameters:
 *      Script  **head      returns
 *      char    *sfile      path of the script file 
 *
 *  Side Effects:
 *      head is modified
 *
 *  Limitations and Comments:
 *      head must be initialized to NULL before calling this routine
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */


int readScript(Script **head,char *sfile)
{
    FILE
        *fp;

    int
        rc,
        n=0;

    char
        send_or_expect[MAX_STRING_LEN],
        value[MAX_STRING_LEN],
        w[MAX_STRING_LEN],
        l[MAX_STRING_LEN];

    rc=0;

    /* open the script file for reading */
    fp=fopen(sfile,"r");
    if (fp == (FILE *) NULL)
    {
        (void) fprintf(stderr,"Unable to read script file: %s\n",sfile);
        return (-1);
    }
    
    n=0;
    while (!(cfgGetline(l,MAX_STRING_LEN,fp)))
    {
        ++n;
        if ((*l != '#') && (*l != '\0'))
        {
            cfgGetword(w,l);
            (void) strcpy(send_or_expect,w);
            (void) strcpy(value,l);
            rc=addtoScriptList(head,send_or_expect,value);
            if (rc != 0)
                break;
        }
    }
    (void) fclose(fp);

    return (rc);
}


/*
 *  releaseScriptList() - free the script linked list
 * 
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Script **head
 *
 *  Side Effects:
 *      head is freed
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */

void releaseScriptList(Script **head)
{
    Script
        *sc;

    while (*head)
    {
        sc=(*head);
        (*head)=(*head)->next;
        if (sc->qualifier)
            (void) free (sc->qualifier);
        if (sc->value)
            (void) free (sc->value);

        if (sc)
            (void) free ((char *) sc);
    }
}



#ifdef TEST_MAIN

int main(int argc,char **argv)
{
    int
        rc;

    Script
        *sc,
        *head=(Script *) NULL;

    rc=readScript(&head,"./.mpppdir/fccc.scr");
    if (rc != 0)
        Error("reading script file\n");

    for (sc=head; sc; sc=sc->next)
    {
        if (sc->qualifer != (char *) NULL)
        {
            Debug2("%s => %s",sc->qualifier,sc->svalue);
        }
    }
    releaseScriptList(&head);
    exit(0);
}
#endif
