/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: DHCP.H $
 *
 * Created by : 
 ******************************************************************************/
/*
 * $History: DHCP.H $
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

#ifndef _DHCP_H_
#define _DHCP_H_

/* DHCP client and server ports */
#define SERVER_PORT 			67
#define CLIENT_PORT 			68

#define DHCP_DISCOVER 			1
#define DHCP_OFFER 				2
#define DHCP_REQUEST 			3
#define DHCP_DECLINE 			4
#define DHCP_ACK 				5
#define DHCP_NAK 				6
#define DHCP_RELEASE 			7
#define DHCP_INFORM  			8

#define BOOTP_REQUEST 			1
#define BOOTP_REPLY 			2

typedef struct DhcpHdr 
{                
    UCHAR		op_code;        /* operation */
    UCHAR   	hw_type;      	/* hardware type */
    UCHAR   	hw_len;       	/* hardware address length */
    UCHAR   	hops;       	/* gateway hops */
    UINT32   	tx_id;        	/* transaction ID */
    UINT16  	seconds;       	/* seconds since boot began */
    UINT16  	flags;      	/* flags */
    UINT32   	clientIP;     	/* client IP address */
    UINT32   	yourIP;     	/* your IP address */
    UINT32   	serverIP;     	/* server IP address */
    UINT32   	gatewayIP;     	/* gateway IP address */
    UCHAR   	client_hw_addr[16]; 	/* client hardware address */
    UCHAR   	serverName[64]; /* server host name */
    UCHAR   	bootFile[128]; 	/* boot bootFile name */
    UCHAR   	options[312];   /* options */
} DHCP_HDR_T;

#define DHCP_OPT_OFFSET 		236		/* size without options */

#endif  /* _DHCP_H_ */