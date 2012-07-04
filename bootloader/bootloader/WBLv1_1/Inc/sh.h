/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: sh.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: sh.h $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/WBLv1_1/Inc
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:28p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/WBLv1_1/Inc
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/28   Time: 5:35p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Inc
 * Add VSS header
 */
#ifndef NU_SH_H
#define NU_SH_H
//------------------------------------------------------------------------------------------

#define NoOfElements(array) (sizeof(array) / sizeof(array[0]))
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

// useful macros
#define _WHITESPACE(c)      ((c == ' ') || (c == '\t') || (c == '\n'))
#define _PUSHARG(p)         {if (argIndex < MAX_TOKENS) ArgStack[argIndex++] = ((char *)(p));}
#define _PUSHLEN(l)         {if (argLenIndex < MAX_TOKENS) ArgLenStack[argLenIndex++] = ((int)(l));}
#define _INITSTACK          argIndex = argLenIndex = 0
#define _STACKSIZE          argIndex



typedef struct _NU_Command
{
	CHAR *cmdName;						// command name
	CHAR *helpText;						// help text
	INT (*action)(INT , CHAR *[]) ;     // action routine for the command
} NU_Command_t;

// a table of commands
typedef struct _NU_CommandTable {
  NU_Command_t *commands ;       // pointer to a vector of commands
  INT size ;                     // the number of commands
  VOID (*error)(void) ;          // error handler for unparsable command
} NU_CommandTable_t ;

extern void sh(int argc, void *argv);

//------------------------------------------------------------------------------------------
#endif