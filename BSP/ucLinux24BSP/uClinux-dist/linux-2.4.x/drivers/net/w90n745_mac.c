/*
 * linux/deriver/net/w90n745_mac.c
 * Ethernet driver for winbond W90N745 ( PC34 Lsshi )
*/

#include <linux/config.h>
#include <linux/module.h>

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>      
#include <linux/slab.h>		 
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>

#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <asm/semaphore.h>
#include <asm/irq.h>
#include "w90n745_mac.h"
#include <asm/arch/flash.h>


#define HAVE_PHY
#define RX_TIMEOUT    1
//#define IC_PLUS
//#define TEST_REST
#undef DEBUG
//#define DEBUG
#define TRACE_ERROR printk
#ifdef	DEBUG
	#define TRACE(str, args...)	printk("W90N745 eth: " str, ## args)
	#define MAC_ASSERT(x)							 \
	do {									 \
		if (!(x))							 \
			printk("ASSERT: %s:%i(%s)\n",				 \
			       __FILE__, __LINE__, __FUNCTION__);		 \
	} while(0);
#else
	#define MAC_ASSERT(x)
	#define TRACE(str, args...)
#endif

/* Global variables  used for MAC driver */
static unsigned long  	gMCMDR = MCMDR_SPCRC | MCMDR_EnMDC | MCMDR_ACP ;//|MCMDR_LBK;

static unsigned long  	gMIEN = EnTXINTR | EnRXINTR | EnRXGD | EnTXCP |
                        EnTxBErr | EnRxBErr | EnTXABT;//| EnTXEMP;//EnDEN
                   

#define RX_DESC_SIZE (3*10)
#define TX_DESC_SIZE (10)
#define CHECK_SIZE
#define PACKET_BUFFER_SIZE 1600
#define PACKET_SIZE   1560
#define TX_TIMEOUT  (50)

#define AUTO_SENSE

struct  n745_priv
{
	struct net_device_stats stats;	
	unsigned long which;
	unsigned long rx_mode;
	volatile unsigned long cur_tx_entry;
	volatile unsigned long cur_rx_entry;
	volatile unsigned long is_rx_all;
//Test	
	unsigned long bInit;
	unsigned long rx_packets;
	unsigned long rx_bytes;
	unsigned long start_time;
#ifdef AUTO_SENSE
	struct 	timer_list timer0; // detect plug/unplug 
	struct 	timer_list timer1; // check auto negotiation result
	char		plugout;
#endif	
	volatile unsigned long tx_ptr;
	unsigned long tx_finish_ptr;
	volatile unsigned long rx_ptr;
	
	unsigned long start_tx_ptr;
	unsigned long start_tx_buf;
	
	//char aa[100*100];
	unsigned long mcmdr;
	volatile unsigned long start_rx_ptr;
	volatile unsigned long start_rx_buf;
	char 		  mac_address[ETH_ALEN];
	volatile  	  RXBD   rx_desc[RX_DESC_SIZE]  __attribute__ ((aligned (16)));
	volatile      TXBD   tx_desc[TX_DESC_SIZE]	__attribute__ ((aligned (16)));
	volatile char rx_buf[RX_DESC_SIZE][PACKET_BUFFER_SIZE]	__attribute__ ((aligned (16)));
	volatile char tx_buf[TX_DESC_SIZE][PACKET_BUFFER_SIZE]	__attribute__ ((aligned (16)));
};
 
char n745_mac_address0[ETH_ALEN]={0x00,0x02,0xac,0x55,0x88,0xa1};

static void init_rxtx_rings(struct net_device *dev);
void notify_hit(struct net_device *dev ,RXBD *rxbd);	
int send_frame(struct net_device * ,unsigned char *,int);
void ResetMACRx(struct net_device * dev);
void output_register_context(int );
static int  n745_init(struct net_device *dev);	
static void   netdev_rx(struct net_device *dev);
static void rx_interrupt(int irq, void *dev_id, struct pt_regs * regs);
static void tx_interrupt(int irq, void *dev_id, struct pt_regs * regs);
int prossess_nata(struct net_device *dev,RXBD * rxbd );	
void ResetTxRing(struct n745_priv * n745_priv);
int ResetMAC0(struct net_device * dev);
int ResetMAC1(struct net_device * dev);
void ResetMAC(struct net_device * dev);
void ResetRxRing(struct n745_priv * n745_priv);
int  MiiStationWrite(int num,unsigned long PhyInAddr,unsigned long PhyAddr,unsigned long PhyWrData);
unsigned long MiiStationRead(int num, unsigned long PhyInAddr, unsigned long PhyAddr);

static int resetPhyOk = 0;
static int timer_num = 0;

volatile struct net_device n745_netdevice[1]=
{
	{init:n745_init}
	//{init:n745_init}
};

void Dump_Register()
{
	
	printk("RXFSM:%d\n",DWORD_READ(RXFSM));
	printk("TXFSM:%d\n",DWORD_READ(TXFSM));
	printk("FSM0:%d\n",DWORD_READ(FSM0));
	printk("FSM1:%d\n",DWORD_READ(FSM1));

}


void n745_WriteCam(int which,int x, unsigned char *pval)
{
	
	unsigned int msw,lsw;
	
 	msw =  	(pval[0] << 24) |
        	(pval[1] << 16) |
        	(pval[2] << 8) |
         	pval[3];

 	lsw = (pval[4] << 24) |
           (pval[5] << 16);
    
 	n745_WriteCam0(which,0,lsw,msw);
 	
 
}

void ResetP(int num)
{
	MiiStationWrite(num,PHY_CNTL_REG,0x0100,RESET_PHY);
	MiiStationWrite(num, 20, PHYAD, MiiStationRead(num, 20, PHYAD) | 2); // Set to RMII 1.0 mode 
}	

int  ResetPhyChip(int num)
{
#ifdef HAVE_PHY
 	unsigned long RdValue;
 	int which=num;
 	volatile int loop=1000*100;

 	//MiiStationWrite(which, PHY_ANA_REG, PHYAD, DR10_TX_HALF|IEEE_802_3_CSMA_CD);
 	
	TRACE_ERROR("\nWait for auto-negotiation complete...");

	if(MiiStationWrite(which, PHY_CNTL_REG, PHYAD, ENABLE_AN | RESTART_AN)==1)
 	{
 		
 		return 1;
 	}	
   
   
 	while (1) 	/* wait for auto-negotiation complete */
   	{
    	
    	RdValue = MiiStationRead(which, PHY_STATUS_REG, PHYAD) ;

    	if(RdValue==(unsigned long)1)
    	{
    		 printk("ResetPhyChip failed 1\n");
    		  return 1;
    	}

    	if ((RdValue & AN_COMPLETE) != 0 && (RdValue & 4)) // Auto-nego. complete and link valid
    	{
      		break;
      	}
      	loop--;
      	if(loop==0)
      	{
      		 return 1;
      	}	 
   	}
   	



	TRACE_ERROR("OK\n");
	resetPhyOk = 1;
 	/* read the result of auto-negotiation */
 	RdValue = MiiStationRead(which, PHY_ANLPA_REG, PHYAD) ;
 	if ((RdValue & 0x100)!=0) 	/* 100MB */
   	{
    	TRACE_ERROR("100MB - FULL\n");
      	n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)|MCMDR_OPMOD|MCMDR_FDUP,which);
   	}
  	else if ((RdValue & 0x80)!=0)
   	{
	  n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)&~MCMDR_FDUP,which);
      	n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)|MCMDR_OPMOD,which);
      	TRACE_ERROR("100MB - HALF\n");	
   	}
 	else if ((RdValue & 0x40)!=0)	/* Full Duplex */
   	{
    	TRACE_ERROR("10MB - FULL\n");
	n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)&~MCMDR_OPMOD,which);
    	n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)|MCMDR_FDUP,which);	
   	}
  	else 	
   	{ 
    	TRACE_ERROR("10MB - HALF\n");
	n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)&~MCMDR_OPMOD,which);
    	n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)&~MCMDR_FDUP,which);
   	}
   	return 0;
#endif


#ifdef IC_PLUS
{
    unsigned long RdValue,i;
 // if (!skip_reset)   
 
	static int reset_phy=0;
MiiStationWrite(num, PHY_ANA_REG, PHYAD, DR100_TX_FULL|DR100_TX_HALF|\
		      DR10_TX_FULL|DR10_TX_HALF|IEEE_802_3_CSMA_CD);
		      
MiiStationWrite(num, PHY_CNTL_REG, PHYAD, ENABLE_AN | RESET_PHY|RESTART_AN);
    
    
    //cbhuang num
    MiiStationWrite(num, 0x16, PHYAD, 0x8420);
    RdValue = MiiStationRead(num, 0x12, PHYAD);	  
    
    MiiStationWrite(num, 0x12, PHYAD, RdValue | 0x80); // enable MII registers


      
  if(num == 1) {    
    for(i=0;i<3;i++)
	   {
		 RdValue = MiiStationRead(num, PHY_STATUS_REG, PHYAD) ;
		 if ((RdValue & AN_COMPLETE) != 0)
		  {
		 	printk("come  cbhuang %s   %s  %d  \n",__FILE__,__FUNCTION__,__LINE__);
			 break;
		  }	 
		}
    if(i==3)
      {
		printk("come  cbhuang %s   %s  %d  \n",__FILE__,__FUNCTION__,__LINE__);
		n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,1)| MCMDR_OPMOD,1);
		n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,1)| MCMDR_FDUP,1);		
		return 0;
	  }
     } 
	  
	  {
	    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,num)|MCMDR_OPMOD,num);	
		n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,num)|MCMDR_FDUP,num);		  
	  }
	  return 0;
 	
 }
#endif
}

void ResetMAC(struct net_device * dev)
{
	struct n745_priv * priv=(struct n745_priv *)dev->priv;
	int    which=priv->which ;
	unsigned long val=n745_ReadReg(MCMDR,which);
	unsigned long flags;
	
	save_flags(flags); cli();
	n745_WriteReg(FIFOTHD,0x10000,which); //0x10100
	n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)&~(MCMDR_TXON|MCMDR_RXON),which);
	n745_WriteReg(FIFOTHD,0x100300,which); //0x10100
	n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)|SWR,which); //lsshi add 2005-4-22 13:05 Software reset
	
	//printk("Reset MAC:%x\n",(unsigned int)&flags);
	//printk("Reset MAC MCMDR:%x\n",n745_ReadReg(MCMDR,which));

	if(!netif_queue_stopped(dev))
	{
		netif_stop_queue(dev);
		//printk("Reset MAC stop queue\n");
	}

   	init_rxtx_rings(dev);
   	dev->trans_start=jiffies;
   	priv->cur_tx_entry=0;
    priv->cur_rx_entry=0;
	priv->rx_ptr=priv->start_rx_ptr ;
	priv->tx_ptr=priv->start_tx_ptr ;
  	
  		//11-21
  
	priv->tx_finish_ptr=priv->tx_ptr;  	
	
	n745_WriteReg(RXDLSA,priv->start_rx_ptr,which);
	n745_WriteReg(TXDLSA,priv->start_tx_ptr,which);
	n745_WriteReg(DMARFC,PACKET_SIZE,which);
  	
	n745_WriteCam(priv->which,0,dev->dev_addr);
	
	n745_WriteReg(CAMEN,n745_ReadReg(CAMEN,priv->which) | 1,priv->which);
    
  	n745_WriteReg(CAMCMR,CAMCMR_ECMP|CAMCMR_ABP|CAMCMR_AMP,which);

    /* Configure the MAC control registers. */
    n745_WriteReg(MIEN,gMIEN,which);    	
	//n745_WriteReg(MCMDR,priv->mcmdr,priv->which);
	if(which==0)
	{
   		Enable_Int(INT_EMCTXINT0);
   		Enable_Int(INT_EMCRXINT0);
   	}
   
  	
    {
    	
    	n745_WriteReg(MCMDR,MCMDR_TXON|MCMDR_RXON|val,which);
    	n745_WriteReg(TSDR ,0,which);
    	n745_WriteReg(RSDR ,0,which);
    }
  
    n745_WriteReg(MISTA,n745_ReadReg(MISTA,which),which); //clear interrupt

    //printk("reset\n");
    restore_flags(flags);
    //up(&priv->locksend);
    
    dev->trans_start = jiffies;
	if(netif_queue_stopped(dev))
	{
		netif_wake_queue(dev);
	}
}
/************************************************************************
* FUNCTION                                                             
*        MiiStationWrite
*                                                                      
* DESCRIPTION                                                          
*        Write to the Mii Station Control register.                             
*                                                                      
* INPUTS                                                               
*        int      num         which MAC of W90N745
*        unsigned long   PhyInAddr   PHY register address
*        unsigned long   PhyAddr     Address to write to.
*        unsigned long   PhyWrData   Data to write.
*                                                                      
* OUTPUTS                                                              
*        None.                                    
*************************************************************************/

int  MiiStationWrite(int num,unsigned long PhyInAddr,unsigned long PhyAddr,unsigned long PhyWrData)
{
 	volatile int i = 1000;
 	int which=num;
    volatile int loop=1000*100;
#ifdef IC_PLUS1
	num = 0;
#endif	
	which=num;
   n745_WriteReg(MIID,PhyWrData,which);
   n745_WriteReg(MIIDA,PhyInAddr|PhyAddr|PHYBUSY|PHYWR|MDCCR1,which);
   while(i--);
   while((n745_ReadReg(MIIDA,which) &PHYBUSY))
   {
   		loop--;
   		if(loop==0)
   			return 1;
   }
   //printk("MiiStationWrite 1\n");
   return 0;
}


/************************************************************************
* FUNCTION                                                             
*        MiiStationRead
*                                                                      
* DESCRIPTION                                                          
*        Read from the Mii Station control register.                             
*                                                                      
* INPUTS                                                               
*        int      num         which MAC of W90N745
*        unsigned long   PhyInAddr   PHY register address.
*        unsigned long   PhyAddr     Address to read from.
*                                                                      
* OUTPUTS                                                              
*        unsigned long   Data read.
*************************************************************************/
unsigned long MiiStationRead(int num, unsigned long PhyInAddr, unsigned long PhyAddr)
{
 	unsigned long PhyRdData ;
 	int which=num;
 	volatile int loop=1000*100;

#ifdef  IC_PLUS1
	num = 0;
#endif	
	which=num;
    #define MDCCR1   0x00a00000  // MDC clock rating
    n745_WriteReg(MIIDA, PhyInAddr | PhyAddr | PHYBUSY | MDCCR1,which);
    while( (n745_ReadReg(MIIDA,which)& PHYBUSY) ) 
    {
    	loop--;
    	if(loop==0)
     		return (unsigned long)1;
    }
    
    PhyRdData = n745_ReadReg(MIID,which) ; 
 	return PhyRdData ;
}

/************************************************************************
* FUNCTION                                                             
*        n745_set_mac_address
*                                                                      
* DESCRIPTION                                                          
*        Set MAC Address For Device By Writing CAM Entry 0,  
*                                                                      
* INPUTS                                                               
*       dev :The MAC which address require to modified
*		addr:New Address 
*                                                                      
* OUTPUTS                                                              
*		Always sucess    
*************************************************************************/ 
static int n745_set_mac_address(struct net_device *dev, void *addr)
{ 
	struct n745_priv * priv=(struct n745_priv *)dev->priv;

	if(netif_running(dev))
		return -EBUSY;
	memcpy(&priv->mac_address[0],addr+2,ETH_ALEN);
	
	memcpy(dev->dev_addr,priv->mac_address,ETH_ALEN);

	memcpy(n745_mac_address0,dev->dev_addr,ETH_ALEN);
		
	TRACE_ERROR("\nSet MaC Address %u:%u:%u:%u:%u:%u\n",
			dev->dev_addr[0],\
			dev->dev_addr[1],\
			dev->dev_addr[2],\
			dev->dev_addr[3],\
			dev->dev_addr[4],\
			dev->dev_addr[5]);
	
	//n745_WriteReg(CAMEN,n745_ReadReg(CAMEN,priv->which) & ~1,priv->which);
	
	return 0;
}

/************************************************************************
* FUNCTION                                                             
*        init_rxtx_rings
*                                                                      
* DESCRIPTION                                                          
*		Initialize the Tx ring and Rx ring.
*                                                                      
* INPUTS                                                               
*       dev :Which Ring is initialized including Tx and Rx Ring.
*                                                                      
* OUTPUTS                                                              
*		None
*************************************************************************/ 
static void init_rxtx_rings(struct net_device *dev)
{
	int i;
	struct n745_priv * n745_priv=dev->priv;
	
	n745_priv->start_tx_ptr =(unsigned long)&n745_priv->tx_desc[0]|NON_CACHE_FLAG;	
	n745_priv->start_tx_buf =(unsigned long)&n745_priv->tx_buf[0] | NON_CACHE_FLAG;
	
	n745_priv->start_rx_ptr =(unsigned long)&n745_priv->rx_desc[0]|NON_CACHE_FLAG;
	n745_priv->start_rx_buf =(unsigned long)&n745_priv->rx_buf[0] | NON_CACHE_FLAG;
	
	
	//Tx Ring
	MAC_ASSERT(n745_priv->start_tx_ptr );
	MAC_ASSERT(n745_priv->start_tx_buf );
	TRACE(" tx which %d start_tx_ptr %x\n",n745_priv->which,n745_priv->start_tx_ptr);

	for ( i = 0 ; i < TX_DESC_SIZE ; i++ )
	{
		//n745_priv->tx_desc[i]=0;
	   	n745_priv->tx_desc[i].SL=0;
	   	n745_priv->tx_desc[i].mode=0;
		n745_priv->tx_desc[i].buffer=(unsigned long)&n745_priv->tx_buf[i]|NON_CACHE_FLAG;	
		n745_priv->tx_desc[i].next=(unsigned long)&n745_priv->tx_desc[i+1]|NON_CACHE_FLAG;	
	   	TRACE(" *tx cur %d desc %x buffer  %x", i,  &n745_priv->tx_desc[i],n745_priv->tx_desc[i].buffer);
	   	TRACE("  next %x\n",n745_priv->tx_desc[i].next);	
	}
	n745_priv->tx_desc[i-1].next=(unsigned long)&n745_priv->tx_desc[0]|NON_CACHE_FLAG;	
	TRACE(" * cur %d desc %x buffer  %x", i-1,  &n745_priv->tx_desc[i-1],n745_priv->tx_desc[i-1].buffer);
	TRACE("  next %x\n",n745_priv->tx_desc[i-1].next);
	
	//Rx Ring
	MAC_ASSERT(n745_priv->start_rx_ptr );
	MAC_ASSERT(n745_priv->start_rx_buf );	
	TRACE(" tx which %d start_rx_ptr %x\n",n745_priv->which,n745_priv->start_rx_ptr);	   
    
    for( i =0 ; i < RX_DESC_SIZE ; i++)
    {    
	    n745_priv->rx_desc[i].SL=RXfOwnership_DMA;
		  
		n745_priv->rx_desc[i].buffer=(unsigned long)&n745_priv->rx_buf[i]|NON_CACHE_FLAG;	
		n745_priv->rx_desc[i].next=(unsigned long)&n745_priv->rx_desc[i+1]|NON_CACHE_FLAG;
		   
		TRACE(" # rx which %d,desc %d desc-addr  %x", n745_priv->which,i, &n745_priv->rx_desc[i]);
		TRACE("  next %x\n",n745_priv->rx_desc[i].next);
	}	
	n745_priv->rx_desc[i-1].next=(unsigned long)&n745_priv->rx_desc[0]|NON_CACHE_FLAG;	
    	
}
#ifdef AUTO_SENSE

#define MAX_AN_CHECK    5
static int an_check;

static void w745_autodetect(unsigned long arg)
{
    struct net_device * dev =(struct net_device *)arg;
    struct n745_priv *  priv=(struct n745_priv *)dev->priv;
    int which=priv->which;
    //unsigned int rxfsm=w740_ReadReg(RXFSM,priv->which);
    //unsigned  long status=w740_ReadReg(MISTA,priv->which);
    unsigned int RdValue;

    RdValue = MiiStationRead(which, PHY_STATUS_REG, PHYAD) ;
		if((RdValue&0x20)==0)
		{
			if(!priv->plugout)
			{
				printk("MAC Line-off...\n");
				*(unsigned int volatile *)(0xfff83020) = 0x50000;
				resetPhyOk = 0;
				priv->plugout=1;
			}	
			
		}
		else
		{
			if(priv->plugout)
			{
				printk("MAC Line-on...\n");
				ResetMAC(dev);
				if(MiiStationWrite(0, PHY_CNTL_REG, PHYAD, ENABLE_AN | RESTART_AN) != 1) {
                    priv->timer1.expires = jiffies + (RX_TIMEOUT) * HZ; // Check every seconds for five times
                    an_check = 1;
                    add_timer(&priv->timer1);
                    timer_num = 1;
                    return;
                }					
			} 
			
		}
	
    priv->timer0.expires = jiffies +RX_TIMEOUT*HZ;
    add_timer(&priv->timer0);
 
}

static void check_an_result(unsigned long arg)
{
    struct net_device * dev =(struct net_device *)arg;
    struct n745_priv *  priv=(struct n745_priv *)dev->priv;
    unsigned int RdValue;

    RdValue = MiiStationRead(0, PHY_STATUS_REG, PHYAD) ;
    
    if(RdValue !=(unsigned long)1) {
        // Needs to read second time to let Davicom 9161 PHYs set the link status
        RdValue = MiiStationRead(0, PHY_STATUS_REG, PHYAD) ;
        if ((RdValue & AN_COMPLETE) != 0 && (RdValue & 4)) // Auto-nego. complete and link valid
        {

	        RdValue = MiiStationRead(0, PHY_ANLPA_REG, PHYAD) ;

        	if ((RdValue & 0x100)!=0) /* 100MB */
        	  {
        	    TRACE_ERROR("100MB - FULL\n");
        	    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,0)|MCMDR_OPMOD|MCMDR_FDUP,0);
        	  }
        	else if ((RdValue & 0x80)!=0)
        	  {
        	    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,0)&~MCMDR_FDUP,0);
        	    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,0)|MCMDR_OPMOD,0);
        	    TRACE_ERROR("100MB - HALF\n");
        	  }
        	else if ((RdValue & 0x40)!=0)/* Full Duplex */
        	  {
        	    TRACE_ERROR("10MB - FULL\n");
        	    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,0)&~MCMDR_OPMOD,0);
        	    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,0)|MCMDR_FDUP,0);
        	  }
        	else 
        	  { 
        	    TRACE_ERROR("10MB - HALF\n");
        	    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,0)&~MCMDR_OPMOD,0);
        	    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,0)&~MCMDR_FDUP,0);
        	  }  
        	 resetPhyOk = 1;
        	 *(unsigned int volatile *)(0xfff83020) = 0x55555;
        	 priv->plugout=0;
        	           	   
        } else if(an_check < MAX_AN_CHECK) {
            priv->timer1.expires = jiffies + (RX_TIMEOUT * 2) * HZ;
            an_check++;
            add_timer(&priv->timer1);    
            return;        
        }    
    }
	  		
	// ok. check state finished. keep monitoring for plug/unplug
    priv->timer0.expires = jiffies +RX_TIMEOUT*HZ;
    add_timer(&priv->timer0);
    timer_num = 0;
 
}

#endif
/************************************************************************
* FUNCTION                                                             
*       n745_open
*                                                                      
* DESCRIPTION                                                          
*		Set Register ,Register ISR ,The MAC began to Receive Package.
*                                                                      
* INPUTS                                                               
*       dev :Pointer to MAC That is Opened.
*                                                                      
* OUTPUTS                                                              
*		Sucess if Return 0		
*************************************************************************/ 
static int   n745_open(struct net_device *dev)
{
  	struct n745_priv * priv;
  	int    which ;

  	priv=(struct n745_priv *)dev->priv;
  	which= priv->which; 
  	  	
  	init_rxtx_rings(dev);
		ResetMAC(dev);  	
  	priv->rx_ptr=priv->start_rx_ptr ;
  	priv->tx_ptr=priv->start_tx_ptr ;
  	
  	n745_WriteReg(FIFOTHD,0x10000,which); //0x10100
  	n745_WriteReg(FIFOTHD,0x100300,which); //0x10100
  	n745_WriteReg(RXDLSA,priv->start_rx_ptr,which);
  	n745_WriteReg(TXDLSA,priv->start_tx_ptr,which);
  	n745_WriteReg(DMARFC,2000,which);
  	
  	n745_WriteCam(priv->which,0,dev->dev_addr);
		n745_WriteReg(CAMEN,n745_ReadReg(CAMEN,priv->which) | 1,priv->which);
  
  	n745_WriteReg(CAMCMR,CAMCMR_ECMP|CAMCMR_ABP|CAMCMR_AMP,which);
  	//n745_WriteReg(CAMCMR,CAMCMR_ECMP|CAMCMR_ABP|CAMCMR_AMP|CAMCMR_AUP,which);	
  	
  	n745_WriteReg(MCMDR,1<<19,which);
	
	*(unsigned int volatile *)(0xfff83020) = 0x50000;
	*(unsigned int volatile *)(0xfff83024) = 0; // Make sure all pins are in input mode.
    ResetP(which);
    if(ResetPhyChip(which)==1)
    {	
    	TRACE_ERROR("ResetPhyChip Failed\n");
			priv->plugout=1;
    	/*return -1;*/ // yachen
    }
    else
    	priv->plugout=0;

  	
    //number interrupt  number     
    TRACE("**** which %d \n", which);
    
    /* Configure the MAC control registers. */
    n745_WriteReg(MIEN,gMIEN,which);
    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)|gMCMDR,which);
    n745_WriteReg(MCMDR,n745_ReadReg(MCMDR,which)|MCMDR_RXON,which);
   	
    priv->mcmdr=n745_ReadReg(MCMDR,which);
    priv->bInit=1; 
    priv->rx_packets=0;
	priv->rx_bytes=0;
	priv->start_time=jiffies;
	
#ifdef AUTO_SENSE

    init_timer(&priv->timer0);

	priv->timer0.expires =jiffies+RX_TIMEOUT*HZ;
	priv->timer0.data = (unsigned long) dev;
	priv->timer0.function = w745_autodetect;	
	add_timer(&priv->timer0);
    timer_num = 0;
	
	init_timer(&priv->timer1);
	priv->timer1.data = (unsigned long) dev;
	priv->timer1.function = check_an_result;	

#endif
	
   if(which==0){
   	/* Tx interrupt vector setup. */
    	AIC_SCR_EMCTX0 = 0x41;
    	/* Rx interrupt vector setup. */
    	AIC_SCR_EMCRX0 = 0x41;
    /* Enable MAC Tx and Rx interrupt. */
    //	Enable_Int(INT_EMCTXINT0);
    //	Enable_Int(EMCRXINT0);
    	/* set MAC0 as LAN port */
    	//MCMDR_0 |= MCMDR_LAN ;
    }
   
    
	if(request_irq(INT_EMCTXINT0+which,&tx_interrupt,SA_INTERRUPT,"",dev))
    {
    	TRACE_ERROR(KERN_ERR "W90N745 : register irq tx failed\n");
    	return -EAGAIN;
    }
    
     //compute    interrupt number   
    if(request_irq(INT_EMCRXINT0+which,&rx_interrupt,SA_INTERRUPT,"",dev))
    {
    	TRACE_ERROR(KERN_ERR "W90N745 : register irq rx failed\n");
    	return -EAGAIN;
    }
    netif_start_queue(dev);
    n745_WriteReg(RSDR ,0,which);

    if(resetPhyOk == 1)
      *(unsigned int volatile *)(0xfff83020) = 0x55555;

    TRACE("%s is OPENED\n",dev->name); 
    return 0;
}
 

static int   n745_close(struct net_device *dev)
{
	struct n745_priv *priv=(struct n745_priv *)dev->priv;
	int which=priv->which;
	
	priv->bInit=0; 
	
#ifdef AUTO_SENSE		
    // there's only one active tiemr at a time. recorded in timer_num.
    if(timer_num == 0)
	    del_timer(&priv->timer0);
	else    
	    del_timer(&priv->timer1);	
#endif
	    
	netif_stop_queue(dev);
	n745_WriteReg(MCMDR,0,which);
	free_irq(INT_EMCTXINT0+which,dev);
	free_irq(INT_EMCRXINT0+which,dev);
	
	TRACE_ERROR(KERN_ERR "W90N745 : n745_close\n");

	return 0;
}

/* Get the current statistics.	This may be called with the card open or
   closed. */
 
static struct net_device_stats * n745_get_stats(struct net_device *dev)    
{
	struct n745_priv *priv = (struct n745_priv *)dev->priv;
	
	return &priv->stats;
}

static void n745_timeout(struct net_device *dev)
{
    struct n745_priv * priv=(struct n745_priv *)dev->priv;
    int which=priv->which;

#ifdef DEBUG
	int i=0;
    unsigned long cur_ptr;
    TXBD  *txbd;
    
	cur_ptr=n745_ReadReg(CTXDSA,which);
	printk("&(priv->tx_desc[%d]):%x,&(priv->tx_desc[%d]:%x\n"
		,priv->cur_tx_entry,&(priv->tx_desc[priv->cur_tx_entry])
		,priv->cur_tx_entry+1,&(priv->tx_desc[priv->cur_tx_entry+1]));
	printk(",cur_ptr:%x,mode:%x,SL:%x\n",
			cur_ptr,((TXBD  *)cur_ptr)->mode,((TXBD  *)cur_ptr)->SL);
	printk("priv->tx_ptr:%x,SL:%x,mode:%x\n",
    		priv->tx_ptr,((TXBD *)(priv->tx_ptr))->SL,((TXBD *)(priv->tx_ptr))->mode);
    printk("0xfff82114:%x,MIEN:%x,MISTA:%x\n",CSR_READ(0xfff82114),
    	n745_ReadReg(MIEN,which),n745_ReadReg(MISTA,which));
	//printk("MAC %d timeout,pid:%d,mode:%d\n",priv->which,current->pid,mode);
	for ( i = 0 ; i < TX_DESC_SIZE ; i++ )
	{
	   	printk("*tx cur %d desc %x buffer %x",i,&priv->tx_desc[i],priv->tx_desc[i].buffer);
	   	printk(" next %x\n",priv->tx_desc[i].next);	
	}
#endif
	{
		printk("RXFSM:%x\n",n745_ReadReg(RXFSM,which));
		printk("TXFSM:%x\n",n745_ReadReg(TXFSM,which));
		printk("FSM0:%x\n",n745_ReadReg(FSM0,which));
		printk("FSM1:%x\n",n745_ReadReg(FSM1,which));
		//if((n745_ReadReg(TXFSM,which)&0x0FFF0000)==0x8200000)
		ResetMAC(dev);
	}
	dev->trans_start = jiffies;

}

static int n745_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
#ifdef DEBUG
    char *data;
    int i=0;
    int len=skb->len;
    for(i=0;i<len;i+=10)
    	printk("%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n",*(data+i),*(data+i+1),
    		*(data+i+2),*(data+i+3),*(data+i+4),*(data+i+5),*(data+i+6),
    		*(data+i+7),*(data+i+8),*(data+i+9));
	printk("\n");
#endif
//printk("n745_start_xmit:dev:%x\n",dev);
    if(!(send_frame(dev,skb->data,skb->len)) )   
    {
    	dev_kfree_skb(skb);
    	TRACE("n745_start_xmit ok\n");
    	return 0;
    }
printk("send failed\n");        
    return -1;	
}

/* The typical workload of the driver:
   Handle the network interface interrupts. */

static void tx_interrupt(int irq, void *dev_id, struct pt_regs * regs)
{
    struct net_device *dev = (struct net_device *)dev_id;
    struct n745_priv *priv = (struct n745_priv *)dev->priv;
    unsigned long status;  
    unsigned long cur_ptr;
    int which=priv->which;
    TXBD  *txbd;
    static  unsigned  reset_rx_loop=0;
    
    unsigned long flags;
    save_flags(flags); cli();
    status=n745_ReadReg(MISTA,which);   //get  interrupt status;
    //n745_WriteReg(MISTA,status&0xFFFF0000,which);  //clear interrupt
    n745_WriteReg(MISTA,status,which);  //clear interrupt
    restore_flags(flags);

	cur_ptr=n745_ReadReg(CTXDSA,which);
#if 0
    if(which==1)
   		printk("tx_ptr:%x,cur_ptr:%x,tx_entry:%d,s:%x\n",priv->tx_ptr,cur_ptr,priv->cur_tx_entry,status);
#endif
   while((&(priv->tx_desc[priv->cur_tx_entry]) != cur_ptr))
    {
    	txbd =(TXBD *)&(priv->tx_desc[priv->cur_tx_entry]);
    	priv->cur_tx_entry = (priv->cur_tx_entry+1)%(TX_DESC_SIZE);
    	
	    TRACE("*txbd->SL %x\n",txbd->SL);
	    TRACE("priv->tx_ptr %x  cru_ptr =%x\n",priv->tx_ptr,cur_ptr);
	    if(txbd->SL &TXDS_TXCP)
	    {
	    	priv->stats.tx_packets++;
		priv->stats.tx_bytes += txbd->SL&0xFFFF;
	    }	
	    else
	    {
		   priv->stats.tx_errors++;    
        }
        
        txbd->SL=0; 
        txbd->mode=0;

        if (netif_queue_stopped(dev))	
		{	
			netif_wake_queue(dev);
		}
	}
	
	if(status&MISTA_EXDEF)
	{
		printk("MISTA_EXDEF\n");
	}
    if((status & MISTA_RDU)&& ++reset_rx_loop==5)
    {
    	TRACE_ERROR("W90N745 MAC In Tx %d RX   I Have Not Any Descript Needed\n",priv->which);
        //ResetMAC(dev);
        //reset_rx_loop=0;
    }
    if(status&MISTA_TxBErr)
    	printk("MISTA_TxBErr\n");
    if(status&MISTA_TDU)
    {
    	//printk("MISTA_TDU\n");
    	if (netif_queue_stopped(dev))	
		{	
			netif_wake_queue(dev);
			TRACE_ERROR("queue restart TDU\n"); 
		}
    }
    TRACE("After %d tx_interrupt status %x  \n",which,status);
}
volatile unsigned long rx_jiffies0=0;
volatile unsigned long rx_jiffies1=0;
extern volatile unsigned long jiffies;

static void rx_interrupt(int irq, void *dev_id, struct pt_regs * regs)
{
	struct net_device *dev = (struct net_device *)dev_id;
	struct n745_priv  *priv = (struct n745_priv *) dev->priv;
	unsigned long status;
	int which=priv->which;
	unsigned long flags;
	if(which==0)
		rx_jiffies0 = jiffies;
	else if(which==1)	
		rx_jiffies1 = jiffies;
    status=n745_ReadReg(MISTA,which);   //get  interrupt status;   
	save_flags(flags); cli();
    //n745_WriteReg(MISTA,status&0xFFFF,which); //clear interrupt
    n745_WriteReg(MISTA,status,which); //clear interrupt
    restore_flags(flags);

    priv->cur_rx_entry++;

    if(status & (MISTA_RDU|MISTA_RxBErr))
    {
    	//printk("No Descript available\n");
    	priv->is_rx_all=RX_DESC_SIZE; //receive all
        netdev_rx(dev);    //start doing
        priv->is_rx_all=0;
  		if(status&MISTA_RxBErr)
  		{
  			printk("MISTA_RxBErr\n");
        	ResetMAC(dev);
        }
        n745_WriteReg(RSDR ,0,which);
	    TRACE("* %d rx_interrupt MISTA %x \n",irq,status);

        return ;
    }
    save_flags(flags); cli();
    n745_WriteReg(MISTA,status,which); //clear interrupt
    restore_flags(flags);
    netdev_rx(dev);
}
 
void  ResetMACRx(struct net_device * dev)
 {
 	struct n745_priv * priv=(struct n745_priv *)dev->priv;
  	unsigned long val=n745_ReadReg(MCMDR,priv->which);
  	//printk("In ResetMAC Rx \n");
  	ResetRxRing(dev->priv);    
    n745_WriteReg(MCMDR,(MCMDR_RXON|val),priv->which);
}

void ResetMACTx(struct net_device * dev)
{
 	struct n745_priv * priv=(struct n745_priv *)dev->priv;
  	unsigned long val=n745_ReadReg(MCMDR,priv->which);
  	printk("In ResetMAC Tx \n");
  	//ResetTxRing(dev->priv);    
    n745_WriteReg(MCMDR,(MCMDR_TXON|val),priv->which);
}
	
 void ResetRxRing(struct n745_priv * n745_priv)
 {
 	
 	 int i;
 	for( i =0 ; i < RX_DESC_SIZE ; i++)
    {    
	    n745_priv->rx_desc[i].SL=0;
	    n745_priv->rx_desc[i].SL|=RXfOwnership_DMA;
	}	  
 	
}
#if 0
void DescriptorLock(struct n745_priv * priv)
{
	unsigned long flags; 
	save_flags(flags); cli();
	if(priv->lock==1)
		
	while(priv->lock==1);
	priv->lock=1;
	restore_flags(flags);
	
}
void DescriptorUnLock(struct n745_priv * priv)
{
	unsigned long flags; 
	save_flags(flags); cli();
	priv->lock=0;
	restore_flags(flags);
}
/*0:failed,1:success*/
int DescriptorTryLock(struct n745_priv * priv)
{
	unsigned long flags; 
	save_flags(flags); cli();
	if(priv->lock)
		return 0;
	priv->lock=1;
	restore_flags(flags);
	return 1;
}
#endif

/* We have a good packet(s), get it/them out of the buffers. */
static void   netdev_rx(struct net_device *dev)
{
	struct n745_priv * priv = (struct n745_priv *)dev->priv;
	RXBD *rxbd;
	unsigned long length;
	unsigned long status;
	int flag=0;

	rxbd=(RXBD *)priv->rx_ptr ;
	
    do
    {

		if(priv->is_rx_all>0)
		{
			flag=1;
    		--priv->is_rx_all;
    	}
    	else if(flag==1)
    	{
    		flag=0;
    		break;
    	}
		else if((n745_ReadReg(CRXDSA,priv->which)==(unsigned long)rxbd))
    	{
    	   	break;
    	}

    /*	if(!(rxbd->SL & RXfOwnership_CPU))
    	{	
    		if(priv->is_rx_all)
    			rxbd->SL |=RXfOwnership_DMA;
    		
    		priv->rx_ptr=( RXBD *)rxbd->next;  
    		rxbd=priv->rx_ptr;
    		continue;
    	}
    */	
    	
    	length = rxbd->SL & 0xFFFF;
    	status = (rxbd->SL & 0xFFFF0000)&((unsigned long)~0 >>2);
    	
    	if(status & RXDS_RXGD)
    	{
			unsigned char  * data;
			struct sk_buff * skb;

			data = (unsigned char *) rxbd->buffer;

			skb = dev_alloc_skb(length+2);  //Get Skb Buffer;
			if(!skb) {
				TRACE_ERROR("W90N745: I Have Not Got Memory In Fun %s\n",__FUNCTION__);
				priv->stats.rx_dropped++;
				return;
			}

			skb->dev = dev;
			skb_reserve(skb, 2);   //For IP Align 4-byte
			skb_put(skb, length);
			eth_copy_and_sum(skb, data, length, 0);  //copy
			skb->protocol = eth_type_trans(skb, dev); 
			priv->stats.rx_packets++;
			priv->stats.rx_bytes += length;
			netif_rx(skb);    // Enqueue for Up Layer
		
		}
		else
		{
		
			if(priv->is_rx_all==RX_DESC_SIZE)
				TRACE_ERROR("Rx error:%x,rxbd:%x,priv->is_rx_all:%d\n",status,rxbd,priv->is_rx_all);
			priv->stats.rx_errors++;
			if(status & RXDS_RP )
			{
				TRACE_ERROR("W90N745 MAC: Receive Runt Packet Drop it!\n");
				priv->stats.rx_length_errors++;
			}
			if(status & RXDS_CRCE )
			{
				TRACE_ERROR("W90N745 MAC Receive CRC  Packet Drop It! \n");
				priv->stats.rx_crc_errors ++;
			}
			if(status & RXDS_ALIE )
			{
				TRACE_ERROR("W90N745 MAC Receive Aligment Packet Dropt It!\n");
				priv->stats.rx_frame_errors++;
			}
			
			if(status &  RXDS_PTLE)
			{
				TRACE_ERROR("W90N745 MAC Receive Too Long  Packet Dropt It!\n");
				priv->stats.rx_over_errors++;
			}
		}

		//rxbd->SL= RX_OWNERSHIP_DMA; //clear status and set dma flag
		rxbd->SL =RXfOwnership_DMA;
		rxbd->reserved = 0;
		priv->rx_ptr=(unsigned long)rxbd->next;
		rxbd=(RXBD *)priv->rx_ptr;
		dev->last_rx = jiffies;
	}while(1);
	priv->is_rx_all = 0;
}

static void n745_set_multicast_list(struct net_device *dev)
{
	
	struct n745_priv *priv = (struct n745_priv *)dev->priv;		 			 
	unsigned long rx_mode;
//printk("n745_set_multicast_list\n");
	int which=priv->which;
	
	 if(dev->flags&IFF_PROMISC)
	 {
		rx_mode = CAMCMR_AUP|CAMCMR_AMP|CAMCMR_ABP|CAMCMR_ECMP;
		TRACE("W90N745 : Set Prommisc Flag \n");
		
	 }
	 else if((dev->flags&IFF_ALLMULTI)||dev->mc_list)
	 {		

		rx_mode=CAMCMR_AMP|CAMCMR_ABP|CAMCMR_ECMP;
	  } 
	 else 
	{
		     rx_mode = CAMCMR_ECMP|CAMCMR_ABP;
		     TRACE("W90N745 :Set Compare Flag\n");
	}	 
	
	//rx_mode=CAMCMR_AMP|CAMCMR_ABP|CAMCMR_ECMP;//|CAMCMR_AUP; 
	priv->rx_mode=rx_mode;
	n745_WriteReg(CAMCMR,rx_mode,which);
	
}
#define SIODEVSTARTNATA 0x6677
#define SIODEVSTARTNATA 0x6688

static int n745_do_ioctl(struct net_device *dev,struct ifreq *ifr,int cmd)
{
	//u16 *data = (u16 *)&ifr->ifr_data;
	struct n745_priv *priv=dev->priv;
	int which = priv->which;
	
	printk("W90N745 IOCTL:\n");
	
	switch(cmd)
	{
		case  SIOCSIFHWADDR:
			 if(dev->flags&IFF_PROMISC)
			 	return -1;

			 memcpy(dev->dev_addr,ifr->ifr_hwaddr.sa_data,ETH_ALEN);
			 
    		 memcpy(n745_mac_address0,dev->dev_addr,ETH_ALEN);
    		    
    		 n745_set_mac_address(dev,dev->dev_addr);  
    		  
		     break; 

		#define SIOCn745MACDEGUG SIOCDEVPRIVATE+1
		case  SIOCn745MACDEGUG :  //For Debug;
			  output_register_context(which);
			  break;	

	    default:
			return -EOPNOTSUPP;
	}
	return 0;
}
void output_register_context(int which)
{
 	printk("		** W90N745 EMC Register %d **\n",which);
 	
 	printk("CAMCMR:%x ",n745_ReadReg(CAMCMR,which)); 
 	printk("CAMEN:%x ",n745_ReadReg(CAMEN,which)); 
 	printk("MIEN: %x ",n745_ReadReg(MIEN,which));
 	printk("MCMDR: %x ",n745_ReadReg(MCMDR,which));
 	printk("MISTA: %x ",n745_ReadReg(MISTA,which)); 
 	printk("TXDLSA:%x ", n745_ReadReg(TXDLSA,which));	
 	printk("RXDLSA:%x \n", n745_ReadReg(RXDLSA,which));	
 	printk("DMARFC:%x ", n745_ReadReg(DMARFC,which));	
 	printk("TSDR:%x ", n745_ReadReg(TSDR,which));	
 	printk("RSDR:%x ", n745_ReadReg(RSDR,which));	
 	printk("FIFOTHD:%x ", n745_ReadReg(FIFOTHD,which));
 	printk("MISTA:%x ", n745_ReadReg(MISTA,which));
 	printk("MGSTA:%x ", n745_ReadReg(MGSTA,which));
 	
 	printk("CTXDSA:%x \n",n745_ReadReg(CTXDSA,which)); 
 	printk("CTXBSA:%x ",n745_ReadReg(CTXBSA,which)); 
 	printk("CRXDSA:%x ", n745_ReadReg(CRXDSA,which));
 	printk("CRXBSA:%x ", n745_ReadReg(CRXBSA,which));
 	printk("RXFSM:%x ",n745_ReadReg(RXFSM,which)); 
 	printk("TXFSM:%x ",n745_ReadReg(TXFSM,which)); 
 	printk("FSM0:%x ",n745_ReadReg(FSM0,which)); 
 	printk("FSM1:%x \n",n745_ReadReg(FSM1,which)); 
	
}

void ShowDescriptor(struct net_device *dev)
{
	int i;
	struct n745_priv * n745_priv=dev->priv;
	for(i=0;i<TX_DESC_SIZE;i++)
		printk("%x mode:%lx,2 SL:%lx\n",&n745_priv->tx_desc[i],n745_priv->tx_desc[i].mode,n745_priv->tx_desc[i].SL);
	for(i=0;i<RX_DESC_SIZE;i++)
		printk("%x SL:%x\n",&n745_priv->rx_desc[i],n745_priv->rx_desc[i].SL);
	printk("tx_ptr:%x,tx_entry:%d\n",n745_priv->tx_ptr,n745_priv->cur_tx_entry);
	printk("rx_ptr:%x\n",n745_priv->rx_ptr);
	
	return;
	
}
int prossess_nata(struct net_device *dev,RXBD * rxbd )
{

	return 1;
}
int send_frame(struct net_device *dev ,unsigned char *data,int length)
{
    struct n745_priv * priv= dev->priv;
    int which;
    TXBD *txbd;
    unsigned long flags;
   
    which=priv->which;

    //if (!down_trylock(&priv->locksend)) {
    txbd=( TXBD *)priv->tx_ptr;
        
    //Have a Descriptor For Transmition?
	/*
    if((txbd->mode&TXfOwnership_DMA))
    {
    	TRACE_ERROR("send_frame failed\n");
    	netif_stop_queue(dev);
    	return -1;
    }
	*/
    //txbd->mode=(TX_OWNERSHIP_DMA|TX_MODE_PAD|TX_MODE_CRC|TX_MODE_IE);
    
    //Check Frame Length
    if(length>1514) 
    {
    	TRACE(" Send Data %d Bytes ,Please  Recheck Again\n",length);
    	length=1514;
    }
    
    txbd->SL=length&0xFFFF;   
    
    memcpy((void *)txbd->buffer,data,length);
    
	txbd->mode=(PaddingMode | CRCMode | MACTxIntEn);
    txbd->mode|= TXfOwnership_DMA;

    {
     	int val=n745_ReadReg(MCMDR,which);
     	if(!(val&	MCMDR_TXON))
     	{
     	      //printk("****n745_WriteReg(MCMDR\n");
     	      n745_WriteReg(MCMDR,val|MCMDR_TXON,which);
     	 }
     	n745_WriteReg(TSDR ,0,which);
    }
    txbd=(TXBD *)txbd->next;
    priv->tx_ptr=(unsigned long)txbd;
    dev->trans_start=jiffies;
    
    save_flags(flags); cli();
    if(txbd->mode&TXfOwnership_DMA)
    	netif_stop_queue(dev);
    restore_flags(flags);
    return 0;
}
void notify_hit(struct net_device *dev ,RXBD *rxbd)
{
	TRACE("notify_hit not implement\n");
}	

#define MAC_ADDR 0x7F010008

static int n745_init(struct net_device *dev)
{
	static int which=0;//Only one mac for W90N745
	struct n745_priv *priv;
	printk("01 %s initial ok!\n",dev->name);

	printk("which:%d\n",which);
	//*((unsigned volatile int *) 0xFFF83020) = 0x55555;//lsshi GPIO to PHY
		
	ether_setup(dev);
	dev->open=n745_open;
	dev->stop=n745_close;
	dev->do_ioctl=n745_do_ioctl;
	dev->hard_start_xmit=n745_start_xmit;
	dev->tx_timeout=n745_timeout;
	dev->get_stats=n745_get_stats;
	dev->watchdog_timeo =TX_TIMEOUT;
	dev->irq=INT_EMCTXINT0+which;
	dev->set_multicast_list=n745_set_multicast_list;
	dev->set_mac_address=n745_set_mac_address;
	dev->priv =(void *)(((unsigned long) kmalloc(sizeof(struct n745_priv),GFP_KERNEL))|NON_CACHE_FLAG);
	
	if(dev->priv == NULL)
		return -ENOMEM;
	memset(dev->priv, 0, sizeof(struct n745_priv));
    
#ifdef CONFIG_WBFLASH
    //if( info.type == BOOTLOADER_INFO )
    	memcpy(n745_mac_address0,(char*)(MAC_ADDR),ETH_ALEN);
#endif
    memcpy(dev->dev_addr,n745_mac_address0,ETH_ALEN);

    priv=(struct n745_priv *)dev->priv;
    priv->which=which;
    priv->cur_tx_entry=0;
    priv->cur_rx_entry=0;

    TRACE("%s initial ok!\n",dev->name);
	return 0;
	
}

int  init_module(void)
{
	int ret;
#ifdef CONFIG_W90N745FLASH
	GetLoadImage();
#endif
	
	memset((void *)n745_netdevice[0].name ,0 ,IFNAMSIZ);
	ret=register_netdev((struct net_device *)&n745_netdevice[0]);
	if(ret!=0)
	{
		TRACE_ERROR("Regiter EMC 0 W90N745 FAILED\n");
		return  -ENODEV;
	}

	return 0;
}

void cleanup_module(void)
{        
   unregister_netdev((struct net_device *)&n745_netdevice[0]);
}

module_init(init_module);
module_exit(cleanup_module);
