/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: serial.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: serial.h $
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
#ifndef SERIAL_H
#define SERIAL_H
//-----------------------------------------------------------------------------------------
extern void sendchar( char *ch );
extern int serial_kbhit(void);
extern int serial_getchar(void);
extern int UART_Speed(int speed, int *divider);
void init_serial(unsigned int port, unsigned int baudRate);

//-----------------------------------------------------------------------------------------
#endif