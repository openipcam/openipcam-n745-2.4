#ifndef MPPP_H
#define MPPP_H
/*
 *  mppp.h  -   header file for mppp
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *
 *  Development History:
 *      who                     when        why
 *      ma_muquit@fccc.edu      17-Nov-1996 first cut
 */

#include <stdarg.h>
#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <signal.h>
#include <termio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>

/* defines */
#define MPPP_VERSION     "1.1b"
#define MPP_INIFILE     "/etc/mppp.cfg"

#define MOEM_BUSY_STR           "BUSY"
#define MODEM_NOCARRIER_STR     "NO CARRIER"
#define MODEM_NODIALTONE_STR    "NO DIALTONE"

#define MODEM_BUSY              100
#define MODEM_NO_CARRIER        101
#define MODEM_NO_DIALTONE       102
#define MODEM_CANCEL_DIALING    103

#define MODEM_INITIALIZING      200
#define MODEM_DIALING           201
#define MODEM_SCRIPTING         202

#define C_RETURN     13
#define L_FEED       10

#ifndef MAXPATHLEN
#define MAXPATHLEN  1024
#endif

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN  1024
#endif


#ifndef LOCK_SUCCESS
#define LOCK_SUCCESS    1
#endif

#ifndef LOCK_FAIL
#define LOCK_FAIL       0
#endif

#ifdef HAVE_MOTIF

#ifdef COLOR_BLACK 
#undef COLOR_BLACK 
#endif
#define COLOR_BLACK     0

#ifdef COLOR_WHITE
#undef COLOR_WHITE
#endif
#define COLOR_WHITE     COLOR_BLACK+1

#ifdef COLOR_RED       
#undef COLOR_RED
#endif
#define COLOR_RED       COLOR_WHITE+1

#ifdef COLOR_GREEN
#undef COLOR_GREEN
#endif
#define COLOR_GREEN     COLOR_RED+1

#ifdef COLOR_YELLOW    
#undef COLOR_YELLOW
#endif
#define COLOR_YELLOW   COLOR_GREEN+1

#ifdef INIT_LAB
#undef INIT_LAB
#endif
#define INIT_LAB        0

#ifdef DIAL_LAB
#undef DIAL_LAB
#endif
#define DIAL_LAB        INIT_LAB+1

#ifndef SCRIPT_LAB
#undef SCRIPT_LAB
#endif
#define SCRIPT_LAB      DIAL_LAB+1

#ifdef PPP_LAB
#undef PPP_LAB
#endif
#define PPP_LAB         SCRIPT_LAB+1


#endif  /* HAVE_MOTIF */

#ifndef __FILE__
#define __FILE__    "?"
#endif


#ifndef __LINE__
#define __LINE__    0
#endif

#ifdef DEBUG
#define Debug4(fmt,v1,v2,v3,v4)                              \
    {char * trick = (fmt);                                   \
    (void) fprintf (stderr,"%s(%d): ", __FILE__,__LINE__);   \
    (void) fprintf (stderr,(trick),(v1),(v2),(v3),(v4));     \
    (void) fprintf (stderr,"\n");}

#define Debug2(fmt,v1,v2) Debug4((fmt),(v1),(v2),0,0)
#else
#define Debug4(fmt,v1,v2,v3,v4)
#define Debug2(fmt,v1,v2)
#endif

#ifndef INCLUDED_FROM_MAIN
#define Extern extern
#else
#define Extern
#endif

/* globals */
Extern int      g_verbose;                  /* verbose on or off */
Extern int      g_quit;                     /* exit leaving ppp conn open */
Extern int      g_quit_time;                /* stay up this many seconds */
Extern char     g_configfile[MAXPATHLEN];   /* full path of config file */


/* struct to hold send-expect command sequence */
typedef struct _Script
{
    char
        *qualifier,         /* "send" or "expect" */
        *value;             /* value of "send" or "expect */

    struct _Script
        *next;
} Script;

/* main Mppp structure */
typedef struct _Mppp
{
    char
        *version,           /* mppp version in ini file */
        pppd_command[128],   /* path of pppd */
        pppd_args[1024],    /* pppd arguments */
        mppp_logfile[MAXPATHLEN],   /* keep connection logs */
        mppp_lockfile[MAXPATHLEN],  /* keep pid of mppp*/
        *this_number,       /* current phone number being dialied*/
        *this_baud,          /* baud rate of the number being dialied*/
        *location,          /* location ..title, NOT null */
        *parity,            /* parity, NOT null*/
        *bits,              /* bits, NOT null*/
        *stopbits,          /* stop bits, NOT null*/
        *password,          /* ppp password, can be null */
        **numbers,          /* holds each phone num in an array */
        **bauds;            /* speed for each phone number */

    int
        max_redial;         /* Max redial attempt for a number */

    int
        max_loop;           /* loop attempt through all the numbers */

    int
        n_numbers;           /* number of phone numbers */

    char
        *device,            /* modem device */
        *init_string,       /* modem init string */
        *init_ok_response,  /* init ok response, usually OK */
        *dial_string,       /* modem dial string, usually ATDT */
        *dial_ok_response,  /* usually CONNECT Baud */
        *hangup_string,     /* usually, +++ATH */
        *hangup_response;   /* hangup response, usually OK */

     char
        script_name[MAXPATHLEN];        /* name of the script file */

     struct _Mppp
        *next;
} Mppp;

/* function prototypes */

Mppp        *allocateMppp(void);
int         compressPathname(char *pathname);
int         cfgGetline(char *s,int n,FILE *fp);
void        cfgGetword(char *word,char *line);
void        destroyMppp(Mppp **mppp);
int         doesFileExist(char *path);
int         dotLockFile(char *fname,int max_attempts,
                        int lock_sleep,int lockfile_age);
void        DestoryScript(void);
void        Error(char *format,...);
int         execPPP(void);
void        fixupThings(int hangup);
char        *getBasedir(char *path);
int         getCancelDialingStatus(void);
char        *getElapsedTimestring(time_t from_time,int hrmin);
void        findBaud(void);
int         getaByte(void);
char        *getDevicename(void);
char        *getLogfile(void);
int         getParity(void);
int         getpppdId(void);
int         getPPPDState(void);
int         getScriptnum(void);
char        *getStringAfterLastTag(char *s1,int tag);
char        *getTime(void);
void        hangupModem(void);
long        hundredSecs(void);
int         isDialing(void);
int         isDirectory(char *path);
int         isFile(char *path);
void        initStartTime(void);
void        makeFullPath(char *src1,char *src2,char *dst);
int         maxFds(void);
int         mppInitrcFiles(void);
char        *mystrtok(char *s,char *delim);
char        *obscureString(char *s);
int         normalizePathname(char *pathname);
int         openDevice(void);
int         spppdDead(void);
void        parseCommandline(int argc,char **argv);
int         parseFilename(char *fullname, char *filename, char *pathname);
void        prepareComm(void);
int         readScript(Script **head,char *sfile);
void        releaseScriptList(Script **head);
void        resetDevice(void);
void        safeCpy(char *dest,char *src,int len);
void        saveParity(int parity);
void        saveSpeed(int speed);
void        sendDealy(float secs);
int         sendEnter(char *s);
void        setCancelDialing(int flag);
void        setComm(void);
void        setDialingStatus(int status);
void        setExpect(char *s);
int         setFcntlFlags(int fd,int flags,int onoff);
void        setLogfile(char *logfile);
void        setpppdId(int id);
void        setPPPDState(int state);
void        setScriptnum(int n);
int         startPPP(void);
char        *stptok(const char *s, char *tok, size_t toklen, char *brk);
int         strDoesContain(char *str,char *regex);
int         stripLastTag(char *path,int tag);
void        stripLineFeed(char *s1);
char        *strUpper(char *string);
char        *strLower(char *string);
int         getSpeed(void);
int         howmanyTokens(char *buf,int sep);
int         terminatePPPD(void);
int         waitFor(char *str,int usleep_time);
int         waitforEnter(void);
int         waitQuiet(float from_now,float for_howlong);
void        Warning(char *format,...);
void        writeComm(char *command);
void        writeLine(char *str);
char        *xstrdup(char *s);
void        writeLog(void);

#ifdef HAVE_MOTIF
void         Beep(int vol);
void         ChangeHighlightColor(int which_lab,int which_color);
XtAppContext GetAppContext(void);
void         InterceptMouseClick(void);
void         SetnameLabel(char *str);
void         SetStatusText(char *text,int replace);
void         StoreStartTime(void);
void         UpdateWindowStates(void);
#endif

#endif  /* MPPP_H */

