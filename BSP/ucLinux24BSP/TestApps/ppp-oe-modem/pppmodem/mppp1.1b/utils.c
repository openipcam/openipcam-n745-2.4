#ifdef HAVE_MOTIF
#include "xhead.h"
#endif

#include "mppp.h"

/*
** return time stamp
** borrowed from Apache
*/
char *getTime()
{
    time_t
        t;
    
    char
        *time_string;

    t=time(NULL);
    time_string=ctime(&t);
    time_string[strlen(time_string)-1]='\0';

return (time_string);        
}

/*
** read the conf file
** adapted from apache 1.1.1 source code
** returns 1 of EOF , 0 otherwise
*/

int cfgGetline(char *s,int n,FILE *fp)
{
    register int
        i=0,
        c;

    *s='\0';

    /*
    ** skip leading white spaces
    */
    do
    {
        c=getc(fp);
    } while (c == '\t' || c == ' ');

    while (1)
    {
        if ((c == '\t') || (c == ' '))
        {
            s[i++]= ' ';
            while ((c == '\t') || (c == ' '))
                c=getc(fp);
        }
        if (c == C_RETURN)
        {
            c=getc(fp);
        }

        if (c == EOF || c == 0x4 || c == L_FEED || i == (n-1))
        {
            /*
            ** blast trailing white space
            */
            while (i && (s[i-1] == ' ')) --i;
            s[i]='\0';
            return (feof(fp) ? 1 : 0);
        }
        s[i]=c;
        ++i;
        c=getc(fp);
    }
}

/*
** cfgGetword()
** reads word form the buffer
** return parameter: word
** from apache 1.1.1
*/
void cfgGetword(char *word,char *line)
{
    int
        x=0,
        y;

    for (x=0;line[x] && isspace(line[x]); x++);

    y=0;
    while(1)
    {
        if (!(word[y] = line[x]))
           break;

        if (isspace(line[x]))
            if ((!x) || (line[x-1] != '\\'))
                break;
        if (line[x] != '\\')
            ++y;

        ++x;
    }

    word[y]='\0';
    while (line[x] && isspace(line[x]))
        ++x;

    for (y=0; (line[y]=line[x]); ++x, ++y);
}

/*
** makeFullPath()
** from NCSA httpd
*/
void makeFullPath(char *src1,char *src2,char *dst)
{
    register int
        x,
        y;

     for (x=0; dst[x]=src1[x]; x++);

     if (!x)
        dst[x++]='/';
     else if ((dst[x-1] != '/'))
        dst[x++]='/';

     for (y=0;dst[x]=src2[y];x++,y++);
}

/*
** safe copy
*/
void safeCpy(char *dest,char *src,int len)
{
    (void) strncpy(dest,src,len);
}  


/*
** check the file is a directory
** returns 1 if direcory, 0 if not or in case or error
*/
int isDirectory(char *path)
{
    struct stat
        sbuf;

    if (stat(path,&sbuf) == -1)
        return (0);

    return (S_ISDIR(sbuf.st_mode));

}

/*
** check if a file exists, the file can be a directory, a special file
** or any kind of unix file. so this routine can not tell if the esisting
** file is a regulare file or not, it just checks the existence
**
** returns 0 if file exists, -1 if not
*/

int doesFileExist(char *path)
{
    int rc=access(path,F_OK);
    return (rc);
}


/*
** checks if a file is a file
** returns:
**  1   on success
**  0  on failure
*/
int isFile(char *path)
{
    struct stat
        sbuf;

    if (stat(path,&sbuf) == -1)
        return (0);

    return (S_ISREG(sbuf.st_mode));
}

/*
** strip the last asked characer from the path if any
** Note: path may be modified.
** returns 0 on success, -1 if memory allocation failure
*/
int stripLastTag(char *path,int tag)
{
    register char
        *scan,
        *place;

    char
        *ss;

    place=(char *) NULL;
    for (scan=path; *scan != '\0'; scan++)
    {
        if (*scan == tag)
        {
            place=scan;
            *place++ = '\0';
        }
    }

    if (place != (char *) NULL)
    {
        ss=strdup(place);
        if (ss == (char *) NULL)
        {
            (void) fprintf (stderr,"malloc failed\n");
            return (-1);
        }
        (void) strcpy(path,place);
        (void) free ((char *) ss);
    }

    return (0);
}

/*
** same as stripLastTag() but this one sends the filename in a malloc'd
** space. 
** return NULL if no slash in filename
** caller is responsible for freeing.
*/
char *getStringAfterLastTag(char *s1,int tag)
{
    register char
        *scan,
        *place;

    char
        *s,
        *path;

    place=(char *) NULL;
    s=(char *) NULL;
    path=(char *) NULL;

    path=strdup(s1);
    if (path == (char *) NULL)
    {
        (void) fprintf (stderr,"malloc failed!\n");
        return ((char *) NULL);
    }

    for (scan=path; *scan != '\0'; scan++)
    {
        if (*scan == tag)
        {
            place=scan;
        }
    }

    if (place != (char *) NULL)
    {
        /*
        ** go past the tag
        */
        place++;
        s=strdup(place);
        if (s == (char *) NULL)
        {
            (void) fprintf (stderr,
                "malloc failed in getStringfterLastTag()\n");
            (void) free((char *) path);
            return ((char *) NULL);
        }
    }

    (void) free((char *) path);
    return (s);
}

/*
** get the base of the name
** returns pointer to the string, NULL if not found
** memory is allocated, the caller is responsible to free the returned
*  string.
** path is not modified
*/
char *getBasedir(char *path)
{
    register char
        *scan,
        *place;

    char
        *s,
        *s1;

    int
        count=0;

    place=(char *) NULL;
    s=(char *) NULL;

    s1=strdup(path);
    if (s1 == (char *) NULL)
    {
        (void) fprintf (stderr,"malloc failed!\n");
        return ((char *) NULL);
    }

    for (scan=s1; *scan != '\0'; scan++)
    {
        if (*scan == '/')
        {
            count++;
            place=scan;
        }
    }

    if (place != (char *) NULL)
    {
        if (count > 1)
        {
            *place='\0';
        }
        else
        {
            /*
            ** only one slash, example: /vmunix
            */
            place++;
            *place='\0';
        }
        s=strdup(s1);
        if (s == (char *) NULL)
        {
            (void) fprintf (stderr,
                 "malloc failed in getBasedir()!\n");
            free(s1);
            return ((char *) NULL);
        }
    }

    (void) free ((char *) s1);
    return (s);
}


#ifdef TESTMAIN2
int main(int argc,char **argv)
{
    FILE
        *fp;

    char
        w[MAX_STRING_LEN],
        l[MAX_STRING_LEN];

    int
        rc=0;
    
    char
        *s;

    s=getStingAfterLastTag("test",'/');
    (void) fprintf (stderr,"s=%s\n",s);
    return(0);

}
#endif


/*
** setFcntlFlags()
** trun on one or more file status flags for a descriptor
** Stevens, Adv Unix prog book: page 67
**  parameters:
**      int fd          file descritor
**      int flags       flags
**      onoff           if 1, turn on, if 0 turn off flags
**
** returns  0 if succeeds
**          -1 in case of error
**
*/

int setFcntlFlags(int fd,int flags,int onoff)
{
    int
        val;

    int
        rc=0;

    val=fcntl(fd,F_GETFL,0);
    if (val < 0)
    {
        rc=(-1);
        goto ExitProcessing;
    }

    if (onoff == 1)     
    {
        val |= flags;       /* turn on */
    }
    else
    {
        val &= ~flags;      /* turn off flags */
    }

    val=fcntl(fd,F_SETFL,val);
    if (val < 0)
    {
        rc=(-1);
        goto ExitProcessing;
    }

ExitProcessing:

    return (rc);
}



/*
 * obscureString -- obscures a string, so that a cleartext search doesn't
 *                  come up with something interesting 
 *
 *  Return Values:
 *      char   *    (malloc'd space) if succeeds
 *      NULL        if fails 
 *
 *  Limitations and Comments:
 *      The caller is responsible to free the string.
 *
 *
 *  Development History:
 *      who                  when       why
 *      ma_muquit@fccc.edu   May-21-97  made it a library routine
 */

char *obscureString(char *s)
{
    unsigned char   
        *cp,
        *ns;
  
    cp=ns=(unsigned char *) strdup(s);
    if (cp == (unsigned char *) NULL)
        return ((char *) NULL);
  
    while (*cp)
        *cp++ ^= 0xff;

return ((char *) ns);
}

static FILE 
    *Sbbfp=(FILE *) NULL;

/* writes a warning message to stderr */
void Warning(char *format,...)
{
    va_list
        args;

    va_start(args,format);

    (void) fprintf(stderr,"Warning: ");
    (void) vfprintf(stderr,format,args);
}


/* writes error message to stderr and exit(1) */
void Error(char *format,...)
{
    va_list
        args;

    va_start(args,format);
    (void) fprintf(stderr,"Error: ");
    (void) vfprintf(stderr,format,args);

    exit(1);
}

/* open /dev/null for writing */
/* returns a pointer to FILE pointer on success, exits on failure */
/* user should call closeBitbucket() for closing it */
FILE  *openBitbucket()
{
    FILE
        *fp;

    fp=fopen("/dev/null","w");
    if (fp == (FILE *) NULL)
    {
        (void) fprintf(stderr,"Failed to open /dev/null for writing!\n");
        exit(1);
    }

    return (fp);
}

/* 
** xstrdup - exits if malloc fails
*/

char *xstrdup(char *s)
{
    char
        *t;

  if ( s == (char*)0 ) 
    return (char*)0;

  t = (char*)malloc(strlen(s)+1);
  if ( t == (char*)0 ) 
  {
    (void) fprintf(stderr,"malloc failed in xstrdup()\n");
    exit(1);
  }
  (void) strcpy(t,s);
  return (t);
}

/* close the static open bitbucket FILE pointer */
void closeBitbucket()
{
    if (Sbbfp != (FILE *) NULL)
        (void) fclose (Sbbfp);
}

/*
** I'm renaming it to mystrtok() in order to avoid conflict with the
** system which might have it
** I also formatted to my coding style
** 10/08/95, muquit@semcor.com
*/

char *mystrtok(char *s,char *delim)
{
	register char
        *spanp;

	register int
        c,
        sc;

	char
        *tok;

	static char
        *last;


	if (s == (char *) NULL && (s = last) == (char *) NULL)
		return ((char *) NULL);

	 /*
	 ** Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) 
    {
		if (c == sc)
			goto cont;
	}

	if (c == 0) 
    {		/* no non-delimiter characters */
		last = (char *) NULL;
		return ((char *)NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */

	for (;;) 
    {
		c = *s++;
		spanp = (char *)delim;
		do 
        {
			if ((sc = *spanp++) == c) 
            {
				if (c == 0)
					s = (char *) NULL;
				else
					s[-1] = '\0';
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	  /* NOTREACHED */
}

#ifdef TEST_MYSTRTOK
int main(int argc,char **argv)
{
    char
        *p;
    char
        *keyword,
        *value,
        buf[1024];

    if (argc != 2)
        exit(1);

    (void) strcpy(buf,argv[1]);
    p=buf;

    while((keyword=mystrtok(p,":")) != (char *) NULL)
    {
        p=(char *) NULL;
        value=mystrtok(p,"|&");
        if (value == (char *) NULL)
        {
            (void) fprintf(stderr,"parameter %s requires a value\n",
                keyword);
            exit(1);
        }
        (void) fprintf(stderr,"keyword=%s value=%s\n",keyword,value);
    }
}
#endif



/*
 *  getElapsedTimestring() - returns the elapsed time in the form
 *                           hr:min:sec or hr:min
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      char    *s          pointer to a static string. subsequent calls
 *                          will overwrite it. 
 *
 *  Parameters:
 *      time_t  stm         starting time in seconds, obtained by calling
 *                          time.
 *      int     hrmin       if 1, then only send hr:min or send hr:mn:sc
 *  Side Effects:
 *      static string in the routine is modified.
 *
 *  Limitations and Comments:
 *      Never try to free the string.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

char *getElapsedTimestring(time_t from_time,int hrmin)
{
    static char
        etime_buf[1024];

    time_t
        ntm;

    long
        sec,
        min,
        hr;

    ntm=time(NULL);
    sec=ntm-from_time;

    min=sec/60L;
    sec -= min*60L;
    hr=min/60L;
    min -= hr*60L;

    if (hrmin == 0)
        (void) sprintf(etime_buf,"%02ld:%02ld:%02ld",hr,min,sec);
    else
        (void) sprintf(etime_buf,"%02ld:%02ld",hr,min);

    return (etime_buf);
}

/*
 *  waitforEnter()  - checks if ENTER key is pressed
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      0   if ENTER key is not pressed
 *      1   if ENTER key is pressed
 *
 *  Parameters:
 *      None
 *
 *  Side Effects:
 *      N/A
 *
 *  Limitations and Comments:
 *      Adapted from dcon by Daniel Chouinard,  Longueuil, Quebec, Canada
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

int waitforEnter(void)
{
    fd_set
        fds;

    int
        rc=0;

    struct timeval
        timeout;

    timeout.tv_sec=0L;
    timeout.tv_usec=10000L;

    FD_ZERO(&fds);
    FD_SET(0,&fds);
    rc=select(1,&fds,0,0,&timeout);
    if (rc)
        return(getchar());
    return(0);
}

#ifdef TEST_WAITFOR
int main(int argc,char **argv)
{
    int
        loop=0;

     while (!loop)
     {
        (void) fprintf(stdout,"\rLoop=%d",loop);
        (void) fflush(stdout);
        loop=waitforEnter();
        if (loop)
        {
            (void) fprintf(stderr,"\nKill\n");
        }
     }

}
#endif


/*
 *  maxFds() - returns max allowable number of open files 
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      number of ax allowable number of open files if succeeds
 *      -1      if fails
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      n/a
 *
 *  Limitations and Comments:
 *      taken from llnlxdir 2.0 source (local.c)
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

int maxFds(void)
{
    int
        max_files=(-1);

#ifdef _SC_OPEN_MAX         /* POSIX */
    if ((max_files = sysconf(_SC_OPEN_MAX)) == -1)
    {
        (void) fprintf (stderr,"Trouble in max_fds() - sysconf() failed");
        return (-1);
    }
#else
#ifdef _NFILE               /* Might be overkill */
    max_files = _NFILE;
#else                       /* Assume BSD */
    max_files = getdtablesize();
#endif
#endif

#if defined(__QNX__)
    /* select will prematurely timeout if value returned by sysconf is used */
    if (max_files > FD_SETSIZE)
        max_files = FD_SETSIZE;
#endif

    return (max_files);
}


/*
 *  howmanyTokens() - checks if there's more than one token of string
 *                   separated by a separator character
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      number of tokens
 *
 *  Parameters:
 *      char    *str        whole string
 *      int     sep         the separator character
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      If there's only one string, then there's no separator. In this
 *      case we want to return 1, that is only one token. Therefor, we
 *      initialized rc to 1.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-01-1997    first cut
 */

int howmanyTokens(char *buf,int sep)
{
    register char
        *p;

    int
        rc=1;
       
    if (*buf == '\0')
        return(0);

    for (p=buf; *p != '\0'; p++)
    {
        if (*p == sep)
        {
            rc++;
        }
    }

    return (rc);
}


/*
 *  stripLineFeed() - strip off the LF character at the end of a string.
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      char    *s      returns.
 *
 *  Side Effects:
 *      string s is changed.
 *
 *  Limitations and Comments:
 *      null terminate and returns as soon as the lf character is found.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-10-1997    first cut
 */

void stripLineFeed(char *s1)
{
    int
        i;

    /* strip line feed character */
    for (i=0; i < strlen(s1); i++)
    {
        if (s1[i] == 0x0a)
        {
            s1[i]='\0';
            break;
        }
    } 
}


/*
**  stptok() -- public domain by Ray Gardner, modified by Bob Stout
**
**   You pass this function a string to parse, a buffer to receive the
**   "token" that gets scanned, the length of the buffer, and a string of
**   "break" characters that stop the scan.  It will copy the string into
**   the buffer up to any of the break characters, or until the buffer is
**   full, and will always leave the buffer null-terminated.  It will
**   return a pointer to the first non-breaking character after the one
**   that stopped the scan.
*/

char *stptok(const char *s, char *tok, size_t toklen, char *brk)
{
      char *lim, *b;

      if (!*s)
            return NULL;

      lim = tok + toklen - 1;
      while ( *s && tok < lim )
      {
            for ( b = brk; *b; b++ )
            {
                  if ( *s == *b )
                  {
                        *tok = 0;
                        for (++s, b = brk; *s && *b; ++b)
                        {
                              if (*s == *b)
                              {
                                    ++s;
                                    b = brk;
                              }
                        }
                        return (char *)s;
                  }
            }
            *tok++ = *s++;
      }
      *tok = 0;
      return (char *)s;
}

