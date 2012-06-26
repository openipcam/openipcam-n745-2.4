/*
 *  menu routines
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *
 *  Limitations and Comments:
 *      adapted from nedit source
 *
 *      the menu building routine from Dan heller's book was used earlier.
 *      but the accelerator was not working on Linux for some reason.
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-24-1997    first cut
 */


#include "xhead.h"
#include "xmppp.h"

typedef void (*MenuCallbackProc)();

/* private functiion prototypes */
static Widget      CreateMenu(Widget parent,char *name,char *label,
                   int mnemonic, Widget *cascade_btn);
static void        DoActionCb(Widget w,XtPointer client_data,
                   XtPointer call_data);
static Widget      CreateMenuItem(Widget parent,char *name, char *label,
                   int mnemonic, MenuCallbackProc callback, void *cb_arg);

/* actions table */
XtActionsRec Actions[]=
{
    {"quit",QuitCb},
    {"editc",NullCb},
    {"about",AboutDialogCb},
};
/*
 *  CreateMenubar() - main routine for creating menu
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      Widget menubar
 *
 *  Parameters:
 *      Widget parent
 *
 *  Side Effects:
 *      n/a
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-24-1997    first cut
 */

Widget CreateMenubar(Widget parent)
{
    Widget
        cascade_w,
        menubar_w,
        menupane_w;

    /* create the menubar (row column) widget */
    menubar_w=XmCreateMenuBar(parent,"menubarw",NULL,0);         

    /* create the File pulldown menu */
    menupane_w=CreateMenu(menubar_w,"filemenu","File",0,NULL);
    CreateMenuItem(menupane_w,"quit","Quit",'Q',
            DoActionCb,"quit");

    /* create the Options pulldown menu */
    menupane_w=CreateMenu(menubar_w,"optionsmenu","Options",0,NULL);
    CreateMenuItem(menupane_w,"editc","Edit Config File",'E',
            DoActionCb,"editc");

    /* create the Help pulldown menu */
    menupane_w=CreateMenu(menubar_w,"helpmenu","Help",0,&cascade_w);
    XtVaSetValues(menubar_w,
        XmNmenuHelpWidget,cascade_w,
        NULL);
    CreateMenuItem(menupane_w,"about","About ...",'A',
            DoActionCb,"about");

    return (menubar_w);
}
/*
 *  CreateMenu() - creates the pull down menu and menu item in it
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      Widget pulldown_menu
 *
 *  Parameters:
 *      Widget  parent
 *      char    *name
 *      char    *label
 *      int     mnemonic
 *      Widget  cascade_btn
 *
 *  Side Effects:
 *      n/a
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-24-1997    first cut
 */


static Widget CreateMenu(Widget parent,char *name,char *label,
    int mnemonic, Widget *cascade_btn)
{
    Widget
        menu_w,
        cascade_w;

    XmString
        xmstr;

    menu_w=XmCreatePulldownMenu(parent,name,NULL,0);       
    cascade_w=XtVaCreateWidget(name,     
        xmCascadeButtonWidgetClass, parent, 
        XmNlabelString, xmstr=XmStringCreateSimple(label),
        XmNsubMenuId, menu_w,
        NULL);
    XmStringFree(xmstr);

    if (mnemonic != 0) 
    {
        XtVaSetValues(cascade_w,
            XmNmnemonic,mnemonic,
            NULL);
    }
    XtManageChild(cascade_w);

    if (cascade_btn != NULL)
        *cascade_btn=cascade_w;

    return (menu_w);
}
/*
 *  CreateMenuItem() - create menu item
 *
 *  RCS
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:35 $
 *  Return Values:
 *      Widget push_buttonw
 *
 *  Parameters:
 *      Widget      parent
 *      char        *name
 *      char        *label
 *      int         mnemonic
 *      MenuCallbackProc    callback
 *      void        *cb_arg
 *
 *  Side Effects:
 *      n/a
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-24-1997    first cut
 */

static Widget CreateMenuItem(Widget parent,char *name, char *label,
    int mnemonic, MenuCallbackProc callback, void *cb_arg) 
{
    Widget
        pushbutton_w;

    XmString
        xmstr;

    pushbutton_w=XtVaCreateWidget(name,
        xmPushButtonWidgetClass,parent,
        XmNlabelString,xmstr=XmStringCreateSimple(label),
        XmNmnemonic,mnemonic,
        NULL);
    XmStringFree(xmstr);

    XtAddCallback(pushbutton_w,
        XmNactivateCallback,(XtCallbackProc)callback,cb_arg);

    XtManageChild(pushbutton_w);

    return (pushbutton_w);
}

/*
 *  DoActinCb() - register actions
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
 *      n/a
 *
 *  Limitations and Comments:
 *      n/a
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Jun-24-1997    first cut
 */


static void DoActionCb(Widget w,XtPointer client_data,XtPointer call_data)
{
    Widget
        menu_w;
#if XmVersion >= 1002
    menu_w=XmGetPostedFromWidget(XtParent(w));
#else
    menu=w;
#endif
    XtCallActionProc(GetShell(menu_w),(char *)client_data,
         ((XmAnyCallbackStruct *)call_data)->event, NULL,0);
}
/*
 *  RegisterMenuActions() - register actions for use in translation
 *                          retlating to menu item commands
 *
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
 *      ma_muquit@fccc.edu   Jun-24-1997    first cut
 */

void RegisterMenuActions(XtAppContext app)
{
    XtAppAddActions(app,Actions,XtNumber(Actions)); 
}
