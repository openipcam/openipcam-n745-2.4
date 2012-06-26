/*-------------------------------------------------------------------------------*/
/* 			Wireless driver for winbond W90N745			 						 */
/* 				version 1.0.2 (used only for Station)					 		 */
/* ------------------------------------------------------------------------------*/
#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>      
#include <linux/slab.h>		 
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/pagemap.h>
#include <linux/vmalloc.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <asm/irq.h>

#include "W90N745Prism.h"
/* tx buffer queue, you can replace it as what you designed */
#include "Queue.h"



extern char PRISM_DesireSSID_DEFAULT[];
extern char PRISM_BSSID[];
extern char PRISM_LLC[];
extern unsigned char Mac_address[6];
extern Queue_txfid TxQueueObj;
/* Debug for now */
extern UINT8 *DebugpBuffer;
//#define PRSIM_DEBUG
extern int nums;

/* -------------------------all functions--------------------------- */  

/* Here handle the tx ,rx and other events*/
void prism_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	UINT16 reg;
	struct net_device *dev = (struct net_device *)dev_id;
	struct w740prism_priv *priv;
	UINT16 txfid;
	int count = MAX_IRQLOOPS_PER_IRQ;
	static int last_irq_jiffy = 0; 			// jiffies value the last time we were called 
	static int loops_this_jiffy = 0;

	if (!dev /*paranoid*/ ) return;

    /* Lock the device */
    priv = (struct w740prism_priv *) dev->priv;

    /* read the EvStat register for interrupt enabled events */
    reg = READREG(PRISM_EVSTAT);
    	
#ifdef WDEBUG
	//printk("Event: %x\n", reg);
#endif
	if (jiffies != last_irq_jiffy)
		loops_this_jiffy = 0;
	last_irq_jiffy = jiffies;
	
	while(reg && count--) {
		if (++loops_this_jiffy > MAX_IRQLOOPS_PER_JIFFY) {
			printk("%s: IRQ handler is looping too much! Shutting down.\n",dev->name);
			WRITEREG(PRISM_INTEN, 0);
			break;
		}
		
		if(PRISM_EVSTAT_ISTICK(reg)) {
			;								//printk("TICK\n");
		}
		
		if(PRISM_EVSTAT_ISWTERR(reg)) {
#ifdef WDEBUG
			printk("WTERR\n");				//It shoudn't occur.
#endif
		}
		
		if(PRISM_EVSTAT_ISINFDROP(reg)) {
			;								//printk("INFDROP\n");
		}
		
		if(PRISM_EVSTAT_ISINFO(reg)) {
			//printk("INFO\n");
			prism_info(dev);
		}
		
		if(PRISM_EVSTAT_ISRX(reg)) {
			prism_rx(dev);
		}
		
		if(PRISM_EVSTAT_ISTXEXC(reg)) {
//			printk("txexc ");
        		prism_txexc();
		}
		
		if(PRISM_EVSTAT_ISTX(reg)) {
			prism_tx(dev);
		}
		
		if(PRISM_EVSTAT_ISALLOC(reg)) {
			txfid = READREG(PRISM_ALLOCFID);
			if(Put_txfid(txfid) < 0)
			{	
				;
			}
			else {
				netif_wake_queue(dev);
				WRITEREG(PRISM_ALLOCFID, DUMMY_FID);
			}
		}
		WRITEREG(PRISM_EVACK, reg);

		reg = READREG(PRISM_EVSTAT);
	}
}


/*------------------------------------------------------------------*/
int prism_open(struct net_device *dev)
{
	int result = 0;
    MOD_INC_USE_COUNT;
    /* change the interupt source from high-level sensitive to low-level sensitive */
    if(((struct w740prism_priv *)(dev->priv))->status < 0) {
	result = prism_initmac(dev);
	if(result >= 0)
		((struct w740prism_priv *)(dev->priv))->status = 0;	//clean it
	}
    /* 	chanel 4 of IRQ2 	*/
    if(request_irq(dev->irq, &prism_interrupt,SA_INTERRUPT,"",dev))
    {
    	printk("W740 wireless driver register irq failed\n");
    	return -EAGAIN;
    }
    	
    netif_start_queue(dev);
#ifdef WDEBUG
    printk("enable interrupt handle,:)\n");
#endif
    	
    return result;
}

int prism_release(struct net_device *dev)
{
	MOD_DEC_USE_COUNT;
	netif_stop_queue(dev);
	free_irq(dev->irq,dev);
    return 0;
}

int prism_config(struct net_device *dev, struct ifmap *map)
{
    if (dev->flags & IFF_UP) /* can't act on a running interface */
        return -EBUSY;
   	return 0;
}

/* real receive function  */
void prism_rx(struct net_device *dev)
{
	int result = 0;
    UINT16 rxfid, dataLen;
    struct sk_buff *skb;
	struct w740prism_priv *priv;
	//prism_rx_frame_t 
	
	if(!dev)
	{
		printk("Null dev in rx\n");
		return;
	}
	priv = (struct w740prism_priv *) dev->priv;

    /* Get the RxFID */
    rxfid = READREG(PRISM_RXFID);
    /* Get the data length */
    result = prism_copy_from_bap(IRQ_BAP, rxfid, 44, &dataLen, 2);
    if (result) {
    	LOGPRINT;
	      goto fail_reset;
	}

	skb = dev_alloc_skb(dataLen+12-6 + 2);
    if (!skb) {
        LOGPRINT;
        priv->stats.rx_dropped++;
        return;
    }
    	
    skb_reserve(skb, 2); 			// align IP on 16B boundary
   	skb_put(skb, dataLen + 6);
	result = prism_copy_from_bap(IRQ_BAP, rxfid, 18, skb->data, 6);
	if (result) {
		LOGPRINT;
        priv->stats.rx_dropped++;
        dev_kfree_skb(skb);
        goto fail_reset;
    }
    result = prism_copy_from_bap(IRQ_BAP, rxfid, 30, skb->data+6, 6);
	if (result) {
		LOGPRINT;
        priv->stats.rx_dropped++;
        dev_kfree_skb(skb);
        goto fail_reset;
    }

    /* Get the 802.3 body */
    result = prism_copy_from_bap(IRQ_BAP, rxfid, 66, skb->data+12, dataLen - 6);
    if (result) {
    	LOGPRINT;
       	priv->stats.rx_dropped++;
       	dev_kfree_skb(skb);
       	LOGPRINT;
       	goto fail_reset;
    }
	
    /* Write metadata, and then pass to the receive level */
    skb->dev = dev;
    skb->protocol = eth_type_trans(skb, dev);
    //skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */
    priv->stats.rx_packets++;
	priv->stats.rx_bytes += (dataLen+6);
#ifdef WIRELESS_SPY
	;//prism_wireless_stat(dev, skb, &desc);
#endif
	//DEBUG("Receive a packet\n");
    netif_rx(skb);
    return;
fail_reset:
	netif_stop_queue(dev);
	prism_reset(dev->priv);
	netif_wake_queue(dev);
	return;
}

int prism_info(struct net_device *dev)
{	
	UINT16 infoid;
	UINT16 len;
	UINT16 infotype = 0;
	UINT16 status = 0;
	struct w740prism_priv *priv = (struct w740prism_priv *)dev->priv;
	
	infoid = READREG(PRISM_INFOFID);
//	printk("infoid: %x\n", infoid);
	infotype = Read_CT_InfoType(infoid, &len);
	if(infotype == -1)
	{
		printk("Meet erro when reading info\n");
		return -1;
	}
	
	switch(infotype) 
	{
		case PRISM_INQ_TALLIES:
			//DEBUG("%s: PRISM_INQ_TALLIES: len: %d\n", dev->name, len);
			
			break;
		case PRISM_INQ_SCAN:
			DEBUG("%s: PRISM_INQ_SCAN: len:%d\n", dev->name, len);
			get_scan_result(infoid, len, priv);
			break;
		case PRISM_INQ_LINKSTATUS:
			DEBUG("%s: PRISM_INQ_LINKSTATUS: len: %d\n", dev->name, len);
			Read_CT_InfoData(infoid, &status, len);	//2 bytes
			prism_lock(priv);
			priv->status = status;
			prism_unlock(priv);
			DEBUG("%s: current link status: %x\n",dev->name, status);
			break;
		default:
			DEBUG("%s: Unkown infotype: %x\n", dev->name, infotype);
			return -1;
	}
	
	return 0;
}

#define PRSIM_DEBUG0
#define PRSIM_DEBUG1
/*
 * Transmit a packet (called by the kernel)
 */
int prism_start_tx(struct sk_buff *skb, struct net_device *dev)
{
	int result = 0;
 
 	result = send_frame(dev, skb->data, skb->len);
 	if(!result)
 	{
 		dev_kfree_skb(skb);
 		return result;
	}
    return result;
}

/* interval transmit function */
static int send_frame(struct net_device *dev ,unsigned char *data,int length)
{
	struct hermes_tx_descriptor txdesc;
    UINT16 fid;
    int result = 0;
    int retrycount = 0;
    UINT8 *pData;
    UINT32 data_off, data_len, len;
    struct ethhdr *eh;
    struct header_struct hdr;
    struct net_device_stats *stats = &(((struct w740prism_priv *) dev->priv)->stats);
    
    //DEBUG("Send a packet\n");	
    /* check stop */
    if (! netif_running(dev)) {
		printk("%s: Tx on stopped device!\n",dev->name);
		return 1;
	}
	
	/* check busy */
	if (netif_queue_stopped(dev)) {
		printk("%s: Tx while transmitter busy!\n", dev->name);
		return 1;
	}
    	
    prism_lock((struct w740prism_priv *)dev->priv);
    disable_irq(dev->irq);
    fid = Get_txfid();
    enable_irq(dev->irq);
    if((signed short)fid < 0) {
    	printk("%s: Tx on erro fid:%x\n", dev->name, fid);	
    	goto fail_reset;
    }
    /* Length of the packet body */
	/* FIXME: what if the skb is smaller than this? */
	len = max_t(int,length - ETH_HLEN, ETH_ZLEN);

	eh = (struct ethhdr *)data;

	/* Build Tx frame structure */
    /* Set up the control field */
    memset(&txdesc, 0, sizeof(txdesc));
    	
	/* set the control to 802.11 */
	txdesc.tx_control = cpu_to_le16(PRISM_TX_TXEX_SET(1) | PRISM_TX_TXOK_SET(1));
	disable_irq(dev->irq);
	result = prism_copy_to_bap(USER_BAP, fid, 0, &txdesc, sizeof(txdesc));
	enable_irq(dev->irq);
	if ( result ) {
     	printk("MAC tx copy_to_bap failed, %d\n", __LINE__);
    	if(result == -1)
    		goto fail_reset;
    	else
    		goto fail;
    }
        
	/* Encapsulate Ethernet-II frames */
	if (ntohs(eh->h_proto) > 1500) { 			// Ethernet-II frame
		data_len = len;
		data_off = HERMES_802_3_OFFSET + sizeof(hdr);
		pData = data + ETH_HLEN;	
		
		memcpy(hdr.dest, eh->h_dest, ETH_ALEN);
		memcpy(hdr.src, eh->h_source, ETH_ALEN);
		
		hdr.len = htons(data_len + ENCAPS_OVERHEAD);	
		/* 802.2 header */
		memcpy(&hdr.dsap, &PRISM_LLC, sizeof(char)*6);
		hdr.ethertype = eh->h_proto;
		disable_irq(dev->irq);
		result = prism_copy_to_bap(USER_BAP, fid, HERMES_802_3_OFFSET, &hdr, sizeof(hdr));
		enable_irq(dev->irq);
		if (result) {
			printk("%s: Error %d writing packet header to BAP\n", dev->name, result);
			if(result == -1)
        		goto fail_reset;
        	else
        		goto fail;
		}
	}
	else {
		data_len = len + ETH_HLEN;
		data_off = HERMES_802_3_OFFSET;
		pData = data;
	}
	disable_irq(dev->irq);
	result = prism_copy_to_bap(USER_BAP, fid, data_off, pData, RUP_EVEN(data_len));
	enable_irq(dev->irq);
	if ( result ) {
    	printk("MAC tx copy_to_bap failed: %d\r\n", __LINE__);
    	goto fail;
    }

    /* Finally, we actually initiate the send */
    if(GetAvailableCellNum(TxQueueObj) <= 0)
		netif_stop_queue(dev);
	retrycount = 4;
retry:
    	/* Issue Tx command */
    result = prism_cmd_transmit(1, fid);
    if (result != 0) {
  		if(--retrycount > 0)
        		goto retry;
        goto fail;
    }
    dev->trans_start = jiffies;
    prism_unlock((struct w740prism_priv *)dev->priv);
        
    return 0;
fail_reset:
	netif_stop_queue(dev);
	prism_reset(dev->priv);
	netif_wake_queue(dev);
fail:
	stats->tx_errors++;
	disable_irq(dev->irq);
    Put_txfid(fid);
    enable_irq(dev->irq);
	prism_unlock((struct w740prism_priv *)dev->priv);
	return -1;
	
}

/* tx reponse function */
int prism_tx(struct net_device *dev)
{
	UINT16 fid;
    UINT16 status;
    int result = 0;

    fid = READREG(PRISM_TXCOMPLFID);
    result = prism_copy_from_bap(IRQ_BAP, fid, 0, &status, sizeof(status));
    if (result) {
    	printk("prism_tx:copy_from_bap failed\r\n");
		return -1;
	}
	((struct w740prism_priv *)dev->priv)->stats.tx_packets++;
    return result; /* Our simple device can not fail */
}

/* tx timeout function, timeout is set in init() */
void prism_tx_timeout (struct net_device *dev)
{
	printk("tx timeout.  :(\n");
	struct w740prism_priv *priv = (struct w740prism_priv *)dev->priv;
	struct net_device_stats *stats = &priv->stats;
	int err = 0;
	
	printk("Tx timeout! Reset Device\n");
	
	stats->tx_errors++;
	err = prism_reset(priv);
	if(err)
		printk("Erro:%d on resetting Device:%s\n", err, dev->name);
	else {
		dev->trans_start = jiffies;
		netif_wake_queue(dev);
	}

}

/* device reset, nothing to say, :( */
int prism_reset(struct w740prism_priv *priv)
{
	int err = 0;
#ifdef WDEBUG
	printk("prism_reset :( \n");
#endif
	spin_lock_bh(priv->lock);
	disable_irq(priv->ndev->irq);
    err = prism_reset_device(priv->ndev);
	enable_irq(priv->ndev->irq);
	spin_unlock_bh(priv->lock);
	return err;
}

/* Device ioctl for setting wlan configuration */
int prism_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	int err = 0;
	struct iwreq *wrq = (struct iwreq *)rq;
	int changed = 0;
	struct w740prism_priv *priv = dev->priv;
	DEBUG("cmd: %x,iwreq:%d\n", cmd,sizeof(struct iwreq));
	switch (cmd) {
	case SIOCGIWNAME:
		DEBUG("%s: SIOCGIWNAME\n", dev->name);
		strcpy(wrq->u.name, "IEEE 802.11b");
		break;
		
	case SIOCGIWAP:
		DEBUG("%s: SIOCGIWAP\n", dev->name);
		wrq->u.ap_addr.sa_family = ARPHRD_ETHER;
//		printk("mac buf addr: %x\n", wrq->u.ap_addr.sa_data);
		err = prism_hw_get_bssid(dev, wrq->u.ap_addr.sa_data);
		break;

	case SIOCGIWRANGE:
		DEBUG("%s: SIOCGIWRANGE\n", dev->name);
		break;

	case SIOCSIWMODE:
		DEBUG("%s: SIOCSIWMODE\n", dev->name);
		prism_lock(priv);
		DEBUG("MOD: %x\n", wrq->u.mode);
		switch (wrq->u.mode) {
		case IW_MODE_ADHOC:
		case IW_MODE_INFRA:
			priv->iw_mode = wrq->u.mode;
			changed = 1;
			break;

		default:
			err = -EINVAL;
			break;
		}
		set_port_type(priv);
		prism_unlock(priv);
		break;

	case SIOCGIWMODE:
		DEBUG("%s: SIOCGIWMODE\n", dev->name);
		prism_lock(priv);
		wrq->u.mode = priv->iw_mode;
		prism_unlock(priv);
		break;

	case SIOCSIWENCODE:
		DEBUG("%s: SIOCSIWENCODE\n", dev->name);

		err = prism_ioctl_setiwencode(dev, &wrq->u.encoding);
		if (! err)
			changed = 1;

		break;

	case SIOCGIWENCODE:
		DEBUG("%s: SIOCGIWENCODE\n", dev->name);
		err = prism_ioctl_getiwencode(dev, &wrq->u.encoding);
		break;

	case SIOCSIWESSID:
		DEBUG("%s: SIOCSIWESSID\n", dev->name);

		err = prism_ioctl_setessid(dev, &wrq->u.essid);
		if (! err)
			changed = 1;

		break;

	case SIOCGIWESSID:
		DEBUG("%s: SIOCGIWESSID\n", dev->name);
		DEBUG("k wrq: %x\n", wrq);
		err = prism_ioctl_getessid(dev, &wrq->u.essid);

		break;

	case SIOCSIWNICKN:
		DEBUG("%s: SIOCSIWNICKN\n", dev->name);
		break;

	case SIOCGIWNICKN:
		DEBUG("%s: SIOCGIWNICKN\n", dev->name);
		break;

	case SIOCGIWFREQ:
		DEBUG("%s: SIOCGIWFREQ\n", dev->name);
		err = prism_hw_get_freq(dev->priv);
		DEBUG("%d: %d\n", __LINE__, err);
		if(err >= 0)
			wrq->u.freq.m = err;
		else
#if 1
			wrq->u.freq.m = 1;	//default to 1
#else
			wrq->u.freq.m = err;
#endif
//		printk("freq: %x, %d\n", wrq->u.freq.m, __LINE__);
		wrq->u.freq.e = 0;
		break;

	case SIOCSIWFREQ:
		DEBUG("%s: SIOCSIWFREQ\n", dev->name);
		err = prism_ioctl_setfreq(dev, &wrq->u.freq);
		if (! err)
			changed = 1;
		break;

	case SIOCGIWSENS:
		DEBUG("%s: SIOCGIWSENS\n", dev->name);
		err = prism_ioctl_getsens(dev, &wrq->u.sens);
		break;

	case SIOCSIWSENS:
		DEBUG("%s: SIOCSIWSENS\n", dev->name);
		err = prism_ioctl_setsens(dev, &wrq->u.sens);
		if(!err)
			changed = 1;
		break;

	case SIOCGIWRTS:
		DEBUG("%s: SIOCGIWRTS\n", dev->name);
		wrq->u.rts.value =((struct w740prism_priv *)(dev->priv))->rts_thresh;
		wrq->u.rts.disabled = (wrq->u.rts.value == 2347);
		wrq->u.rts.fixed = 1;
		break;

	case SIOCSIWRTS:
		DEBUG("%s: SIOCSIWRTS\n", dev->name);
		err = prism_ioctl_setrts(dev, &wrq->u.rts);
		if( !err )
			changed = 1;
		break;

	case SIOCSIWFRAG:
		DEBUG("%s: SIOCSIWFRAG\n", dev->name);
		break;

	case SIOCGIWFRAG:
		DEBUG("%s: SIOCGIWFRAG\n", dev->name);
		break;

	case SIOCSIWRATE:
		DEBUG("%s: SIOCSIWRATE\n", dev->name);
		err = prism_ioctl_setiwrate(dev, &wrq->u.bitrate);
		if (! err)
			changed = 1;
		break;

	case SIOCGIWRATE:
		DEBUG("%s: SIOCGIWRATE\n", dev->name);
		err = prism_ioctl_getiwrate(dev, &wrq->u.bitrate);
		break;

	case SIOCSIWPOWER:
		DEBUG("%s: SIOCSIWPOWER\n", dev->name);
		err = prism_ioctl_setpower(dev, &wrq->u.power);
		if(!err)
			changed = 1;
		break;

	case SIOCGIWPOWER:
		DEBUG("%s: SIOCGIWPOWER\n", dev->name);
		err = prism_ioctl_getpower(dev, &wrq->u.power);
		break;

	case SIOCGIWTXPOW:
		DEBUG("%s: SIOCGIWTXPOW\n", dev->name);
		/* The card only supports one tx power, so this is easy */
		break;

#if WIRELESS_EXT > 10
	case SIOCSIWRETRY:
		DEBUG("%s: SIOCSIWRETRY\n", dev->name);
		break;

	case SIOCGIWRETRY:
		DEBUG("%s: SIOCGIWRETRY\n", dev->name);
		break;
#endif /* WIRELESS_EXT > 10 */

	case SIOCSIWSPY:
		DEBUG("%s: SIOCSIWSPY\n", dev->name);
		err = prism_ioctl_setspy(dev, &wrq->u.data);
		break;

	case SIOCGIWSPY:
		DEBUG("%s: SIOCGIWSPY\n", dev->name);
		err = prism_ioctl_getspy(dev, &wrq->u.data);
		break;

	case SIOCGIWAPLIST:
		DEBUG("%s: SIOCGIWAPLIST\n", dev->name);
		err = prism_ioctl_getaplist(dev, &wrq->u.data);
		break;
		
	case SIOCGIWPRIV:
		DEBUG("%s: SIOCGIWPRIV\n", dev->name);
#if 0
		if (wrq->u.data.pointer) {
			struct iw_priv_args privtab[] = {
				{ SIOCIWFIRSTPRIV + 0x0, 0, 0, "force_reset" },
				{ SIOCIWFIRSTPRIV + 0x1, 0, 0, "card_reset" },
				{ SIOCIWFIRSTPRIV + 0x2,
				  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
				  0, "set_port3" },
				{ SIOCIWFIRSTPRIV + 0x3, 0,
				  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
				  "get_port3" },
				{ SIOCIWFIRSTPRIV + 0x4,
				  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
				  0, "set_preamble" },
				{ SIOCIWFIRSTPRIV + 0x5, 0,
				  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
				  "get_preamble" },
				{ SIOCIWFIRSTPRIV + 0x6,
				  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
				  0, "set_ibssport" },
				{ SIOCIWFIRSTPRIV + 0x7, 0,
				  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
				  "get_ibssport" }
			};

			err = verify_area(VERIFY_WRITE, wrq->u.data.pointer, sizeof(privtab));
			if (err)
				break;
			
			wrq->u.data.length = sizeof(privtab) / sizeof(privtab[0]);
			if (copy_to_user(wrq->u.data.pointer, privtab, sizeof(privtab)))
				err = -EFAULT;
		}
#endif
		break;
	       
	case SIOCIWFIRSTPRIV + 0x0: /* force_reset */
	case SIOCIWFIRSTPRIV + 0x1: /* card_reset */
		DEBUG("%s: SIOCIWFIRSTPRIV + 0x0 (force_reset)\n",
		      dev->name);
		netif_stop_queue(dev);
		err = prism_reset(dev->priv);
		netif_wake_queue(dev);
		break;

	case SIOCIWFIRSTPRIV + 0x2: /* set_port3 */
		DEBUG("%s: SIOCIWFIRSTPRIV + 0x2 (set_port3)\n",
		      dev->name);
		break;

	case SIOCIWFIRSTPRIV + 0x3: /* get_port3 */
		DEBUG("%s: SIOCIWFIRSTPRIV + 0x3 (get_port3)\n",
		      dev->name);
		break;

	case SIOCIWFIRSTPRIV + 0x4: /* set_preamble */
		DEBUG("%s: SIOCIWFIRSTPRIV + 0x4 (set_preamble)\n",
		      dev->name);
		break;

	case SIOCIWFIRSTPRIV + 0x5: /* get_preamble */
		DEBUG("%s: SIOCIWFIRSTPRIV + 0x5 (get_preamble)\n",
		      dev->name);

		break;
	case SIOCIWFIRSTPRIV + 0x6: /* set_ibssport */
		DEBUG("%s: SIOCIWFIRSTPRIV + 0x6 (set_ibssport)\n",
		      dev->name);
		break;

	case SIOCIWFIRSTPRIV + 0x7: /* get_ibssport */
		DEBUG("%s: SIOCIWFIRSTPRIV + 0x7 (get_ibssport)\n",
		      dev->name);
		break;

	case SIOCIWFIRSTPRIV + 0x8:
		DEBUG("%s: SIOCGIWSTATS\n",
		      dev->name);
		err = prism_ioctl_getstat(dev, wrq);
//		printk("%s: %d\n", __FILE__,__LINE__);
		break;
	case SIOCIWFIRSTPRIV + 0x9:
		DEBUG("%s: SIOCGIWLINKSTATS\n",
		      dev->name);
		err = prism_ioctl_getlinkstat(dev, &wrq->u.data);
//		printk("%s: %d\n", __FILE__,__LINE__);
		break;
	case SIOCSIWSCAN:
		DEBUG("%s: SIOCSIWSCAN\n",
		      dev->name);
		if(netif_running(dev)) {
#ifdef SCAN_ENABLE
			err = prism_ioctl_setscan(dev, &wrq->u.data);
#endif
		}
		DEBUG("addr: %x\n", wrq);
		break;
		
	case SIOCGIWSCAN:
		DEBUG("%s: SIOCGIWSCAN\n",
		      dev->name);
		DEBUG("kwrq: %x\n", wrq);
		break;
		
	
	default:
		printk("%s:No such cmd: %d\n", dev->name, cmd);
		err = -EOPNOTSUPP;
	}
	
	if (! err && changed && netif_running(dev)) {
		/* We need the xmit lock because it protects the
		   multicast list which orinoco_reset() reads */
		spin_lock_bh(&dev->xmit_lock);
		
		netif_stop_queue(dev);
		err = prism_reset(dev->priv);
		netif_wake_queue(dev);
		
		spin_unlock_bh(&dev->xmit_lock);
		if (err) {
			/* Ouch ! What are we supposed to do ? */
			printk("prism: Failed to set parameters on %s\n",
			       dev->name);
			netif_device_detach(dev);
		}
	}		
	
	
	
	
	return 0;
}

/* device status */
struct net_device_stats *prism_stats(struct net_device *dev)
{
	struct w740prism_priv *priv = (struct w740prism_priv *) dev->priv;
	prism_lock(priv);
	prism_unlock(priv);
    return &priv->stats;
}

struct iw_statistics *prism_wireless_stats(struct net_device *dev)
{
	struct w740prism_priv *priv = (struct w740prism_priv *) dev->priv;
	struct iw_statistics *wstats = &priv->wstats;
	int err = 0;
	int temp1, temp2;
	
	prism_lock(priv);
	if (priv->iw_mode == IW_MODE_ADHOC) {
		memset(&wstats->qual, 0, sizeof(wstats->qual));
		/* If a spy address is defined, we report stats of the
		 * first spy address - Jean II */
		if (SPY_NUMBER(priv)) {
			wstats->qual.qual = priv->spy_stat[0].qual;
			wstats->qual.level = priv->spy_stat[0].level;
			wstats->qual.noise = priv->spy_stat[0].noise;
			wstats->qual.updated = priv->spy_stat[0].updated;
		}
	} else {
		struct {
			u16 qual, signal, noise;
		} __attribute__ ((packed)) cq;

		Read_RID_Config(PRISM_RID_COMMSQUALITY, &cq);
	
		DEBUG("%s: Global stats = %X-%X-%X\n", dev->name,
		      cq.qual, cq.signal, cq.noise);	
		wstats->qual.qual = cq.qual;
		wstats->qual.level = cq.signal;
		wstats->qual.noise = cq.noise;
		wstats->qual.updated = 7;
		DEBUG("%s: Global stats = %X-%X-%X\n", dev->name,
		      cq.qual, cq.signal, cq.noise);
		
	}
	/* FIXME: Hmm.. seems a bit ugly, I wonder if there's a way to
	   do better - dgibson */
	prism_cmd_inquiry(PRISM_INQ_TALLIES);
               
	prism_unlock(priv);

	if (err)
		return NULL;
		
	return wstats;
}

static void prism_set_multicast_list(struct net_device *dev)
{
}

/* it shouldn't be useful, setting mac address from inside */
static int prism_set_mac_address(struct net_device *dev, void *addr)
{
#if 0
	struct w740_priv * priv=(struct w740_priv *)dev->priv;

	if(netif_running(dev))
		return -EBUSY;
	memcpy(&priv->mac_address[0],addr+2,ETH_ALEN);
	
	memcpy(dev->dev_addr,priv->mac_address,ETH_ALEN);
	if(priv->which)
		memcpy(w740_mac_address1,dev->dev_addr,ETH_ALEN);
	else
		memcpy(w740_mac_address0,dev->dev_addr,ETH_ALEN);
		
	TRACE_ERROR("\nSet MaC Address %u:%u:%u:%u:%u:%u\n",
			dev->dev_addr[0],\
			dev->dev_addr[1],\
			dev->dev_addr[2],\
			dev->dev_addr[3],\
			dev->dev_addr[4],\
			dev->dev_addr[5]);
	
	//w740_WriteReg(CAMEN,w740_ReadReg(CAMEN,priv->which) & ~1,priv->which);
	
	return 0;
#endif
	return 0;
}

/* Driver init() */
int prism_init(struct net_device *dev)
{
	int result = 0;

    	//ether_setup(dev); /* assign some of the fields *///??
	dev->open            = prism_open;
    dev->stop            = prism_release;
    dev->set_config      = prism_config;
    dev->hard_start_xmit = prism_start_tx;
    dev->do_ioctl        = prism_ioctl;
    dev->get_stats       = prism_stats;
    dev->get_wireless_stats = prism_wireless_stats;
    //dev->change_mtu      = snull_change_mtu;  
    //dev->rebuild_header  = snull_rebuild_header;
    //dev->hard_header     = snull_header;
	dev->irq = 4;		//:)
    dev->tx_timeout     = prism_tx_timeout;
    dev->watchdog_timeo = TX_TIMEOUT;

	/* keep the default flags, just add NOARP */
    	//dev->flags           |= IFF_NOARP;
  	dev->hard_header_cache = NULL;      /* Disable caching */
    SET_MODULE_OWNER(dev);

    /*
    * Then, allocate the priv field. This encloses the statistics
    * and a few private fields.
    */
    dev->priv = kmalloc(sizeof(struct w740prism_priv), GFP_KERNEL);
    if (dev->priv == NULL)
	    return -ENOMEM;
    memset(dev->priv, 0, sizeof(struct w740prism_priv));
    	
    result = prism_initmac(dev);
    if(result<0)
    	return result;
    memcpy(dev->dev_addr, ((struct w740prism_priv *) dev->priv)->mac_address, ETH_ALEN);
    ((struct w740prism_priv *)(dev->priv))->ndev = dev;
    	
    spin_lock_init(& ((struct w740prism_priv *) dev->priv)->lock);
    init_MUTEX_LOCKED(&((struct w740prism_priv *) dev->priv)->sema);
    
    /* debug data */
    DebugpBuffer = kmalloc(sizeof(UINT8)*6, GFP_KERNEL);
    if (DebugpBuffer == NULL)
	     return -ENOMEM;
	        
	if(result < 0) /* init fail, and need try again */
	{
		((struct w740prism_priv *)(dev->priv))->status = -1;	
	}
	
	memcpy(((struct w740prism_priv *)(dev->priv))->desired_essid, "PC32WebCam", strlen("PC32WebCam")+1);
	ether_setup(dev); /* assign some of the fields */
	
//	printk("address length: %d\n", dev->addr_len);
	
    return (result>=0?0:-1);
}

/* Our device struct */
struct net_device prism_dev = 
{
    name: "wlan0",
    init: prism_init,  	/* init function */
};

/* module init call function */
int prism_init_module(void)
{	
	int result = 0;
	printk("Welcome wireless network! :)\n");
    if ( (result = register_netdev(&prism_dev)) ) {
     	printk("prism: error %i registering device \"%s\"\n",
                   	result, prism_dev.name);
     	return -ENODEV;   
    }
#ifndef PRSIM_DEBUG
    EXPORT_NO_SYMBOLS;
#endif

    return 0;
}

/* module cleanup function, our don't clean up in our system now */
void prism_cleanup(void)
{
    kfree(prism_dev.priv);
    unregister_netdev(&prism_dev);
   	return;
}

module_init(prism_init_module);
module_exit(prism_cleanup);
