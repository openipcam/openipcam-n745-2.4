/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 * Winbond W90N740 Boot Loader
 *
 * Module Name:	NETKERNEL.C
 *
 * Created by : 
 ******************************************************************************/
#include "w90p710.h"
#include "net.h"
#include "netbuf.h"


#define USE_SIB             0

#define NET_TERM_PORT		65500

UCHAR  _HostMAC[6] = { 0x00, 0x50, 0x51, 0x52, 0x53, 0x57 };
UCHAR	 _HostIP[4] =  { 192, 168, 0, 201 };

UINT16 _IP_packet_ID = 1000;

INT  _netbuf_ready = 0;
static INT  _mac_inited = 0;

/* for TCP */
TCP_CONN	_NetTerm;


INT  NET_flush(void);


UINT16  Nchksum(UINT16 *cp, INT cnt)
{
	volatile UINT16 i1=0, i2;

	while (cnt--)
	{
		i2 = *cp++;
		i2 = NC2(i2);
		i1 += i2;
		if (i1 < i2)
			i1++;
	}
	return NC2(i1);
}



void  TCP_SendPacket(UCHAR *buff, INT len, UCHAR code, INT in_intr)
{
	IP_PACKET	*ip;
	TCP_PACKET	*tcp;
	TCP_PSEUDO	pseudo;
	NETBUF		*txbuf;
	
	if (in_intr)
		txbuf = NetBuf_AllocateIR();
	else
		txbuf = NetBuf_Allocate();
	
	if (txbuf == NULL)
	{
		return;
	}

	txbuf->len = sizeof(TCP_PACKET) + len;
	if (len != 0)
		memcpy((CHAR *)&txbuf->packet[sizeof(TCP_PACKET)], (CHAR *)buff, len);

	txbuf->packet[sizeof(TCP_PACKET)+1] = 0;

	/*- prepare Ethernet header, 14 bytes -*/
	ip = (IP_PACKET *)txbuf->packet;
	tcp = (TCP_PACKET *)txbuf->packet;
	memcpy((CHAR *)ip->tMAC, (CHAR *)_NetTerm.peerMAC, 6);
	memcpy((CHAR *)ip->sMAC, (CHAR *)_HostMAC, 6);
	ip->protocol = NC2(PROTOCOL_IP);

	/*- prepare IP header, 20 bytes -*/
	ip->vers_hlen = 0x45;     		/* fixed value, do not change it */
	ip->stype = 0;            		/* no special priority */
	ip->tlen = NC2(sizeof(TCP_PACKET) + len - 14);
	ip->id = NC2(_IP_packet_ID);
	_IP_packet_ID++;
	ip->ffrag = 0;
	ip->ttl = 64;
	ip->ippro = IP_PRO_TCP;
	ip->hdrchksum = 0;
	memcpy((CHAR *)ip->srcIP, (CHAR *)_HostIP, 4);
	memcpy((CHAR *)ip->destIP, (CHAR *)_NetTerm.peerIP, 4);
	ip->hdrchksum = ~Nchksum((UINT16 *)&(ip->vers_hlen), 10);  /* 20 bytes */

	/*- prepare TCP header -*/
	tcp->sport = NC2(NET_TERM_PORT);
	tcp->dport = NC2(_NetTerm.peerPort);
	
	PUT32(tcp->seqNum, 0, _NetTerm.seqNum);
	PUT32(tcp->ackNum, 0, _NetTerm.ackNum);

	if (code & TCP_SYN)
		tcp->len_code = (6 << 12) | code;
	else	
		tcp->len_code = (5 << 12) | code;
	tcp->len_code = NC2(tcp->len_code);
	
	tcp->win_size = _NetTerm.win_size;
	tcp->tcpchksum = 0;
	tcp->urgent = 0;
	
	/* Caculate the TCP pseudo header checksum */	
	memcpy((CHAR *)pseudo.srcIP, (CHAR *)_HostIP, 4);
	memcpy((CHAR *)pseudo.destIP, (CHAR *)_NetTerm.peerIP, 4);
	pseudo.protocol = NC2(IP_PRO_TCP);
	pseudo.tcp_len = NC2(20 + len);
	tcp->tcpchksum = Nchksum((UINT16 *)&pseudo, 6);
	
	tcp->tcpchksum = ~(Nchksum((UINT16 *)&(tcp->sport), (20 + len + 1) / 2));
	if (tcp->tcpchksum == 0)
		tcp->tcpchksum = 0xffff;

	NIC_SendPacket(txbuf);
	
	if (!(code & TCP_SYN))
		_NetTerm.seqNum += len;
}



INT  TCP_Core(UCHAR *packet, INT len)
{
	TCP_PACKET	*tcp = (TCP_PACKET *)packet;
	UCHAR		option[4] = { 0x02, 0x04, 0x5, 0xAC };	/* Maximum Segment Size 1452 */
	UCHAR		code;
	UINT32	seqNum, ackNum;
	UCHAR		*data = &packet[sizeof(TCP_PACKET)];
	INT			data_len = NC2(tcp->tlen) - 40;
	INT			idx;



	if (NC2(tcp->dport) != NET_TERM_PORT)
		return -1;
	
	code = NC2(tcp->len_code) & 0x3F;
	seqNum = GET32(tcp->seqNum, 0);
	ackNum = GET32(tcp->ackNum, 0);
	
	if (code & (TCP_RST | TCP_FIN))
	{
		if (NC2(tcp->sport) != _NetTerm.peerPort)
			return 0;
		_NetTerm.state = TCP_DISCONNECT;

		return 0;
	}
	
	switch (_NetTerm.state)
	{
		case TCP_DISCONNECT:
			if (code & TCP_SYN)
			{
				_NetTerm.ackNum = seqNum + 1;
				_NetTerm.rx_head = _NetTerm.rx_tail = 0;
				_NetTerm.win_size = TCP_WIN_SIZE;
				memcpy((CHAR *)_NetTerm.peerMAC, (CHAR *)tcp->sMAC, 6);
				memcpy((CHAR *)_NetTerm.peerIP, (CHAR *)tcp->srcIP, 4);
				_NetTerm.peerPort = NC2(tcp->sport);
				TCP_SendPacket(option, 4, TCP_ACK | TCP_SYN, 1);
				_NetTerm.state = TCP_CONNECTING;
				//uprintf("TCP connecting\n");
			}
			return 0;
		
		case TCP_CONNECTING:
			if (seqNum != _NetTerm.ackNum)
			{	/* incorrect sequence number, discard it */
				return 0;
			}
	
			if (code & TCP_ACK)
			{
				if (ackNum == _NetTerm.seqNum + 1)
				{
					_NetTerm.state = TCP_CONNECTED;
				
					_NetTerm.seqNum++;
					NET_flush();
				}
			}
			return 0;
			
		case TCP_CONNECTED:
			if (NC2(tcp->sport) != _NetTerm.peerPort)
				return 0;		/* port incorrect, ignore it */

			if (seqNum != _NetTerm.ackNum)
			{	/* incorrect sequence number, retransmit the last packet */
				return 0;
			}

			if (code & TCP_ACK)
			{
				_NetTerm.seqNum = ackNum;
				if (data_len == 0)
					return 0;
			}
		
			for (idx = 0; idx < data_len; idx++)
			{				
				_NetTerm.rx_buff[_NetTerm.rx_tail] = data[idx];
				_NetTerm.rx_tail = (_NetTerm.rx_tail+1) % TCP_RX_BUF_SIZE;
			}
			_NetTerm.ackNum += data_len;
		
			return 0;		
		
		case TCP_CLSOING:
			if (seqNum != _NetTerm.ackNum)
			{	/* incorrect sequence number, discard it */
				return 0;
			}
	}
	return 0;
}



void  ARP_Reply(UCHAR *target_ip, UCHAR *target_mac)
{
	ARP_PACKET  *arp;
	NETBUF      *txbuf;



	if ((txbuf = NetBuf_AllocateIR()) == NULL)
	{
		return;
	}

	/*- prepare and send ARP reply packet -*/
	txbuf->len = sizeof(ARP_PACKET);
	arp = (ARP_PACKET *)txbuf->packet; 
	memcpy((CHAR *)arp->tMAC, (CHAR *)target_mac, 6);
	memcpy((CHAR *)arp->sMAC, (CHAR *)_HostMAC, 6);
	arp->protocol  = NC2(PROTOCOL_ARP);
	arp->htype     = NC2(HT_ETHERNET);
	arp->ptype     = NC2(ARP_PTYPE);
	arp->hlen      = 6;
	arp->plen      = 4;
	arp->operation = NC2(ARP_REPLY);
	memcpy((CHAR *)arp->sender_ha, (CHAR *)_HostMAC, 6);
	memcpy((CHAR *)arp->sender_ip, (CHAR *)_HostIP, 4);
	memcpy((CHAR *)arp->target_ha, (CHAR *)target_mac, 6);
	memcpy((CHAR *)arp->target_ip, (CHAR *)target_ip, 4);
	NIC_SendPacket(txbuf);
}



INT  PacketProcessor(UCHAR *packet, INT len)
{
	ARP_PACKET	*arp = (ARP_PACKET *)packet;
	IP_PACKET	*ip  = (IP_PACKET *)packet;
	UDP_PACKET	*udp = (UDP_PACKET *)packet;
	
	if (packet[0] == 0xFF)   	/* this is a broadcast packet */
	{
		/* 
		 *  We manage the ARP reply process here. 
		 *  In the following code, if we have received a ARP request,
		 *  we send ARP reply immediately.
		 */
		if ((!CompareIP(arp->target_ip, _HostIP)) &&
			(arp->protocol == NC2(PROTOCOL_ARP)) && (arp->operation == NC2(ARP_REQUEST)))
		{                            
			ARP_Reply(arp->sender_ip, arp->sender_ha);
			return 0;
		}

		if ((ip->ippro == IP_PRO_UDP) && (udp->sport == NC2(67)))
			return -1;			/* DHCP packet */

		return 0;
	}
	else                    	/* this is a multicast or unicast packet */
	{
		/*
		 *  This is a unicast packet to us. We are only interested
		 *  in the TCP packets. If this is a TCP packet and we are
		 *  the target host, we will pass this packet to the TCP processor.
		 */
		if ((ip->ippro == IP_PRO_TCP) && (!CompareIP(ip->destIP, _HostIP)))
		{
			TCP_Core(packet, len);
			return 0;
		}
				
		/*
		 * Check ICMP Echo Request packet -
		 * if matched, we reply it right here
		 */
		if ((ip->ippro == IP_PRO_ICMP) && (!CompareIP(ip->destIP, _HostIP)) &&
			(packet[34] == 0x08))
		{
			NETBUF		*txbuf;
			IP_PACKET	*tx_ip;

			
			if ((txbuf = NetBuf_AllocateIR()) == NULL)
				return -1;

			/* duplicate packet then modify it */
			memcpy((CHAR *)&txbuf->packet[0], (CHAR *)&packet[0], len);
			txbuf->len = len;
				
			tx_ip = (IP_PACKET *)txbuf->packet; 
			memcpy((CHAR *)tx_ip->tMAC, (CHAR *)ip->sMAC, 6);
			memcpy((CHAR *)tx_ip->sMAC, (CHAR *)_HostMAC, 6);
			tx_ip->protocol = NC2(PROTOCOL_IP);
			tx_ip->vers_hlen = 0x45; 		/* fixed value, do not change it */
			tx_ip->stype = 0;            	/* no special priority */
			tx_ip->tlen = NC2(60);
			tx_ip->id = NC2(_IP_packet_ID);
			tx_ip->ffrag = 0;
			tx_ip->ttl = 64;
			tx_ip->ippro = IP_PRO_ICMP;
			tx_ip->hdrchksum = 0;
			memcpy((CHAR *)tx_ip->srcIP, (CHAR *)_HostIP, 4);
			memcpy((CHAR *)tx_ip->destIP, (CHAR *)ip->srcIP, 4);
			tx_ip->hdrchksum = ~Nchksum((UINT16 *)&tx_ip->vers_hlen, 10);  /* 20 bytes */

			_IP_packet_ID++;
				
			/* ICMP reply */
			txbuf->packet[34] = 0;
			
			/* ICMP checksum */
			txbuf->packet[36] = 0;
			txbuf->packet[37] = 0;
			*(UINT16 *)&txbuf->packet[36] = ~Nchksum((UINT16 *)&txbuf->packet[34], (txbuf->len - 34) / 2);
			
   			NIC_SendPacket(txbuf);
   			
   			return 0;
		}

		if ((ip->ippro == IP_PRO_UDP) && (udp->sport == NC2(67)))
			return -1;		/* DHCP packet */

		if ((ip->ippro == IP_PRO_UDP) && (!CompareIP(ip->destIP, _HostIP)))
			return -1;		/* UDP packet for us */
	}
	
	return 0;
}



INT  NET_flush()
{
	if (_NetTerm.state != TCP_CONNECTED)
	{
		if (_NetTerm.tx_head+2 >= TCP_TX_BUF_SIZE)
			_NetTerm.tx_head -= 2;
		return -1;
	}
	
	if (_NetTerm.tx_head == 0)
		return 0;
	TCP_SendPacket(_NetTerm.tx_buff, _NetTerm.tx_head, TCP_PSH | TCP_ACK, 0);
	_NetTerm.tx_head = 0;
	return 0;
}


extern CHAR TxReady0;
void  NET_putchar(CHAR Ch)
{
	volatile unsigned int i;

	
//	NET_flush();
	_NetTerm.tx_buff[_NetTerm.tx_head++] = Ch;
//	if( Ch == '\n' )
//		_NetTerm.tx_buff[_NetTerm.tx_head++] = '\r';
	
	NET_flush();


	if( Ch == '\n' )
	{
//		NET_flush();
//		while(!TxReady0);
		//for(i=0;i<DELAY;i++);
		_NetTerm.tx_buff[_NetTerm.tx_head++] = '\r';
		NET_flush();
	}

#if 0	
	if (Ch == '\n')
		_NetTerm.tx_buff[_NetTerm.tx_head++] = '\r';
		
	if ((Ch == '\r') || (Ch == '\n') || 
		(_NetTerm.tx_head + 2 >= TCP_TX_BUF_SIZE) ||
		(_NetTerm.tx_head + 2 >= 1452))
		NET_flush();
#endif		
}



CHAR  NET_getchar()
{
	CHAR	ch;
	
	while ((volatile)_NetTerm.rx_head == (volatile)_NetTerm.rx_tail)
		;
		
	ch = _NetTerm.rx_buff[_NetTerm.rx_head];
	_NetTerm.rx_head = (_NetTerm.rx_head+1) % TCP_RX_BUF_SIZE;
	
	return ch; 
}


INT  NET_kbhit()
{
	CHAR	ch;
	
	if((volatile)_NetTerm.rx_head == (volatile)_NetTerm.rx_tail)
		return 0; 
	else
		return 1;
}





INT  Net_Init(INT dhcp)
{
	_NetTerm.tx_head = _NetTerm.tx_tail = 0;
	_NetTerm.rx_head = _NetTerm.rx_tail = 0;
	_NetTerm.state = TCP_DISCONNECT;
	_NetTerm.my_port = NET_TERM_PORT;
	_NetTerm.seqNum = 0x12345678;


#if USE_SIB
	memcpy((CHAR *)_HostMAC, (CHAR *)_bmSIB.HostMAC, 6);
	memcpy((CHAR *)_HostIP, (CHAR *)_bmSIB.HostIP, 4);
#else
	GetMacAddress(_HostMAC);	
#endif

	if (!_netbuf_ready)
	{
		if (NetBuf_Init() < 0)
			return -1;
		_netbuf_ready = 1;
	}

	if (_mac_inited == 0)
	{
		NIC_InitDriver();
		_mac_inited = 1;
	}
	else
       	Mac_EnableInt();

	NIC_EnableBroadcast();

	if (dhcp)
	{
		if (GetIpByDhcp() < 0)
		{
	
			NIC_ShutDown();
			return -1;
		}
	}

	return 0;
}




#if 0		/* for debugging */


#define  FALSE  0
#define  TRUE   1


#define vaStart(list, param) list = (CHAR*)((INT)&param + sizeof(param))
#define vaArg(list, type) ((type *)(list += sizeof(type)))[-1]


static void  NU_PutString(CHAR *string)
{
	while (*string != '\0')
	{
		NET_putchar(*string);
		string++;
	}
}



static void  PutRepChar(CHAR c, INT count)
{
	while (count--)
		NET_putchar(c);
}


static void  PutStringReverse(CHAR *s, INT index)
{
	while ((index--) > 0)
		NET_putchar(s[index]);
}


static void  PutNumber(INT value, INT radix, INT width, CHAR fill)
{
	CHAR    buffer[40];
	INT     bi = 0;
	UINT  uvalue;
	UINT16  digit;
	UINT16  left = FALSE;
	UINT16  negative = FALSE;

	if (fill == 0)
		fill = ' ';

	if (width < 0)
	{
		width = -width;
		left = TRUE;
	}

	if (width < 0 || width > 80)
		width = 0;

	if (radix < 0)
	{
		radix = -radix;
		if (value < 0)
		{
			negative = TRUE;
			value = -value;
		}
	}

	uvalue = value;

	do
	{
		if (radix != 16)
		{
			digit = uvalue % radix;
			uvalue = uvalue / radix;
		}
		else
		{
			digit = uvalue & 0xf;
			uvalue = uvalue >> 4;
		}
		buffer[bi] = digit + ((digit <= 9) ? '0' : ('A' - 10));
		bi++;

		if (uvalue != 0)
		{
			if ((radix == 10)
				&& ((bi == 3) || (bi == 7) || (bi == 11) | (bi == 15)))
			{
				buffer[bi++] = ',';
			}
		}
	}
	while (uvalue != 0);

	if (negative)
	{
		buffer[bi] = '-';
		bi += 1;
	}

	if (width <= bi)
		PutStringReverse(buffer, bi);
	else
	{
		width -= bi;
		if (!left)
			PutRepChar(fill, width);
		PutStringReverse(buffer, bi);
		if (left)
			PutRepChar(fill, width);
	}
}


static CHAR  *FormatItem(CHAR *f, INT a)
{
	CHAR   c;
	INT    fieldwidth = 0;
	INT    leftjust = FALSE;
	INT    radix = 0;
	CHAR   fill = ' ';

	if (*f == '0')
		fill = '0';

	while ((c = *f++) != 0)
	{
		if (c >= '0' && c <= '9')
		{
			fieldwidth = (fieldwidth * 10) + (c - '0');
		}
		else
			switch (c)
			{
				case '\000':
					return (--f);
				case '%':
					NET_putchar('%');
					return (f);
				case '-':
					leftjust = TRUE;
					break;
				case 'c':
					{
						if (leftjust)
							NET_putchar(a & 0x7f);

						if (fieldwidth > 0)
							PutRepChar(fill, fieldwidth - 1);

						if (!leftjust)
							NET_putchar(a & 0x7f);
						return (f);
					}
				case 's':
					{
						if (leftjust)
							NU_PutString((CHAR *)a);
						if (fieldwidth > strlen((CHAR *)a))
							PutRepChar(fill,
									   fieldwidth - strlen((CHAR *)a));
						if (!leftjust)
							NU_PutString((CHAR *)a);
						return (f);
					}
				case 'd':
				case 'i':
					radix = -10;
					break;
				case 'u':
					radix = 10;
					break;
				case 'x':
					radix = 16;
					break;
				case 'X':
					radix = 16;
					break;
				case 'o':
					radix = 8;
					break;
				default:
					radix = 3;
					break;      /* unknown switch! */
			}
		if (radix)
			break;
	}

	if (leftjust)
		fieldwidth = -fieldwidth;

	PutNumber(a, radix, fieldwidth, fill);

	return (f);
}



void  NU_printf(CHAR *f, ...) /* variable arguments */
{
	CHAR  *argP;

	vaStart(argP, f);       /* point at the end of the format string */
	while (*f)
	{                       /* this works because args are all ints */
		if (*f == '%')
			f = FormatItem(f + 1, vaArg(argP, INT));
		else
			NET_putchar(*f++);
	}
}


#endif