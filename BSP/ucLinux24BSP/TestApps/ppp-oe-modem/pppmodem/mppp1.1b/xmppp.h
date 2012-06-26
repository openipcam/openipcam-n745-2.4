#ifndef XMPPP_H
#define XMPPP_H

/*
 *  xmppp.h  - header file for xmppp. must be included after xhead.h
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *
 *  Development History:
 *      who                     when        why
 *      ma_muquit@fccc.edu      17-Nov-1996 first cut
 */

#ifndef True
#define True    1
#endif

#ifndef False
#define False 0
#endif

/* function prototypes */
void        AboutDialogCb(Widget w,XEvent *event,String *args,Cardinal *nArgs);
Widget      CreateMenubar(Widget parent);
Pixmap      CreatePPPONPixmap(void);
Pixmap      CreatePPPOFFPixmap(void);
void        DoMotif(int argc,char **argv);
Display     *GetDisplay(void);
Widget      GetShell(Widget w);
Window      GetrootWindow(void);
int         MXError (Display *display,XErrorEvent *error);
void        NullCb(Widget w,XEvent *event,String *args,Cardinal *nArgs);
void        RegisterMenuActions(XtAppContext app);
void        QuitCb(Widget w,XEvent *event,String *args,Cardinal *nArgs);
void        WatchIconicStates(Widget w,XtPointer unused,XEvent *event);
XmString    XitStringToXmString (String str);
void        XitAddXmStringConverter (XtAppContext);
void        cfgEditCb(Widget widget,XtPointer client_data,XtPointer call_data);



#endif  /* XMPPP_H */
