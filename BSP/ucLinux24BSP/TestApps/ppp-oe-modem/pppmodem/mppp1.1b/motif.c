#include "xhead.h"
#include "mppp.h"

#include <X11/xpm.h>
#include "about.xpm"

#define INCLUDED_FROM_MAIN
#include "xmppp.h"

#ifndef Mmin
#define Mmin(x,y)  (((x) < (y)) ? (x) : (y))
#endif

#ifndef CHANGE_BG
#define CHANGE_BG   101
#endif

#ifndef CHANGE_FG
#define CHANGE_FG   102
#endif

#ifndef PPP_CONNECTING
#define PPP_CONNECTING  200
#endif
#ifndef PPP_CONNECTED
#define PPP_CONNECTED   201
#endif

#ifndef YES_PLEASE
#define YES_PLEASE      100
#endif

#ifndef NO_THANKS
#define NO_THANKS       200
#endif

#ifndef I_CHANGED_MY_MIND
#define I_CHANGED_MY_MIND   300
#endif

/* fallback rerources */
static char *app_defaults[]=
{
#include "appdef.h"
    NULL
}; 
/* static things */

static Display          *s_display;
static Widget           s_toplevelW,
                        s_pppPbW,
                        s_debugTextW,
                        s_indic_labW[4],
                        s_name_labW,
                        s_editcfg_dW=(Widget) NULL,
                        s_counter_labW;

static int              s_depth;

static Window           s_rootwindow;        
static XtAppContext     s_app;

static Pixel
    s_color[10],
    s_red_pixel,
    s_green_pixel,
    s_yellow_pixel,
    s_black_pixel,
    s_white_pixel;

static XtIntervalId
    s_elapse_timerId=(XtIntervalId) NULL;

static int
    s_iconic_state;

static Pixmap
    s_on_icon_pixmap=(Pixmap) NULL,
    s_off_icon_pixmap=(Pixmap) NULL;

extern time_t       s_start_time_t;

/* private prototypes*/
static void                 CreateUI(Widget parent);
static unsigned long        MallocateNamedColor(Display *display,char *name);
static int                  MbestPixel(Display *display, Colormap colormap,
                                       XColor *colors, unsigned int 
                                       number_colors,XColor *color);
static void                 SetColor(Widget w,Pixel color,int which_one);
static void                 ConnectCb(Widget,XtPointer,XtPointer);
static void                 ChangeLabel(char *str);
static void                 StartElapseTimer(Widget w,XtIntervalId tid);
static void                 UpdateElapsedTime(void);
static void                 SetElapsedTime(char *s);
static void                 Deiconified(void);
static void                 Iconified(void);
static void                 MakeSensitive(Widget w,int what);
static int                  AskUser(char *message);
static void                 AskResponse(Widget w,XtPointer client_data,
                                        XtPointer call_data);
static void                 SetScrollbarColors(Widget vsbW, Widget hsbW,
                                               char *color_name);
static void                 replaceAboutdSymbolPixmap(Widget widget);
static void                 createEditcfgD(void);

/*
 *  DoMotif() - create the user interface
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int  argc        number of args passed from main
 *      char **argv      arg verctor passed from main
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


void DoMotif(int argc,char **argv)
{
    int
        screen;

    pid_t
        pid;

    char
        rcfilename[MAXPATHLEN];

    pid=fork();

    switch (pid)
    {
        case (-1):
        {
            break;
        }

        case 0:     /* child */
        {
            break;
        }

        default:    /* prent goes bye-bye */
        {
            exit(0);
            break;  /* won't be here */
        }
    }

    *rcfilename='\0';
    /* create the toplevel widget */
    s_toplevelW=XtVaAppInitialize(&s_app,"Xmppp",
        NULL,0,&argc,argv,app_defaults,NULL);
                
    /* set the error handler */
    XSetErrorHandler(MXError);


    /* process command line options */
    parseCommandline(argc,argv);

    /* add the event handler for decting iconic state */
    XtAddEventHandler(s_toplevelW,
        StructureNotifyMask,
        False,
        (XtEventHandler) WatchIconicStates,   
        (XtPointer) NULL);

    /* create the static mostly used vars */
    s_display=XtDisplay(s_toplevelW);
    screen=XDefaultScreen(s_display);
    s_rootwindow=XDefaultRootWindow(s_display);
    s_depth=XDefaultDepth(s_display,screen);
    s_black_pixel=BlackPixel(s_display,screen);
    s_color[COLOR_BLACK]=s_black_pixel;

    s_white_pixel=WhitePixel(s_display,screen);
    s_color[COLOR_WHITE]=s_white_pixel;

    /* allocate colors we'll need */
    s_red_pixel=(Pixel) MallocateNamedColor(s_display,"red");
    s_color[COLOR_RED]=s_red_pixel;

    s_green_pixel=(Pixel) MallocateNamedColor(s_display,"green");
    s_color[COLOR_GREEN]=s_green_pixel;

    s_yellow_pixel=(Pixel) MallocateNamedColor(s_display,"yellow");
    s_color[COLOR_YELLOW]=s_yellow_pixel;

    /* create the user interface */
    CreateUI(s_toplevelW);

    /* realize the toplevel widget */
    XtRealizeWidget(s_toplevelW);

    /* create the pixmaps */
    s_on_icon_pixmap=CreatePPPONPixmap();
    s_off_icon_pixmap=CreatePPPOFFPixmap();

    /* set the off pixmap by default */
    if (s_off_icon_pixmap != (Pixmap) NULL)
    {
        XtVaSetValues(s_toplevelW,
            XmNiconPixmap,s_off_icon_pixmap,
            NULL);
    }

    /* main loop */
    XtAppMainLoop(s_app);
}


/*
 *  MXError - X error handling routine
 *      This function catches X -lib errors and handles them.  It forgives
 *      the non-fatal errors.  If no error is handled, the application will
 *      exit with the error
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *
 *  Return Values:
 *      True or False
 *
 *  Parameters:
 *      Display *display        the X display
 *      XErrorEvent *error      pointer to XErrorEvent
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   11-Oct-95      first cut  
 */

#include <X11/Xproto.h>

int MXError (Display *display,XErrorEvent *error)
{
    int
        xerrcode;

    xerrcode = error->error_code;

    if (xerrcode == BadAlloc ||
          (xerrcode == BadAccess && error->request_code==88))
    {
        return (False);
    }
    else
    {
        switch (error->request_code)
        {
            case X_GetGeometry:
            {
                if (error->error_code == BadDrawable)
                    return (False);
                break;
            }

            case X_GetWindowAttributes:
            case X_QueryTree:
            {
                if (error->error_code == BadWindow)
                    return (False);
                break;
            }

            case X_QueryColors:
            {
                if (error->error_code == BadValue)
                    return(False);
                break;
            }
        }
    }
    return (True);
}



/*
 *  GetDisplay()    - returns the static display 
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      Display *display
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      Assuming s_display is already created 
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

Display *GetDisplay(void)
{
    return (s_display);
}

/*
 *  GetrootWindow()    - returns the static root window
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      Display *display
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      Assuming s_rootwindow is already created 
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-04-1997    first cut
 */

Window GetrootWindow(void)
{
    return (s_rootwindow);
}


/*
 *  WatchIconicStates() - watch iconic states
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget      w                   toplevel widget
 *      XtPointer   client_data         not used
 *      Event       *event              pointer to X Event struct
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


void WatchIconicStates(Widget w,XtPointer client_data,XEvent *event)
{
    switch (event->type)
    {
        case MapNotify:
        {
            Deiconified();
            break;
        }

        case UnmapNotify:
        {
            Iconified();
            break;
        }
    }
}


/*
 *  QuitCb() - callback routine for Quit menu item.
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget widget
 *      XEvent *event
 *      String *args
 *      Cardinal *nArgs
 *
 *  Side Effects:
 *      exit
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */
void QuitCb(Widget w,XEvent *event,String *args,Cardinal *nArgs)
{
    int
        rc,
        answer;

    if (getPPPDState() == 1)        /* connected */
    {
        answer=AskUser("Kill pppd before quiting?\nIf you do not kill pppd,\nyour Internet link will stay up after exiting xmppp!");

        switch (answer)
        {
            case YES_PLEASE:
            {
                rc=terminatePPPD();
                if (rc == 0)
                {
                    Debug2("pppd terminated",0,0);
                }
                else
                {
                    (void) fprintf(stderr,"Could not kill pppd!\n");
                }
                break;
            }

            case NO_THANKS:
            {
                break;
            }

            case I_CHANGED_MY_MIND:
            {
                return;
                break;  /* won't be here */
            }
        }   /* switch */
    }
    exit(0);
}

/* null callback */
void NullCb(Widget w,XEvent *event,String *args,Cardinal *nArgs)
{
        (void) fprintf(stderr,"Not implemented yet\n");
        return;
}



/*
 *  CreateUI()  - create the main user interface
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget  parent      toplevel widget
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      toplevel must be created first 
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

static void CreateUI(Widget parent)
{

    Widget
        menubarW,
        mainwW,
        formW,
        name_frameW,
        bottom_frameW,
        bottom_formW,
        debug_frameW,
        debug_labW,
        frameW,
        counter_frameW,
        dummy_labW;

    Dimension
        counter_lab_width;

    XmString
        counter_lab_xmstr;

    Widget
        hsbW,
        vsbW;

    XmFontList
        flist;

    int
        i;


    /* 
     * the following widgets used to be static in this file 
     * I din't change their name after bringing them here
     */
    Widget
        s_init_labW,
        s_dial_labW,
        s_script_labW,
        s_ppp_labW;

    int
        n;

    Arg
        args[10];

    /* create main window widget */
    mainwW=XtVaCreateManagedWidget("mainww",
        xmMainWindowWidgetClass,parent,
        NULL);

    /* create the menu bar */
    /*
    menubarW=XmCreateMenuBar(mainwW,
        "menubarw",
        NULL, 
        0);
    */

    /* create menu items */
#ifdef XXX
    (void) MBuildPulldownMenu(menubarW,"File","filemenu",'F',file_items);
    (void) MBuildPulldownMenu(menubarW,"Options","optionsmenu",
                                'O',option_items);
    helpW=MBuildPulldownMenu(menubarW,"Help","helpmenu",'H',help_items);
    XtVaSetValues(menubarW,
        XmNmenuHelpWidget,  
        helpW,
        NULL);
#endif
     
    menubarW=CreateMenubar(mainwW);
    RegisterMenuActions(s_app);

    /* create a form on main window widget (the main form) */
    formW=XtVaCreateWidget("mformw",
        xmFormWidgetClass,mainwW,
        NULL);


    /* 
    ** create a dummy label, do not manage it, we'll use it get the
    ** fontlist for determining the width of the counter string
    */
    dummy_labW=XtVaCreateWidget("dummylab",
        xmLabelWidgetClass,formW,
        NULL);
    /* 
    ** determine the with of the counter string, and then set the width of
    ** the frame holding it little bigger
    */

    XtVaGetValues(dummy_labW,
        XmNfontList,&flist,
        NULL);

    counter_lab_xmstr=XmStringCreateSimple("00:00:00");
    counter_lab_width=XmStringWidth(flist,counter_lab_xmstr);
    Debug2("counter_lab_width=%d",counter_lab_width,0);
    XmStringFree(counter_lab_xmstr);
    Debug2("counter lab width=%d",counter_lab_width,0);

    /* create the frame for counter */
    counter_frameW=XtVaCreateManagedWidget("counterframew",
        xmFrameWidgetClass,formW,
        XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,2,
        XmNrightAttachment,XmATTACH_FORM,
        XmNrightOffset,2,
        XmNwidth,counter_lab_width+6,
        NULL);

    /* create the label for counter */
    s_counter_labW=XtVaCreateManagedWidget("counterlabw",
        xmLabelWidgetClass,counter_frameW,
        XmNalignment,XmALIGNMENT_CENTER,
        NULL);

    /* create the frame to hold the name of site */
    name_frameW=XtVaCreateManagedWidget("nameframew",
        xmFrameWidgetClass,formW,
        XmNleftAttachment,XmATTACH_FORM,
        XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,2,
        XmNleftAttachment,XmATTACH_FORM,
        XmNleftOffset,2,
        XmNrightAttachment,XmATTACH_WIDGET,
        XmNrightOffset,2,
        XmNrightWidget,counter_frameW,
        NULL);

    /* create the label for name */
    s_name_labW=XtVaCreateManagedWidget("namelab",
        xmLabelWidgetClass,name_frameW,
        XmNalignment,XmALIGNMENT_CENTER,
        XmNrecomputeSize,True,
        NULL);

    /* create a frame at the bottom */
    bottom_frameW=XtVaCreateManagedWidget("tframew",
        xmFrameWidgetClass,formW,
        XmNleftAttachment,XmATTACH_FORM,
        XmNleftOffset,2,
        XmNtopAttachment, XmATTACH_NONE,
        XmNrightAttachment,XmATTACH_FORM,
        XmNrightOffset,2,
        XmNbottomAttachment,XmATTACH_FORM,
        XmNbottomOffset,2,
        NULL);

    /* creae a form on the frame*/
    bottom_formW=XtVaCreateWidget("bformw",
        xmFormWidgetClass,bottom_frameW,
        NULL);

    /* create the main action push button */
    s_pppPbW=XtVaCreateManagedWidget("pppb",
        xmPushButtonWidgetClass,bottom_formW,
        XmNleftOffset,XmATTACH_FORM,
        NULL);

    XtAddCallback(s_pppPbW,XmNactivateCallback,
        (XtCallbackProc) ConnectCb,
        NULL);


    /* create a frame to hold the init label --------------*/
    frameW=XtVaCreateManagedWidget("indicator",
        xmFrameWidgetClass,bottom_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,s_pppPbW,
        XmNleftOffset,10,
        XmNshadowType, XmSHADOW_OUT,
        XmNshadowThickness, 2,
        XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,6,
        NULL);

    /* create the init label on it */
    s_init_labW=XtVaCreateManagedWidget("",
        xmLabelWidgetClass,frameW,
        XmNrecomputeSize,False,
        NULL);
    s_indic_labW[0]=s_init_labW;

    /* frame for dial label */
    frameW=XtVaCreateManagedWidget("indicator",
        xmFrameWidgetClass,bottom_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,s_init_labW,
        XmNleftOffset,2,
        XmNshadowType, XmSHADOW_OUT,
        XmNshadowThickness, 2,
        XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,6,
        NULL);

    /* create the dial label on it */
    s_dial_labW=XtVaCreateManagedWidget("",
        xmLabelWidgetClass,frameW,
        XmNrecomputeSize,False,
        NULL);

    s_indic_labW[1]=s_dial_labW;

    /* frame for script label */
    frameW=XtVaCreateManagedWidget("indicator",
        xmFrameWidgetClass,bottom_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,s_dial_labW,
        XmNleftOffset,2,
        XmNshadowType, XmSHADOW_OUT,
        XmNshadowThickness, 2,
        XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,6,
        NULL);

    /* create the script label on it */
    s_script_labW=XtVaCreateManagedWidget("",
        xmLabelWidgetClass,frameW,
        XmNrecomputeSize,False,
        NULL);

    s_indic_labW[2]=s_script_labW;

    /*frame for ppp label */
    frameW=XtVaCreateManagedWidget("indicator",
        xmFrameWidgetClass,bottom_formW,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,s_script_labW,
        XmNleftOffset,2,
        XmNshadowType, XmSHADOW_OUT,
        XmNshadowThickness, 2,
        XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,6,
        NULL);

    /* create the ppp label on it */
    s_ppp_labW=XtVaCreateManagedWidget("",
        xmLabelWidgetClass,frameW,
        XmNrecomputeSize,False,
        NULL);

    s_indic_labW[3]=s_ppp_labW;
    /* 
     * change background color of the labels to red 
     */
     for (i=0; i < 4; i++)
        SetColor(s_indic_labW[i],s_red_pixel,CHANGE_BG);

    /* create the window for displaying elapsed time */
/************
    s_timer_windowW=XtVaCreateManagedWidget("timerw",
        xmLabelWidgetClass,bottom_formW,
        XmNtopAttachment,XmATTACH_FORM,
        XmNbottomAttachment,XmATTACH_FORM,
        XmNleftAttachment,XmATTACH_WIDGET,
        XmNleftWidget,s_ppp_labW,
        XmNleftOffset,5,
        XmNrightAttachment, XmATTACH_FORM,
        NULL);
****************/
    /* create the debug text widget ----------------------------*/
    debug_frameW=XtVaCreateManagedWidget("debugframew",
        xmFrameWidgetClass,formW,
        XmNtopAttachment,XmATTACH_WIDGET,
        XmNtopWidget,name_frameW,
        XmNtopOffset,5,
        XmNleftAttachment,  XmATTACH_FORM,
        XmNleftOffset,2,
        XmNrightAttachment, XmATTACH_FORM,
        XmNrightOffset,2,
        XmNbottomAttachment,XmATTACH_WIDGET,
        XmNbottomOffset,2,
        XmNbottomWidget,bottom_frameW,
        NULL);

    /* title for frame */
    debug_labW=XtVaCreateManagedWidget("dlabw",
        xmLabelWidgetClass,debug_frameW,
        XmNchildType,XmFRAME_TITLE_CHILD,
        NULL);
    n=0;
    XtSetArg(args[n], XmNeditable, False);      n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
    XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
    s_debugTextW=XmCreateScrolledText(debug_frameW,
        "debugtextw",args,n);

    XtVaGetValues(XtParent(s_debugTextW),
        XmNverticalScrollBar, &vsbW,
        XmNhorizontalScrollBar, &hsbW,
        NULL);

    SetScrollbarColors(vsbW,hsbW,"gray65");
    /* manage widgets */
    XtManageChild(s_debugTextW);
    XtManageChild(bottom_formW);
    XtManageChild(formW);
    XtManageChild(menubarW);
}



/* X Color related utility routines */

/*
 *  MallocateNamedColor - allocates color from it's name
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $   
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      text
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
 *      pixel value  if succeeds
 *      257          if failes
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      when    who     why
 *  9/23/93     mm      first cut
 *  6/8/94      mm      returns best pixel if colormap is full
 */

static unsigned long MallocateNamedColor(Display *display,char *name)
{
    XColor
        def;
    int
        status;

    int
        screen;

    Colormap
        colormap;

    screen = DefaultScreen(display);
    colormap = DefaultColormap (display,screen);

    if (XParseColor (display,colormap,name,&def ))
    {
        status = XAllocColor (display,colormap,&def);
        if (status == 0)
        {
            /*
            ** colormap is full, so find the closest color
            */

            (void) MbestPixel (display, colormap, (XColor *) NULL,
                (unsigned int) Mmin(DisplayCells(display,screen),256),
                &def);
        }
    }
    else
    {
        (void) fprintf (stderr,"Could not Parse Color:%s\n", name);
        return(257);
    }

    return(def.pixel);
}

/* 
 *  MbestPixel()    -   Returns the best pixel
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      This function returns the closes possible color rquested.
 *      It simply determines the closest point in the RGB space of the
 *      requested R, G, B values.
 *
 *      Adapted from Imagemagick 3.0.0
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      Display         *display;
 *      Colormap        colormap;
 *      XColor          *colors;
 *      unsigned int    number_colors;
 *      XColor          *color;
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      XColor  *color;
 *
 *  Return Values:
 *      value   description
 *      300     on success
 *      301     on failure
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      when    who     why
 *  6/3/94      mm      ????
 */

static int MbestPixel(Display *display, Colormap colormap,
    XColor *colors,unsigned int number_colors,XColor *color)
{
  int
    ii,
    m_colors;

  register int
    blue_distance,   
    green_distance,
    i,
    red_distance;

  unsigned long
    distance,
    min_distance;
  
  m_colors = colors == (XColor *) NULL;

  if (m_colors)
  {
    colors = (XColor *) malloc(number_colors*sizeof(XColor));
    if (colors == (XColor *) NULL)
    {
        (void) fprintf (stderr,"Unable to get X Server Color!\n");
        return (301);
    }

    for (i=0; i < number_colors; i++)
    {
        colors[i].pixel = (unsigned long) i;
    }

    if (number_colors > 256)
    {
        number_colors = 256;
    }
     XQueryColors(display,colormap,colors,number_colors);
  }

  color->pixel=0;
  min_distance=(unsigned long) (~0);    /* just a big number */
  ii=0;

  for (i=0; i < number_colors; i++)
  {
    red_distance=(int) (colors[i].red >> 8)-(int) (color->red >> 8);
    green_distance=(int) (colors[i].green >> 8)-(int) (color->green >> 8);
    blue_distance=(int) (colors[i].blue >> 8)-(int) (color->blue >> 8);

    /*
    ** distance is distance, the co-ordinates values do not make any
    ** diffetence, i.e., negative co-ordinate value will give the same
    ** distance, because the points are squired.  We do not care about
    ** taking square root of the distance, because, we just need to find
    ** the point closest to the requested point. distance is not important
    */

    distance=red_distance*red_distance+green_distance*green_distance+
      blue_distance*blue_distance;

    if (distance < min_distance)
    {
        min_distance=distance;
        color->pixel=colors[i].pixel;
        ii=i;
    }
  }
    /*
    ** this is the closest pixel. So, share it, allocate as Read only
    */

    (void) XAllocColor(display,colormap,&colors[ii]);
    if (m_colors)
    {
        (void) free((char *) colors);
    }

    return (300);
}


/*
 *  SetColor()  - sets the background or foreground color of a widget
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *
 *
 *  Parameters:
 *      Widget  w               change color of this widget
 *      Pixel   color           change to this color
 *      int     which_one       CHANGE_BG or CHANGE_FG
 *
 *  Side Effects:
 *
 *
 *  Limitations and Comments:
 *     passed color must be alloacated before calling. 
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

static void SetColor(Widget w,Pixel color,int which_one)
{
    if (which_one == CHANGE_BG)
        XtVaSetValues(w,
            XmNbackground,color,
            NULL);
    else
        XtVaSetValues(w,
            XmNforeground,color,
            NULL);
}


/*
 *  ConnectCb() - connect push button callback routine
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget      w       
 *      XtPointer   client_data
 *      XtPointer   call_data
 *
 *  Side Effects:
 *      actions taken based on connection state
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

static void ConnectCb(Widget w,XtPointer client_data,XtPointer call_data)
{
    int
        rc;

    char
        buf[228];

    if (isDialing() == 1)           /* dialing */
    {
        Debug2("Cancel Dialing...",0,0);
        setCancelDialing(1);
    }
    else
    {
        if (getPPPDState() == 1)        /* connected */
        {
            rc=terminatePPPD();
            if (rc == 0)
            {
                UpdateWindowStates();
                SetStatusText(" \n>>> Disconnected <<<<\n",0);
                writeLog();
            }
            else
            {
                Beep(0);
                SetStatusText("\nCould not kill pppd!\n",0);
            }
        }
        else
        {
            XmTextPosition
                lpos,
                rpos;

             lpos=XmTextGetLastPosition(s_debugTextW);
            (void) sprintf(buf,"[%s] X mppp Version %s\n",
                getTime(),MPPP_VERSION);
            SetStatusText(buf,1);

            rpos=lpos+(XmTextPosition) strlen(buf)-1;
            Debug2("lpos=%d rpos=%d",lpos,rpos);
            XmTextSetHighlight(s_debugTextW,lpos,rpos,
                XmHIGHLIGHT_SELECTED);
            ChangeLabel("Cancel");

            /* start ppp */
            (void) startPPP();
        }
    }
}

/*
 *  ChangeLabel() - change the label string of connect button
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      char *str       the label string
 *
 *  Side Effects:
 *      label is changed.
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

static void ChangeLabel(char *str)
{
    XmString
        xmstr;

    xmstr=XmStringCreateSimple(str);
    XtVaSetValues(s_pppPbW,
        XmNlabelString,xmstr,
        NULL);

    XmStringFree(xmstr);
    
    /* flush event queue */
    XmUpdateDisplay(s_pppPbW);

}


/* set name label */
void SetnameLabel(char *str)
{
    XmString
        xmstr;

    xmstr=XmStringCreateSimple(str);
    XtVaSetValues(s_name_labW,
        XmNlabelString,xmstr,
        NULL);
    XmStringFree(xmstr);

}



/*
 *  InterceptMouseClick() - intercent mouse click on stop button
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
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

void InterceptMouseClick(void)
{
    XEvent
        event;


    XSync(s_display,False);
    while (XCheckMaskEvent(s_display,
        (ButtonPressMask |
        ButtonReleaseMask | 
        ButtonMotionMask |
        StructureNotifyMask |
        ExposureMask),
        &event))
    {
        XButtonEvent *bevent = &(event.xbutton);
        XExposeEvent *xevent = &(event.xexpose);
        XConfigureEvent *cevent = &(event.xconfigure);
        /*
        XAnyEvent *aevent=&(event.xany);
        */

        if ((bevent->window == XtWindow(s_pppPbW)) ||
            (xevent->type == Expose) ||
            (cevent->type == ConfigureNotify))
         {
            XtDispatchEvent (&event);
         }
    }

}


/*
 *  GetAppContext() - returns the status app context to a routine called
 *                    from somewhere.
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      XtAppContext    app
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      app context must exist.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */


XtAppContext GetAppContext(void)
{
    return (s_app);
}

/*
 *  ChangeHighlightColor() - changes the color of the inidicator
 *                            buttons.
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int which_lab       INIT_LAB or DIAL_LAB or SCRIPT_LAB or
 *                          PPP_LAB
 *
 *      int which _color    COLOR_RED or COLOR_GREEN or COLOR_YELLOW
 *                          COLOR_BLUE, COLOR_BLACK, COLOR_WHITE
 *
 *  Side Effects:
 *      the colors must the allocated already.
 *
 *  Limitations and Comments:
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    firs cut
 */


void ChangeHighlightColor(int which_lab,int which_color)
{
    XtVaSetValues(s_indic_labW[which_lab],
        XmNbackground,s_color[which_color],
        NULL);

    XmUpdateDisplay(s_indic_labW[which_lab]);
}

/*
 *  SetStatusText() - insert/replace text in status window
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      char    *text       status text
 *      int     replace     if 0, insert , if 1, replace everyhing in the
 *                          text widget.
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


void SetStatusText(char *text,int replace)
{
    XmTextPosition
        text_pos;

    if (replace == 0)   /* insert */
    {
        text_pos=XmTextGetLastPosition(s_debugTextW);
        XmTextInsert(s_debugTextW,text_pos,text);
    }
    else /* replace */
    {
        XmTextSetString(s_debugTextW,text);
    }
    text_pos=XmTextGetLastPosition(s_debugTextW);
    XmTextShowPosition (s_debugTextW,text_pos);

    XmUpdateDisplay(s_debugTextW);
}



/*
 *  StartElapseTimer()  - starts the time elapse timer. It cals the
 *                        routie to updates the elpased time in the label
 *                        at the bottom riht corner when the main window is
 *                        not iconified. if iconified, it updates the icon name.
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
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

static void StartElapseTimer(Widget w,XtIntervalId tid)
{
    UpdateElapsedTime();
    s_elapse_timerId=XtAppAddTimeOut(s_app,1000L,
        (XtTimerCallbackProc) StartElapseTimer,
        (XtPointer) NULL);
}



/*
 *  UpdateElapsedTime() - update the label
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none
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

static void UpdateElapsedTime(void)
{
    char
        *str;

    str=getElapsedTimestring(s_start_time_t,0);
    /* 
    ** if the window was iconified when the dialing was in progress, we
    ** may have lost this event. So check only once to find the state.
    ** because all events will be dispatched on time from now on. we'r
    ** in the timer event loop you know!
    */
    /*
    ** well this code segment is not working. I think it's a window
    ** manager bug. Because if the window is in iconic state, the resource
    ** XmNiconic should return True
    */

#ifdef BUGG_MWM_SUX
    if (first_time == 1)
    {
        Boolean
            is;
        XtVaGetValues(s_toplevelW,
            XmNiconic,&s_iconic_state,
            NULL);
        first_time=0;
        Debug2("Iconic state=%d",is,0);
    }
#endif

    SetElapsedTime(str);
}



/*
 *  SetElapsedTime() - sets the elapsed time string to appropriate
 *                     place. For example, if the main window is iconified,
 *                     it will update the icon name other wise it will
 *                     update label in the timer window.
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      char    *str        string to update with
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

static void SetElapsedTime(char *s)
{
    XmString
        xmstr;

    if (s_iconic_state == True)
    {
        XtVaSetValues(s_toplevelW,
            XmNiconName,s,
            NULL);
    }
    else
    {
        xmstr=XmStringCreateSimple(s);
        XtVaSetValues(s_counter_labW,
            XmNlabelString,xmstr,
            NULL);
        XmStringFree(xmstr);
    }
}



/*
 *  Deiconified() - called if the main window is deiconified or mapped
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none
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

static void Deiconified(void)
{
    s_iconic_state=False;
    Debug2("----- Deiconified ------",0,0); 
}

/*
 *  Iconified() - called if the main window is Iconified
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none
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

static void Iconified(void)
{
    s_iconic_state=True;
    Debug2("----- Iconified ------",0,0); 
} 



/*
 *  UpdateWindowStates() - updates object states in the window. For example,
 *                 if PPP is up or down  it will change the icon pixmap based
 *                 on it. If PPP is down,it will remove the elampsed timer
 *                 and change the icon name to xmppp.
 *                 This function must not be called from a loop, it will
 *                 only be called when specific situation changes, e.g.,
 *                 connection succeeded, or died.
 *                 This routien will be called from the signal handler
 *                 which gets notified if pppd dies for some reason.
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
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

void UpdateWindowStates(void)
{
    int
        i;

    if (getCancelDialingStatus() == 1)
    {
        setCancelDialing(0);        
    }

    if ((getPPPDState() == 0) && (getpppdId() == -1))  /* pppd is dead */
    {
        Debug2("UpdateWindowStates() pppd is really dead!",0,0);
        /* clean up */
        if (s_elapse_timerId)
        {
            XtRemoveTimeOut(s_elapse_timerId);
            s_elapse_timerId=(XtIntervalId) NULL;
        }
            /* make the connect button sensitive */
            MakeSensitive(s_pppPbW,True);
            /* change the label to "Connect" */
            ChangeLabel("Connect");
            /* change color of the inidicator to red */

             for (i=0; i < 4; i++)
                ChangeHighlightColor(i,COLOR_RED);

            /* set the elapsed timer label to 00:00:00 */
            /*
            ** well, let not change the elapsed timer, it will give us
            ** some clue in case pppd died
            */
            /*
             SetElapsedTime("00:00:00");
             */

            /* set off icon */
            if (s_off_icon_pixmap != (Pixmap) NULL)
            {
                XtVaSetValues(s_toplevelW,
                    XmNiconPixmap,s_off_icon_pixmap,
                    NULL);
            }

            /* change icon name to xmppp, timer might have changed the name*/
            XtVaSetValues(s_toplevelW,
                XmNiconName,"xmppp",
                NULL);
    }
    else
    {
        /* Make the connect button sensitive */
        MakeSensitive(s_pppPbW,True);

        /* change the label to "Hangup" */
        ChangeLabel("Hangup");

        /* start elapse timer */
        if (s_elapse_timerId == ((XtIntervalId) NULL))
            StartElapseTimer((Widget) NULL,(XtIntervalId) NULL);

        /* set on icon */
        if (s_on_icon_pixmap != (Pixmap) NULL)
        {
            XtVaSetValues(s_toplevelW,
                XmNiconPixmap,s_on_icon_pixmap,
                NULL);
        }
    }
}



/*
 *  
 *  MakeSensitive() - make a widget sensitive/insensitive
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget  widget  the widget 
 *      int what        1, make sensitive, 0, make insensitive
 *
 *  Side Effects:
 *      sensitivity is changed
 *
 *  Limitations and Comments:
 *      Woks with widgets only, not gadgets
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

static void MakeSensitive(Widget w,int what)
{
    if (what == 1)
    {
        if (!XtIsSensitive(w))  /* in sensitive */
        {
            XtSetSensitive(w,True);
        }
    }
    else
    {
        if (XtIsSensitive(w))
            XtSetSensitive(w,False);
    }
}



/*
 *  StoreStartTime() - record the start time (time_t) in a static variable
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none
 *
 *  Side Effects:
 *      s_start_time is changed
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-07-1997    first cut
 */

/*
void StoreStartTime(void)
{
    time(&s_start_time);
}
*/

/*
 *  AboutDialogCb() - create and displays the about dialog
 *                    called from the About menu item.
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      none of the params are used
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

void AboutDialogCb(Widget w,XEvent *event,String *xargs,Cardinal *nArgs)
{
    static Widget
        saboutdW=(Widget) NULL;
    
    int
        n=0;

    Arg
        args[10];

    if (!saboutdW)
    {
        XitAddXmStringConverter(s_app);

        n=0;
        XtSetArg (args[n], XmNautoUnmanage, True); n++;
        XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
        
        saboutdW=XmCreateInformationDialog(s_toplevelW,"aboutdw",
            args,n);
        
        XtUnmanageChild(XmMessageBoxGetChild(saboutdW,XmDIALOG_HELP_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(saboutdW,XmDIALOG_CANCEL_BUTTON));
        replaceAboutdSymbolPixmap(saboutdW);
    }

    if (saboutdW)
    {
        if (!XtIsManaged(saboutdW))
            XtManageChild(saboutdW);
    }
}


/*
 *  AnskUser() - ask yes/no question, goes to a simulated loop until it gets
 *               an answer.
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      YES_PLEASE           if answer is Yes
 *      NO_THANKS            if answer is  No
 *
 *  Parameters:
 *      char    *message     Question to ask.
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      adapted from Dan's book Vol1
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-10-1997    first cut
 */

static int AskUser(char *message)
{
    static Widget
        qdw=(Widget) NULL;

    int
        n=0;

    Arg
        args[5];

    static int
        answer;


    XmString
        xmsq;
    answer=0;

    if (qdw == (Widget) NULL)
    {
        n=0;
        XtSetArg(args[n],XmNdialogStyle,
            XmDIALOG_PRIMARY_APPLICATION_MODAL);  n++;
        XtSetArg(args[n],XmNautoUnmanage,True);   n++;
        qdw=XmCreateQuestionDialog(s_toplevelW,"askuserd",args,n);

        XtAddCallback(qdw,
            XmNokCallback,(XtCallbackProc) AskResponse,
            &answer);

        XtAddCallback(qdw,
            XmNcancelCallback,(XtCallbackProc) AskResponse,
            &answer);
        XtAddCallback(qdw,
            XmNhelpCallback,(XtCallbackProc) AskResponse,
            &answer);

        /*
        XtUnmanageChild (XmMessageBoxGetChild(qdw,XmDIALOG_HELP_BUTTON));
        */
    }

    /* assuming message is not NULL. I'm calling it, so it will be NULL! */
    /*
    xmsq=XmStringCreateSimple(message);
    */
    xmsq=XmStringCreateLtoR(message,XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(qdw,
        XmNmessageString,xmsq,
        NULL);

    XmStringFree(xmsq);

    XBell(s_display,0);
    XtManageChild(qdw);
    XtPopup (XtParent(qdw),XtGrabNone);

    /*
    ** simulate main loop until user provides an answer.  Whenever an
    ** answer is received, "answer" changes from callback routine.
    ** If we do not simulate main loop, control will be handed over to
    ** main main loop and we won't have chance to take action after
    ** getting the answer
    */

    while (answer == 0 || XtAppPending(s_app))
    {
        XtAppProcessEvent(s_app,XtIMAll);
    }

    XtPopdown(XtParent(qdw));

    return (answer);

}



/*
 *  AskResponse() - callback routine of OK and Cancel button of
 *                  Question dialog
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget      w
 *      XtPointer   client_data (returns)
 *      XtPointer   call_data
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-10-1997    first cut
 */

static void AskResponse(Widget w,XtPointer client_data,XtPointer call_data)
{
    XmAnyCallbackStruct 
        *cbs=(XmAnyCallbackStruct *) call_data;
    int
        *ans;

    ans=(int *) client_data;

    switch (cbs->reason)
    {
        case XmCR_OK:
        {
            (*ans) = YES_PLEASE;
            break;
        }

        case XmCR_CANCEL:
        {
            (*ans) = NO_THANKS;
            break;
        }

        case XmCR_HELP:
        {
            (*ans)=I_CHANGED_MY_MIND;
            break;
        }
    }
}


/*
 *  Beep()      - beep
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      int volume      volume (-100 - 100)
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      s_diaplay must exist
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-11-1997    first cut
 */

void Beep(int vol)
{
    XBell(s_display,vol);
}



/*
 *  SetScrollbarColors()    -   set scrollbar colors.
 *      This function determines the best color for the scrollbars
 *      based on the color which will be it's background color.
 *      We have to do this way, because we did not create the scrollbar, we
 *      got it from scrolled window
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget vsbW         vertical scroll bar
 *      Widget hsbW         horizontal scroll bar
 *      char   *color_name  name of the color
 *
 *  Side Effects:
 *
 *
 *  Limitations and Comments:
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-11-1997    first cut
 */

static void SetScrollbarColors(Widget vsbW,Widget hsbW,char *color_name)
{ 
    Pixel
         bgcolor;

    Colormap
        colormap;
  
    Pixel
        scb_bg,
        scb_topshadow,
        scb_bottomshadow,
        scb_select,
        scb_border;


    bgcolor=(Pixel) MallocateNamedColor(s_display,color_name);
    scb_bg = bgcolor;

    /*
    ** get the colormap
    */
    XtVaGetValues (vsbW,
        XmNcolormap, &colormap,
        NULL);

    /*
    ** let motif calculate the new colors based on the bg color
    */
  
    XtVaSetValues (vsbW,
        XmNbackground,          scb_bg,
        NULL);   

    if (hsbW != (Widget) NULL)
    {
        XtVaSetValues(hsbW,
            XmNbackground, scb_bg,
            NULL);
    }
    XmGetColors (XtScreen(vsbW),
        colormap, scb_bg,
        &scb_border,
        &scb_topshadow,
        &scb_bottomshadow,
        &scb_select);   /* trough color, selec_color will be for toggle b*/

    /*
    ** set the colors
    */

    XtVaSetValues (vsbW,
        XmNbackground,          scb_bg,
        XmNtopShadowColor,      scb_topshadow,
        XmNbottomShadowColor,   scb_bottomshadow,
        XmNtroughColor,         scb_select,
        XmNarmColor,            scb_select,
        XmNborderColor,         scb_border,
        NULL);


    if (hsbW != (Widget) NULL)
    {
        XtVaSetValues (hsbW,
            XmNbackground,          scb_bg,
            XmNtopShadowColor,      scb_topshadow,
            XmNbottomShadowColor,   scb_bottomshadow,
            XmNtroughColor,         scb_select,
            XmNarmColor,            scb_select,
            XmNborderColor,         scb_border,
            NULL);
    }
}




/*
 *  replaceAboutdSympolPixmap() - replace the about dialog symbol pixmap
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget  widget     the about dialog
 *
 *  Side Effects:
 *      the default pixmap is replaced
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-14-1997    first cut
 */

static void replaceAboutdSymbolPixmap(Widget widget)
{
    XpmAttributes
        xpmatts;

    Pixmap
        pix_returned,
        mask,
        button_pixmap;

    Pixel
        bg,
        fg;

    XGCValues
        gcvals;

    GC
        gc;

    int
        rc;

    long
        gcmask=0L,
        dynamic_mask=0L,
        dont_care_mask=0L;

    pix_returned=(Pixmap) NULL;
    mask=(Pixmap) NULL;
    button_pixmap=(Pixmap) NULL;


    xpmatts.closeness=65536;
    xpmatts.valuemask = XpmSize | XpmCloseness;

    rc=XpmCreatePixmapFromData(s_display,s_rootwindow,
        about_xpm,
        &pix_returned,
        &mask,
        &xpmatts);

    if (rc != XpmSuccess)   
        return;

    button_pixmap=XCreatePixmap(s_display,pix_returned,
        xpmatts.width,
        xpmatts.height,
        s_depth);

    XtVaGetValues(widget,
        XmNbackground,&bg,
        XmNforeground,&fg,
        NULL);

    gcmask=GCForeground | GCBackground;
    gcvals.foreground=bg;
    gcvals.background=fg;
    gc=XtAllocateGC(widget,s_depth,gcmask,&gcvals,dynamic_mask,dont_care_mask);
    XFillRectangle(s_display,button_pixmap,gc,0,0,
        xpmatts.width,xpmatts.height);
    XSetClipMask(s_display,gc,mask);
    XSetClipOrigin(s_display,gc,0,0);
    XCopyArea(s_display,pix_returned,button_pixmap,gc,
        0,0,xpmatts.width,xpmatts.height,0,0);
    XtVaSetValues(widget,
        XmNsymbolPixmap,button_pixmap,
        NULL);

    if (pix_returned != (Pixmap) NULL)
        XFreePixmap(s_display,pix_returned);

    if (mask != (Pixmap) NULL)
        XFreePixmap(s_display,mask);

}



/*
 *  createEditcfgD() - create the edit cfg dialog
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
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
 *      ma_muquit@fccc.edu   Jun-14-1997    first cut
 */

static void createEditcfgD(void)
{
    static int
        initialized=False;

    XmString
        xmstr;

    Widget
        actiona_formW,
        sepW,
        scrolledW,
        rcW,
        w_frame,
        w_form,
        w_label,
        w_tf;

    char
        *label;

    Dimension
        sw_width,
        form_width;

    int
        i,
        len;

    /* create dialog only once */
    if (initialized)
        return;
    initialized=True;

    /* create a form dialog */
    s_editcfg_dW=XmCreateFormDialog(s_toplevelW,"editcfgdw",NULL,0);

    /* create for form for action area */
    actiona_formW=XtVaCreateWidget("actionaw",
        xmFormWidgetClass,s_editcfg_dW,
        XmNmarginHeight,10,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        NULL);
        
    /* create ok, cancel button == todo */


    XtManageChild(actiona_formW);

    /* create a separator */
    sepW=XtVaCreateManagedWidget("esepw",
        xmSeparatorWidgetClass,s_editcfg_dW,
        XmNbottomAttachment,XmATTACH_WIDGET,
        XmNbottomWidget,actiona_formW,
        XmNleftAttachment,XmATTACH_FORM,
        XmNrightAttachment,XmATTACH_FORM,
        NULL);

    /* create a scrolled window to hold things */
    scrolledW=XtVaCreateManagedWidget("escrollw",
        xmScrolledWindowWidgetClass,s_editcfg_dW,
        XmNscrollingPolicy,XmAUTOMATIC,
        XmNtopAttachment,XmATTACH_FORM,
        XmNtopOffset,10,
        XmNbottomAttachment,XmATTACH_WIDGET,
        XmNbottomWidget,sepW,
        XmNbottomOffset,10,
        XmNleftAttachment,XmATTACH_FORM,
        XmNbottomOffset,10,
        XmNleftAttachment,XmATTACH_FORM,
        XmNleftOffset,10,
        XmNrightAttachment,XmATTACH_FORM,
        XmNrightOffset,10,
        NULL);

    XtVaGetValues(scrolledW,
        XmNwidth, &sw_width,
        NULL);
    form_width = sw_width+40;

    /* create rc widget*/
    rcW=XtVaCreateWidget("ercw",
        xmRowColumnWidgetClass,scrolledW,
        NULL);

    /* layout widgets */
    for (i=0; i < 10; i++)
    {
        w_frame=XtVaCreateWidget("eframe",
            xmFrameWidgetClass,rcW,
            NULL);
    
        w_form=XtVaCreateWidget("form",
            xmFormWidgetClass, w_frame,
            XmNwidth,form_width,
            NULL);

        len=40;
        label=XtNewString("This is a test");
            
        xmstr=XmStringCreateSimple(label);
        XtFree(label);

        w_label=XtVaCreateManagedWidget("label",
            xmLabelWidgetClass,w_form,
            XmNlabelString,xmstr,
            XmNtopAttachment,       XmATTACH_FORM,
            XmNbottomAttachment,    XmATTACH_FORM,
            XmNleftAttachment,      XmATTACH_FORM,  
            XmNleftOffset,          6,
            NULL);

        XmStringFree(xmstr);

        w_tf=XtVaCreateManagedWidget("textfieldw",
            xmTextFieldWidgetClass,w_form,
            XmNtopAttachment,       XmATTACH_FORM,
            XmNbottomAttachment,    XmATTACH_FORM,
            XmNrightAttachment,     XmATTACH_FORM,
            XmNleftAttachment,      XmATTACH_POSITION,
            XmNleftPosition,        100,
            XmNtopOffset,           6,
            XmNbottomOffset,        6,
            XmNrightOffset,         6,
            NULL);


        XtManageChild(w_form);
        XtManageChild(w_frame);
    }
    /* manage */
    XtManageChild(rcW);
    XtManageChild(scrolledW);
}


/*
 *  cfgEditCb() - callback routine for editing configuration file
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      none
 *
 *  Parameters:
 *      Widget  widget              not used
 *      XtPointer client_data       not used
 *      XtPointer call_data         not used
 *
 *  Side Effects:
 *      n/a
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-16-1997    first cut
 */

void cfgEditCb(Widget widget,XtPointer client_data,XtPointer call_data)
{
    createEditcfgD();
    if (!XtIsManaged(s_editcfg_dW))
        XtManageChild(s_editcfg_dW);
}
/*
 *  GetShell() - clib throught the widget tree to find out the parent
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      Widget shell
 *
 *  Parameters:
 *      Widget w
 *
 *  Side Effects:
 *      none
 *
 *  Limitations and Comments:
 *      none
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-24-1997    first cut
 */

Widget GetShell(Widget w)
{
    Widget
        parent;

    while (True)
    {
        parent=XtParent(w);
        if (parent == NULL)
            return (NULL);
        if (XtClass(parent) == applicationShellWidgetClass) 
            break;
        w=parent;
    }
    return (parent);
}
