/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: sh.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: sh.c $
 * 
 * *****************  Version 12  *****************
 * User: Yachen       Date: 07/07/18   Time: 8:17p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Modified SET_usage() function.
 * 1. Let it show the correct parameter for setting ip and mac address.
 * 2. Display mac option in USB version bootloader as well. uClinux kernel
 * might need it. 
 * 
 * *****************  Version 11  *****************
 * User: Yachen       Date: 07/03/13   Time: 4:15p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Fixed a GPIO config bug
 * 
 * *****************  Version 10  *****************
 * User: Yachen       Date: 07/01/24   Time: 2:17p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * 
 * *****************  Version 9  *****************
 * User: Yachen       Date: 07/01/24   Time: 9:26a
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Restore UNZIP function
 * 
 * *****************  Version 8  *****************
 * User: Yachen       Date: 06/08/16   Time: 5:49p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * 1. Add command INTF
 * 2. USB -on is no more required for MT & FT even USB is disabled.
 * bootloader wil disable USB after transmit complete.
 * 
 * *****************  Version 7  *****************
 * User: Yachen       Date: 06/03/31   Time: 3:39p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Fixed image 0 size error bug in SET_action()
 * 
 * *****************  Version 6  *****************
 * User: Yachen       Date: 06/02/15   Time: 11:07a
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Add run-time USB enable/disable function
 * 
 * *****************  Version 5  *****************
 * User: Yachen       Date: 06/02/13   Time: 11:25a
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Disable USB before deleting inage 0, restore it afterward
 * 
 * *****************  Version 4  *****************
 * User: Yachen       Date: 06/01/20   Time: 7:30p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Add USB status flag, disable USB only if it's enabled
 * 
 * *****************  Version 2  *****************
 * User: Yachen       Date: 06/01/19   Time: 2:18p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Add USB support. User can chose between USB and MAC in shell.map
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/shell
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/shell
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 8  *****************
 * User: Wschang0     Date: 04/12/27   Time: 9:27a
 * Updated in $/W90P710/FIRMWARE/shell
 * Fix the bug that not backup the serial number when setting other
 * options
 * 
 * *****************  Version 7  *****************
 * User: Wschang0     Date: 04/11/22   Time: 3:18p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add serial number item
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 03/11/05   Time: 11:04a
 * Updated in $/W90P710/FIRMWARE/shell
 * Add RMII Option (SET_action)
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/08/27   Time: 1:39p
 * Updated in $/W90P710/FIRMWARE/shell
 * Modify CHK_action to check image checksum
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/27   Time: 11:29a
 * Updated in $/W90P710/FIRMWARE/shell
 * Add ATTRIB_action function
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/26   Time: 9:29a
 * Updated in $/W90P710/FIRMWARE/shell
 * DEL_action was modified to be more smart
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:07p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add VSS header
 */
#include <string.h>
#include <rt_heap.h>
#include <errno.h>
#include "cdefs.h"
#include "sh.h"
#include "xmodem.h"
#include "platform.h"
#include "flash.h"
#include "zlib.h"
#include "armzip.h"
#include "flash.h"
#include "bib.h"
#include "uprintf.h"
#include "xmodem.h"
#include "net.h"
#include "tftp.h"
#include "serial.h"
 
extern int enableUSB; 
// for download buffer
#define STATIC_DOWNLOAD_BUFFER 0x100000

// announce
#define SHELL_ANNOUNCE "\nW90P710 Command Shell v1.0 Rebuilt on " __DATE__ " at " __TIME__ "\n"

// for SWI
char command_buffer[256];


// Use a stack algorithm to parse a command string into individual tokens
#define MAX_TOKENS 20
static char *ArgStack[MAX_TOKENS];
static int ArgLenStack[MAX_TOKENS];
static argIndex = 0;
static argLenIndex = 0;


// main shell command
VOID NU_parse (CHAR *prompt);


// forward action routine definitions
 int H_action(int argc, char *argv[]);
 int D_action(int argc, char *argv[]);
 int E_action(int argc, char *argv[]);
 int MX_action(int argc, char *argv[]);
 int MT_action(int argc, char *argv[]);
 int G_action(int argc, char *argv[]);
 int B_action(int argc, char *argv[]);
 int FT_action(int argc, char *argv[]);
 int FX_action(int argc, char *argv[]);
 int CP_action(int argc, char *argv[]);
 int LS_action(int argc, char *argv[]);
 int CHK_action(int argc, char *argv[]);
 int SET_action(int argc, char *argv[]);
 int RUN_action(int argc, char *argv[]);
 int DEL_action(int argc, char *argv[]);
 int MSET_action(int argc, char *argv[]);
 int TERM_action(int argc, char *argv[]);
 int CACHE_action(int argc, char *argv[]);
 int UNZIP_action(int argc, char *argv[]);
 int BOOT_action(int argc, char *argv[]);
 int ATTRIB_action(int argc, char *argv[]);

 void ERROR_action(void);


#define MAX_NUM 8
int _toupper(int c);
static unsigned int  HYC_DumpAddress(unsigned int addr, int width, int swap);
static int  EditMemory(unsigned int taddr, int align, int swap);
int validhex(char *s);
unsigned int s2hex(char *s);
INT s2ip(CHAR *s, CHAR *ip);
int s2dec(char *string);
UINT32 s2u(char *string);
char * GetAddress(void);
INT wmemcpy(VOID * dest, VOID * src, UINT32 size);

// IRQ
extern void DisableIRQ(void);
extern void EnableIRQ(void);

// BIB info
extern 	void BIB_ShowInfo(tbl_info *);

// for TFTP server 
int _net_init=0;
int _net_put=0;
int _dhcp=0;

// from command.c
extern NU_Command_t NU_Commands[];
extern NU_CommandTable_t NU_CommandTable;


VOID sh(INT argc, VOID *argv)
{
	extern char shell_prompt[];
	uprintf("\n\n");
	NU_parse (shell_prompt);
}



// parse a string (insofar as you have it) into a series of tokens
static void _parser(char *c, int len)
{
    int whitespace, tlen;
    int i;
    int quotes;

    // init
    quotes= FALSE;
    whitespace = FALSE;
    _INITSTACK;
    _PUSHARG(c);
    tlen = 0;

    // build up the set of tokens (a token is any string of non-whitespace)
    while (len--)
    {
        if ( _WHITESPACE(*c) && (quotes==FALSE) )
        {
            // if we were in a token, push its length in bytes
            if (!whitespace)
                _PUSHLEN(tlen);
            // ALWAYS set these (saves a state/event check 
            whitespace = TRUE;
            tlen = 0;
        }
        else if( *c=='"' )
        {
        	quotes = !quotes;
        }
        else
        {
            // not whitespace
            tlen++;             // count the length of the token

            if (whitespace)
            {
                _PUSHARG(c);
            }
            // ALWAYS set these (saves a state/event check) 
            whitespace = FALSE;
        }
        c++;
    }
    // did we hit the end of the input when we were in the middle of a token?
    if (!whitespace)
        _PUSHLEN(tlen);

    // make all the strings zero terminated
    for (i = 0; i < _STACKSIZE; i++)
    {
        char *c = ArgStack[i];
        c[ArgLenStack[i]] = '\0';
        //uprintf("[%d]%s\n",i,c);
    }
}

static int _compare(char *s1, int l1, char *s2, int l2)
{
    // case insensitive matching algorithm
    if (l1 != l2)
        return -1;
    else
    {
        while (l1--)
        {
            if (_toupper(*s1++) != _toupper(*s2++))
                return -1;
        }
        return 0;
    }
}

static int _tryTable(char *c, int l, NU_CommandTable_t * table)
{
    INT i;
    INT create_task=0;

	/* create new task ? */
	if( c[l-1] == '&' )
	{
		l--;
		create_task=1;
		//uprintf("_tryTable: c:%s  l:%d\n",c,l);
	}
		
    for (i = 0; i < table->size; i++)
    {
        if (_compare(c,l,table->commands[i].cmdName,strlen(table->commands[i].cmdName)) == 0)
        {
            // We have found the command, now check that the correct 
            // number of arguments have been given.
            (table->commands[i].action) (_STACKSIZE, ArgStack);
            return TRUE;
        }
    }
    return FALSE;
}

static int _parseCommand(char *s, int l)
{
#ifdef DEBUG
    int i;
#endif    

    // table
    NU_CommandTable_t *table;

    // break the command into tokens
    _parser(s, l);
	
    // Now scan through the tables of commands seeing what ones match
    table = &NU_CommandTable;

    // Use the table to find the command and execute it
    if (!_tryTable(ArgStack[0], ArgLenStack[0], table))
    {
        // Didn't find the command, call the table specific error handling 
        // routine
        (table->error) ();
    }
    uprintf("\n");
    return (0);
}




// ***************************************************************************
// global (handy) routines
//

static void Hex2String(unsigned char *str, unsigned char *mac, int bcnt)
{
    int  idx;   
    char  *cptr;        
    
    cptr = (char *)str;
    for (idx=0; idx<bcnt; idx++)
    {
       if (mac[idx] >= 0xA0)
           *cptr++ = (mac[idx] >> 4) - 10 + 'A';
       else
           *cptr++ = (mac[idx] >> 4) + '0';
           
       if ((mac[idx] & 0xF) >= 0xA)
           *cptr++ = (mac[idx] & 0xF) - 10 + 'A';
       else
           *cptr++ = (mac[idx] & 0xF) + '0';
    }
    *cptr = 0;
}

static int  ScanUserInput(char *Str, int Length)
{
    char  chr, count;

    count=Length;
    while (1)
    {
       chr = ugetchar();
       if (chr == 0)                   /* control character pressed */
       {
          ugetchar();
          continue;
       }
       
       if (chr == 0x1B)                /* ESC */
           return -1;
           
       if (chr == 0x08)
       {                               /* backspace */
          if (count > 0)               /* have characters in Str */
          {
             count--;                  /* delete a character */
             uprintf("%c%c%c", 0x08, 0x20, 0x08);
          }
       }
       else
       {
          if (chr == 0x0d)
          {
             if (count == 0)
                 uprintf("\n");
             Str[count] = '\0';
             return 0;                 /* finish */
          }
             
          if (! (((chr >= '0') && (chr <= '9')) ||
                ((chr >= 'a') && (chr <= 'f')) ||
                ((chr >= 'A') && (chr <= 'F')) ||
                (chr == '.'))   )
              continue;                /* not leagle character */
              
          if (count >= Length)
              continue;
              
          uprintf("%c", chr);
          Str[count] = chr;            /* read in a character */
          count++;
       }
    } /* end of while */
}



static int  EditMemory(unsigned int taddr, int align, int swap)
{
	unsigned char  pstr[18];
    unsigned char  *str;
    unsigned char *cptr;
    unsigned int   word;
    unsigned short hword;
    unsigned char  byte;

	// check alignment
	if( taddr & (align-1) )
	{
		uprintf("ERROR: Alignment error!\n");
		return -1;
	}
    
	str=(unsigned char *)(pstr+2);
    while (1)
    {
       uprintf("[%X]  ", taddr);
       switch (align)
       {
          case 4:
          	if( swap )
          	{
               cptr = (unsigned char *)taddr;
               word = cptr[0] | cptr[1]<<8 | cptr[2]<<16 | cptr[3]<<24;
               cptr = (unsigned char *)&word;
               Hex2String(str, cptr, 4);
               uprintf("%s", str);
               if (ScanUserInput((char *)str, 8) < 0)
               {
                  uprintf("\n\n");
                  return 0;
               }
               word = *(unsigned int *)taddr;
               word = (word<<24) + ((word&0xFF00)<<8) + ((word&0x00FF0000)>>8) + (word>>24);
               pstr[0]=str[0];
               pstr[1]=str[1];
               str[0]=str[6];
               str[1]=str[7];
               str[6]=pstr[0];
               str[7]=pstr[1];
               pstr[0]=str[2];
               pstr[1]=str[3];
               str[2]=str[4];
               str[3]=str[5];
               str[4]=pstr[0];
               str[5]=pstr[1];
               *(unsigned int *)taddr = s2hex((char *)pstr);
               uprintf("\r[%X]  %08X --> %08X\n", taddr, word, *(unsigned int *)taddr);
               taddr += 4;
          	}
          	else
          	{
               cptr = (unsigned char *)taddr;
               word = cptr[0]<<24 | cptr[1]<<16 | cptr[2]<<8 | cptr[3];
               cptr = (unsigned char *)&word;
               Hex2String(str, cptr, 4);
               uprintf("%s", str);
               if (ScanUserInput((char *)str, 8) < 0)
               {
                  uprintf("\n\n");
                  return 0;
               }
               word = *(unsigned int *)taddr;
               *(unsigned int *)taddr = s2hex((char *)pstr);
               uprintf("\r[%X]  %08X --> %08X\n", taddr, word, *(unsigned int *)taddr);
               taddr += 4;
            } 
               break;
               
          case 2:
          	if( swap )
          	{
               cptr = (unsigned char *)taddr;
               hword = cptr[0] | cptr[1]<<8;
               cptr = (unsigned char *)&hword;
               Hex2String(str, cptr, 2);
               uprintf("%s", str);
               if (ScanUserInput((char *)str, 4) < 0)
               {
                  uprintf("\n\n");
                  return 0;
               }
               hword = *(unsigned short *)taddr;
               hword = ((hword&0x00FF)<<8)+((hword&0xFF00)>>8);
               pstr[0]=str[0];
               pstr[1]=str[1];
               str[0]=str[2];
               str[1]=str[3];
               str[2]=pstr[0];
               str[3]=pstr[1];
               *(unsigned short *)taddr = s2hex((char *)pstr);
               uprintf("\r[%X]  %04X -> %04X\n", taddr, hword, *(unsigned short *)taddr);
               taddr += 2;
          	}
          	else
          	{
               cptr = (unsigned char *)taddr;
               hword = cptr[0]<<8 | cptr[1];
               cptr = (unsigned char *)&hword;
               Hex2String(str, cptr, 2);
               uprintf("%s", str);
               if (ScanUserInput((char *)str, 4) < 0)
               {
                  uprintf("\n\n");
                  return 0;
               }
               hword = *(unsigned short *)taddr;
               *(unsigned short *)taddr = s2hex((char *)pstr);
               uprintf("\r[%X]  %04X -> %04X\n", taddr, hword, *(unsigned short *)taddr);
               taddr += 2;
            }  
               break;

          case 1:
               cptr = (unsigned char *)taddr;
               Hex2String(str, cptr, 1);
               uprintf("%s", str);
               if (ScanUserInput((char *)str, 2) < 0)
               {
                  uprintf("\n\n");
                  return 0;
               }
               byte = *(unsigned char *)taddr;
               *(unsigned char *)taddr = s2hex((char *)pstr);
               uprintf("\r[%X]  %02X -> %02X\n", taddr, byte, *(unsigned char *)taddr);
               taddr ++;
               break;
               
       }  /* end of switch */
    }  /* end of while */
}


static unsigned int  HYC_DumpAddress(unsigned int addr, int width, int swap)
{
    unsigned int  paddr, start, end, idx;
    int size;
    int i;
	 size=16;
    start = addr & (~3);
    end = start + 256;
    paddr = start;
    while (paddr < end)
    {
       uprintf("[%08X]", paddr);
       for (idx=0; idx<size; idx+=width)
       {
          if (idx == size/2)
              uprintf(" -");              
          if (paddr+idx < addr) 
          {
          	for(i=0;i<2*width+1;i++)
              uprintf(" ");
          }    
          else
          {
          		switch(width)
          		{
          			case 1:
		              uprintf(" %02X", *(unsigned char *)(paddr+idx));
		              break;
		            case 2:
		            	if( swap )
			            	uprintf(" %04X", ((*(unsigned short *)(paddr+idx))>>8)+
			            	(((*(unsigned short *)(paddr+idx))&0xFF)<<8) );
		            	else
		              		uprintf(" %04X", *(unsigned short *)(paddr+idx));
		              break;
		            case 4:
		            	if( swap )
		                	uprintf(" %08X", ((*(unsigned int *)(paddr+idx))>>24)+
		                	(((*(unsigned int *)(paddr+idx))&0x00FF0000)>>8)+
		                	(((*(unsigned int *)(paddr+idx))&0x0000FF00)<<8)+
		                	(((*(unsigned int *)(paddr+idx))&0x000000FF)<<24));
		            	else
		              		uprintf(" %08X", *(unsigned int *)(paddr+idx));
		              break;
          		}
          }    
       }  
       
       uprintf("  ");
       for (idx=0; idx<16; idx++)
       {
          if (paddr+idx <addr)
              uprintf(" ");
          else if ( (*(unsigned char *)(paddr+idx) <= 0x20) || 
                  (*(unsigned char *)(paddr+idx) > 0x7F))
              uprintf(".");
          else
              uprintf("%c", *(unsigned char *)(paddr+idx));           
       }  
       paddr += 16;
       uprintf("\r\n");
    }
    return end;
}







// ask a yes/no question and return the result
int bootPROM_yesno(char *question, int prefered)
{
    int c;

    // ask the question
    uprintf(question);
    if (prefered)
        uprintf("[Yn]? ");
    else
        uprintf("[Ny]? ");

    // get the answer and intepret it
    c = ugetchar();

    uputchar(c);
    if (c == '\n')
        return prefered;
    c = _toupper(c);
    uputchar('\n');
    return (c == 'Y');
}

// take input and parse it until an exit command is received
//#define MAX_DOSKEY_NUM	20
#define MAX_DOSKEY_NUM	2
VOID NU_parse (CHAR *prompt)
{
	void ExceptionCheck(void);
    char buffer[80];
    int offset, c;
    int done = 0;
    char doskey[MAX_DOSKEY_NUM][80];
    int doskey_len[MAX_DOSKEY_NUM]={0};
    int doskey_index, doskey_head;
    int use_doskey=0;
//    int i;

    //CHAR *path="NU";
    
	doskey_head = -1;
	doskey_index = 0;
    while (0 == done)
    {
		ExceptionCheck();

        offset = 0;
        uprintf (prompt);


        while (offset < 80)
        {
            c = ugetchar ();

			if (c == '\b')
            {
   	            if (offset > 0)
       	        {
           	        // Rub out the old character & update the console output
                   offset--;
                   buffer[offset] = 0;
     	           uprintf ("\r%s%s \b", prompt, buffer);
	               //uprintf(prompt);
	               //uprintf("%s \b", buffer);
               	}
           	}
#if 1           	
           	else if ( c == 27 )
           	{
           		char key1, key2;
           		key1 = ugetchar();
           		if( (key1 == 91) && (doskey_head >= 0) ) /* Arrow key && command exists in doskey buffer */
           		{
	           		key2 = ugetchar();
	           		
           			if( !use_doskey )
           			{
	           			doskey_index = doskey_head;
	           			use_doskey = 1;
	           		}
           			if( (key2 == 65) ) /* up */
           			{
           				if( doskey_len[doskey_index] )
           				{
	        				memcpy(buffer, doskey[doskey_index], doskey_len[doskey_index]);
    	    				offset = doskey_len[doskey_index];
							offset--;
							buffer[offset] = 0;
							uprintf ("\r                                                                               ");
							uprintf ("\r%s%s \b", prompt, buffer);
						}
						
						doskey_index --;
						if( doskey_index < 0 ) doskey_index = MAX_DOSKEY_NUM-1;
           			}
           			else if( (key2 == 66) ) /* down */
           			{
           				if( doskey_len[doskey_index] )
           				{
	        				memcpy(buffer, doskey[doskey_index], doskey_len[doskey_index]);
    	    				offset = doskey_len[doskey_index];
							offset--;
							buffer[offset] = 0;
							uprintf ("\r                                                                               ");
							uprintf ("\r%s%s \b", prompt, buffer);
						}
						doskey_index ++;
						if( doskey_index >= MAX_DOSKEY_NUM ) doskey_index = 0;
           			}
           		}
           	}
#endif           	
	        else
    	    {
                if (c == '\r')
           	        c = '\n';       // treat \r as \n

	            buffer[offset++] = c;
   		        uprintf("%c",c);
				//uprintf("(%d)",c);

                if (c == '\n')
          	        break;
	        }
        }
        if (buffer[0] != '\n')
        {
        	/* for SWI */
			memcpy(command_buffer, buffer, offset);           	
			command_buffer[offset]='\0';
			if( !use_doskey )
			{
				/* for doskey, record the command */
				doskey_head++;
				memcpy(doskey[doskey_head], buffer, offset);
				doskey_len[doskey_head] = offset;
				if( doskey_head >= MAX_DOSKEY_NUM ) doskey_head = 0;
			}
			
			/* flush doskey status */
			use_doskey = 0;
			
            done = _parseCommand (buffer, offset);
        }
    }
}

// End of File - script.c

void ExceptionCheck(void)
{
	extern unsigned int ExceptionStatus;
	extern unsigned int ExceptionLinkReg;
	
	if( ExceptionStatus==0x10 )
		uprintf("ERROR: Data Abort @ pc=0x%08x\n", ExceptionLinkReg);
	if( ExceptionStatus==0x08 )
		uprintf("ERROR: Prefetch Abort @ pc=0x%08x\n", ExceptionLinkReg);
	if( ExceptionStatus==0x04 )
		uprintf("ERROR: Undefine Instruction @ pc=0x%08x\n", ExceptionLinkReg);

    ExceptionStatus=0;
    ExceptionLinkReg=0;
}





// support
int _toupper(int c)
{
    if ((c >= 'a') && (c <= 'z'))
        c += 'A' - 'a';
    return c;
}

int chartohex(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;
    return -1;
}

unsigned int s2hex(char *s)
{
    char nStack[MAX_NUM];
    int n;
    unsigned int result;
    int shift;

    // hop over the 0x start
    s += 2;

    // stack the number in reverse order
    n = 0;
    while ((*s != '\0') && (n < MAX_NUM))
    {
        nStack[n++] = *s++;
    }

    // Pull characters off the stack, converting to hex on the way
    result = 0;
    shift = 0;
    while (n > 0)
    {
        result += (chartohex(nStack[--n]) << shift);
        shift += 4;
    }

    return result;
}

int validhex(char *s)
{
    // Is this string a valid hex number?
    // For example, of the form 0xNNNNNNNN

    // valid length?
    if (strlen(s) > 10)
        return 0;
    if (strlen(s) <= 2)
        return 0;

    // valid start? (0x or 0X)
    if ((s[0] != '0') || ((s[1] != 'X') && (s[1] != 'x')))
        return 0;

    s += 2;

    // valid characters?
    while (*s != '\0')
    {
        if (chartohex(*s) < 0)
            // this character not valid
            return 0;

        // go onto the next character
        s++;
    }

    // success
    return 1;
}

// Check string is a valid positive decimal number.
int validdec(char *string)
{
    char digit;

    while ((digit = *string++) != '\0')
        if ((digit < '0') || (digit > '9'))
            return 0;

    return 1;
}


INT s2ip(CHAR *s, CHAR *ip)
{
	INT len,i;
	INT offset;
	CHAR *p;
	p=s;
	while( *p!='\0' )
	{
		len=0;
		while( (*p!='.') && (*p!='\0'))
		{
			len++;
			p++;	
		}
		*ip=0;
		offset=1;
		for(i=len-1;i>=0;i--)
		{
			*ip+=(s[i]-'0')*offset;
			offset*=10;
		}
		if( *p )p++;
		s=p;
		ip++;
	}
	
	return 0;
}


// Convert string to a positive decimal number.
int s2dec(char *string)
{
    char digit;
    int number = 0;

    while ((digit = *string++) != '\0')
        if ((digit < '0') || (digit > '9'))
            return -1;
        else
            number = (number * 10) + (digit - '0');

    return number;
}

UINT32 s2u(char *string)
{
    char digit;
    UINT32 number = 0;

    while ((digit = *string++) != '\0')
        if ((digit < '0') || (digit > '9'))
            return -1;
        else
            number = (number * 10) + (digit - '0');

    return number;
}

char * GetAddress(void)
{
	static char str[11];
	int i;
	char ch;
	
	for(i=0;i<10;i++)str[i]=0;
	i=0;
	while(1)
	{
		ch=ugetchar();
		if( ch == '\n' || ch == '\r' )
		{
			uputchar('\n');
			return str;
		}
		else
		{
			if( ch == '\b' )
			{
			  if(  i >= 1 )
			  {
				str[--i]='\0';
				uprintf("\b \b");
			  }	
			}
			else
			{
			
				if( i<10 )
				{
					str[i++]=ch;
					uputchar(ch);
				}
			}		
		}
	}
}

INT wmemcpy(VOID * dest, VOID * src, UINT32 size)
{
	INT i;
	size=size+3;// for word boundary
	
	for(i=0;i<size/4;i++)
		*((UINT32 *)dest+i)=*((UINT32 *)src+i);
	
	return (i*4);
}


// ********************** Standard action routines *****************************


int INTF_action(int argc, char *argv[])
{

#ifdef __USB__
	uprintf("USB\n");
#else	
	uprintf("MAC\n");
#endif

}

void ERROR_action(void)
{
    uprintf("ERROR: unrecognised command\n");
}


 int H_action(int argc, char *argv[])
{
    int i;

    // Who/what are we?
    uprintf("%s",SHELL_ANNOUNCE);
    uprintf("\n");

    // print all of the standard commands and their help text
    for (i = 0; i < NU_CommandTable.size; i++)
    {
        uprintf("%-8s %s", NU_CommandTable.commands[i].cmdName,
                     NU_CommandTable.commands[i].helpText);
    }

    // NOTE: commands allowed in both upper and lower-case
    return 0;
}

static int  D_usage()
{
     uprintf("usage: D -[w,h,b,s] <taddr>\n");
     uprintf("       -w, -W	Word alignment\n");
     uprintf("       -h, -H	Half-word alignment\n");
     uprintf("       -b, -B	Byte alignment\n");
     uprintf("       -s, -S	Swap target\n");
     uprintf("       <taddr>	Target memory address.\n");
     return -1;
}
 int D_action(int argc, char *argv[])
{
    static unsigned int hexAddress = 0;
    int i,cmd=4,swap=0;

    // did the user supply an address?
    for(i=0;i<argc;i++)
    {
        if (validhex(argv[i]))
        {
	        hexAddress = s2hex(argv[i]);
	    }
	    else
	    {
			if( argv[i][0]=='-' )
			{
				switch(argv[i][1])
				{
					case 'W':
					case 'w':
						cmd=4;
						break;
					case 'H':
					case 'h':
						cmd=2;
						break;
					case 'B':
					case 'b':
						cmd=1;
						break;
					case 's':
					case 'S':
						swap=1;
						break;
					case '?':
						return D_usage();
					default:
						cmd=4;
						break;
				}
			}
	    }
    }
	
    // We've got an address, now display its contents
    uprintf("Displaying memory at 0x%X\n", hexAddress);
    hexAddress = HYC_DumpAddress(hexAddress,cmd,swap);
    return 0;
}

static int  E_usage()
{
     uprintf("usage: E -[w,h,b,s] <taddr>\n");
     uprintf("       -w, -W	Word alignment\n");
     uprintf("       -h, -H	Half-word alignment\n");
     uprintf("       -b, -B	Byte alignment\n");
     uprintf("       -s, -S	Swap target\n");
     uprintf("       <taddr>	Target memory address.\n");
     return -1;
}
 int  E_action(int argc, char *argv[])
{
    unsigned int  taddr;	
    int i,cmd=4,swap=0;
	
    if ( argc < 2 )
        return E_usage();

	for(i=0;i<argc;i++)
	{
		if( validhex(argv[i]) )
		{
		    taddr = s2hex(argv[i]);
		}
		else
		{
			if( argv[i][0]=='-' )
			{
				switch(argv[i][1])
				{
					case 'W':
					case 'w':
						cmd=4;
						break;
					case 'H':
					case 'h':
						cmd=2;
						break;
					case 'B':
					case 'b':
						cmd=1;
						break;
					case 's':
					case 'S':
						swap=1;
						break;
					case '?':
						return E_usage();
					default:
						cmd=4;
						break;
				}
			}
		}
	}
        
    return EditMemory(taddr, cmd, swap);
}



 int  MX_action(int argc, char *argv[])
{
	char *p;

	unsigned int destAddress=0x8000;
	unsigned int fileSize;

	if( argc < 2 )
	{
	
		uprintf("Please enter destination address (0x8000):");
		p=GetAddress();
		if( validhex( p ) )
			destAddress = s2hex(p);
		else
			uprintf("Image load address default to 0x%08x\n",destAddress);
	}
	else
	{
		if( !validhex(argv[1]) )
		{
            uprintf("Invalid hex address (%s)\n", argv[1]);
            return 0;
		}
	
		destAddress=s2hex(argv[1]);
	}

	uprintf("Waiting for download\n");
	uprintf("Press Ctrl-x to cancel ... \n");
	fileSize=0;
	if( xmodem(destAddress,&fileSize)==X_SSUCCESS )
	{
		uprintf("\nDownload successed!\n");
	}
	else
	{
		uprintf("\nDownload error!\n");
	}

	return 0;	
}


 int  MT_action(int argc, char *argv[])
{
	char *p;

	unsigned int destAddress=0x8000;
	unsigned int fileSize;

	if( argc < 2 )
	{
	
		uprintf("Please enter destination address (0x8000):");
		p=GetAddress();
		if( validhex( p ) )
			destAddress = s2hex(p);
		else
			uprintf("Image load address default to 0x%08x\n",destAddress);
	}
	else
	{
		if( !validhex(argv[1]) )
		{
            uprintf("Invalid hex address (%s)\n", argv[1]);
            return 0;
		}
	
		destAddress=s2hex(argv[1]);
	}
#ifdef __USB__
	uprintf("usb...\n");
	if(enableUSB == 0)
		Enable_USB();
	usb_down(destAddress, &fileSize);
	if(enableUSB == 0)
		Disable_USB();	
	uprintf("\nUSB Download Successed\n");

#else	
	//*(unsigned int volatile *)(0xfff83020) = 0x50000;
	if( !_net_init )
	{
		*(unsigned int volatile *)(0xfff83020) = 0x50000;
		if( Net_Init(_dhcp) < 0 )  //==========>
		{
			uprintf("ERROR: Network initialization failed!\n");
			return -1;
		}
		_net_init=1;
	}
	fileSize=0;
	uprintf("Waiting for download ...\n");  //============>
	if( TFTP_Download((unsigned char *)destAddress,(unsigned long *)&fileSize,_dhcp)==0 )
	{
		uprintf("\nDownload successed!\n");
	}
	else
	{
		uprintf("\nDownload error!\n");
	}
#endif	

	return 0;	
}


 int G_action(int argc, char *argv[])
{
    static unsigned int address = 0x8000;
//	unsigned int j;

    if (argc > 1)
    {
		if (!validhex(argv[1]))
		{
		    uprintf("Invalid hex address (%s)\n", argv[1]);
		    return -1;
		}

		address = s2hex(argv[1]);
    }
#ifdef __USB__
	if(enableUSB) Disable_USB();
#endif		

    // Pass control to the code we have just loaded
    // with argv and argc set as zero
	swi_run(0x200,address);


    /*
     * ** If we return we have no idea what the state of the system
     * ** is, it is very possible (even likely) that some of our global
     * ** data has been trashed, therefore it is not safe to continue
     * ** executing from this point.
     */
#ifdef __USB__
		if(enableUSB) Enable_USB();
#endif     
    return 0;
}


 int B_action(int argc, char *argv[])
{
    int speed;
    int status, baudRate;

    if (argc != 2)
    {
        uprintf("Invalid number of arguments\n");
        uprintf("[1200,2400,4800,9600,14400,19200,28800,38400,57600,115200,230400,460800]\n");
        return -1;
    }

    if (validdec(argv[1]))
    {
        speed = s2dec(argv[1]);
    }
    else
    {
        uprintf("Invalid number - %s\n", argv[1]);
        return -1;
    }

    status = UART_Speed(speed, &baudRate) ;
    if (0 == status)
    {
        uprintf("Unsupported speed.\n");
        return -1;
    }

#ifdef SEMIHOSTED

    uprintf("Cannot change Baud Rate in Semihosted images.\n");

#else

    init_serial(0, baudRate) ;

#endif

    return 0;
}

static int MSET_usage()
{
		uprintf("\nUsage: MSET -[w,W,H,h,B,b] [saddr] [size] [value].\n");
	    uprintf("       -w, -W	Word alignment\n");
    	uprintf("       -h, -H	Half-word alignment\n");
	    uprintf("       -b, -B	Byte alignment\n");
    	uprintf("       [saddr]  Start address to be filled from.\n");
    	uprintf("       [size]   Size of the memory to be filled.\n");
    	uprintf("       [value]  The vaule to be filled into memory.\n"); 
    	return -1;
}
 int MSET_action(int argc, char *argv[])
{
	int setmem(unsigned int saddr, unsigned int size, unsigned int value, int align, int swap);
	unsigned int arg[3];
	int i,j,align=4,swap=0;

	if( argc < 4 )
	{
		return MSET_usage();
	}

	for(i=0,j=0;i<argc;i++)
	{
		if( validhex(argv[i]) )
		{
			arg[j++]=s2hex(argv[i]);
		}
		else
		{
			if( argv[i][0]=='-' )
			{
				switch(argv[i][1])
				{
					case 'W':
					case 'w':
						align=4;
						break;
					case 'H':
					case 'h':
						align=2;
						break;
					case 'B':
					case 'b':
						align=1;
						break;
					case '?':
						return MSET_usage();
					default:
						align=4;
						break;
				}
			}
		}
	}
	if( j < 3 )return MSET_usage();

	setmem(arg[0],arg[1],arg[2],align,swap);

    return 0;


}

int setmem(unsigned int saddr, unsigned int size, unsigned int value, int align, int swap)
{
	unsigned int i;
	
	if( (saddr & (align-1)) || (size & (align-1)) )
	{
		uprintf("ERROR: Alignment error!\n");
		return -1;
	}
	
	for(i=0;i<size;i+=align)
	{
		switch(align)
		{
			case 4:
				*((volatile unsigned int *)(saddr+i))=(unsigned int)value;
				break;
			case 2:
				*((volatile unsigned short *)(saddr+i))=(unsigned short)value;
				break;
			case 1:
				*((volatile unsigned char *)(saddr+i))=(unsigned char)value;
				break;
			default:
				break;
		}
	}
	return size;
}

int  CP_usage()
{
     uprintf("usage: CP [saddr] [taddr] [length]\n");
     uprintf("       [saddr]   source address to be copied from\n");
     uprintf("       [taddr]   target address to be copied to\n");
     uprintf("       [length]  The length of memory block to be moved\n"); 
     return -1;
}
 int CP_action(int argc, char *argv[])
{
    static unsigned int   saddr, taddr, length, idx;

    if (argc < 4)
        return CP_usage();
        
    if ((!validhex(argv[1])) || (!validhex(argv[2])) || (!validhex(argv[3])))
    {
       uprintf("Invalid hex value\n");
       return -1;
    }
    saddr =  s2hex(argv[1]);
    taddr =  s2hex(argv[2]);
    length = s2hex(argv[3]);
    
    for (idx=0; idx<length; idx++)
         *(unsigned char *)taddr++ = *(unsigned char *)saddr++;

    return 0;
}


static int USB_usage(void)
{
#ifdef __USB__
     uprintf("usage: USB -[on,off]\n");
     uprintf("       -on, off	Enable/Disable USB interface \n");
     uprintf("       -?		Usage help\n");
#else
	uprintf("This bootloader doesn't support USB, please use TCP/IP instead\n");

#endif
	return(-1);
}

 int USB_action(int argc, char *argv[])
{

#ifdef __USB__

	int i;
	

	if( argc < 2 )
	{
		if( enableUSB ) uprintf("USB is enabled\n");
		else	uprintf("USB is disabled\n");
	}


	for(i=0;i<argc;i++)
	{
		if( !strcmp(argv[i],"-?") )
			return USB_usage();
		
		if( !strcmp(argv[i],"-on") )
		{
			if(enableUSB)
				Disable_USB();
			Enable_USB();
			enableUSB = 1;	
			uprintf("USB is enabled!\n");
		}
		if( !strcmp(argv[i],"-off") )
		{
			Disable_USB();
			enableUSB = 0;
			uprintf("USB is disabled!\n");
		}

	}
	
	return 0;
#else
	return USB_usage();
#endif	
}



static int CACHE_usage(void)
{
     uprintf("usage: CACHE -[on,off,f]\n");
     uprintf("       -on, off	Enable/Disable cache \n");
     uprintf("       -f		Flush cache\n");
     uprintf("       -?		Usage help\n");
     return -1;
}
 int CACHE_action(int argc, char *argv[])
{
	int config;
	int status;
	int i;
	
	status=CAHCNF;
	if( argc < 2 )
	{
		if( status )uprintf("Cache is enabled\n");
		else	uprintf("Cache is disabled\n");
	}

	config=0;
	for(i=0;i<argc;i++)
	{
		if( !strcmp(argv[i],"-?") )
			return CACHE_usage();
		

		if( !strcmp(argv[i],"-f") )
		{
			CAHCON=0x87;
			while(CAHCON);
			uprintf("Cache flush ok!\n");
		}
		if( !strcmp(argv[i],"-on") )
		{
			CAHCON=0x87;
			while(CAHCON);
			CAHCNF=0x7;
			uprintf("Cache is enabled!\n");
		}
		if( !strcmp(argv[i],"-off") )
		{
			CAHCNF=0x0;
			uprintf("Cache is disabled!\n");
		}

	}
	
	return 0;
}

INT FX_usage()
{
	uprintf("Usage: FX [ImageNo.] [ImageName] [base address] [exec address] -[a,c,x,f,z] -nofooter\n");
	uprintf("		-a 		Active image\n");
	uprintf("		-c 		Image needs to be copy to RAM\n");
	uprintf("		-x 		Executable image\n");
	uprintf("		-f 		File system image\n");
	uprintf("		-z 		Compressed image\n");
	uprintf("		-nofooter	No footer be written\n");
	return -1;
}
 int FX_action(int argc, char *argv[])
{
	UINT32 image_num;
	tfooter * pfooter;
	tfooter footer;
	UINT i,j;
	UINT32 image_type=0;
	CHAR ch;
	INT	 nofooter = 0;
	
	if( argc < 6 )
		return FX_usage();
	
	for(i=1;i<argc;i++)
	{
		j=0;
		if( !strcmp(argv[i],"-nofooter") )
		{
			nofooter = 1;	
		}
		else if( argv[i][j++]=='-' )
		{
			while(argv[i][j]!='\0')
			{
				switch(argv[i][j++])
				{
					case 'a':
						image_type|=IMAGE_ACTIVE;
						break;
					case 'c':
						image_type|=IMAGE_COPY2RAM;
						break;
					case 'x':
						image_type|=IMAGE_EXEC;
						break;
					case 'f':
						image_type|=IMAGE_FILE;
						break;
					case 'z':
						image_type|=IMAGE_COMPRESSED;
						break;
					default:
						return FX_usage();
				}
			}
		}
	}

	
	image_num=s2dec(argv[1]);
	if( FindImage(image_num, &pfooter) )
	{
		uprintf("Find image %d existed!\n",image_num);
		uprintf("Do you want to delete it?[y/N]\n");
		ch=ugetchar();
		
		if( (ch=='y') || (ch=='Y') )
		{
		
#ifdef __USB__
			if(enableUSB)   Disable_USB();
#else
				if( _net_init )DisableIRQ();							
#endif			
			uprintf("Deleting image %d ...\n",image_num);
			DelImage(image_num);
			
#ifdef __USB__
				if(enableUSB) Enable_USB();
#else
				if( _net_init )EnableIRQ();							
#endif		
		}
		else
		{
			return -1;	
		}
	}
//	else
	{
		if( !validhex(argv[3]) || !validhex(argv[4]) || !validdec(argv[1]) )
		{
			uprintf("ERROR: Invalid parameters!\n");
			return -1;
		}
		footer.num=image_num;
		strcpy(footer.name, argv[2]);
		footer.base=s2hex(argv[3]);
		footer.length=FLASH_BLOCK_SIZE;
		footer.load_address=footer.exec_address=s2hex(argv[4]);
		footer.signature=SIGNATURE_WORD;
		footer.type=image_type;
	
		if( CorruptCheck(&footer) )
		{
			uprintf("ERROR: Data corrupted, please check the free space of flash.\n");
		}
		else
		{
			UINT32 src=footer.load_address;
			UINT32 fileSize=0;
			uprintf("Waiting for download\n");
			uprintf("Press Ctrl-x to cancel ... \n");
			if( src > FLASH_BASE )src=STATIC_DOWNLOAD_BUFFER;
			if( xmodem(src,&fileSize)==X_SSUCCESS )
			{
				uprintf("\nFlash programming ...  \n");
				footer.length=fileSize;
			
				//if( _net_init )DisableIRQ(); // avoid flash R/W confliction
#ifdef __USB__
				 	if(enableUSB) Disable_USB();
#else
				if( _net_init )DisableIRQ();							
#endif			
				if( nofooter )
				{
					if( WriteRawImage(&footer, src)!=0 )
					{
				
						//if( _net_init )EnableIRQ();
#ifdef __USB__
				if(enableUSB)			Enable_USB();
#else
						if( _net_init )EnableIRQ();							
#endif								
					
						uprintf("ERROR: Write failed.\n");
						return -1;
					}
				}
				else
				{
					if( WriteImage(&footer, src)!=0 )
					{
					
#ifdef __USB__
				if(enableUSB)			Enable_USB();
#else
						if( _net_init )EnableIRQ();							
#endif						
						uprintf("ERROR: Write failed.\n");
						return -1;
					}
				}
			
#ifdef __USB__
				if(enableUSB)			Enable_USB();
#else
						if( _net_init )EnableIRQ();							
#endif			
			}
			else
			{
				uprintf("\nDownload error!\n");
				return -1;
			}
		}
	}
	
	
	return 0;
}


INT FT_usage()
{
	uprintf("Usage: FT [ImageNo.] [ImageName] [base address] [exec address] -[a,c,x,f,z] -nofooter\n");
	uprintf("		-a 		Active image\n");
	uprintf("		-c 		Image needs to be copy to RAM\n");
	uprintf("		-x 		Executable image\n");
	uprintf("		-f 		File system image\n");
	uprintf("		-z 		Compressed image\n");
	uprintf("		-nofooter	No footer be written\n");
	return -1;
}
 int FT_action(int argc, char *argv[])
{
	UINT32 image_num;
	tfooter * pfooter;
	tfooter footer;
	UINT i,j;
	UINT32 image_type=0;
	CHAR ch;
	INT	 nofooter = 0;
	
	if( argc < 6 )
		return FT_usage();
	
	
	
	for(i=1;i<argc;i++)
	{
		j=0;
		
		if( !strcmp(argv[i],"-nofooter") )
		{
			nofooter = 1;	
		}
		else if( argv[i][j++]=='-' )
		{
			while(argv[i][j]!='\0')
			{
				switch(argv[i][j++])
				{
					case 'a':
						image_type|=IMAGE_ACTIVE;
						break;
					case 'c':
						image_type|=IMAGE_COPY2RAM;
						break;
					case 'x':
						image_type|=IMAGE_EXEC;
						break;
					case 'f':
						image_type|=IMAGE_FILE;
						break;
					case 'z':
						image_type|=IMAGE_COMPRESSED;
						break;
					default:
						return FT_usage();
				}
			}
		}
	}

	
	image_num=s2dec(argv[1]);
	if( FindImage(image_num, &pfooter) )
	{
		uprintf("Find image %d existed!\n",image_num);
		uprintf("Do you want to delete it?[y/N]\n");
		ch=ugetchar();
		
		if( (ch=='y') || (ch=='Y') )
		{
	
#ifdef __USB__
				if(enableUSB)	Disable_USB();
			
#else
				if( _net_init )DisableIRQ();							
#endif		
			uprintf("Deleting image %d ...\n",image_num);
			DelImage(image_num);
#ifdef __USB__
			if(enableUSB)	Enable_USB();
					
#else						
			if( _net_init )EnableIRQ();
#endif			
		
		}
		else
		{
			return -1;	
		}
	}
//	else
	{
		if( !validhex(argv[3]) || !validhex(argv[4]) || !validdec(argv[1]) )
		{
			uprintf("ERROR: Invalid parameters!\n");
			return -1;
		}
		footer.num=image_num;
		strcpy(footer.name, argv[2]);
		footer.base=s2hex(argv[3]);
		footer.length=FLASH_BLOCK_SIZE;
		footer.load_address=footer.exec_address=s2hex(argv[4]);
		footer.signature=SIGNATURE_WORD;
		footer.type=image_type;
	
		if( CorruptCheck(&footer) )
		{
			uprintf("ERROR: Data corrupted, please check the free space of flash.\n");
		}
		else
		{
			UINT32 src=footer.load_address;
			UINT32 fileSize=0;
#ifndef __USB__			
			if( !_net_init )
			{
				*(unsigned int volatile *)(0xfff83020) = 0x50000;
				if( Net_Init(_dhcp) < 0 )  //=========>
				{
					uprintf("ERROR: Network initialization failed!\n");
					return -1;
				}
				_net_init=1;
			}
#endif			
			if( src > FLASH_BASE )src=STATIC_DOWNLOAD_BUFFER;
			uprintf("Waiting for download ...\n");
#ifdef __USB__
			if(enableUSB == 0)
				Enable_USB();
			usb_down(src, &fileSize);
			if(enableUSB == 0)
				Disable_USB();			
			if(1)
#else			
			if( TFTP_Download((UCHAR *)src,(ULONG *)&fileSize,_dhcp)==0 )  //====>
#endif			
			{
				uprintf("\nFlash programming ...  \n");
				footer.length=fileSize;

#ifdef __USB__
				if(enableUSB)	Disable_USB();
		
#else
				if( _net_init )DisableIRQ();							
#endif				
				if( nofooter )
				{
					if( WriteRawImage(&footer, src)!=0 )
					{
				
#ifdef __USB__
				if(enableUSB)			Enable_USB();
			
#else						
						if( _net_init )EnableIRQ();
#endif							
						uprintf("ERROR: Write failed.\n");
						return -1;
					}
				}
				else
				{
					if( WriteImage(&footer, src)!=0 )
					{
					
#ifdef __USB__
				if(enableUSB)			Enable_USB();
				
#else						
						if( _net_init )EnableIRQ();
#endif							
						uprintf("ERROR: Write failed.\n");
						return -1;
					}
				}				
#ifdef __USB__
				if(enableUSB)	Enable_USB();
	
#else						
				if( _net_init )EnableIRQ();
#endif					
			}
			else
			{
				uprintf("\nDownload error!\n");
				return -1;
			}
		}
	}
	
	
	

	
	return 0;
}


INT DEL_usage()
{
	uprintf("Usage: DEL [ImageNo.] [b{blockNo.}] [-all]\n");
	uprintf("       [ImageNo.]	Delete the image\n");
	uprintf("       [b{blockNo.}]	Delete the block\n");
	uprintf("       -all		Delete all blocks\n");
	return -1;
}
 int DEL_action(int argc, char *argv[])
{
	tfooter * pfooter;
	UINT32 image_num;
	UINT i;
	
	if( !strcmp(argv[1],"-all") )
	{
		uprintf("Press Ctrl-c to stop\n");
#ifndef USE_NO_IMAGE0
		for(i=2;i<FlashSize()/FLASH_BLOCK_SIZE;i++)
#else		
		for(i=1;i<FlashSize()/FLASH_BLOCK_SIZE;i++)
#endif		
		{
			uprintf("Deleting ... Block %d\n",i);
#ifdef __USB__
			if(enableUSB)	Disable_USB();
#else
			if( _net_init )DisableIRQ();							
#endif				
			DelBlock(i);
#ifdef __USB__
			if(enableUSB)	Enable_USB();
#else						
			if( _net_init )EnableIRQ();
#endif					
			if( ukbhit() )
			{
				char ch;
				ch=ugetchar();
				if( ch==3 )break;
			}
		}
		return 0;
	}
	else
	{
		if( argv[1][0]=='b' || argv[1][0]=='B' )
		{
			argv[1][0]='0';
			if( validdec( argv[1] ) )
			{
#ifdef __USB__
			if(enableUSB)	Disable_USB();
#else
			if( _net_init )DisableIRQ();							
#endif				
			DelBlock(s2dec(argv[1]));
#ifdef __USB__
			if(enableUSB)	Enable_USB();
#else						
			if( _net_init )EnableIRQ();
#endif				
			}
			else
			{
				uprintf("ERROR: Invalid block number\n");
			}
		}
		else
		{
			if( validdec(argv[1]) )
			{
				image_num=s2dec(argv[1]);
				if( FindImage(image_num, &pfooter) )
				{
					uprintf("Deleting Image %d ...\n",image_num);
#ifdef __USB__
				if(enableUSB)		Disable_USB();
#else
					if( _net_init )DisableIRQ();							
#endif	
					DelImage(image_num);
#ifdef __USB__
				if(enableUSB)		Enable_USB();
#else						
					if( _net_init )EnableIRQ();
#endif					
				}
				else
				{
					uprintf("ERROR: Image %d is not existed\n",image_num);
				}
			}
			else
			{
				return DEL_usage();			
			}
		}
	}
	
	return 0;
}

 int RUN_action(int argc, char *argv[])
{
	tfooter * pfooter;
	UINT32 image_num;
	UINT32 i;

	image_num=s2dec(argv[1]);
	if( FindImage(image_num, &pfooter) )
	{
		if( (pfooter->type & IMAGE_COPY2RAM) && !(pfooter->type & IMAGE_COMPRESSED))
		{
			for(i=0;i<((pfooter->length&(~0x3))+4)/4;i++)
				*((volatile unsigned int *)pfooter->load_address+i)=*((volatile unsigned int *)pfooter->base+i);
		}
		if( pfooter->type & IMAGE_COMPRESSED )
		{
			UNZIP_action(argc, argv);
		}
		if( pfooter->type & IMAGE_EXEC )
			swi_run(EXEC_SWI_NUM, pfooter->exec_address);
	}
	else
	{
		uprintf("ERROR: Image %d is not existed\n",image_num);
	}
	
	
	return 0;
}


 int LS_action(int argc, char *argv[])
{
	tfooter ** footer;
	UINT32 footer_num;
	INT i;
	
	if( (footer_num=FindFooter(&footer))==-1 )
	{
		uprintf("No image found!\n");
		return 0;
	}
	
	//uprintf("footer num %\n", footer_num);
	
	for(i=0;i<footer_num;i++)
	{
		uprintf("Image:%2d name:%-s base:0x%08x size:0x%08x exec:0x%08x -",
		footer[i]->num,
		footer[i]->name,
		footer[i]->base,
		footer[i]->length,
		footer[i]->exec_address);
		
		if( footer[i]->type & IMAGE_ACTIVE )uputchar('a');
		if( footer[i]->type & IMAGE_COPY2RAM )uputchar('c');
		if( footer[i]->type & IMAGE_EXEC )uputchar('x');
		if( footer[i]->type & IMAGE_FILE )uputchar('f');
		if( footer[i]->type & IMAGE_COMPRESSED )uputchar('z');
		uputchar('\n');		
	}
	
	if( i == 0 )
	{
		uprintf("No image found!\n");
	}


	return 0;
}

 int UNZIP_action(int argc, char *argv[])
{
	Bytef *dest;
	Bytef *src;
	uLongf len,slen;
	unsigned int i;
	unsigned int image_num=0;
	tfooter * pfooter;
	
	if(argc != 2 )
	{
		uprintf("Usage: unzip [ImageNo.]\n");
		return -1;
	}
	
	if( validdec(argv[1]) )
	{
		image_num=s2dec(argv[1]);
	}
	else
	{
		uprintf("Usage: unzip [ImageNo.]\n");
		return -1;
	}	

	if( FindImage(image_num, &pfooter)!=1)
	{
		uprintf("ERROR: Image %d couldn't be found\n", image_num);
		return -1;
	}
	
	dest = (Bytef *)(pfooter->load_address);
	src = (Bytef *)(pfooter->base);
	slen = (uLongf)(pfooter->length);
	len=-1;
	//dest = (Bytef *)0x400000;
	//src  = (Bytef *)0x100000;
	i=0;

//	slen=560337; // size of linux.zip, you should load it to memory by AxD first
	//========>
	if( validZipID(src) == Z_OK )
	{
		int status;
		if( (status=unzip(dest,&len,src,slen)) != Z_OK )
		{
			uprintf("\nERROR: unzip failed! %d\n",status);	
			return 0;
		}
	}
	else
	{
		uprintf("ERROR: Invalid zip file\n");
	}
	
	return 0;


}

INT CHK_usage()
{
	uprintf("Usage: CHK [ImageNo.]");

	return -1;
}
/* The CHK_action is used to check the image checksum */
 int CHK_action(int argc, char *argv[])
{
	INT i;
	INT status=0;
	
	if( argc == 1 )
	{
		for(i=1;i<MAX_FOOTER_NUM;i++)
		{
			if( ImageCheck(i) )
			{
				uprintf("ERROR: Image %d checksum error found !\n",i);
				status++;
			}
		}
	}
	else
	{
		if( validdec(argv[1]) )
		{
			if( ImageCheck(s2dec(argv[1])) )
			{
				uprintf("ERROR: Image %d checksum error found !\n",i);
				status++;
			}
		}
		else
			return CHK_usage();
	}

	uprintf("Image check finished. %d checksum error found.\n",status);
	return 0;
}

static INT SET_usage(void)
{
#ifdef __USB__
	uprintf("Usage: SET	-[mac0 [addr], usb [0, 1],cache [on, off],buffer [base] [size],baudrate [baud rate setting],sn [serial number]]\n");
	uprintf(" -mac0	 	[addr]		Set MAC  Address\n");	
	uprintf(" -usb		[1, 0]	Enable/Disable USB interface\n");
#else
	uprintf("Usage: SET	-[mac0 [addr],ip0 [addr],dhcp [0,1],cache [on, off],buffer [base] [size],baudrate [baud rate setting],sn [serial number]]\n");
	uprintf(" -mac0	 	[addr]		Set MAC  Address\n");
	uprintf(" -ip0	 	[ip addr]	IP Address\n");
	uprintf(" -dhcp	 	[1,0] 		Enable/Disable Dhcp client\n");	
#endif	

	uprintf(" -cache	[on,off]	Enable/Disable cache when processing images\n");
	uprintf(" -buffer	[base] [size] Set the buffer used by UNZIP and TFTP server\n");
	uprintf(" -baudrate	[baud rate setting] Set the default baud rate\n");
	uprintf(" -sn	 	[serial number]	Set the serial number\n");
	return -1;
}
 INT SET_action(int argc, char *argv[])
{
	CHAR *str;
	CHAR *buffer;
	CHAR *p;
	tbl_info * info;
	INT i,j,len;
	CHAR ip0[4];
	CHAR mac0[6];
	//CHAR ip1[4];
	//CHAR mac1[6];
	CHAR stmp[5];
	UINT32 dhcp;
	UINT32 net_mac;
	//UINT32 phy;
	//UINT32 rmii;
	UINT32 serial_no;
	UINT32	usb;
	tfooter * footer;
	tfooter image_footer;
	UINT32 set_flag=0;
	UINT32 buf_base,buf_size,cache;
	INT divider,speed,baudrate;
	extern struct __Heap_Descriptor my_heap;

#ifndef USE_NO_IMAGE0	
	if( !FindImage(0, &footer) )
	{
		uprintf("ERROR: Can't find image 0.\n");
		return -1;
	}
	
	// backup image 0 before write
    buffer = (CHAR *) __Heap_Alloc( &my_heap, (size_t)FLASH_BLOCK_SIZE );
	//memcpy( buffer, footer->base, FLASH_BLOCK_SIZE);		
	wmemcpy( buffer, (VOID *)footer->base, FLASH_BLOCK_SIZE);		
	info=(tbl_info *)buffer;
	
#else
	blockSize=flash[FindFlash()].BlockSize(FLASH_BASE+BOOTER_BLOCK_LENGTH-sizeof(tbl_info));	
    buffer = (CHAR *) __Heap_Alloc( &my_heap, (size_t)blockSize );
	//memcpy( buffer+blockSize-sizeof(tbl_info), (CHAR *)(FLASH_BASE+BOOTER_BLOCK_LENGTH-sizeof(tbl_info)), sizeof(tbl_info));		
	wmemcpy( buffer+blockSize-sizeof(tbl_info), (CHAR *)(FLASH_BASE+BOOTER_BLOCK_LENGTH-sizeof(tbl_info)), sizeof(tbl_info));		
	info=(tbl_info *)(buffer+blockSize-sizeof(tbl_info));

#endif


	if( info->type != BOOTLOADER_INFO )
	{
		uprintf("ERROR: Can't fine boot loader information.\n");
		__Heap_Free( &my_heap, buffer );
		return -1;
	}

	// read all boot information
	memcpy(mac0, info->mac0, 6);
	memcpy(ip0, info->ip0, 4);
	//memcpy(mac1, info->mac1, 6);
	//memcpy(ip1, info->ip1, 4);
	cache=info->cache;
	dhcp=info->dhcp;
	net_mac=info->net_mac;
	//phy=info->phy;
	buf_base=info->buf_base;
	buf_size=info->buf_size;
	baudrate=info->baudrate;
	usb = (info->usb == 0) ? 0:1; // maybe flash is clean
	//rmii=info->rmii=((info->rmii==1)?1:0);
	if(baudrate==-1)
	{
		baudrate=115200; // for compatibility
		info->baudrate=115200;
	}
	serial_no=info->serial_no;

	for(i=1;i<argc;i++)
	{
		// Usage help
		if (!strcmp(argv[i],"-?") )
		{
			__Heap_Free( &my_heap, buffer );
			return SET_usage();
		}
	
		// Setting IP address 
		if( (!strcmp(argv[i],"-ip0")) /*|| (!strcmp(argv[i],"-ip1"))*/ )
		{
			set_flag=1;
			if( strlen(argv[i+1])<7 )
			{
				uprintf("ERROR: Invalid ip address\n");
				__Heap_Free( &my_heap, buffer );
				return -1;
			}
			p=argv[i+1];
			str=p;
			j=0;
			while(1)
			{
				if( *p=='.' || *p=='\0' )
				{
					*p='\0';
					if( !validdec(str) )
					{
						uprintf("ERROR: Invalid ip address\n");
						__Heap_Free( &my_heap, buffer );
						return -1;
					}
				//	if( argv[i][3]-'0' )
				//		ip1[j++]=s2dec(str);
				//	else
						ip0[j++]=s2dec(str);
						
					if( j==4 )break;
					p++;
					str=p;
				}
				else
				{
					p++;
				}
				
			}
		}
		if( (!strcmp(argv[i],"-mac0"))/* || (!strcmp(argv[i],"-mac1")) */)
		{
			set_flag=1;
			len=strlen(argv[i+1]);
			if( (len != 17) && (len != 12) )
			{
				uprintf("ERROR: Invalid mac address\n");
				__Heap_Free( &my_heap, buffer );
				return -1;
			}
			
			p=argv[i+1];
			stmp[0]='0'; // 0x##
			stmp[1]='x';
			str=&stmp[2];
			str[2]='\0';
			for(j=0;j<6;j++)
			{
				str[0]=*p;
				str[1]=*(p+1);
				str[2]='\0';
				if( len == 17 )p+=3;
				else p+=2;
				if( !validhex(stmp) )
				{
					uprintf("ERROR: Invalid mac address\n");
					__Heap_Free( &my_heap, buffer );
					return -1;
				}
				
			//	if( argv[i][4]-'0' )
			//		mac1[j]=s2hex(stmp);
			//	else
					mac0[j]=s2hex(stmp);
					
			}
		}
		
		if( !strcmp(argv[i],"-dhcp") )
		{
			set_flag=1;
			dhcp=(argv[i+1][0]-'0')&0x1;		
		}
		
		if( !strcmp(argv[i],"-net_mac") )
		{
			set_flag=1;
			net_mac=(argv[i+1][0]-'0')&0x1;		
		}
#if 0		
		if( !strcmp(argv[i],"-phy") )
		{
			set_flag=1;
			phy=(argv[i+1][0]-'0')&0x3;
		}
#endif
		if( (!strcmp(argv[i],"-buffer")))
		{
			set_flag=1;
			if( (!validhex(argv[i+1])) || (!validhex(argv[i+2])) )
			{
				uprintf("ERROR: Invalid hex number\n");
				__Heap_Free( &my_heap, buffer );
				return -1;
			}
			buf_base=s2hex(argv[i+1]);
			buf_size=s2hex(argv[i+2]);
		}
		if( (!strcmp(argv[i],"-cache")))
		{
			set_flag=1;
			if( !strcmp(argv[i+1],"on")  )
			{
				cache=1;
			}
			else
			{
				if( !strcmp(argv[i+1],"off") )
				{
					cache=0;
				}
				else
				{
					uprintf("ERROR: Invalid parameter\n");
					__Heap_Free( &my_heap, buffer );
					return -1;
				}
			}
		}
		if( (!strcmp(argv[i],"-baudrate")))
		{
			set_flag=1;
			if( !validdec(argv[i+1]) )
			{
				uprintf("ERROR: Invalid number\n");
				__Heap_Free( &my_heap, buffer );
				return -1;
			}
			speed=s2dec(argv[i+1]);
			if( !UART_Speed(speed,&divider) )
			{
				uprintf("ERROR: Invalid buad rate\n");
				__Heap_Free( &my_heap, buffer );
				return -1;
			}
			
			baudrate=speed;
		}
	
		if( (!strcmp(argv[i],"-usb")))
		{
			set_flag=1;
			if( !validdec(argv[i+1]) )
			{
				uprintf("ERROR: Invalid number\n");
				__Heap_Free( &my_heap, buffer );
				return -1;
			}
			usb=((s2dec(argv[i+1])==1)?1:0);
		}
		
		if( (!strcmp(argv[i],"-sn")))
		{
			set_flag=1;
			if(validdec(argv[i+1]))
				serial_no=s2dec(argv[i+1]);
			else if(validhex(argv[i+1]))
				serial_no=s2hex(argv[i+1]);
			else
			{
				uprintf("ERROR: Invalid number\n");
				__Heap_Free( &my_heap, buffer );
				return -1;
			}
		}
		
	}

	if( set_flag )
	{
		memcpy(info->mac0, mac0, 6);
		memcpy(info->ip0, ip0, 4);
		//memcpy(info->mac1, mac1, 6);
		//memcpy(info->ip1, ip1, 4);
		info->cache=cache;
		info->dhcp=dhcp;
		info->net_mac=net_mac;
		//info->phy=phy;
		info->buf_base=buf_base;
		info->buf_size=buf_size;
		info->baudrate=baudrate;
		//info->rmii=rmii;
		info->serial_no=serial_no;
		info->usb=usb;
		// delete image 0

#ifndef USE_NO_IMAGE0
#ifdef __USB__
			if(enableUSB)	Disable_USB();
#endif

		DelImage(0);
#ifdef __USB__
			if(enableUSB)	Enable_USB();
#endif		
		image_footer.num=0;
		image_footer.base=BL_IMAGE0_BASE;
		image_footer.length=sizeof( tbl_info ); // write everything except old footer
		image_footer.load_address=image_footer.exec_address=image_footer.base;
		image_footer.signature=SIGNATURE_WORD;
		image_footer.type=IMAGE_FILE;
		strcpy(image_footer.name,"BOOT INFO");
			
		//if( _net_init )DisableIRQ(); // avoid flash R/W confliction
#ifdef __USB__
			if(enableUSB)	Disable_USB();
#else
			if( _net_init )DisableIRQ();							
#endif			
		
		if( WriteImage(&image_footer, (UINT32)info) )
		{
			//if( _net_init )EnableIRQ();
#ifdef __USB__
			if(enableUSB)	Enable_USB();
#else
			if( _net_init )EnableIRQ();							
#endif				
			uprintf("ERROR: Fail to update boot loader information.\n");
			__Heap_Free( &my_heap, buffer );
			return -1;
		}
		else
		{
			//if( _net_init )EnableIRQ();
#ifdef __USB__
			if(enableUSB)	Enable_USB();
#else
			if( _net_init )EnableIRQ();							
#endif			
			uprintf("Boot loader infomation updated.\n");
		}
#else
		//if( _net_init )DisableIRQ(); // avoid flash R/W confliction
#ifdef __USB__
			if(enableUSB)	Disable_USB();
#else
			if( _net_init )DisableIRQ();							
#endif			
		WriteBIB(info,buffer);
		//if( _net_init )EnableIRQ();
#ifdef __USB__
			if(enableUSB) Enable_USB();
#else
			if( _net_init )EnableIRQ();							
#endif		
		uprintf("Boot loader infomation updated.\n");
#endif		

	}
	
	BIB_ShowInfo(info);
	
	
//	uprintf("\tIP .... :%d.%d.%d.%d\n",ip[0],ip[1],ip[2],ip[3]);
//	uprintf("\tMAC ... :%02x:%02x:%02x:%02x:%02x:%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
//	uprintf("\tDHCP=%d\n", dhcp);
//	uprintf("\tnet_mac=%d\n", net_mac);
//	uprintf("\tphy=%d\n", phy);
	
	__Heap_Free( &my_heap, buffer );

	if( set_flag )
	{
		uprintf("\nWARNING: The new setting will be valid after rebooting.\n");
	}
	return 0;
}

 int TERM_action(int argc, char *argv[])
{
	if( (argv[1][0]!='0') && (argv[1][0]!='1') )
	{
		uprintf("ERROR: TERM 0 : serial port output, TERM 1 : TCP/IP output\n");
		return -1;
	}

	if( (argv[1][0]-'0') )
	{
		uprintf("Warning: Debug port switch to network\n");
	}
	else
	{
		uprintf("Warning: Debug port switch to serial port\n");
	}
	

	if( (!_net_init) && (argv[1][0]-'0') )
	{
#ifndef __USB__	
		if( Net_Init(_dhcp) < 0 ) //===========>
#endif		
		{
			uprintf("ERROR: Network initialization failed!\n");
			return -1;
		}
		_net_init=1;
	}
	
	SetNetWrite((argv[1][0]-'0'));

	return 0;
	
}


 int BOOT_action(int argc, char *argv[])
{
	UINT32 tmp;
	uprintf("\nRebooting the system ...\n");
	tmp=CLKSEL;
	tmp|=0x1;
	CLKSEL=tmp;
	return 0;
}

INT ATTRIB_usage()
{
	uprintf("Usage: ATTRIB [ImageNo.] [-a,c,x,f,z]\n");
	uprintf("		-a 		Active image\n");
	uprintf("		-c 		Image needs to be copy to RAM\n");
	uprintf("		-x 		Executable image\n");
	uprintf("		-f 		File system image\n");
	uprintf("		-z 		Compressed image\n");
	return -1;
}

int ATTRIB_action(int argc, char *argv[])
{
	tfooter * footer=NULL;
	int i;
	int image_type=0;

	if( argc == 3 )
	{
		if( validdec(argv[1]) )
		{
			if( FindImage( s2dec(argv[1]), &footer ) )
			{
				if( argv[2][0]=='-' )
				{
					i=1;
					while(argv[2][i]!='\0')
					{
						switch(argv[2][i++])
						{
							case 'a':
								image_type|=IMAGE_ACTIVE;
								break;
							case 'c':
								image_type|=IMAGE_COPY2RAM;
								break;
							case 'x':
								image_type|=IMAGE_EXEC;
								break;
							case 'f':
								image_type|=IMAGE_FILE;
								break;
							case 'z':
								image_type|=IMAGE_COMPRESSED;
								break;
							default:
								return ATTRIB_usage();
						}
					}
					
					if( !ChangeImageType( s2dec(argv[1]), image_type ) )
					{
						uprintf("Image attribution changed successfully.\n");
					}
					else
					{
						uprintf("ERROR: Image attribution change failed.\n");
						return -1;
					}
					
				}
				else
					return ATTRIB_usage();
			
			}
			else
			{
				uprintf("ERROR: Can't find the image %d\n",s2dec(argv[1]));
				return -1;
			}
		}
		else
		{
			uprintf("ERROR: Invalid image number!\n");
			return -1;
		}
			
	}
	else
		return ATTRIB_usage();
 	
	return 0; 
}
 


//******************** End Of Standard action routines ***************************
