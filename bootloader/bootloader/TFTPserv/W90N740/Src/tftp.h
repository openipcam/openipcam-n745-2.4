/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: tftp.h $
 *
 * Created by : 
 ******************************************************************************/
/*
 * $History: tftp.h $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/TFTPserv/W90N740/Src
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/TFTPserv/W90N740/Src
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/09/24   Time: 7:53p
 * Updated in $/W90N740/FIRMWARE/TFTPserv/W90N740/Src
 * Add header
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