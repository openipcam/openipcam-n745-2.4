/*****************************************************************************/
/*
	XmString Converter

	Author:		Krist Paquay
			krist@tpg.tpg.oz.au

	24/2/95

*/
/*****************************************************************************/

#include <stdio.h>

#include <X11/Intrinsic.h>

#include <Xm/Xm.h>


/*****************************************************************************/

XmString
XitStringToXmString (string)
String 
    string;
{
XmString	the_xm_string, xm_sub_string, new_xm_string;
int		len, i, j;
char		*sub_string, *tag;
Boolean		in_tag;

len = strlen(string);

sub_string = XtMalloc(len+1);
tag = XtMalloc(len+1);

tag[0] = sub_string[0] = '\0';

in_tag = False;

the_xm_string = NULL;

for (i=0, j=0; i<len; i++)
  {
  switch (string[i])
    {
    case '<':
      if (in_tag)
	tag[j++] = string[i];
      else if (i>0 && string[i-1] == '\\')
	sub_string[j-1] = string[i];
      else if (j>0)
	{
	sub_string[j] = '\0';

	xm_sub_string = XmStringCreateLtoR(sub_string,
		      (tag[0] == '\0' ? XmSTRING_DEFAULT_CHARSET : tag));
	new_xm_string = XmStringConcat(the_xm_string, xm_sub_string);
	XmStringFree(the_xm_string);
	XmStringFree(xm_sub_string);
	the_xm_string = new_xm_string;

	j = 0;
	in_tag = True;
	}
      else
	in_tag = True;
      break;

    case '>':
      if (in_tag)
	{
	tag[j] = '\0';
	in_tag = False;
	j = 0;
	}
      else
	sub_string[j++] = string[i];
      break;

    default:
      if (in_tag)
	tag[j++] = string[i];
      else
	sub_string[j++] = string[i];
      break;
    }
  }

if (in_tag)
  {
  fprintf(stderr, "XitStringToXmString():  Unclosed Tag, '>' missing !\n");
  }
else
  {
  sub_string[j] = '\0';

  xm_sub_string = XmStringCreateLtoR(sub_string,
		      (tag[0] == '\0' ? XmSTRING_DEFAULT_CHARSET :tag));
  new_xm_string = XmStringConcat(the_xm_string, xm_sub_string);
  XmStringFree(the_xm_string);
  XmStringFree(xm_sub_string);
  the_xm_string = new_xm_string;
  }

XtFree(sub_string);
XtFree(tag);

return the_xm_string;
}

/*****************************************************************************/

#define	done(type, value)					\
	{							\
	    if (toVal->addr != NULL) {				\
		if (toVal->size < sizeof(type)) {		\
		    toVal->size = sizeof(type);			\
		    return False;				\
		}						\
		*(type*)(toVal->addr) = (value);		\
	    }							\
	    else {						\
		static type static_val;				\
		static_val = (value);				\
		toVal->addr = (XPointer)&static_val;		\
	    }							\
	    toVal->size = sizeof(type);				\
	    return True;					\
	}

/*****************************************************************************/

static Boolean
XitCvtStringToXmString (display,args,num_args,fromVal,toVal,
	converter_data)
Display
    *display;
XrmValuePtr
    args;
Cardinal
    *num_args;
XrmValuePtr
    fromVal;
XrmValuePtr
    toVal;
XtPointer
    *converter_data;
	
{
char		*name = (char *)fromVal->addr;
XmString	the_xm_string;

if (*num_args != 0)
  {
  XtErrorMsg("wrongParameters","XitCvtStringToXmString","XtToolkitError",
	"String to XmString conversion needs no arguments",
	(String *)NULL, (Cardinal *)NULL);
  
  return False;
  }

/*
	If the input string doesn't start with '<' we just do what
	the standard Motif converter does.
	This is only for performance reasons.
	The function XitStringToXmString() can handle
	input not starting  with '<'.
*/

if (name && name[0] == '<')
  {
  the_xm_string = XitStringToXmString(name);
  }
else
  the_xm_string = XmStringCreateLtoR(name, XmSTRING_DEFAULT_CHARSET);

if (the_xm_string)
  {
  done(XmString, the_xm_string)
  }
else
  {
  XtStringConversionWarning(name, "XmString");
  return False;
  }
}

/*****************************************************************************/

static void
XitCvtFreeXmString (app,toVal,converter_data,args,num_args)
XtAppContext
    app;
XrmValuePtr
    toVal;
XtPointer
    converter_data;
XrmValuePtr
    args;
Cardinal
    *num_args;
{
XmString	the_xm_string;

the_xm_string = *((XmString *)toVal->addr);

XmStringFree(the_xm_string);
}

/*****************************************************************************/

void
XitAddXmStringConverter (app_context)
XtAppContext
    app_context;
{
/* String to XmString */

/*
	Register our Converter and Destructor
	The caching style is "XtCacheAll | XtCacheRefCount".
	Thus we cache everything until the relevant widget is destroyed
	and there and no more references.
*/

XtAppSetTypeConverter(app_context, XtRString, XmRXmString,
		XitCvtStringToXmString, NULL, 0,
		XtCacheAll | XtCacheRefCount, XitCvtFreeXmString);

		/*
			Not the same caching as the original Motif converter !
			Motif uses (XtCacheNone | XtCacheRefCount)
			but i think caching is better...
		*/
}
