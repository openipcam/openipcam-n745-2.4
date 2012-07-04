/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: tftp.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: tftp.h $
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

#ifndef TFTP_SERVER_H
#define TFTP_SERVER_H
//-----------------------------------------------------------------------
extern int TFTP_Download(unsigned char *filebuf, unsigned long *filesize, int dhcp);
extern int  Net_Init(int dhcp);
extern char  NET_getchar(void);
extern int  NET_kbhit(void);
extern void  NET_putchar(char ch);

extern void SetPhyChip(int num);
extern void SetMacNumber(int num);
extern void GetIpAddress(char *ip);
extern void SetIpAddress(char *ip);
extern void SetMacAddress(char *mac);
extern void GetMacAddress(char *mac);

//-----------------------------------------------------------------------
#endif