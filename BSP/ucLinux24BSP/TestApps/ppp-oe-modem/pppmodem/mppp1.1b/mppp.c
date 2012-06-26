/*
 *  main mppp routines
 *
 *  RCS:
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *
 *  Description:
 *
 *      main mppp routines
 *
 *  Limitations and Comments:
 *
 *  N/A
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-04-1997    first cut [A vacation hack]
 */

#ifdef HAVE_MOTIF

#include "xhead.h"
#include "xmppp.h"

#endif  /* HAVE_MOTIF */

#include "mppp.h"

/* private function protos */
static void usage(char *progname);
static speed_t modemSpeed (char *baud);
static void zombieCollect(int sig);
static int check_io(int fd,int tmout,char *buf,int *buflen);

struct sigaction sact;

static int debug_level=3;  /* maximum verbosity */


/* local globals */
static Mppp         *smppp=(Mppp *) NULL;
static int          s_modemfd=(-1);
static int          s_pppd_is_dead=0;
static int          s_cancel_dialing=0;
static int          s_is_dialing=0;
static char         *s_baud;
static Script       *s_sehead=(Script *) NULL;
static char         *phone_number;
time_t              s_start_time_t=(time_t) NULL;
static char         *s_start_time=(char *) NULL;

/*
 *  allocateMppp() - mallocs and initializes Mppp struct
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      Mppp    *    on success
 *      NUL          on failure
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
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

Mppp *allocateMppp()
{
    Mppp
        *mppp;

    mppp=(Mppp *) malloc (sizeof(Mppp));
    if (mppp == (Mppp *) NULL)
    {
        Warning("malloc failed for allocateMppp()\n");
        return ((Mppp *) NULL);
    }

    mppp->version=(char *) NULL;
    *mppp->pppd_args='\0';
    *mppp->mppp_logfile='\0';
    *mppp->pppd_command='\0';
    mppp->location=(char *) NULL;
    mppp->this_number=(char *) NULL;
    mppp->this_baud=(char *) NULL;
    mppp->parity=(char *) NULL;
    mppp->bits=(char *) NULL;
    mppp->stopbits=(char *) NULL;
    mppp->password=(char *) NULL;
    mppp->numbers=(char **) NULL;
    mppp->bauds=(char **) NULL;

    mppp->max_redial=1;
    mppp->max_loop=1;
    mppp->n_numbers=0;

    mppp->device=(char *) NULL;
    mppp->init_string=(char *) NULL;
    mppp->init_ok_response=(char *) NULL;
    mppp->dial_string=(char *) NULL;
    mppp->dial_ok_response=(char *) NULL;
    mppp->hangup_string=(char *) NULL;
    mppp->hangup_response=(char *) NULL;

    return (mppp);
}



/*
 *  destroyMppp()   - frees the Mppp struct
 * 
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Mppp    **mppp
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      never access it after freeing it.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */
void destroyMppp(Mppp **mppp)
{

    int
        i;

    if ((*mppp)->version != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->version);
        (*mppp)->version=(char *) NULL;
    }

    if ((*mppp)->location != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->location);
        (*mppp)->location=(char *) NULL;
    }

    if ((*mppp)->this_number != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->this_number);
        (*mppp)->this_number=(char *) NULL;
    }

    if ((*mppp)->this_baud != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->this_baud);
        (*mppp)->this_baud=(char *) NULL;
    }

    if ((*mppp)->parity != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->parity);
        (*mppp)->parity=(char *) NULL;
    }

    if ((*mppp)->bits != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->bits);
        (*mppp)->bits=(char *) NULL;
    }

    if ((*mppp)->stopbits != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->stopbits);
        (*mppp)->stopbits=(char *) NULL;
    }

    if ((*mppp)->password != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->password);
        (*mppp)->password=(char *) NULL;
    }

    if ((*mppp)->n_numbers > 0)
    {
        for(i=0; i < (*mppp)->n_numbers; i++)
        {
            if (((*mppp)->numbers[i]) != (char *) NULL)
            {
                (void) free ((*mppp)->numbers[i]);
                (*mppp)->numbers[i]=(char *) NULL;
            }
            if (((*mppp)->bauds[i]) != (char *) NULL)
            {
                (void) free ((*mppp)->bauds[i]);
                (*mppp)->bauds[i]=(char *) NULL;
            }
        }

        if (((*mppp)->numbers) != (char **) NULL)
        {
            (void) free ((char *) ((*mppp)->numbers));
            (*mppp)->numbers=(char **) NULL;
        }
        if (((*mppp)->bauds) != (char **) NULL)
        {
            (void) free ((char *) ((*mppp)->bauds));
            (*mppp)->bauds=(char **) NULL;
        }
        (*mppp)->max_redial=1;
        (*mppp)->n_numbers=0;


    }
    
    if ((*mppp)->device != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->device);
        (*mppp)->device=(char *) NULL;
    }

    if ((*mppp)->init_string != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->init_string);
        (*mppp)->init_string=(char *) NULL;
    }
        
    if ((*mppp)->init_ok_response != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->init_ok_response);
        (*mppp)->init_ok_response=(char *) NULL;
    }
    if ((*mppp)->dial_string != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->dial_string);
        (*mppp)->dial_string=(char *) NULL;
    }

    if ((*mppp)->dial_ok_response != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->dial_ok_response);
        (*mppp)->dial_ok_response=(char *) NULL;
    }

    if ((*mppp)->hangup_string != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->hangup_string);
        (*mppp)->hangup_string=(char *) NULL;
    }

    if ((*mppp)->hangup_response != (char *) NULL)
    {
        (void) free ((char *) (*mppp)->hangup_response);
        (*mppp)->hangup_response=(char *) NULL;
    }

    if (*mppp != (Mppp *) NULL)
    {
        (void) free ((char *) *mppp);
        *mppp = (Mppp *) NULL;
    }

}


/*
 *  readmpppConfig() - read the mppp config file
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      exit(1)     in case of error
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      static smppp struct is filled.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */


int readmpppConfig(void)
{
    struct passwd
        *pw;

    char
        *s,
        *tmp_txt,
        *value,
        *abaud,
        *p,
        buf[MAXPATHLEN];

    FILE
        *cfg=(FILE *) NULL;

    char
        l[MAX_STRING_LEN],
        w[MAX_STRING_LEN];

    char
        *cfgb,
        cfg_base[MAXPATHLEN];

    int
        ok=0,
        i=0,
        ntokens,
        n=0;

    char
        filename[1024],
        pathname[1024];

     /* check if rc file is supplied at comamnd line */
     if (*g_configfile != '\0')
     {
         Debug2("Config file=%s",g_configfile,0);

        /* find out the filename and it's path */
        parseFilename(g_configfile,filename,pathname);
        Debug2("filename=%s",filename,0);
        Debug2("file's path=%s",pathname,0);

        (void) strcpy(buf,pathname);
        (void) strcat(buf,filename);

        cfgb=getBasedir(pathname);
        (void) strcpy(cfg_base,cfgb);
        (void) free (cfgb);
        Debug2("file's path after stripping last /=%s",cfg_base,0);
     }
     else if ((s=getenv("MPPP")) != (char *) NULL)
     {

        /* 
        ** check if a environment variable called MPPP exists
        ** if it does, then use this file as the config file
        */
        (void) strcpy(buf,s);
        /* get the base of the buf and use it as cfg_base */
        cfgb=getBasedir(buf);
        if (cfgb == (char *) NULL)
            Error("No base directory found\n");
        (void) strcpy(cfg_base,cfgb);

        (void) free (cfgb);

        Debug2("config file=%s",buf,0);
        Debug2("config base=%s",cfg_base,0);
    }
    else
    {
        #if 0
        /* get the user's home directory */
        pw=getpwuid(getuid());
        if (pw == (struct passwd *) NULL)
        {
            (void) fprintf(stderr,"faield to get uid entry from passwd file\n");
            return (-1);
        }

        (void) strncpy(buf,pw->pw_dir,sizeof(buf)-1);
        (void) strncat(buf,"/.mpppdir",20);
    
        /* save the base of cfg file */
        (void) strcpy(cfg_base,buf);

        (void) strncat(buf,"/",2);
        (void) strncat(buf,MPP_INIFILE,50);
        #endif
	sprintf(buf,"%s",MPP_INIFILE);
	printf("buf===%s\n",buf);
    }

    /* check if the file exists */
    if (isFile(buf) != 1)
        Error("File \"%s\" does not exist!\n",buf);

    /* open the config file */
    cfg=fopen(buf,"r");
    if (cfg == (FILE *) NULL)
        Error ("Unable to open \"%s\" for reading\n",buf);

    /* allocate Mppp */
    smppp=allocateMppp();
    if (smppp == (Mppp *) NULL)
        Error("malloc failed at allocateMppp()\n");

    /* start readig the cfg file */
    n=0;
    while ((cfgGetline(l,MAX_STRING_LEN,cfg) == 0))
    {
        ++n;
        if ((*l != '#') && (*l != '\0'))
        {

            cfgGetword(w,l);
            /* first check if the MpppVersion directive exists */
            if (strcasecmp(w,"MpppVersion") == 0)
            {
                ok++;
            }
            else if (strcasecmp(w,"device") == 0)
            {
                cfgGetword(w,l);
                smppp->device=xstrdup(w);
                Debug2("device=%s",smppp->device,0);
            }
            else if (strcasecmp(w,"pppcommand") == 0)
            {
                cfgGetword(w,l);
                (void) strcpy(smppp->pppd_command,w);
                Debug2("command=%s",smppp->pppd_command,0);
            }
            else if (strcasecmp(w,"pppdargs") == 0)
            {
                if (*l == '\0')
                    Error("No pppd arguments specified\n");
                (void) strcpy(smppp->pppd_args,l);
            }
            else if (strcasecmp(w,"mppplogfile") == 0)
            {
                #if 0 //mcli
                if (*l == '\0')
                {
                    Error("No logfile specified\n");
                }
                if (*l != '/')
                    makeFullPath(cfg_base,l,smppp->mppp_logfile);
                else
                    (void) strncpy(smppp->mppp_logfile,l,
                                   sizeof(smppp->mppp_logfile)-1);
                #endif
            }
            else if (strcasecmp(w,"location") == 0)
            {
                /*
                cfgGetword(w,l);
                */
                smppp->location=xstrdup(l);
                Debug2("location=%s",smppp->location,0);
            }
            else if (strcasecmp(w,"maxredial") == 0)
            {
                if (*l != '\0')
                {
                    smppp->max_redial=atoi(l);
                    Debug2("config: max_redial=%d",smppp->max_redial,0);
                }
                else
                    Error("No MaxRedial found in cfg file\n");
            }
            if (strcasecmp(w,"maxloop") == 0)
            {
                if (*l != '\0')
                {
                    smppp->max_loop=atoi(l);
                }
                else
                    smppp->max_loop=1;
            }
            else if (strcasecmp(w,"InitString") == 0)
            {
                cfgGetword(w,l);
                smppp->init_string=xstrdup(w);
                Debug2("init_string=%s",smppp->init_string,0);
            }
            else if (strcasecmp(w,"InitOKResponse") == 0)
            {
                cfgGetword(w,l);
                smppp->init_ok_response=xstrdup(w);
                Debug2("init_response=%s",smppp->init_ok_response,0);
            }
            else if (strcasecmp(w,"DialString") == 0)
            {
                cfgGetword(w,l);
                smppp->dial_string=xstrdup(w);
                Debug2("dial_string=%s",smppp->dial_string,0);
            }
            else if (strcasecmp(w,"DialOKResponse") == 0)
            {
                if (*l != '\0')
                {
                    smppp->dial_ok_response=xstrdup(l);
                }
                else
                    Error("No DialOKResponse string in cf file\n");
            }
            else if (strcasecmp(w,"HangupString") == 0)
            {
                cfgGetword(w,l);
                smppp->hangup_string=xstrdup(w);
                Debug2("hangup_string=%s",smppp->hangup_string,0);
            }
            else if (strcasecmp(w,"HangupResponse") == 0)
            {
                cfgGetword(w,l);
                smppp->hangup_response=xstrdup(w);
                Debug2("hangup_response=%s",smppp->hangup_response,0);
            }
            else if (strcasecmp(w,"parity") == 0)
            {
                cfgGetword(w,l);
                smppp->parity=xstrdup(w);
                Debug2("parity=%s",smppp->parity,0);
            }
            else if (strcasecmp(w,"bits") == 0)
            {
                cfgGetword(w,l);
                smppp->bits=xstrdup(w);
                Debug2("bits=%s",smppp->bits,0);
            }
            else if (strcasecmp(w,"stopbits") == 0)
            {
                cfgGetword(w,l);
                smppp->stopbits=xstrdup(w);
                Debug2("stopbits=%s",smppp->stopbits,0);
            }
            else if (strcasecmp(w,"password") == 0)
            {
                cfgGetword(w,l);
                if (strlen(w) != 0)
                {
                    smppp->password=xstrdup(w);
                    Debug2("password=\"%s\"",smppp->password,0);
                }
            }
            else if (strcasecmp(w,"numbers") == 0)
            {
                if (*l != '\0')
                {
                    Debug2("Numbers=%s",l,0);

                    /* try to tokenize the numer */
                    ntokens=howmanyTokens(l,'|');
                    if (ntokens > 0)
                    {
                        /* make copy of real number */
                        tmp_txt=xstrdup(l);

                        smppp->n_numbers=ntokens;
                        Debug2("Numbers =%d",ntokens,0);
                        
                        /* now tokenize */
                        smppp->numbers=
                            (char **) malloc(ntokens*sizeof(char *));
                        if (smppp->numbers == (char **) NULL)
                            Error("malloc failed for an_tokens.\n");

                        /* 
                         * now we'r assuming,each alt number is followed
                         * by a : and the speed. So we'r malloc'ng anyway.
                         * later if we find out that there's no baud
                         * specified, we'll exit. so it's ok.
                         */

                        smppp->bauds=
                            (char **) malloc(ntokens*sizeof(char *));
                        if (smppp->bauds == (char **) NULL)
                            Error("malloc failed for bauds.\n");

                        i=0;
                        p=tmp_txt;
                        while ((value=mystrtok(p,":")))
                        {
                            p=(char *) NULL;
                            abaud=mystrtok(p,"|");
                            if (abaud == (char *) NULL)
                            {
                                Error("Alt number %s requires a speed!\n",
                                    value);
                            }
                            smppp->numbers[i]=xstrdup(value);
                            smppp->bauds[i]=xstrdup(abaud);
                            i++;
                        }
                        (void) free(tmp_txt);
                    }
                }
                else
                {
                    Error("No Number to dial!\n");
                }
            }
            else if (strcasecmp(w,"script") == 0)
            {
                if (*l == '\0')
                {
                    Error("No script name found\n");
                }
                cfgGetword(w,l);
                if (*w != '/')
                    makeFullPath(cfg_base,w,smppp->script_name);
                else
                    safeCpy(smppp->script_name,w, sizeof(smppp->script_name)-1);

                Debug2("script name=%s",smppp->script_name,0);
            }
        }
    }

    if (cfg)
        (void) fclose(cfg);

    if (!ok)
        Error("Not a mppp config file, no MpppVersion directive found\n");
    return (0);

}

/*
 *  openDevice() - opens the modem device 
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *
 *  Return Values:
 *      0       on success
 *      -1      on failure
 *
 *  Parameters:
 *      none
 *      The modem device name is obtained by reading cfg file
 *
 *  Side Effects:
 *      modem device characteristics are changed.
 *
 *  Limitations and Comments:
 *
 *      taken from ezppp source
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-30-1997    first cut
 */


int  openDevice(void)
{
    struct termios
        tty;
        
    int
        rc=0;

    speed_t
        speed;

    if (s_modemfd >=0 )
    {
        (void) close(s_modemfd);
        s_modemfd=(-1);
    }
    s_modemfd=open(smppp->device,O_RDWR|O_EXCL|O_NDELAY);
    if (s_modemfd < 0)
    {
        rc=(-1);
        goto ExitProcessing;
    }


    if (tcgetattr(s_modemfd,&tty) < 0)
    {
        rc=(-1);
        goto ExitProcessing;
    }


    tty.c_cc[VMIN]= 1;
    tty.c_cc[VTIME]=0;
    tty.c_oflag = 0;
    tty.c_lflag = 0;

    tty.c_cflag &= ~(CSIZE | CSTOPB | PARENB | CLOCAL);
    tty.c_cflag |= CS8 | CREAD;
    tty.c_iflag = IGNBRK | IGNPAR;
    tty.c_lflag &= ~ICANON; 
    tty.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHOKE);

    tty.c_cflag |= CRTSCTS;

    speed=modemSpeed(s_baud);
    cfsetospeed(&tty,speed);
    cfsetispeed(&tty,speed);

    if (tcsetattr(s_modemfd,TCSAFLUSH,&tty) < 0)
    {
        rc=(-1);
        goto ExitProcessing;
    } 

    if (tcsendbreak(s_modemfd,0) < 0)
    {
        rc=(-1);
        goto ExitProcessing;
    }

ExitProcessing:
    if (rc == -1)
    {
        if (s_modemfd >= 0)
        {
            (void) close(s_modemfd);
            s_modemfd=(-1);
        }
    }
    return(rc);
}

/*
 * modemSpeed() -  convert string modem speed in cfg file to to a speed_t
 *                 type to set the modem.
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      speed_t
 *
 *  Parameters:
 *      char *baud_rate      the baud rate in string (e.g., "57600"
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      for linux only. as I don't know how other systems defines speed_t
 *      baud is obtained from smppp->baud, therefore this struct must
 *      be valid and initialized properly.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */

static speed_t modemSpeed(char *baud_rate)
{
    int
        baud;

    speed_t
        speed=B0;

    baud=atoi(baud_rate);
    switch(baud)
    {
        case 0: 
        {
            speed=B0;
            break;
        }
        case 50:  
        {
            speed=B50;
            break;    
        }
        case 75:  
        {
            speed=B75;
            break;    
        }
        case 110:  
        {
            speed=B110;
            break;    
        }
        case 150:  
        {
            speed=B150;
            break;    
        }
        case 300:  
        {
            speed=B300;
            break;    
        }
        case 600:  
        {
            speed=B600;
            break;    
        }
        case 12000:  
        {
            speed=B1200;
            break;    
        }
        case 2400:  
        {
            speed=B2400;
            break;    
        }
        case 4800:  
        {
            speed=B4800;
            break;    
        }
        case 9600:  
        {
            speed=B9600;
            break;    
        }
        case 19200:  
        {
            speed=B19200;
            break;    
        }
        case 38400:  
        {
            speed=B38400;
            break;    
        }
#ifdef HIGH_SPEED

        case 57600:  
        {
            speed=B57600;
            break;    
        }
        case 115200:  
        {
            speed=B115200;
            break;    
        }
        case 230400:  
        {
            speed=B230400;
            break;    
        }
        case 460800:  
        {
            speed=B460800;
            break;    
        }
#endif

    }

    return (speed);
}

/*
 *  sendEnter() - sends CR, CR/LF or CR to the modem device
 * 
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      0       on success
 *      -1      on failure
 *
 *  Parameters:
 *      char    *cr     string either of CR, CR/LF or LF
 *                      or
 *                      ^cr, ^crlf, ^lf
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      s_modemfd is the open modem file descriptor which is a global or a 
 *      static int in this file
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-30-1997    first cut
 */

int sendEnter(char *cr)
{
    int
        rc=0;
    if (strncasecmp(cr,"^cr",3) == 0)    /* CR */
    {
        (void) write(s_modemfd,"\r",1);
    }
    else if(strncasecmp(cr,"^crlf",5) == 0) /* cr/lf */
    {
        (void) write(s_modemfd,"\r\n",2);
    }
    else if (strncasecmp(cr,"^lf",3) == 0) /* lf */
    {
        (void) write(s_modemfd,"\n",1);
    }
    else if (strncasecmp(cr,"cr",2) == 0)
    {
        (void) write(s_modemfd,"\r",1);
    }
    else if (strncasecmp(cr,"cr/lf",5) == 0)
    {
        (void) write(s_modemfd,"\r\n",2);
    }
    else if (strncasecmp(cr,"lf",2) == 0)
    {
        (void) write(s_modemfd,"\n",1);
    }
    else
        rc=(-1);

    return (rc);
}



/*
 *  hangupModem()   - sends the hangup to modem
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      modem is reset
 *
 *  Limitations and Comments:
 *      s_modemfd is the open modem fd
 *      taken from ezppp source code.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-30-1997    first cut
 */

void hangupModem(void)
{
    struct termios
        store_tty,
        temp_tty;

    tcgetattr(s_modemfd,&store_tty);
    tcgetattr(s_modemfd,&temp_tty);

    temp_tty.c_cflag |= HUPCL;

    cfsetospeed(&temp_tty,B0);
    cfsetispeed(&temp_tty,B0);

    tcsetattr(s_modemfd,TCSANOW,&temp_tty);
    tcsendbreak(s_modemfd,0);

    sleep(1);

    tcsetattr(s_modemfd,TCSANOW,&store_tty);

}



/*
 * getDevicename() - returns the name of the modem device 
 * 
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      char    *name
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      it gets the device name from smpp struct, therefore the sturct
 *      must be valid and static in the file or global.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */

char *getDevicename(void)
{
    return (smppp->device);
}


/*
 *  writeLine() - writes line to the modem device
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      char    *string     string to write
 *
 *  Side Effects:
 *      N/A
 *
 *  Limitations and Comments:
 *      if The line does not end with CR, CRLF or LF, a CR is sent to the
 *      modem device after the line is sent.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 *                           Mar-22-1998    do not send CR after writing
 *                                          a line. If the caller needs it
 *                                          she can do it explicitly.
 */

void writeLine(char *str)
{
    int
        n;

    if ((strcasecmp(str,"^cr") == 0)   ||
        (strcasecmp(str,"^crlf") == 0) ||
        (strcasecmp(str,"^lf") == 0))
    {
        sendEnter(str);
        return;
    }
    n=write(s_modemfd,str,strlen(str));
    sleep(1);
}


/*
 *  strDoesContain () - finds if a specified string/portion is available
 *                      in a string.
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      1      if found
 *      0      if not found
 *
 *  Parameters:
 *      char *str        scan this string
 *      char *lookfor    look for this substring
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      use GNU regex library.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut with GNU regex.
 *                           Mar--22-1998   regex fails if the part of
 *                                          the string we'r looking for
 *                                          appears before the real string.
 *                                          so we'll use strstr() instead
 */

int strDoesContain(char *str,char *lookfor)
{
    int
        rc=0;
    if (strstr(str,lookfor))
        rc=1;
    else
        rc=0;

    return(rc);

}

/*
 * execPPP()    - exec pppd
 * 
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      0  on success
 *      -1 on failure
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      pid of the pppd can be obtained by callied getpppdId()
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */

int execPPP(void)
{
    char
        pbuf[1024],
        *ptokens[100],
        *ptr=(char *) NULL,
        **s,
        **namelist;

    int
        i,
        ntokens=0,
        id;

    /* prepare the argument vector for execv */
    namelist=(char **) malloc(BUFSIZ*sizeof(char *));
    if (namelist == (char **) NULL)
    {
        (void) fprintf(stderr,"malloc failed in execPPP()!\n");
        return (-1);
    }

    /*
    ** check if pppd args are supplied
    */
    if (*smppp->pppd_args != '\0')
    {
        Debug2("smpppd->pppd-args=%s",smppp->pppd_args,0);
        ptr=smppp->pppd_args;
        do
        {
            ptr=stptok(ptr,pbuf,sizeof(pbuf)," ");
            if (*pbuf != '\0')
                ptokens[ntokens++]=xstrdup(pbuf);
        } while (ptr && *ptr);
    }
    if (ntokens > 0)
    {
        Debug2("pppd args taken from cfg file",0,0);
        s=namelist;
        *s++ = "pppd";
        for (i=0; i < ntokens; i++)
            *s++ = ptokens[i];
        *s++ = "modem";
        *s++ = smppp->device;
        *s++ = s_baud;
        *s = '\0';
    }
    else
    {
        s=namelist;
        *s++ = "pppd";
        *s++ = "lock";
        *s++ = "-detach";
        *s++ = "defaultroute";
        *s++ = "crtscts";
        *s++ = "modem";
        *s++ = smppp->device;
        *s++ = s_baud;
        *s = '\0';
    }

    id=vfork();
    
    switch (id)
    {
        case (-1):
        {
            (void) fprintf(stderr,"Unable to fork!\n");
            return (-1);
            break;  /* won't be here */
        }

        case 0:     /* child */
        {
            /*
            (void) close(s_modemfd);
            */
            sleep(5);
            execv(smppp->pppd_command,namelist);
            (void) fprintf(stderr,"execv() failed\n");
            exit(1);
            break;
        }

        default:
        {
            Debug2("pppd id=%d",id,0);
            setpppdId(id);
            break;
        }
    }

    return (0);
}



/*
 *  terminatePPPD() - send SIGTERM to pppd process
 * 
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      0   if succeded in terminating
 *      -1  if fails
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
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */

int terminatePPPD (void)
{
    
    int
        rc,
        pppd_id;

    pppd_id=getpppdId();
    Debug2("Terminating pppd=%d",pppd_id,0);
    if (pppd_id >= 0)
    {
        rc=kill(pppd_id,SIGTERM);
        if (rc < 0)
            (void) fprintf(stderr,"Unable to terminate process; %d\n",pppd_id);
    }
    else
    {
        rc=(-1);
    }

    setpppdId(-1);
    setPPPDState(0);

    return (rc);
}

/*
 *  zombieCollect() - wait for children so they won't become zombies
 * 
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int sig     type of signal
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */


static void zombieCollect(int sig) 
{
    pid_t
        pid;

    /* if modem device is open, close it */
    if (s_modemfd >=0 )
    {
        (void) close(s_modemfd);
        s_modemfd=(-1);
    }
    /*
    pid=wait(&st);
    */
    pid=waitpid((pid_t) (-1),(int *) 0,WNOHANG);
    if (pid == getpppdId())
    {
        (void) fprintf(stderr,
            "\n\nSignal received...Collecting zombie! ... pppd is dead!\n");
        s_pppd_is_dead=1;
        setpppdId(-1);
        setPPPDState(0);
    
#ifdef HAVE_MOTIF
        Beep(0);
        SetStatusText("\n >>>> oops! pppd died suddenly! <<<<\n",0);
        writeLog();
        UpdateWindowStates();
#endif
        /* reset the signal */
        sigaction(SIGCHLD, &sact, (struct sigaction *)NULL);
    }
}


/*
 *  checkIO()   - checks if there is any IO pending.
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      1       if any io pending
 *      0       if not
 *
 *  Parameters:
 *      int fd              open file descriptor
 *      int tmout           timeout (not used at this time)
 *      char *buf           fill this buffer with data (returns)
 *      int  *buflen        lenngth of the buf (returns)
 *
 *  Side Effects:
 *      buf must have space to hold at least 127 bytes.
 *
 *      maximum 127 bytes are filled in buf.
 *
 *      buf is modified
 *      buflen is modified
 *
 *  Limitations and Comments:
 *      adapted from minicom ipc.c
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */


/* Check if there is IO pending. */
static int check_io(int fd,int tmout,char *buf,int *buflen)
{               
  int
    n=0,
    x=0;
          
  n=read(fd,buf,127);
#if 0  
  if(n > 0)
  {//=========>
  	int i = 0;
  	printf("===>");
  	for(; i < n; i++)
  		printf(" %02x", buf[i]);
  	printf("\n");	
  }else if(n < 0) {
  	;
  	//write(fd, "\r\n", 2);
  	//printf("%02x\n", *(unsigned char volatile *)(0xfff80114));
	//printf("read error %d\n", n);  	
  	
  }	
#endif 	
  buf[n > 0 ? n : 0] = 0;
  if (buflen) *buflen = n;
  if (n > 0) x |= 1;
        
  return(x);
}



/*
 *  ispppdDead()  - retuns the state of pppd
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      1           pppd is dead
 *      0           pppd is alive
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      s_pppd_is_dead is chnaged by other routines. this routine just
 *      returns it.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

int ispppdDead(void)
{
    return(s_pppd_is_dead);
}


/*
 *  startPPP()  - start ppp 
 *              
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      0   on success
 *      -1  on failure
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      a lot
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */

int startPPP(void)
{
    int
        pppd_pid,
        i,
        rc=0;

    int
        max_try=1;

    char
        debug_buf[BUFSIZ],
        buf[BUFSIZ];

    Script
        *sc;

    int
        max_busy,
        attempt=0,
        this_num=0,
        ntry=0,
        nloop=0,
        p_num=1,
        max_redial=1,
        max_loop=0;


#ifndef HAVE_MOTIF
    int
        t_loop=0;
#endif

    (void) setbuf(stderr,NULL);
    (void) setbuf(stdout,NULL);

    /* first check if mppp is allocated before, if so destroy it */
    if (smppp != (Mppp *) NULL)
    {
        Debug2("Destroying Existing smppp",0,0);
        destroyMppp(&smppp);
    }
    /* read the config file */
    (void) readmpppConfig();

    /* store the logfile name */
    setLogfile(smppp->mppp_logfile);


    /*--------------for debug only --------------------*/
        for (i=0; i < smppp->n_numbers; i++)
        {
            Debug2("numbers[%d]=%s",i,smppp->numbers[i]);
            Debug2("bauds[%d]=%s",i,smppp->bauds[i]);
        }

    /*--------------for debug only --------------------*/

    /* we may be here second time, destroy s_sehead if it was already set */
    releaseScriptList(&s_sehead);

    /* read the script file */
    rc=readScript(&s_sehead,smppp->script_name);
    if (rc != 0)
    {
        (void) fprintf(stderr,"Error reading script: %s\n",
            smppp->script_name);
        return (-1);
    }
    /* set the signal handler so if pppd died for some reason, we'll know */
    sact.sa_handler=zombieCollect;
    sigemptyset(&sact.sa_mask);
    sigaddset(&sact.sa_mask, SIGCHLD);
    sigaction(SIGCHLD, &sact, (struct sigaction *)NULL);


    /*-------------------- for debug only ----------------*/
    for (sc=s_sehead;sc;sc=sc->next)
    {
        if (sc->qualifier != (char *) NULL)
        {   
            Debug2("%s => %s",sc->qualifier,sc->value);
        }
    }
    /*-------------------- for debug only ----------------*/

#ifdef HAVE_MOTIF
    SetnameLabel(smppp->location);
#endif

    attempt=0;
    this_num=0;
    ntry=0;
    nloop=0;
    max_busy=0;
    max_loop=0;
    p_num=1;
    s_baud="38400";
    max_redial=smppp->max_redial;
    
    if (smppp->n_numbers > 0)
        max_busy=smppp->max_redial*smppp->n_numbers;
    else
        max_busy=smppp->max_redial;

    max_loop=smppp->max_loop;

    Debug2("max_busy=%d,max_loop=%d",max_busy,max_loop);

    max_loop--;
    if (max_loop < 0)
        max_loop=0;

DialAgain:
    s_is_dialing=1;

    if (ntry >= max_busy)
    {
        if (nloop < max_loop)
        {
            ntry=0;
            attempt=0;
            this_num=0;
        }
        else
        {
            rc=(-1);
#ifdef HAVE_MOTIF
            Beep(0);Beep(0);
            SetStatusText("\n >> Unable to establish ppp connecting <<\n",0);
            fixupThings(1);
#else
            (void) fprintf(stderr,"\nUnable to establish ppp connecting\n");
#endif
            goto ExitProcessing;
        }
        nloop++;
    }

    if (ntry < max_busy)
    {
        if (attempt >= max_redial)
        {
            attempt=0;
            this_num++;
#ifdef HAVE_MOTIF
            SetStatusText("\n",0);
#endif
        }
        phone_number=smppp->numbers[this_num];
        s_baud=smppp->bauds[this_num];
    }
    else
    {
        rc=(-1);
#ifdef HAVE_MOTIF
        Beep(0);Beep(0);
        SetStatusText("\nUnable to establish ppp connecting\n",0);
#else
        (void) fprintf(stderr,"Unable to establist ppp connection!\n");
#endif
        goto ExitProcessing;
    }
    attempt++;
    ntry++;

    /* open the modem device */
    rc=openDevice();
    if (rc == -1)
    {
        Warning("Unable to open modem device: %s\n",getDevicename());
        (void) sprintf(debug_buf,"Unable to open modem device: %s\n",
            getDevicename());
        fixupThings(0);
#ifdef HAVE_MOTIF
        Beep(0);Beep(0);
        SetStatusText(debug_buf,0);
#else
        (void) fprintf(stderr,"%s\n",debug_buf);
#endif
        return (-1);
    }

    Debug2("device opened",0,0);
    /* hangup modem, in case it is hosed. it might come back */
    hangupModem();

    (void) sprintf(debug_buf,
        "\nAttempt (%d/%d), Loop (%d/%d), ph # %s, baud %s\n",
        attempt,max_redial,nloop+1,max_loop+1,phone_number,s_baud);
    Debug2("%s",debug_buf,0);
#ifdef HAVE_MOTIF
    SetStatusText(debug_buf,0);
#else
    (void) fprintf(stderr,"%s\n",debug_buf);
#endif
    sleep(1);

    /*---------------------- Send Init string  ----------------------------*/
    /* send modem init string */

#ifdef HAVE_MOTIF
    ChangeHighlightColor(INIT_LAB,COLOR_YELLOW);
    if (g_verbose == 1)
        SetStatusText("Initializing Modem ...\n",0);
    else
        SetStatusText("Initializing Modem ...",0);
#else
    if (g_verbose == 1)
        (void) fprintf(stderr,"Initializing Modem ...\n");
    else
        (void) fprintf(stderr,"Initializing Modem ...");
#endif
    (void) sprintf(debug_buf," [Sending] %s\n",smppp->init_string);
#ifdef HAVE_MOTIF
    if (g_verbose == 1)
        SetStatusText(debug_buf,0);
#else
    if (g_verbose == 1)
        (void) fprintf(stderr,"%s",debug_buf);
#endif
    (void) sprintf(debug_buf," [Expecting] %s",smppp->init_ok_response);
#ifdef HAVE_MOTIF
    if (g_verbose == 1)
        SetStatusText(debug_buf,0);
#else
    if (g_verbose == 1)
    (void) fprintf(stderr,"%s",debug_buf);
#endif

//	printf("send init_string");
    writeLine(smppp->init_string);
    /*
    ** send a CR
    */
 //   printf("send cr\n");
    sendEnter("cr");

    /* 
    ** wait for init response from modem 
    ** try 10 times, sleep 5 secs between each try 
    **
    ** try to guess max_try, it should be little more than the number of
    ** bytes of init_string+init_response
    */
    rc=waitFor(smppp->init_ok_response,max_try);
    if (rc != 0)
    {
        if (rc == MODEM_CANCEL_DIALING)
        {
#ifdef HAVE_MOTIF
            Beep(0);
            SetStatusText("\n\n >>> Dialing cancelled <<<\n",0);
#else
            (void) fprintf(stderr,"\n\n >>> Dialing cancelled <<<\n");
#endif
            fixupThings(1);
            return (-1);
        }
        else
        {
        (void) fprintf(stderr,
               "\nFailed to initialize modem device: %s\n",smppp->device);
            return (-1);
        }

    }
#ifdef HAVE_MOTIF
    ChangeHighlightColor(INIT_LAB,COLOR_GREEN);
#endif


    sleep(2);
    /*---------------------- Dial number ----------------------------*/

#ifdef HAVE_MOTIF
    if (g_verbose == 0)
    {
        SetStatusText(" initialized\n",0);
    }
     ChangeHighlightColor(DIAL_LAB,COLOR_YELLOW);
#else
     if (g_verbose == 0)
         (void) fprintf(stderr,"initialized\n");
#endif
     if (g_verbose == 1)
        (void) sprintf(debug_buf,"\nDialing %s, Speed %s ... \n"
                                 ,phone_number,s_baud);
     else
        (void) sprintf(debug_buf,"Dialing %s, Speed %s ..."
                                 ,phone_number,s_baud);

#ifdef HAVE_MOTIF
        SetStatusText(debug_buf,0);
#else
        (void) fprintf(stderr,"%s",debug_buf);
#endif

    (void) sprintf(debug_buf," [Sending] %s%s\n",
        smppp->dial_string,phone_number);
#ifdef HAVE_MOTIF
    if (g_verbose == 1)
        SetStatusText(debug_buf,0);
#else
    if (g_verbose == 1)
       (void) fprintf(stderr,"%s",debug_buf);
#endif

   (void) sprintf(debug_buf," [Expecting] %s",smppp->dial_ok_response); 
#ifdef HAVE_MOTIF
    if (g_verbose == 1)
        SetStatusText(debug_buf,0);
#else
    if (g_verbose == 1)
        (void) fprintf(stderr,"%s",debug_buf);
#endif

    /* dial the phone number */
    (void) sprintf(buf,"%s%s",smppp->dial_string,phone_number);
    writeLine(buf);
    /*
    ** send a CR
    */
   sendEnter("cr");
	//usleep(5000 * 1000);

    /* wait for dial_response */
    rc=waitFor(smppp->dial_ok_response,max_try);
    switch (rc)
    {
        case 0:
        {
#ifdef HAVE_MOTIF
            ChangeHighlightColor(DIAL_LAB,COLOR_GREEN);
#endif
		printf("Bingo!!!!!!!!!!!!!!!!!!!!!\n");
            break;
        }
        case (-1):
        {
            (void) fprintf(stderr,
                "Error reading dial response: %s\n",smppp->dial_ok_response);
            goto ExitProcessing;
            break;  /* won't be here */
        }

        case MODEM_BUSY:
        {
#ifdef HAVE_MOTIF
            Beep(0);Beep(0);
            SetStatusText("\n*** Line BUSY ***\n\n",0);
#else
            (void) fprintf(stderr,"\n *** Line BUSY ***\n");
#endif
            close(s_modemfd);
            s_modemfd=(-1);
            goto DialAgain;
            break;  /* won't be here */
        }

        case MODEM_NO_CARRIER:
        {
#ifdef HAVE_MOTIF
            Beep(0);Beep(0);
            SetStatusText("\n*** No Carrier! ***\n\n",0);
#else
            (void) fprintf(stderr,"\n *** No Carrier! ***\n");
#endif
            close(s_modemfd);
            goto DialAgain;
            s_modemfd=(-1);
            break;  /* won't be here */
        }

        case MODEM_NO_DIALTONE:
        {

#ifdef HAVE_MOTIF
            Beep(0);Beep(0);
            SetStatusText("\n*** No Dial Tone! ***\n\n",0);
#else
            (void) fprintf(stderr,"\n *** No Dial Tone! ***\n");
#endif
            close(s_modemfd);
            s_modemfd=(-1);
            goto DialAgain;
            break;  /* won't be here */
        }

        case MODEM_CANCEL_DIALING:
        {
#ifdef HAVE_MOTIF
            Beep(0);
            SetStatusText("\n\n >>> Dialing cancelled <<<\n",0);
#else
            (void) fprintf(stderr,"\n\n >>> Dialing cancelled <<<\n");
#endif
            fixupThings(1);
            return (-1);
        }
        default:
        {
#ifdef HAVE_MOTIF
            Beep(0);Beep(0);
            SetStatusText("\n *** Unknown error in startPPP() ***\n\n",0);
#else
            (void) fprintf(stderr,"Unknown error in startPPP()!\n");
#endif
            goto ExitProcessing;
        }
    }
    /* now we'r in the script section */

#ifdef HAVE_MOTIF
    ChangeHighlightColor(SCRIPT_LAB,COLOR_YELLOW);
#endif
        /* if we'r here, dialing succeeded */
#ifdef HAVE_MOTIF
        if (g_verbose == 0)
            SetStatusText(" -done-\n",0);
#else
        if (g_verbose == 0)
        {
            (void) fprintf(stderr," -done-\n");
            (void) fflush(stderr);
        }
#endif
    if (g_verbose == 1)
    {
        (void) sprintf(debug_buf,"\nInitiating send-expect sequence ...\n");
    }
    else
        (void) sprintf(debug_buf,"Initiating send-expect sequence ...");

#ifdef HAVE_MOTIF
    SetStatusText(debug_buf,0);
#else
    (void) fprintf(stderr,"%s",debug_buf);
#endif
    for (sc=s_sehead;sc;sc=sc->next)
    {
        if (sc->qualifier != (char *) NULL)
        {
            if (strcasecmp(sc->qualifier,"send") == 0)
            {
                (void) sprintf(debug_buf," [Sending] %s\n",sc->value);
#ifdef HAVE_MOTIF
                if (g_verbose == 1)
                    SetStatusText(debug_buf,0);
#else
                if (g_verbose == 1)
                    (void) fprintf(stderr,"%s",debug_buf);
#endif
                writeLine(sc->value);
                /*
                ** CR or CRLF must be explicitly stated int he scr file
                */
            }
            else
            {
                (void) sprintf(debug_buf," [Expecting] %s",sc->value);
#ifdef HAVE_MOTIF
                if (g_verbose == 1)
                    SetStatusText(debug_buf,0);
#else
                if (g_verbose == 1)
                    (void) fprintf(stderr,"%s",debug_buf);
#endif
                rc=waitFor(sc->value,100);
                if (rc != 0)
                {
                    if (rc == MODEM_CANCEL_DIALING)
                    {
#ifdef HAVE_MOTIF
                        Beep(0);
                        SetStatusText("\n >>> Dialing cancelled <<<\n",0);
#else
                        (void) fprintf(stderr,"\n >>> Dialing cancelled <<<\n");
#endif
                        fixupThings(1);
                        return (-1);
                    }
                    else
                    {
                        (void) fprintf(stderr,
                            "Unknown error in send-expect sequence!\n");
                         hangupModem();
                         return (-1);
                    }
                }
            }
        }
    }
    (void) sprintf(debug_buf," -done-\n");
#ifdef HAVE_MOTIF
    SetStatusText(debug_buf,0);
    ChangeHighlightColor(SCRIPT_LAB,COLOR_GREEN);
#else
    (void) fprintf(stderr,"%s",debug_buf);
    (void) fflush(stderr);
#endif
    if (rc == 0)
    {
#ifdef HAVE_MOTIF
        ChangeHighlightColor(PPP_LAB,COLOR_YELLOW);
#endif
        (void) sprintf(debug_buf,"Negotiating ppp handshake...\n");
#ifdef HAVE_MOTIF
        SetStatusText(debug_buf,0);
#else
        (void) fprintf(stderr,"%s",debug_buf);
        (void) fflush(stderr);
#endif
        sleep(2);
        rc=execPPP();
        if (rc != 0)
        {
            (void) fprintf(stderr,"Failed to exec pppd\n");
            /* do something */
        }
        Debug2("All Happy!!!!!!!!!!!!",0,0);

        /* init start time */
        initStartTime();

#ifdef HAVE_MOTIF
        SetStatusText(" -done-\n",0);
        ChangeHighlightColor(PPP_LAB,COLOR_GREEN);
#else
        (void) fprintf(stderr," -done-\n");
        (void) fflush(stderr);
#endif
        pppd_pid=getpppdId();
        if (pppd_pid == -1)
        {
            rc=1;
            goto ExitProcessing;
        }

        /* send signal 0 to the process and see if it's alive */
        /* if it is dead, outta here! */
#ifndef HAVE_MOTIF
        (void) fprintf(stderr,"\n --- Press Enter to close connection ---\n");
#endif
        (void) sprintf(debug_buf,"\n\nConnected! (Number %s) (Baud %s)\n",
            phone_number,s_baud);
#ifdef HAVE_MOTIF
        SetStatusText(debug_buf,0);
#else
        (void) fprintf(stderr,"%s",debug_buf);
#endif
        setPPPDState(1);
        s_is_dialing=0;
        /*
        StoreStartTime();
        */
#ifdef HAVE_MOTIF
        UpdateWindowStates();
#else
        if (g_quit == 1)    /* exit with 0 */
        {
            Debug2("g_quit=%d",g_quit,0);
            if (g_quit_time == 0)
            {
                Debug2("g_quit_time=0, exiting in 5 seconds",0,0);
                sleep(5);
                exit(0);
            }
            else
            {
                Debug2("going to timeout loop",0,0);
                i=g_quit_time;
                while (1)
                {
                    if (s_pppd_is_dead)
                    {
                        rc=(-1);
                        writeLog();
                        goto ExitProcessing;
                    }
                    i--;
                    if ( i <= 0)
                    {
                        rc=(-1);
                        if (kill(pppd_pid,SIGTERM) == 0)
                            writeLog();
                        goto ExitProcessing;
                    }
                    sleep(1);
                }   /* end while */
            }
        }

        while(!t_loop)
        {
            if (s_pppd_is_dead)
            {
                writeLog();
                goto ExitProcessing;
            }
            #if 0
            (void) sprintf(debug_buf,
                "\r%s -> %s",
                    s_start_time,
                    getElapsedTimestring(s_start_time_t,0));
            (void) fprintf(stdout,"%s",debug_buf);
            (void) fflush(stdout); 
            #endif         

            /* Exit if Enter key is pressed */          
            t_loop=waitforEnter(); 
            if(t_loop==0x33)    
            //if (t_loop)
            {
                if ((kill(pppd_pid,SIGTERM)) == 0)
                    writeLog();
            }
            
        }
#endif
    }

ExitProcessing:
    /* hangup for now, as we are testing */

    if (rc != 0)
        hangupModem();

    return (rc);
}


/*
 *  waitFor ()  - wait for expected responses from Modem
 * 
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      0                   if found what we expected
 *      MODEM_BUSY          if modem is busy
 *      MODEM_NO_CARRIER    if modem has no carrier
 *      MODEM_NO_DIALTONE   if modem has no dial tone
 *      -1                  if malloc or some other kind of errors
 *
 *  Parameters:
 *      char    *str                wait for this string
 *      int     usleep_time         sleep micro seconds (not used right now)
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-31-1997    first cut
 */

int waitFor(char *str,int usleep_time)
{
    int
        s_timeout=0,
        found,
        rc,
        i;

    /*-------------------*/
    time_t
        now,
        last;

    char
        *s=(char *) NULL,
        tmpbuf[128],
        buf[128],
        modbuf[128];

    int
        modidx=0;
    
    int
        x=0,
        maxretries=5,
        retries=0;


    /*-------------------*/

	s_timeout=300;
 
    
    /* initialize automatics */
    rc=0;
    i=0;

    while (++retries <= maxretries)
    {
#ifdef HAVE_MOTIF
        InterceptMouseClick();
        if (getCancelDialingStatus() == 1)      /* cancel dialing */
        {
            rc=MODEM_CANCEL_DIALING;
            goto ExitProcessing;
        }
#endif
        Debug2("Attempt:   %d",retries,0);
        s_timeout=300;
         //  s_timeout = 60; //===========
        time(&now);
        last=now;

         modbuf[0]='\0';
         modidx=0;
         s=buf;
         buf[0]='\0';

        while (s_timeout > 0)
        {
        	//printf("%d\n", s_timeout);
#ifdef HAVE_MOTIF
            InterceptMouseClick();
            if (getCancelDialingStatus() == 1)      /* cancel dialing */
            {
                rc=MODEM_CANCEL_DIALING;
                goto ExitProcessing;
            }
#endif
            if (*s == '\0')
            {
                x=check_io(s_modemfd,1000,buf,NULL);
                s=buf;
            }
            if (x & 1)      /* found something */
            {
                if (*s == '\r' || *s == '\n')
                {
                    modbuf[modidx]=0;
                    modidx=0;
                    Debug2("modbuf=%s",modbuf,0);
                }
                else if (modidx < 127)
                {
                    modbuf[modidx++]= *s;
                    modbuf[modidx]=0;
                }

                /* Skip to next received char */
                if (*s) s++;
            }
#ifdef HAVE_MOTIF
            InterceptMouseClick();
            if (getCancelDialingStatus() == 1) /* cancel dialing */
            {
                rc=MODEM_CANCEL_DIALING;
                goto ExitProcessing;
            }
#endif
           // printf("modbuf==%s\n",modbuf);
            found=strDoesContain(modbuf,str);
            if (found == 1)
            {
                /*
                ** if we'r here we found what we expected
                */
                if (debug_level == 3)
                {
                    (void) sprintf(tmpbuf," [Found] %s\n",modbuf);
#ifdef HAVE_MOTIF
                    if (g_verbose == 1)
                        SetStatusText(tmpbuf,0);
#else
                    if (g_verbose == 1)
                        (void) fprintf(stderr,"%s",tmpbuf);
#endif
                }
                Debug2("Found what we expected=%s",modbuf,0);
                rc=0;
                goto ExitProcessing;
            }
            else
            {
                if (strcasecmp(modbuf,MOEM_BUSY_STR) == 0)
                {
                    if (debug_level == 3)
                    {
                        (void) sprintf(tmpbuf," [Found] %s\n",modbuf);
#ifdef HAVE_MOTIF
                    if (g_verbose == 1)
                        SetStatusText(tmpbuf,0);
#else
                    if (g_verbose == 1)
                        (void) fprintf(stderr,"%s",tmpbuf);
#endif
                    }
                    rc=MODEM_BUSY;
                    goto ExitProcessing;
                }
                else if (strcasecmp(modbuf,MODEM_NOCARRIER_STR) == 0)
                {
                    if (debug_level == 3)
                    {
                        (void) sprintf(tmpbuf," [Found] %s\n",modbuf);
#ifdef HAVE_MOTIF
                    if (g_verbose == 1)
                        SetStatusText(tmpbuf,0);
#else
                    if (g_verbose == 1)
                        (void) fprintf(stderr,"%s",tmpbuf);
#endif
                    }
                    rc=MODEM_NO_CARRIER;
                    goto ExitProcessing;
                }
                else if (strcasecmp(modbuf,MODEM_NODIALTONE_STR) == 0)
                {
                    if (debug_level == 3)
                    {
                        (void) sprintf(tmpbuf," [Found] %s\n",modbuf);
#ifdef HAVE_MOTIF
                    if (g_verbose == 1)
                        SetStatusText(tmpbuf,0);
#else
                    if (g_verbose == 1)
                        (void) fprintf(stderr,"%s",tmpbuf);
#endif
                    }
                    rc=MODEM_NO_DIALTONE;
                    goto ExitProcessing;
                }
            }
            time(&now);
            if (last != now)
            {
                s_timeout -= (now-last);
                if (s_timeout < 0)
                    s_timeout=0;
            }
            usleep(250000);
            //usleep(100);
            last=now;
        }   /* while */
    }

ExitProcessing:

    return (rc);
}


/*
 *  isStillDialing()  - checks if dialing is going on
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      1           if dialing in progress
 *      0           if not
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      checks based on the static variable s_is_dialing
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

int isDialing(void)
{
    Debug2("isDialing(), s_is_dialing=%d",s_is_dialing,0);
    return (s_is_dialing);
}


/*
 *  setDialingStatus() - sets the dialing status
 *  
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int status      sets the staus to s_is_dialing (1 or 0)
 *                      1 means dialing, 0 means not dialing
 *  Side Effects:
 *      s_is_dialing static variable is changed
 *
 *  Limitations and Comments:
 *      s_didaling_status static variable is changed
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

void setDialingStatus(int status)
{
    s_is_dialing=status;
    Debug2("setDialingStatus(), s_is_dialing=%d",s_is_dialing,0);
}


/*
 *  getDialingStatus() - check if dialing is going on
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      the vaue of s_is_dialing
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
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

int getDialingStatus(void)
{
    return (s_is_dialing);
}





/*
 *  setCancelDialing() - sets the flag for cancelling dialing
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int flag        1 or 0, 1 means cancel, 0 means not
 *
 *  Side Effects:
 *      s_cancel_dialing is changed
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

void setCancelDialing(int flag)
{
    s_cancel_dialing=flag;
}




/*
 *  getCancelDialingStatus() - check if canelling is requested
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      0               no need to cancel dialing
 *      1               cancel dialing
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *     depends on static variable s_cancel_dialing
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

int getCancelDialingStatus(void)
{
    return (s_cancel_dialing);
}


/*
 *  fixupThings() - sets some static variables, call some routines to put
 *                  things back together.
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int hangup      if 1, hangup modem
 *
 *  Side Effects:
 *
 *
 *  Limitations and Comments:
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

void fixupThings(int hangup)
{
    setDialingStatus(0);
    setpppdId(-1);
    setPPPDState(0);
    
    if (hangup == 1)
        hangupModem();

#ifdef HAVE_MOTIF
    UpdateWindowStates();
#endif

}



/*
 *  DestoryScript() - release the Script linked list from outside.
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      se_sehead is freed.
 *
 *  Limitations and Comments:
 *      don't use s_sehead after freeing it
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-09-1997    first cut
 */

void DestoryScript(void)
{
    releaseScriptList(&s_sehead);
}


/*
 * writeLog() - write the log afer disconnecting
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *     none 
 *
 *  Limitations and Comments:
 *     depends on static vars in this file
 *          char *s_start_time
 *          char *phone_number
 *          time_t s_start_time_t
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-14-1997    first cut
 */

void writeLog(void)
{
    FILE
        *fp;

    char
        buf[128];

    if (*smppp->mppp_logfile == '\0')
    {
        Warning("No logfile specified!, no log will be written\n");
        return;
    }
    fp=fopen(smppp->mppp_logfile,"a");
    if (fp == (FILE *) NULL)
    {
        Warning("Unable to open log file for writing, no log will be written\n",
            smppp->mppp_logfile);
        return;
    }
    (void) sprintf(buf,"%s\t%s\t%s\t%s\n",
        s_start_time,getTime(),phone_number,
        getElapsedTimestring(s_start_time_t,0));

    (void) fprintf(fp,"%s",buf);
    (void) fflush(fp);
    (void) fclose(fp);

#ifdef HAVE_MOTIF
    Beep(0);
    Beep(0);
    SetStatusText(buf,0);
#endif
}



/*
 *  initStartTime() - initializes the start time
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      the following static variables are changed
 *          time_t  s_start_time_t
 *          char    *s_start_time
 *
 *  Limitations and Comments:
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-15-1997    first cut
 */

void initStartTime(void)
{

    /* save the start time */
    s_start_time_t=(time_t) NULL;
    time(&s_start_time_t);

    if (s_start_time != (char *) NULL)
    {
        (void) free((char *) s_start_time);
        s_start_time=(char *) NULL;
    }


    s_start_time=xstrdup(ctime(&s_start_time_t));
    /* strip line feed character */
    stripLineFeed(s_start_time);
}
/*
 *  parseCommandline() - parse command line options
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int argc
 *      char **argv
 *
 *  Side Effects:
 *      n/a
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-25-1997    first cut
 */

void parseCommandline(int argc,char **argv)
{
    char
        *option;

    int
        i;

    for (i=1; i < argc; i++)
    {
        option=argv[i];
        if ((*option == '-') || (*option == '+'))
        {
            switch(*(option+1))
            {
                case 'c':
                {
                    if (strncmp("config",option+1,4) == 0)
                    {
                        if (*option == '-')
                        {
                            i++;
                            if (i == argc)
                                Error("Missing filename in -config\n");
                        }
                        (void) strncpy(g_configfile,argv[i],
                                       sizeof(g_configfile)-1);
                        break;
                    }
                    Error("Unrecognized option: %s\n",option);
                    break;  /* won't be here */
                }

                case 'h':
                {
                        usage(argv[0]);
                    break;  /* won't be here */
                }
                case 'v':
                {
                    if (strncmp("verbose",option+1,4) == 0) 
                    {
                        if (*option == '-')
                        {
                            Debug2("verbose ON",0,0);
                            g_verbose=1;
                            break;
                        }
                        else if (*option == '+')
                        {
                            Debug2("verbose OFF",0,0);
                            g_verbose=0;
                            break;
                        }
                    }
                    else
                        Error("Unrecognized option: %s\n",option);

                    break;  /* won't be here */
                }
                case 'V':
                {
                    if (strncmp("V",option+1,1) == 0)
                    {
                        (void) fprintf(stderr,
                               "mppp/xmppp by Muhammad A Muquit\n");
                        (void) fprintf(stderr,
                               "http://www.fccc.edu/users/muquit\n\n");
                        (void) fprintf(stderr," mppp/xmppp version %s\n\n",
                                       MPPP_VERSION);
                        exit(1);
                    }
                    break;
                }
                
                case 'q':
                {
                    if (strncmp("quit",option+1,1) == 0)
                    {
                        g_quit=1;
                        if (*option == '-')
                        {
                            i++;
                            if (i == argc)
                            {
                                Error("Missing uptime value in %s\n",option);
                            }
                            g_quit_time=atoi(argv[i])*60;
                            break;
                        }
                    }
                    break;
                }

                default:
                {
                    Error("Unrecognized option: %s\n",option);
                }
            }   /* end switch */
        }
    }
}

/*
 *  usage() - shows usage and exit with 1
 *
 *  RCS
 *      $Revision: 1.2 $
 *      $Date: 2006-08-14 02:58:43 $
 *  Return Values:
 *      exit(1)
 *
 *  Parameters:
 *      char *progname
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-26-1997    first cut
 */

static void usage(char *progname)
{
    char
        **p;

    static char
        *options[]=
        {
            "-config config file    specifies the config file",
            "-h                     shows this help",
            "-verbose               verbose mode on",
            "-V                     shows version",
            "-quit minutes          exits keeping ppp connection open (only for mppp)"
            "                       If the value of minutes is 0, then the ppp connection will stay up until it is disconnected manually. If a value is specified then the pppd will be killed after the time (in mintutes )is elapsed.", 
        };

(void) fprintf(stderr,
"=========================================================================\n");
(void) fprintf(stderr,
    "mppp and xmppp version %s\n\n",MPPP_VERSION);
(void) fprintf(stderr,"PPP dialers for Linux\n");
(void) fprintf(stderr,"http://www.fccc.edu/users/muquit/\n\n");
(void) fprintf(stderr,
"=========================================================================\n");

(void) fprintf(stderr,"%s [options]\n",progname);
(void) fprintf (stderr,"\nWhere options include:\n");  
    for (p=options; *p != (char *) NULL; p++) 
        (void) fprintf (stderr," %s\n",*p);

    (void) fprintf(stderr,"\n");

    (void) fprintf(stderr,"NOTE: All standard command line options are available for xmppp\n");
    exit(1);
}
