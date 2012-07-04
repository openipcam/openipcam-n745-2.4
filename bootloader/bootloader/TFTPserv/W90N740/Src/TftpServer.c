/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 * Winbond W90N740 Boot Loader
 *
 * Module Name:	TFTPSERVER.C
 *
 * Created by : 
 ******************************************************************************/
#include "net.h"
#include "netbuf.h"
#include "net.h"

/* to here */


//#define TFTP_DEBUG          1

static UCHAR	_PeerIP[4], _PeerMAC[6];
static UINT16	_PeerPort, _HostPort;

extern void  NU_printf(char *f, ...);


void  UDP_Send( UCHAR *sMac, UCHAR *sIP, UINT16 sPort,
				UCHAR *tMac, UCHAR *tIP, UINT16 tPort,
				UCHAR *txdata, INT len)
{
	UDP_PACKET   *udp;
	NETBUF       *txbuf;
	UDP_PSEUDO   pseudo;
		
	if ((txbuf = NetBuf_Allocate()) == NULL)
	{
		uprintf("Fail to allocate a net buffer...\n");
		return;
	}

	txbuf->len = sizeof(UDP_PACKET) + len;
	udp = (UDP_PACKET *)txbuf->packet; 

	/*- prepare Ethernet header, 14 bytes -*/
	memcpy((char *)&txbuf->packet[sizeof(UDP_PACKET)], (char *)txdata, len);
	memcpy((char *)udp->tMAC, (char *)tMac, 6);
	memcpy((char *)udp->sMAC, (char *)sMac, 6);
	udp->protocol = NC2(PROTOCOL_IP);

	/*- prepare IP header, 20 bytes -*/
	udp->vers_hlen = 0x45;     /* fixed value, do not change it */
	udp->stype = 0;            /* no special priority */
	udp->tlen = NC2(sizeof(UDP_PACKET)-14+len);
	udp->id = NC2(_IP_packet_ID);
	_IP_packet_ID++;
	udp->ffrag = 0;
	udp->ttl = 64;
	udp->ippro = IP_PRO_UDP;
	udp->hdrchksum = 0;
	memcpy((char *)udp->srcIP, (char *)sIP, 4);
	memcpy((char *)udp->destIP, (char *)tIP, 4);
	udp->hdrchksum = ~Nchksum((UINT16 *)&udp->vers_hlen, 10);  /* 20 bytes */

	/*- prepare UDP header 8 bytes -*/
	udp->sport = NC2(sPort);
	udp->dport = NC2(tPort);
	udp->mlen = NC2(8 + len);
	udp->udpchksum = 0;
	memcpy((char *)pseudo.srcIP, (char *)sIP, 4);
	memcpy((char *)pseudo.destIP, (char *)tIP, 4);
	pseudo.protocol = NC2(IP_PRO_UDP);
	pseudo.udp_len = udp->mlen;
	udp->udpchksum = Nchksum((UINT16 *)&pseudo, 6);
	udp->udpchksum = ~(Nchksum((UINT16 *)&udp->sport, ((len+8)+1)/2));
	if (udp->udpchksum == 0)
		udp->udpchksum = 0xffff;
	NIC_SendPacket(txbuf);
}


void  TFTP_ACK(UINT16 ack_num)
{
	UCHAR  txdata[6];

#if TFTP_DEBUG
	uprintf("ACK:%d\n", ack_num);
#endif	
	txdata[0] = 0;
	txdata[1] = OPCODE_ACK;
	txdata[2] = ack_num >> 8;
	txdata[3] = ack_num & 0xFF;
	UDP_Send(_HostMAC, _HostIP, _HostPort, _PeerMAC, _PeerIP, _PeerPort, txdata, 4);
}




UDP_PACKET  *NetTask(UINT32 wait)
{
	UDP_PACKET  *udp;
	NETBUF      *rxbuf;

	while (wait-- > 0)
	{
#if TFTP_DEBUG
		;
#endif      
		if (_iqueue_first == NULL)
			continue;
	   
	   /* 
		*  Retrieve an incoming packet from "iqueue", which is mantained
		*  by _iqueue_first and _iqueue_last only.  The packet driver
		*  feeds the incoming queue and we consumes the incoming queue.
		*/
       	Mac_DisableInt();
		rxbuf = _iqueue_first;
		_iqueue_first = _iqueue_first->next;
		if (_iqueue_first == NULL)
			_iqueue_last = NULL;
       	Mac_EnableInt();

		udp = (UDP_PACKET *)rxbuf->packet;
		
		/* Check BOOTP packets */	
		if ((udp->ippro == IP_PRO_UDP) &&
			(udp->sport == NC2(67)))	/* 67 is DHCP server port */
			return udp;			
		
		/* 
		 * If this is a UDP packet and is for us, we return it here.
		 */
		if ((udp->ippro == IP_PRO_UDP) && (!CompareIP(udp->destIP, _HostIP)))
			return udp;
	   
	}   
	return NULL;    
}



INT TFTP_Download(UCHAR *filebuf, ULONG *filesize, INT dhcp)
{
	UDP_PACKET  *udp;
	UCHAR     *tftp_pkt;
	UINT16    ack_num, recv_num;
	INT         file_offset;
	INT         recv_size;
	INT         retry_cnt;

	_IP_packet_ID = 1000;   /* start from 1000 */
	_HostPort = TFTP_SERVER_PORT;
	

	/*
	 *  In the following while-loop, we expect that ARP request will be
	 *  isssued from one of the TFTP clients. We will send ARP reply
	 *  in NetTask(). After ARP process, the TFTP client will send the
	 *  first TFTP packet to us.  We will leave this while-loop if
	 *  the first TFTP packet was received.
	 */
	while (1)
	{
		udp = NetTask(0x100000); 
	   
		if (udp == NULL) continue;
	   
		if (udp->dport == NC2(TFTP_SERVER_PORT))
		{
			tftp_pkt = ((UCHAR *)udp) + sizeof(UDP_PACKET);
			if (tftp_pkt[1] == OPCODE_WRITE)
				break;
		}
		else
			NetBuf_Free((NETBUF *)udp);
	}
	   
	memcpy((char *)_PeerIP, (char *)udp->srcIP, 6);
	memcpy((char *)_PeerMAC, (char *)udp->sMAC, 6);
	_PeerPort = NC2(udp->sport);
	_HostPort = TFTP_SERVICE_PORT;   /* By TFTP Spec., it's 69 */

	uprintf("TFTP client: %d.%d.%d.%d\n", _PeerIP[0], _PeerIP[1], _PeerIP[2], _PeerIP[3]);

	NetBuf_Free((NETBUF *)udp);

	TFTP_ACK(0);        /* ACK the first packet */
	
	/* 
	 *  ACK block number start from zero. This ACK number is also used
	 *  to expect the next TFTP packet block number.
	 */
	ack_num = 0;        
	
	/* file downlaod buffer offset */
	file_offset = 0;
	
	while (1)
	{
		udp = NetTask(0x1000000);
	   
		if (udp == NULL)
		{
#if TFTP_DEBUG
			uprintf("\nTime-out, count:[%d]\n", retry_cnt);
#endif       
			if (retry_cnt++ >= MAX_RETRY)
			{
				uprintf("\nDownload failed!\n");
				return -1;
			}

			/* 
			 *  If time is out, the last ACK packet may be lost. 
			 *  By TFTP specification, we must resend the last 
			 *  ACK packet. 
			 */
			TFTP_ACK(ack_num);
			continue;
		}

		/* 
		 *  Packet received, qualify this packet
		 *  We must make sure both the source and sink IPs are correct.
		 *  And both source and sink port numbers are correct.
		 *  In addition, we must make sure that this packet is a TFTP
		 *  data packet with the expected block number.
		 */
		tftp_pkt = ((UCHAR *)udp) + sizeof(UDP_PACKET);
		if (CompareIP(udp->srcIP, _PeerIP) ||
			(udp->sport != NC2(_PeerPort)) || (udp->dport != NC2(_HostPort)) ||
			(tftp_pkt[1] != OPCODE_DATA))
		{
#if TFTP_DEBUG
			uprintf("Wrong IP or port number!\n");
#endif       
			/*
			 *  We can resend the last ACK packet to ask TFTP client
			 *  resend the expected packet.
			 */
			TFTP_ACK(ack_num);
			NetBuf_Free((NETBUF *)udp);
			continue;
		}
	   
		/* 
		 *  Expected DATA packet received 
		 *  We must check if the block number is the expected number.
		 *  If it's not the expected number, we resend the last ACK 
		 *  packet to request the next packet.
		 */
		recv_num = tftp_pkt[2]<<8 | tftp_pkt[3];
		if ( recv_num != ack_num+1 )
		{
#if TFTP_DEBUG
			uprintf("\nIncorrect block number:[%d], expect:[%d]\n", recv_num, ack_num+1);
#endif			
			NetBuf_Free((NETBUF *)udp);
			TFTP_ACK(ack_num);
			continue;
		}
		  
		recv_size = NC2(udp->mlen) - 8 - 4;   /* 8:UDP header size, 4:TFTP data packet header size */
	   
		/* copy the received data block into file buffer */
		memcpy((char *)&filebuf[file_offset], (char *)&tftp_pkt[4], recv_size);
		  
#if TFTP_DEBUG
		printf("UDP packet received, block:%d size:%d\n", recv_num, recv_size);
#endif          
		NetBuf_Free((NETBUF *)udp);

		ack_num++;                      /* proceed to the next packet */
		TFTP_ACK(ack_num);              /* ACK to requset the next packet */
		file_offset += recv_size;

		if (ack_num % 64 == 0)
		{
			uprintf("                                      \r");
			uprintf("%d bytes downloaded      \r", file_offset);
		}
	   
		/* 
		 *  According to TFTP Spec., a TFTP data block with size smaller
		 *  than 512 is the last block.
		 *  If we have received a block smaller than 512, we can terminate
		 *  the TFTP download process. OK!
		 */
		if (recv_size < 512)           
			break;           /* Finished! */
	}

	uprintf("Download OK, file size:%d\n", file_offset);

	*filesize = file_offset;
					   
	return 0;
}


#if 0
extern char  NET_getchar(void);
INT  main(INT argc, void *argv[])
{
	ULONG  fileSize;
	
	Net_Init(1);	/* with DHCP */
	
	while(1)
	{
		char ch;
		ch=NET_getchar();
		NET_putchar(ch);
		if( ch==13 )
			NET_putchar('\r');
		if( ch==13 )
			NET_putchar('\n');
		putchar(ch);
		printf("(%d)\n",ch);
	}

	//TFTP_Download((UCHAR *)0x500000, &fileSize, 0);
	
	
	//TFTP_Download((UCHAR *)0x600000, &fileSize, 0);
	//TFTP_Download((UCHAR *)0x600000, &fileSize, 0);

	return 0;
}

#endif

