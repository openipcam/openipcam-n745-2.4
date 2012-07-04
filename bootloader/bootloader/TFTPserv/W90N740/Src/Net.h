/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: Net.h $
 *
 * Created by : 
 ******************************************************************************/
/*
 * $History: Net.h $
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
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:26p
 * Updated in $/W90N740/FIRMWARE/TFTPserv/W90N740/Src
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/09/24   Time: 7:53p
 * Updated in $/W90N740/FIRMWARE/TFTPserv/W90N740/Src
 * Add header
 */

#ifndef  _net_h_
#define  _net_h_

#include "cdefs.h"

#define NIC_InitDriver			Mac_Initialize
#define NIC_SendPacket			Mac_SendPacket
#define NIC_ShutDown			Mac_ShutDown

#define NIC_EnableBroadcast  	Mac_EnableBroadcast
#define NIC_DisableBroadcast 	Mac_DisableBroadcast


/* BIG ENDIAN */
#ifdef __BIG_ENDIAN
# define  NC2(x)              (x)
#else
/* LITTLE ENDIAN */ 
# define  NC2(x)              ((((x)>>8)&0xFF)|(((x)<<8)&0xFF00))
#endif

#define  PROTOCOL_ARP        0x0806
#define  PROTOCOL_IP         0x0800

#define  HT_ETHERNET         0x0001
#define  ARP_PTYPE           0x0800

#define  ARP_REQUEST         0x01
#define  ARP_REPLY           0x02
#define  RARP_REQUEST        0x03
#define  RARP_REPLY          0x04

#define  IP_PRO_ICMP         1
#define  IP_PRO_TCP          6
#define  IP_PRO_UDP          17

#define  TFTP_SERVER_PORT    69
#define  TFTP_SERVICE_PORT   1060

/* TFTP operation code */
#define  OPCODE_READ    1
#define  OPCODE_WRITE   2
#define  OPCODE_DATA    3
#define  OPCODE_ACK     4
#define  OPCODE_ERROR   5

#define  MAX_RETRY      5

/* !! Note !!
   If you want to apply any packets to any one of the following structures,
   you must make sure that the packets are started at word boundary. 
   Otherwise, unexpected conditions may arise.
  */ 


typedef struct ETHER_HEADER
{
   	UCHAR   tMAC[6];       /* MAC address of the sink */
   	UCHAR   sMAC[6];       /* MAC address of the source */
   	UINT16 protocol;      /* protocol type */
}  	ETHER_PACKET;



typedef struct ARP_PACKET
{
   	UCHAR    tMAC[6];      /* MAC address of the sink */
   	UCHAR    sMAC[6];      /* MAC address of the source */
   	UINT16  protocol;     /* protocol -> 0806 */
   	UINT16  htype;        /* hardware type */
   	UINT16  ptype;        /* protocol, 0x0800:Internet */
   	UCHAR    hlen;         /* hardware length */
   	UCHAR    plen;         /* protocol length */
   	UINT16  operation;    /* operation, 1:ARP request, 2:ARP reply, 3:RARP request, 4:RARP reply */
   	UCHAR    sender_ha[6]; /* sender hardware address */
   	UCHAR    sender_ip[4]; /* sender IP address */
   	UCHAR    target_ha[6];	/* target hardware address */
   	UCHAR    target_ip[4]; /* target IP address */
}  	ARP_PACKET;



typedef struct IP_PACKET
{
   	UCHAR    tMAC[6];      /* MAC address of the sink */
   	UCHAR    sMAC[6];      /* MAC address of the source */
   	UINT16  protocol;     /* protocol -> 0800 */
   	UCHAR    vers_hlen;    /* bits[7..4]:VERS, bits[3..0]:HLEN */
   	UCHAR    stype;        /* service type */
   	UINT16  tlen;         /* total length, start from "vers_hlen" field */
   	UINT16  id;           /* IP packet identification */
   	UINT16  ffrag;        /* bits[15..12]:flags, bits[11..0]:fragment offset */
   	UCHAR    ttl;          /* time-to-live */
   	UCHAR    ippro;        /* IP protocol, 1:ICMP, 6:TCP, 17:UDP */
   	UINT16  hdrchksum;    /* header check sum */
   	UCHAR    srcIP[4];     /* source IP */
   	UCHAR    destIP[4];    /* destination IP */
} 	IP_PACKET;



typedef struct UDP_PACKET
{
   	UCHAR    tMAC[6];      /* MAC address of the sink */
   	UCHAR    sMAC[6];      /* MAC address of the source */
   	UINT16  protocol;     /* protocol -> 0800 */
   	UCHAR    vers_hlen;    /* bits[7..4]:VERS, bits[3..0]:HLEN */
   	UCHAR    stype;        /* service type */
   	UINT16  tlen;         /* total length, start from "vers_hlen" field */
   	UINT16  id;           /* IP packet identification */
   	UINT16  ffrag;        /* bits[15..12]:flags, bits[11..0]:fragment offset */
   	UCHAR    ttl;          /* time-to-live */
   	UCHAR    ippro;        /* IP protocol, 1:ICMP, 6:TCP, 17:UDP */
   	UINT16  hdrchksum;    /* header check sum */
   	UCHAR    srcIP[4];     /* source IP */
   	UCHAR    destIP[4];    /* destination IP */
   	UINT16  sport;        /* source port */
   	UINT16  dport;        /* destination port */
   	UINT16  mlen;         /* message length, start from "sport" */
   	UINT16  udpchksum;    /* check sum of UDP packet, including virtual header */
}  	UDP_PACKET;


typedef struct UDP_PSEUDO  	/* UDP pseudo header, for check sum calculation */
{
   	UCHAR    srcIP[4];
   	UCHAR    destIP[4];
   	UINT16  protocol;
   	UINT16  udp_len;	
}  	UDP_PSEUDO;



typedef struct TCP_PACKET
{
   	UCHAR    tMAC[6];      /* MAC address of the sink */
   	UCHAR    sMAC[6];      /* MAC address of the source */
   	UINT16  protocol;     /* protocol -> 0800 */
   	UCHAR    vers_hlen;    /* bits[7..4]:VERS, bits[3..0]:HLEN */
   	UCHAR    stype;        /* service type */
   	UINT16  tlen;         /* total length, start from "vers_hlen" field */
   	UINT16  id;           /* IP packet identification */
   	UINT16  ffrag;        /* bits[15..12]:flags, bits[11..0]:fragment offset */
   	UCHAR    ttl;          /* time-to-live */
   	UCHAR    ippro;        /* IP protocol, 1:ICMP, 6:TCP, 17:UDP */
   	UINT16  hdrchksum;    /* header check sum */
   	UCHAR    srcIP[4];     /* source IP */
   	UCHAR    destIP[4];    /* destination IP */
   	UINT16  sport;        /* source port */
   	UINT16  dport;        /* destination port */
   	UCHAR    seqNum[4];	/* sequence number */
   	UCHAR    ackNum[4];	/* ACK number */
   	UINT16  len_code;		/* header length and code bits */
   	UINT16  win_size;		/* window size */
   	UINT16  tcpchksum;	/* check sum of TCP packet, including virtual header */
   	UINT16  urgent;       /* urgent pointer */
}  	TCP_PACKET;


typedef struct TCP_PSEUDO  	/* TCP pseudo header, for check sum calculation */
{
   	UCHAR    srcIP[4];
   	UCHAR    destIP[4];
   	UINT16  protocol;
   	UINT16  tcp_len;	
}  	TCP_PSEUDO;


#define TCP_TX_BUF_SIZE		1400
#define TCP_RX_BUF_SIZE		1024
#define TCP_WIN_SIZE		(NC2(TCP_RX_BUF_SIZE))


typedef struct TCP_CONN
{
	UCHAR tx_buff[TCP_TX_BUF_SIZE];
	UCHAR rx_buff[TCP_RX_BUF_SIZE];
	INT	tx_head, tx_tail;
	INT rx_head, rx_tail;
	INT win_size;	/* current window size */
	INT	state;		/* TCP connection state */
	UINT32	seqNum;
	UINT32	ackNum;
	UCHAR peerMAC[6];	/* peer MAC address */
	UCHAR peerIP[4];	/* peer IP address */
	UINT16	peerPort;	/* peer TCP port number */
	UINT16	my_port;	/* my port number */
}	TCP_CONN;

enum 
{
	TCP_DISCONNECT = 0,
	TCP_CONNECTING,
	TCP_CONNECTED,
	TCP_CLSOING
};


/* TCP code bits */
#define TCP_FIN		0x01
#define TCP_SYN		0x02
#define TCP_RST		0x04
#define TCP_PSH		0x08
#define TCP_ACK		0x10
#define TCP_URG		0x20


#define  CompareIP(ip1,ip2)  ((ip1[3]^ip2[3])|(ip1[2]^ip2[2])|(ip1[1]^ip2[1])|(ip1[0]^ip2[0]))

#define GET16(bptr,n)   	(bptr[n+1] | (bptr[n] << 8))
#define GET32(bptr,n)   	(bptr[n+3] | (bptr[n+2] << 8) | (bptr[n+1] << 16) | (bptr[n] << 24))
#define PUT16(bptr,n,val)	bptr[n+1] = val & 0xFF;				\
							bptr[n] = (val >> 8) & 0xFF;
#define PUT32(bptr,n,val)	bptr[n+3] = val & 0xFF;				\
							bptr[n+2] = (val >> 8) & 0xFF;		\
							bptr[n+1] = (val >> 16) & 0xFF;		\
							bptr[n] = (val >> 24) & 0xFF;


extern UCHAR   _HostMAC[6];
extern UCHAR 	_HostIP[4];
extern UINT16 _IP_packet_ID;

extern INT   Net_Init(int dhcp);
extern VOID  Get_MAC_Address(UCHAR *mac);
extern INT   GetIpByDhcp(void);
extern UINT16  Nchksum(UINT16 *cp, int cnt);
extern INT   PacketProcessor(UCHAR  *packet, int len);

extern INT _dhcp;
extern INT _phy;
extern INT _rmii;

#define NET_PHY				0x0
#define NET_IC_PLUS			0x1
#define NET_MARVELL6052		0x2




#endif  /* _net_h_ */ 