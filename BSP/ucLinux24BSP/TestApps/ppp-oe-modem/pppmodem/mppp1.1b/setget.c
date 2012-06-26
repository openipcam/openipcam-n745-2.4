#ifdef HAVE_MOTIF
#include "xhead.h"
#endif

#include "mppp.h"

/*
 *  saveSpeed()  - sets the speed for routine getSpeed()
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int speed       speed to set
 *
 *  Side Effects:
 *      static variable speed is changed.
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-30-1997    first cut
 */

static int
    s_parity=0,
    s_speed=0;

static int
    pppd_id=(-1);

static int
    s_nscript=(-1);

static int
    s_pppd_state=0;

void saveSpeed(int s)
{
    s_speed=s;
}



/*
 *  getSpeed() - returns the speed set by the routine saveSpeed()
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      s_speed > 0     if there's already a speed set
 *      0               if no speed is set by saveSpeed
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      should be called after making a call to saveSpeed
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-30-1997    first cut
 */

int getSpeed(void)
{
    return (s_speed);
}


/*
 *  saveParity()  - sets the Parity for routine getParity()
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int Parity       Parity to set
 *
 *  Side Effects:
 *      static variable Parity is changed.
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-30-1997    first cut
 */

void saveParity(int p)
{
    s_parity=p;
}


/*
 *  getParity() - returns the parity set by the routine saveParity()
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      s_parity > 0     if there's already a parity set
 *      0               if no parity is set by saveparity
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      should be called after making a call to saveParity
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-30-1997    first cut
 */

int getParity(void)
{
    return (s_parity);
}

/* set and get pppd id */

void setpppdId(int id)
{
    pppd_id=id;
}

int getpppdId(void)
{
    return (pppd_id);
}


/*
 *  setScriptnum()  - sets number of items in script 
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      number of items     if any
 *      -1                  otherwise
 *
 *  Parameters:
 *      int n               set this number
 *
 *  Side Effects:
 *      n/a
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

void setScriptnum(int n)
{
    s_nscript=n;
}

/*
 *  getScriptnum()  - returns the number of items in script set previously
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      number of items     if any
 *      -1                  otherwise
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      n/a
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

int getScriptnum(void)
{
    return(s_nscript);
}


/*
 *  setPPPDState() - set the pppd state
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int state       0 or 1, 1 means pppd is up, 0 means pppd is dead
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

void setPPPDState(int state)
{
    s_pppd_state=state;
}



/*
 * getPPPDState()   - gets the ppd state as set by setPPPState 
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *
 *
 *  Parameters:
 *
 *
 *  Side Effects:
 *
 *
 *  Limitations and Comments:
 *      s_pppd_state must be set earlier
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

int getPPPDState(void)
{
    return (s_pppd_state);
}



/*
 *  setLogfile() - set the logfile path 
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      char **s        file to set
 *
 *  Side Effects:
 *      logfile is changed
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-14-1997    first cut
 */


static char *logfile=(char *) NULL;

void setLogfile(char *s)
{
    if (*s == '\0')
        return;

    if (logfile != (char *) NULL)
    {
        (void) free ((char *) logfile);
        logfile=(char *) NULL;
    }

    if (s != (char *) NULL)
    {
        logfile=(char *) malloc(strlen(s)+1);
        if (logfile != (char *) NULL)
            (void) strcpy(logfile,s);

    }
}


/*
 *  getLogfile() - send the log file previously set with setLogfile()
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      char    *logfile        NULL if no logfile is set
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
 *      ma_muquit@fccc.edu   Jun-14-1997    first cut
 */

char *getLogfile(void)
{
    return (logfile);
}
