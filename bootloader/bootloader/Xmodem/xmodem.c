/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: xmodem.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: xmodem.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/Xmodem
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:28p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/Xmodem
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:10p
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * Adjust the delay time for print 'C'
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/12/25   Time: 2:32p
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * Increase the "C" trigger interval to avoid the xmodem transfer error in
 * minicom terminal
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/20   Time: 11:02a
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * Change author name from "Wschang0" to "wschang0"
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 10:58a
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * Add VSS information header
 */


#include "xmodem.h"
#include "uprintf.h"

__weak extern sleep(int ms);
unsigned short crcTable[256];
void calctable (void);

int xmodem(unsigned int buffer,unsigned int *fileSize)
{
	unsigned char ch,j;
	unsigned char packetNo;
	int i;
	int startPacket;
    unsigned short crc;
    unsigned short crcWord;
    int status;
    unsigned int delayCnt;
    unsigned int oldDelay;
    unsigned int ccnt;
	
	ccnt=0;
	startPacket=1;
	packetNo=1;
	*fileSize=0;
	delayCnt=0;
	oldDelay=0;

	calctable();
	
	while(1)
	{
		if( delayCnt > XMODEM_TIMEOUT_VALUE)
		{
			status=X_STIMEOUT;
			break;
		}
		if( ukbhit() )
		{
			if( delayCnt > oldDelay+10000 )uputchar(ACK);
			oldDelay=delayCnt;
			delayCnt=0;
			
			ch=ugetchar();
#ifdef XMODEM1K
			if( ch==STX )
#else
			if( ch==SOH )
#endif
			{
				if( startPacket )
					startPacket=0;
	
				ch=ugetchar();
				if( ch==packetNo )
				{
					ch=ugetchar();
					if( (ch+packetNo)==0xFF )
					{
						crc=0;
#ifdef XMODEM1K
						for(i=0;i<1024;i++)
#else
						for(i=0;i<128;i++)
#endif						
						{
							ch=ugetchar();
						    j = (crc >> 8) ^ ch;
							crc = (crc << 8) ^ crcTable[j];
							*((volatile unsigned char *)buffer+*fileSize+i)=ch;
						}
#ifdef XMODEM1K
						*fileSize+=1024;	
#else
						*fileSize+=128;	
#endif						
						ch=ugetchar();
						crcWord=ch;
						ch=ugetchar();
						crcWord=(crcWord<<8)+ch;
						if( crc==crcWord )
						{
							uputchar(ACK);
							packetNo++;
						}
						else
						{
							uputchar(NAK);
						}
						
					}
					else
					{
#ifdef XMODEM1K
						uputchar(NAK);
#else
						uputchar(CANCEL);
						uputchar(CANCEL);
						uputchar(CANCEL);
						uputchar(CANCEL);
						uputchar(CANCEL);
						break;
#endif						
					}
				
				}
				else
				{
					uputchar(CANCEL);
					uputchar(CANCEL);
					uputchar(CANCEL);
					uputchar(CANCEL);
					uputchar(CANCEL);
					status=X_SPACKET_NUM_ERROR;
					break;
				}
			
			}
			else
			{
				if( ch==EOT )
				{
					uputchar(ACK);
					status=X_SSUCCESS;
					break;
				}
				else
				{
					if( ch==CANCEL )
					{
						status=X_SUSERCANCEL;
						break;
					}
				}
			}
			
		}
		else
		{
			delayCnt++;
			if( startPacket )
			{
				if( (delayCnt&0x3FFFF)==0 ) 
				{
					uputchar('C');
				}
			}
			
			/* fix the fifo keep problem of linux */
			if( ccnt == 0 )
			{
				if( &sleep ) sleep(4000);
				else
				{
					volatile unsigned kk;
					for(kk=0;kk<0x2000000;kk++);
				}
				ccnt = 1;
			}
			
		}
	}

	return status;
}
/*==========================================================================*/
#define CCITT 0x1021
unsigned short  _crc(unsigned short n)
{
    unsigned short  i, acc;

    for (n<<=8, acc=0, i=8; i > 0; i--, n<<=1)
        acc = ((n^acc) & 0x8000) ? ((acc<<1) ^ CCITT) : (acc<<1);
    return (acc);
}

void calctable ()
{
    static unsigned char crcTableBuilt=0;
    unsigned short  i;

    if (! crcTableBuilt)
    {
        crcTableBuilt = 1;
        for (i=0; i < 256; i++)
            crcTable[i] = _crc (i);
    }
}

