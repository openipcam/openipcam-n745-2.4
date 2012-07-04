/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 * Winbond W90N740 Boot Loader
 *
 * Module Name:	DHCP.C
 *
 * Created by : 
 ******************************************************************************/
#include "netbuf.h"
#include "net.h"
#include "dhcp.h"


extern void  UDP_Send(UCHAR *sMac, UCHAR *sIP, UINT16 sPort,
					  UCHAR *tMac, UCHAR *tIP, UINT16 tPort,
					  UCHAR *txdata, INT len);
extern UDP_PACKET  *NetTask(UINT32 wait);


extern UCHAR  _HostMAC[];
extern UCHAR	_HostIP[];


__align(4) UCHAR  _DhcpRawBuffer[1200];

static UCHAR _DhcpOptions[] = { 0x63, 0x82, 0x53, 0x63, 0x35, 0x01, DHCP_DISCOVER };


#if 0
void HexDumpBuffer(UCHAR *buf, INT size)
{
	INT  	idx, i;
	
	idx = 0;
	while (size > 0)
	{
		for (i = 0; i < 16; i++)
   			uprintf("%02x ", buf[idx + i]);
		uprintf("  ");
		for (i = 0; i < 16; i++)
		{
			if ((buf[idx + i] >= 0x20) && (buf[idx + i] < 127))
   				uprintf("%c", buf[idx + i]);
   			else
   				uprintf(".");
   			size--;
		}
		idx += 16;
		uprintf("\r\n");
	}
}
#endif



INT  GetIpByDhcp()
{
	UCHAR		*cptr;
	INT			opt_len, len, retry, offer;
	UCHAR		serverMAC[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	UCHAR		serverIP[] = { 0xff, 0xff, 0xff, 0xff };
	UINT32	tx_id = 0x8900;			/* just give a start value */
    DHCP_HDR_T  *dhcpRx, *dhcpTx;
	UDP_PACKET  *udp;

    dhcpTx = (DHCP_HDR_T *)&_DhcpRawBuffer[0]; 
    dhcpRx = (DHCP_HDR_T *)&_DhcpRawBuffer[600]; 

    memset((char *)&dhcpTx[0], 0, sizeof(DHCP_HDR_T));
    dhcpTx->op_code = BOOTP_REQUEST;
    dhcpTx->hw_type = 1;		/* Hardware type: Ethernet */
    dhcpTx->hw_len = 6;			/* Hardware address length */
    dhcpTx->hops = 5;
    dhcpTx->tx_id = tx_id++;
    dhcpTx->seconds = 0;		/* we do not use this */
    
    /* set host IP as 0.0.0.0 */

    memset((char *)&(dhcpTx->clientIP), 0, 4);
    memset((char *)_HostIP, 0, 4);
    /* give host MAC address */

    memcpy((char *)&(dhcpTx->client_hw_addr), (char *)_HostMAC, 6);
    memcpy((char *)dhcpTx->options, (char *)_DhcpOptions, sizeof(_DhcpOptions));
    
    opt_len = sizeof(_DhcpOptions);
    cptr = dhcpTx->options + opt_len;
       
    *cptr++ = 55;      			/* param request */
    *cptr++ = 4;       			/* length */
    *cptr++ = 0x01;    			/* net mask */
    *cptr++ = 0x03;    			/* gateway */
    *cptr++ = 0x06;    			/* DNS */
    *cptr++ = 0x0f;    			/* DNS domain */
    opt_len += 6;
    *cptr = 0xff;         		/* end of option area */

	uprintf("DHCP DISCOVER...\n");
    
wait_offer:        		 

    for (retry = 0; retry < 16; retry++)
    {
        if (retry)
        	uprintf("DHCP DISCOVER retry: %d\n", retry);

        UDP_Send(_HostMAC, _HostIP, CLIENT_PORT, 
        		 serverMAC, serverIP, SERVER_PORT, 
        		 (UCHAR *)dhcpTx, DHCP_OPT_OFFSET + opt_len + 1);

		udp = NetTask(0x600000); 

		if (udp == NULL)
			continue;
			
		//HexDumpBuffer((UCHAR *)udp, 256);
	
		memcpy((char *)dhcpRx, (char *)udp + sizeof(UDP_PACKET), NC2(udp->mlen));

		if ((udp->dport == NC2(CLIENT_PORT)) &&
			(dhcpRx->op_code == BOOTP_REPLY) &&
			(dhcpRx->tx_id == dhcpTx->tx_id))
			break;
			
	    NetBuf_Free((NETBUF *)udp);
    }  /* end of for */
    
	if (retry >= 5)
		return -1;				/* DHCP failed */
		
	cptr = (UCHAR *)&dhcpRx->options + 4;
	
	/* find out DHCP OFFER and copy server identifier */
	offer = 0;
    while (*cptr != 0xff) 
    {
    	len = cptr[1] + 2;
        if (*cptr == 53)  
        {
            if (cptr[2] == DHCP_OFFER)
                offer = 1;
        }
        if (*cptr == 54) 		/* add server ID */
        {
            memcpy((char *)(dhcpTx->options) + opt_len, (char *)cptr, len);
            opt_len += len;
        }
        cptr += len;
    }
    
    NetBuf_Free((NETBUF *)udp);
    
    if (offer == 0)
    {
       	uprintf("No Offer!!\n");
        goto wait_offer;             /* wrong type, read again */
    }

	cptr = 	(UCHAR *)(dhcpTx->options) + opt_len;
	*cptr++ = 50;
    *cptr++ = 4;
    memcpy((char *)cptr, (char *)&(dhcpRx->yourIP), 4);
    opt_len += 6;

    dhcpTx->options[6] = DHCP_REQUEST; 
    dhcpTx->options[opt_len] = 0xff;
    dhcpTx->tx_id = tx_id++;
    
	uprintf("DHCP REQUEST...\n");

wait_reply:
    for (retry = 0; retry < 16; retry++)    
    {
        if (retry)
        	uprintf("DHCP REQUEST retry: %d\n", retry);

        UDP_Send(_HostMAC, _HostIP, CLIENT_PORT, 
        		 serverMAC, serverIP, SERVER_PORT, 
        		 (UCHAR *)dhcpTx, DHCP_OPT_OFFSET + opt_len + 1);

		udp = NetTask(0x600000); 

		if (udp == NULL)
			continue;
			
		memcpy((char *)dhcpRx, (char *)udp + sizeof(UDP_PACKET), NC2(udp->mlen));
			
		if ((udp->dport != NC2(CLIENT_PORT)) ||
			(dhcpRx->op_code != BOOTP_REPLY))
		{
        	uprintf("DHCP not the wanted packet!\n");
			NetBuf_Free((NETBUF *)udp);
			goto wait_reply;		/* not a DHCP reply packet */
		}
                     
        if (dhcpRx->tx_id != dhcpTx->tx_id)
        {
        	uprintf("DHCP transaction ID mismatch!\n");
        	NetBuf_Free((NETBUF *)udp);
        	goto wait_reply;
        }

		cptr = (UCHAR *)&dhcpRx->options + 4;
    	
    	while (*cptr != 0xff) 
    	{
	    	len = cptr[1] + 2;
            if (cptr[0] == 53) 
            {
                if (cptr[2] == DHCP_ACK)
                    goto acked;
                if (cptr[2] == DHCP_NAK)
                {
                	 uprintf("DHCP Naked!\n");
                	 NetBuf_Free((NETBUF *)udp);
                	 return -1;
                }
            }
            cptr += len;
        }
        NetBuf_Free((NETBUF *)udp);
    }
    
	return -1;				/* DHCP failed */

acked:
	uprintf("DHCP ACKed...\n");

    memcpy((char *)_HostIP, (char *)&(dhcpRx->yourIP), 4);
    uprintf("IP Address. . . . . . . . . . . . : %d.%d.%d.%d\n", _HostIP[0], _HostIP[1], 
            	_HostIP[2], _HostIP[3]); 

	cptr = (UCHAR *)&dhcpRx->options + 4;

    while (*cptr != 0xff) 
    {
        len = cptr[1];
        switch (*cptr) 
        {
           case 1:                 /* subnet mask */
    			uprintf("Subnet Mask . . . . . . . . . . . : %d.%d.%d.%d\n", cptr[2], cptr[3], cptr[4], cptr[5]);
                break;
           case 2:                 /* time offset */
                break;
           case 3:                 /* router, take 1 */
    			uprintf("Default Gateway . . . . . . . . . : %d.%d.%d.%d\n", cptr[2], cptr[3], cptr[4], cptr[5]);
                break;
           case 4:                 /* time server */
    			uprintf("Time server: %d.%d.%d.%d\n", cptr[2], cptr[3], cptr[4], cptr[5]);
                break;
#if 0                
           case 6:                 /* DNS server, take 2 */
    			uprintf("DNS: %d.%d.%d.%d\n", cptr[2], cptr[3], cptr[4], cptr[5]);
                break;
           case 7:                 /* log server */
                break;
           case 9:                 /* LPR server */
                break;
           case 15:                /* domain name */
                break;
           case 18:                /* boot bootFile name */
                break;
           case 19:                /* IP forwarding, 0 = no */
                break;
           case 51:                /* address lease time */
                break;
           case 53:                /* message type */
                break;
           case 54:                /* server identifier */
                break;
           case 58:                /* renewal time */
                break;
           case 59:                /* rebinding time */
                break;
#endif                
    	} 
    	cptr += len + 2;
  	}  
    NetBuf_Free((NETBUF *)udp);
    return 0;
}


