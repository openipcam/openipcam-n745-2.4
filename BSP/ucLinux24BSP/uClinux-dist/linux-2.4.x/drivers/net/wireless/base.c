/*------------------------------------------------------------------------------*/
/* 		    Wireless driver base functions for winbond W90N745					*/
/* 				version 1.0.2(used only for Station)							*/
/*------------------------------------------------------------------------------*/
#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>      
#include <linux/slab.h>		 
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#include "W90N745Prism.h"
#include "Queue.h"

#if 0
#define PRISM_SSIDLEN_DEFAULT    ((UINT16)0x0008)
char PRISM_DesireSSID_DEFAULT[] = {(char)'P', (char)'C', (char)'3',(char)'0',(char)'W',
                              				(char)'L', (char)'A', (char)'N'};
#else
#define PRISM_SSIDLEN_DEFAULT    ((UINT16)0x000a)
char PRISM_DesireSSID_DEFAULT[] __attribute__ ((aligned (2)))
	= {(char)'P', (char)'C', (char)'3', (char)'2',(char)'W',
                                   				(char)'e', (char)'b', (char)'C', (char)'a',(char)'m'};
#endif// declararion desired SSID

// Access Point's MAC address
char PRISM_BSSID[] = {(char)0x00,(char)0x05,(char)0x5D,(char)0xEB,(char)0x94,(char)0x24};

// 802.11 frame header type RFC1042
char PRISM_LLC[] = {(char)0xaa,(char)0xaa,(char)0x03,(char)0x00,(char)0x00,(char)0x00};

#if 1
	//unsigned char Mac_address[6] = {0x00, 0x60, 0xB3, 0x73, 0x26, 0x3d};
	unsigned char Mac_address[6] = {0x00, 0x60, 0xB3, 0x73, 0x26, 0x3C};
#else
	unsigned char Mac_address[6] = {0x00, 0x60, 0xB3, 0x73, 0x26, 0x3E};
#endif
/* Debug for now */
UINT8 *DebugpBuffer = NULL;

Queue_txfid TxQueueObj;

/* The frequency of each channel in MHz */
const long channel_frequency[] = {
	2412, 2417, 2422, 2427, 2432, 2437, 2442,
	2447, 2452, 2457, 2462, 2467, 2472, 2484
};

#define NUM_CHANNELS ( sizeof(channel_frequency) / sizeof(channel_frequency[0]) )

/* This tables gives the actual meanings of the bitrate IDs returned by the firmware. */
struct {
	int bitrate; /* in 100s of kilbits */
	u16 prism_txratectrl;
} bitrate_table[] = {
	{110, 15}, /* Entry 0 is the default */
	{10, 1},
	{20, 2},
	{20, 3},
	{55, 4},
	{55, 7},
	{110, 8},
};
#define BITRATE_TABLE_SIZE (sizeof(bitrate_table) / sizeof(bitrate_table[0]))
//#define PRSIM_DEBUG
int nums = 5;

/*------------------------------Prism Base Functions ----------------------------------*/
/* */
int prism_cmd_access(UINT16 write, UINT16 rid)
{
	int result = 0;
	UINT16 cmd;

	cmd = PRISM_CMD_CMDCODE_SET(PRISM_CMDCODE_ACCESS)|PRISM_CMD_WRITE_SET(write);
	result = prism_docmd_wait(cmd, rid, 0, 0);
	return result;
}

/* prism transmit a buffer command, fid-->buffer */
int prism_cmd_transmit(UINT16 reclaim, UINT16 fid)
{
	int result = 0;
	UINT16 cmd;

	cmd = PRISM_CMD_CMDCODE_SET(PRISM_CMDCODE_TX)|PRISM_CMD_RECL_SET(reclaim);
	result = prism_docmd_wait(cmd, fid, 0, 0);
	return result;
}

int prism_cmd_inquiry(UINT16 infoType)
{
	int result = 0;
	UINT16 cmd;

	cmd = PRISM_CMD_CMDCODE_SET(PRISM_CMDCODE_INQ);
	result = prism_docmd_nowait(cmd, infoType, 0, 0);

	return result;
}

/* after init, the net device should be enabled */
int prism_cmd_enable(UINT16 macport)
{
	int result = 0;
	UINT16 cmd;

	cmd = PRISM_CMD_CMDCODE_SET(PRISM_CMDCODE_ENABLE)|PRISM_CMD_MACPORT_SET(macport);
	result = prism_docmd_wait(cmd, 0,0,0);
	return result;
}

/* the net device disable function */
int prism_cmd_disable(UINT16 macport)
{
	int result = 0;
	UINT16 cmd;

	cmd = PRISM_CMD_CMDCODE_SET(PRISM_CMDCODE_DISABLE)| PRISM_CMD_MACPORT_SET(macport);
	result = prism_docmd_wait(cmd, 0,0,0);
	return result;
}

/* when status register need us to diagnose, do it */
int prism_cmd_diagnose()
{
    int result = 0;
    UINT16 cmd;

    cmd = PRISM_CMD_CMDCODE_SET(PRISM_CMDCODE_DIAG);
    result = prism_docmd_wait(cmd, DIAG_PATTERNA, DIAG_PATTERNB, 0);
    return result;
}

/* copy data to buffer access path */
/* Note: only accept the buf which address is align 2 in our arm7 system */
int prism_copy_to_bap(UINT16 bap, UINT16 id, UINT16 offset, void *buf, UINT32 len)
{
	int result = 0;
	UINT8 *d = (UINT8*)buf;
	UINT16 *dw = buf;
	UINT32 selectreg;
	UINT32 offsetreg;
	UINT32 datareg;
	UINT32 i;
	volatile UINT16 reg;
	UINT16 savereg;
	int k = 0;

    selectreg = bap ? PRISM_SELECT1 : PRISM_SELECT0;
	offsetreg = bap ? PRISM_OFFSET1 : PRISM_OFFSET0;
    datareg   = bap ? PRISM_DATA1 : PRISM_DATA0;


	k = BAP_BUSY_TIMEOUT;
    /* Write id to select reg */
    reg = READREG(offsetreg);
    while(PRISM_OFFSET_ISBUSY(reg) && k) {
    	k--;
		udelay(5);
		reg = READREG(offsetreg);
	}

	if(!k) {
		result = -1;
		LOGPRINT;
		goto failed;
	}
	if (PRISM_OFFSET_ISERR(reg)) {
        result = -1;
        LOGPRINT;
        return result;
    }

	WRITEREG(selectreg, id);
	
	k = BAP_BUSY_TIMEOUT;
	/* Write offset to offset reg */
	reg = READREG(offsetreg);
	while (PRISM_OFFSET_ISBUSY(reg) && k) {
		k--;
		udelay(5);
		reg = READREG(offsetreg);
	}
	if(!k) {
		result = -1;
		LOGPRINT;
		goto failed;
	}
	if (PRISM_OFFSET_ISERR(reg)) {
    	result = -1;
    	LOGPRINT;
    	return result;
    }

	WRITEREG(offsetreg, offset);

	k = BAP_BUSY_TIMEOUT;
	/* Wait for offset[busy] to clear */
	reg = READREG(offsetreg);
	while (PRISM_OFFSET_ISBUSY(reg) && k) {
		k--;
		udelay(5);
		reg = READREG(offsetreg);
	}
    if (PRISM_OFFSET_ISERR(reg)) {
    	result = -1;
    	LOGPRINT;
    	return result;
    }
	/* Write even(len) buf contents to data reg */
	for ( i = 0; i < (len>>1); i++ ) {
	    WRITEREG(datareg, *dw++);
	}

    /* If len odd, handle last byte */
	if ( len % 2 )
    {
    	savereg = READREG(datareg);
    	WRITEREG(offsetreg, (offset+(len&0xfffe)));

    	/* Wait for offset[busy] to clear (see BAP_TIMEOUT) */
    	k = BAP_BUSY_TIMEOUT;
    	reg = READREG(offsetreg);
    	while (PRISM_OFFSET_ISBUSY(reg) && k) {
    		k--;
    		udelay(5);
    		reg = READREG(offsetreg);	
    	}
    	if(!k) {
			result = -1;
			LOGPRINT;
			goto failed;
		}
    	((UINT8*)(&savereg))[0] = d[len-1];
    	WRITEREG(datareg, savereg);
    }
failed:
    if (result)
        printk("copy_to_bap failed\r\n");
    return result;
}


/* Set configuration / information record */
int Write_RID_Config(UINT16 rid, void *buf, UINT16 len)
{
    int result = 0;
    prism_rec_t rec;

    rec.rid = rid;
    rec.reclen = (len/2)?((len+1)/2+1):((len/2) + 1);
    
    /* write the record header */
    result = prism_copy_to_bap(USER_BAP, rid, 0, &rec, 4);
    if ( result )
    	printk("Failure writing record header\n");

    /* write the record data (if there is any) */
    if ( len > 0 )
    {
        result = prism_copy_to_bap(USER_BAP, rid, 4, buf, len);
        if ( result )
            printk("Failure writing record data\r\n");
    }
    result = prism_cmd_access(1, rid);
    return result;
}

/* get configuration / information record */
int Read_RID_Config(UINT16 rid, void *buf)
{
    int result = 0;
    prism_rec_t rec;
    short len;
	
	result = prism_cmd_access(0, rid);
    	
    /* read the record header */
    result = prism_copy_from_bap(IRQ_BAP, rid, 0, &rec, 4);
    if ( result ) {
    	result = -1;
    	printk("Failure writing record header\r\n");
    	goto fail;
    }
	
	len = (rec.reclen-1)*2;
#ifdef WDEBUG
	printk("len:%d, rec.len: %d\n", len, rec.reclen);		
#endif
	
    /* read the record data (if there is any) */
    if(len > 0);
    {	
    	result = prism_copy_from_bap(IRQ_BAP, rid, 4, buf, len);
    	if ( result ) {
    		result = -1;
            printk("Failure writing record data\r\n");
            goto fail;
        }
	}
	result = rec.reclen;
fail:
    return result;
}

/* get Communication Tallies record's type and length*/
int Read_CT_InfoType(UINT16 Infofid, UINT16 *len)
{
	int result = 0;
    prism_rec_t rec;
    
    result = prism_copy_from_bap(IRQ_BAP, Infofid, 0, &rec, 4);
    if ( result ) {
    	result = -1;
    	printk("Failure writing record header\r\n");
    	goto fail;
    }
    
    *len = (rec.reclen-1)*2;
    result = rec.rid;
fail:
    return result;
}

/* get Commnication Tallies record's data */
int Read_CT_InfoData(UINT16 Infofid, void *buf, int len)
{
 
    int result = 0;
    
    /* read the record data (if there is any) */
    if(len > 0);
    {	
    	result = prism_copy_from_bap(USER_BAP, Infofid, 4, buf, len);//??user_bap
    	if ( result ) {
    		result = -1;
            printk("Failure writing record data\r\n");
            goto fail;
        }
	}
	result = len;
fail:

    return result;
}


/*------------------------------------------------------------------*/
/* send a command to device and wait for completion */
int prism_docmd_wait(UINT16 cmd, UINT16 parm0, UINT16 parm1, UINT16 parm2)
{
    volatile UINT16 reg;
    volatile UINT16 result;
    volatile UINT16 counter, i;
    	
    volatile UINT16 prismStatus;
    volatile UINT16 prismResp0;
    volatile UINT16 prismResp1;
    volatile UINT16 prismResp2;
    	
    i = CMD_BUSY_TIMEOUT;
    /* wait for the busy bit to clear */
    reg = READREG(PRISM_CMD);
    while (PRISM_CMD_ISBUSY(reg) && i) {
    	i--;
    	udelay(5);
    	reg = READREG(PRISM_CMD);
    }

    if (!PRISM_CMD_ISBUSY(reg))
    {
    	/* busy bit clear, write command */
    	WRITEREG(PRISM_PARAM0, parm0);
    	WRITEREG(PRISM_PARAM1, parm1);
    	WRITEREG(PRISM_PARAM2, parm2);
    	WRITEREG(PRISM_CMD, cmd);

    	/* Now wait for completion */
    	counter = 0;
    	reg = READREG(PRISM_EVSTAT);
		/* if is TX cmd, timout/30 */
    	if((cmd&0x1f) == PRISM_CMDCODE_TX)
    		i = CMD_COMPL_TIMEOUT/30;
    	else
    		i = CMD_COMPL_TIMEOUT;
    	while (!PRISM_EVSTAT_ISCMD(reg) && i)
    	{
            i--;
            udelay(10);
            reg = READREG(PRISM_EVSTAT);
        }
		
        if (PRISM_EVSTAT_ISCMD(reg))
        {
        	result = 0;
        	prismStatus = READREG(PRISM_STATUS);
        	prismResp0 = READREG(PRISM_RESP0);
        	prismResp1 = READREG(PRISM_RESP1);
        	prismResp2 = READREG(PRISM_RESP2);
        	WRITEREG(PRISM_EVACK, 0x0010);
        	result = PRISM_STATUS_RESULT_GET(prismStatus);
        }
        else
        {
	    	//printk("timeout:reg[%x]\n", reg);
        	result = prism_cmd_diagnose();
        	//printk("result [%x]\n", result);
        }
    }
    	
    return result;
}

/* send a command to device and no wait for completion */
int prism_docmd_nowait(UINT16 cmd, UINT16 parm0, UINT16 parm1, UINT16 parm2)
{
    volatile UINT16 reg;
    volatile UINT16 result;
    volatile UINT16 counter, i;
    	
    volatile UINT16 prismStatus;
    volatile UINT16 prismResp0;
    volatile UINT16 prismResp1;
    volatile UINT16 prismResp2;
    	
    i = CMD_BUSY_TIMEOUT;
    /* wait for the busy bit to clear */
    reg = READREG(PRISM_CMD);
    while (PRISM_CMD_ISBUSY(reg) && i) {
    	i--;
    	udelay(5);
    	reg = READREG(PRISM_CMD);
    }

    if (!PRISM_CMD_ISBUSY(reg))
    {
    	/* busy bit clear, write command */
    	WRITEREG(PRISM_PARAM0, parm0);
    	WRITEREG(PRISM_PARAM1, parm1);
    	WRITEREG(PRISM_PARAM2, parm2);
    	WRITEREG(PRISM_CMD, cmd);

    	/* Now wait for completion */
    	counter = 0;
    	reg = READREG(PRISM_EVSTAT);
		/* if is TX cmd, timout/30 */
    	if((cmd&0x1f) == PRISM_CMDCODE_TX)
    		i = CMD_COMPL_TIMEOUT/30;
    	else
    		i = CMD_COMPL_TIMEOUT;
    	while (!PRISM_EVSTAT_ISCMD(reg) && i)
    	{
            i--;
            udelay(10);
            reg = READREG(PRISM_EVSTAT);
        }
		
        result = 0;
        prismStatus = READREG(PRISM_STATUS);
        prismResp0 = READREG(PRISM_RESP0);
        prismResp1 = READREG(PRISM_RESP1);
        prismResp2 = READREG(PRISM_RESP2);
        WRITEREG(PRISM_EVACK, 0x0010);
        result = PRISM_STATUS_RESULT_GET(prismStatus);
    }
    	
    return result;
}


/* prism command allocate mem */
int prism_cmd_allocate(UINT16 len, unsigned short *txfid)
{
    int result = 0;
    UINT16 cmd;
    UINT16 reg;
    UINT32 i;

    if (len % 2)
    	result = -1;
    else
    {
    	cmd = PRISM_CMD_CMDCODE_SET(PRISM_CMDCODE_ALLOC);
    	result = prism_docmd_wait(cmd, len, 0, 0);
    }
    	
    reg = READREG(PRISM_EVSTAT);
    i = ALLOC_COMPL_TIMEOUT;
    while (!PRISM_EVSTAT_ISALLOC(reg) && i)
    {
    	i--;
        udelay(10);
        reg = READREG(PRISM_EVSTAT);
    }
    	
    if(!PRISM_EVSTAT_ISALLOC(reg)) {
    	printk("Alloc event timeout\n");
    	return -ETIMEDOUT;
    }	
    
    *txfid = READREG(PRISM_ALLOCFID);
    WRITEREG(PRISM_EVACK, PRISM_EVACK_ALLOC_SET(1));
    return result;
}

/* prism command definition, for device initialization */
int prism_cmd_initialize()
{
    int result = 0;

    result = prism_docmd_wait(PRISM_CMDCODE_INIT,0,0,0);
    return result;
}

/* this function wish the wireless device has been in slot , :P*/
/* otherwize it will fail :(, map device register to memory, and */
/* access memory to setup our device */
int prism_initmac(struct net_device *dev)
{
	prism_cnfDesireSSID_t ssid;
	unsigned short reg = 0;
	volatile int i;
	int result = 0;
	struct w740prism_priv *priv = (struct w740prism_priv *)dev->priv;
	
	/*---------------------------------------------------------------------------*/
	/*        should init for w90740 extern I/0 2 as programm interrupt, for tx, rx.           */           
	/*	     But our system has inited it :)										   */	
	//*((volatile unsigned int *) 0xFFF83000) = 0x00054554;   	// config GPIO
	//*((volatile unsigned int *) 0xFFF83000) = 0x00054554;   	// config GPIO
//	i = *((volatile unsigned int *) 0xFFF83000);
//	*((volatile unsigned int *) 0xFFF83000) = i & 0xffffcfff;	// config GPIO
	printk("GPIO: %x\n", *((volatile unsigned int *) 0xFFF83000));

//    *((volatile unsigned int *) 0xFFF83004) = 0x00100000;   		// GPIO20 -> output 
//    *((volatile unsigned int *) 0xFFF83008) = 0x00000000;   		// GPIO20 -> 0 
    *((volatile unsigned int *) 0xFFF83004) = 0x00000003;   		// GPIO0 and GPIO1 -> output 
    *((volatile unsigned int *) 0xFFF83008) = 0x00000002;   		// GPIO1 -> 1

    /*        System memory map. map external I/O bank 2 to start address:0x80000000  */
    /*	     length: 32M :P, is right no collusion									      */
    *((volatile unsigned int *) EXT2CON_REG) = 0x8007bffe;  	//ROM2 for Mem access

    // Set external click 0x00003ff8	
    *((volatile unsigned int *) 0xFFF0000c) = 0x00003fe8;   		// PLL OFF, 15MHz

    /*--------------------------------------------------------------------------------*/
#if 1
    *((volatile unsigned int *) 0xFFF83008) = 0x00000000;   		// GPIO1 -> 0 

    *((volatile unsigned short *) 0xC00003e0) = 0x80;       		// set Configuration Option Register 
    for(i = 0; i < 500000; i++);
    if ((reg = *((volatile unsigned short *) 0xC00003e0)) != 0x80) {
		printk("wireless driver reset failed, %x\n", reg);
		result= -1;
		goto out;
	}
    else
    {
    	printk("wireless memory reset to system map adress: %x, length: %dM\n" , 0xc0000000, 32);
	}

    /*be related with little endian,so value 0x4100 ??? */
    *((volatile unsigned short *) 0xC00003e0) = 0x41;       		//set Configuration Option Register
    for(i = 0; i < 30000; i++);
    if ((reg = *((volatile unsigned short *) 0xC00003e0)) != 0x41) {
		printk("wireless driver map to system map failed, %x\n", reg);
		result= -1;
		goto out;
	}
    else
    {
    	printk("wireless memory map to system map adress: %x, length: %dM\n" , 0xc0000000, 32);
	}
	*((volatile unsigned int *) 0xfff82010) = 0x01;
	
#if 0	
	WRITEREG(PRISM_SWSUPPORT0, HERMES_MAGIC);
	printk("magic value: %x\n", READREG(PRISM_SWSUPPORT0));
	printk("magic value: %x\n", READREG(PRISM_SWSUPPORT0));
	printk("magic value: %x\n", READREG(PRISM_SWSUPPORT0));
#endif
	result = prism_cmd_initialize();
    if (result != 0) {
    	printk("Initialize command failed\r\n");
    	result = -1;
    	goto out;
    }
    else
    	;//printk("Initialize command sucess\r\n");
    	
    /* make sure interrupts are disabled and any layabout events cleared */
    WRITEREG(PRISM_INTEN, PRISM_INTEN_DISABLE);
    WRITEREG(PRISM_EVACK, PRISM_EVENT_MASK);

	/* init fid queue */
    Init_Queue();
	Queue_dump();

    /* sets 0xFC84 TxRates */
    priv->bitratemode = DEFAULT_TXRATE_MODE;
    prism_hw_setiwrate(priv);
    
    /* sets Rxsthreshold */
    priv->rts_thresh = DEFAULT_RXSTHRESHOLD;
    prism_hw_setrts(priv);
    
    /* set ap density */
    priv->ap_density = DEFAULT_SENSE;
    prism_hw_setapdensity(priv);
    
    /* set up power */
    priv->pm_on = DEFAULT_POWMANAGE;
    priv->pm_mcast = DEFAULT_MULITRECEIVE;
    priv->pm_period = DEFAULT_MAXSLEEP;
    priv->pm_timeout = DEFAULT_PMHOLDDURATION;
    prism_hw_setpower(priv);
    /* sets 0xFC2A auth type */
    reg = PRISM_Authentication;
    Write_RID_Config(PRISM_RID_CNFAUTHENTICATION, &reg, PRISM_RID_CNFAUTHENTICATION_LEN);
	
	/* sets link type */
	priv->iw_mode = IW_MODE_INFRA;
	reg = priv->port_type = PRISM_MacPortType; 
	Write_RID_Config(PRISM_RID_CNFPORTTYPE, &reg, PRISM_RID_CNFPORTTYPE_LEN);
	
	/* set 0xFC02 desired SSID */
#if 1
    ssid.ssidLen = PRISM_SSIDLEN_DEFAULT;
    memcpy(ssid.ssidName, PRISM_DesireSSID_DEFAULT, ssid.ssidLen);
    Write_RID_Config(PRISM_RID_CNFOWNSSID, (UINT8 *)&ssid, PRISM_RID_CNFOWNSSID_LEN);
    Write_RID_Config(PRISM_RID_CNFDESIREDSSID, (UINT8 *)&ssid, PRISM_RID_CNFDESIREDSSID_LEN);
#endif

    /* Retrieve the maximum frame size */
    reg = FRAME_DATA_MAXLEN;
    Write_RID_Config(PRISM_RID_CNFMAXDATALEN, &reg, PRISM_RID_CNFMAXDATALEN_LEN);
    
    WRITEREG(PRISM_EVSTAT, PRISM_EVENT_MASK);
    WRITEREG(PRISM_INTEN, PRISM_INTEN_ENABLEBITS);

    /* enable MAC port 0 */
    prism_cmd_enable(0);
    //printk("Enable the Mac port 0,?\n");
    

#endif
    result = Read_RID_Config(PRISM_RID_CNFOWNMACADDR, priv->mac_address);
    for(i = 0; i < 6; i++)
    	printk("%x ", priv->mac_address[i]);
    printk("\n");
    result = Read_RID_Config(PRISM_RID_CNFDESIREDSSID, &ssid);

    //printk("*************************************************\n");
   // printk("TX Size: %d\n", sizeof(struct hermes_tx_descriptor));
    memcpy(priv->desired_essid, ssid.ssidName, ssid.ssidLen);
    priv->desired_essid[ssid.ssidLen] = '\0';
    
    printk("essid: %s, addr: %x\n", priv->desired_essid, priv->desired_essid);
    //printk("\n");
out:
	*((volatile unsigned int *) 0xFFF0000c) = 0x00003ff8; 	// 80MHz	 
	return result;
}

/* real device reset, called by prism_reset() */
int prism_reset_device(struct net_device *dev)
{
	prism_cnfDesireSSID_t ssid;
	struct w740prism_priv * priv = dev->priv;
	u16 reg;
	volatile int i;
	int k = CMD_BUSY_TIMEOUT;
	int ret = 0;

	priv = dev->priv;
	*((volatile unsigned int *) 0xFFF0000c) = 0x00003fe8;   	// PLL OFF, 15MHz	
#if 1
	i = *((volatile unsigned int *) 0xFFF83008);
    *((volatile unsigned int *) 0xFFF83008) = i | 0x00000002;   /* GPIO1 -> 1 */
    for (i=0; i<10; i++);
	i = *((volatile unsigned int *) 0xFFF83008);
    *((volatile unsigned int *) 0xFFF83008) = i & 0xfffffffd;   /* GPIO1 -> 0 */
	//printk("<<<reset>>>\n");
    for(i = 0; i < 30000; i++);
	*((volatile unsigned int *) 0xfff82124) = 0x10;		// add by chp; disable interrupt

	*((volatile unsigned short *) 0xC00003e0) = 0x80;       	// set Configuration Option Register 

    for(i = 0; i < 500000; i++);
    if ((reg = *((volatile unsigned short *) 0xC00003e0)) != 0x80) {
		printk("wireless driver reset failed, %x\n", reg);
		ret = -1;
		goto out;
	}
    else
    {
    	printk("wireless memory reset to system map adress: %x, length: %dM\n" , 0xc0000000, 32);
	}

	/*be related with little endian,so value 0x4100 ??? */
    *((volatile unsigned short *) 0xC00003e0) = 0x41;       	// set Configuration Option Register
    for(i = 0; i < 30000; i++);
    if ((reg = *((volatile unsigned short *) 0xC00003e0)) != 0x41) {
		printk("wireless driver map to system map failed, %x\n", reg);
		ret = -1;
		goto out;
	}
    else
    {
    	printk("wireless memory map to system map adress: %x, length: %dM\n" , 0xc0000000, 32);
	}
	*((volatile unsigned int *) 0xfff82010) = 0x01;
#endif	
	/* make sure interrupts are disabled and any layabout events cleared */
    WRITEREG(PRISM_INTEN, PRISM_INTEN_DISABLE);
    WRITEREG(PRISM_EVACK, PRISM_EVENT_MASK);
    
    /* First wait for the command register to unbusy */
	reg = READREG(PRISM_CMD);
	while ( (PRISM_CMD_ISBUSY(reg)) && k ) {
		k--;
		udelay(1);
		reg = READREG(PRISM_CMD);
	}
	//printk("hermes_issue_cmd: did %d retries.\n", CMD_BUSY_TIMEOUT-k);
    	
    reg = READREG(PRISM_EVSTAT);
    WRITEREG(PRISM_EVACK, reg);
    
    
    ret = prism_cmd_initialize();
    if (ret != 0) {
    	printk("Initialize command failed\r\n");
    	ret = -1;
    	goto out;
    }
    else
    	;//printk("Initialize command sucess\r\n");
    
   	Init_Queue();
	Queue_dump();
	 
	/* sets link type */
	reg = priv->port_type; 
	DEBUG("port type: %x\n", priv->port_type);
	Write_RID_Config(PRISM_RID_CNFPORTTYPE, &reg, PRISM_RID_CNFPORTTYPE_LEN);
	DEBUG("allow ibss %x\n", priv->allow_ibss);
#if 0
    reg = PRISM_TxRate;
    Write_RID_Config(PRISM_RID_TXRATECNTL, &reg, PRISM_RID_TXRATECNTL_LEN);
#else
	prism_hw_setiwrate(priv);
#endif
	
	/* set RxSthread */
	prism_hw_setrts(priv);


	/* set ap density */
	prism_hw_setapdensity(priv);
	
	/* set power */
	prism_hw_setpower(priv);
	
    /* set 0xFC02 desired SSID */
    ssid.ssidLen = strlen(priv->desired_essid);
    memcpy(ssid.ssidName, priv->desired_essid, ssid.ssidLen);
    printk("essid: %s; addr: %x\n", priv->desired_essid, priv->desired_essid);
    Write_RID_Config(PRISM_RID_CNFOWNSSID, (UINT8 *)&ssid, PRISM_RID_CNFOWNSSID_LEN);
    Write_RID_Config(PRISM_RID_CNFDESIREDSSID, (UINT8 *)&ssid, PRISM_RID_CNFDESIREDSSID_LEN);
	
    /* Retrieve the maximum frame size */
    reg = FRAME_DATA_MAXLEN;
    Write_RID_Config(PRISM_RID_CNFMAXDATALEN, &reg, PRISM_RID_CNFMAXDATALEN_LEN);
    
    /* set the channel */
    reg = priv->channel;
    Write_RID_Config(PRISM_RID_CNFOWNCHANNEL, &reg, PRISM_RID_CNFOWNCHANNEL_LEN);

    /* Set up encryption */
#if 1
	ret = prism_hw_setup_wep(priv);
	if (ret) {
		printk(KERN_ERR "%s: Error %d activating WEP.\n",
		       dev->name, ret);
		goto out;
	}
#endif
	WRITEREG(PRISM_EVSTAT, PRISM_EVENT_MASK);
    WRITEREG(PRISM_INTEN, PRISM_INTEN_ENABLEBITS);

    /* enable MAC port 0 */
    prism_cmd_enable(0);
    //printk("Enable the Mac port 0,?\n");

	*((volatile unsigned int *) 0xfff82120) = 0x10;		// add by chp; enable interrupt

out:
	 *((volatile unsigned int *) 0xFFF0000c) = 0x00003ff8; 	//80MHz 
    return ret;
}

/*----------------------------------------------------------------
* prism_txexc
*
* Handles the TxExc event.  A Transmit Exception event indicates
* that the MAC's TX process was unsuccessful - so the packet did
* not get transmitted.
*
* Arguments:
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	interrupt
----------------------------------------------------------------*/
void prism_txexc()
{
    UINT16 status;
    UINT16 fid;
    int result = 0;

    /* Collect the status and display */
    fid = READREG(PRISM_TXCOMPLFID);
    result = prism_copy_from_bap(IRQ_BAP, fid, 0, &status, sizeof(status));

    //printk(" %x: [%x]\n", fid, status);

    if (result)
    	printk("prism_txexc:copy_from_bap failed\r\n");
}

/* copy data from buffer access path */
/* Note: only accept the buf which address is align 2 */
int prism_copy_from_bap(UINT16 bap, UINT16 id, UINT16 offset, void *buf, UINT32 len)
{
   	int result = 0;
   	UINT8 *d = (UINT8*)buf;
   	UINT16 *dw = buf;
   	UINT32 selectreg;
   	UINT32 offsetreg;
   	UINT32 datareg;
   	UINT32 i;
   	volatile UINT16 reg = 0;
	int k = 0;
	


   	selectreg = bap ? PRISM_SELECT1 : PRISM_SELECT0;
	offsetreg = bap ? PRISM_OFFSET1 : PRISM_OFFSET0;
    datareg   = bap ? PRISM_DATA1 : PRISM_DATA0;

	k = BAP_BUSY_TIMEOUT;	
    /* Write id to select reg */
    reg = READREG(offsetreg);
   	while (PRISM_OFFSET_ISBUSY(reg) && k) {
   		k--;
   		udelay(5);
   		reg = READREG(offsetreg);
   	}
   	if(!k) { 
   		result = -1;
   		LOGPRINT;
   		goto failed;
   	}
   	WRITEREG(selectreg, id);

	k = BAP_BUSY_TIMEOUT;
    /* Write offset to offset reg */
    reg = READREG(offsetreg);
   	while (PRISM_OFFSET_ISBUSY(reg) && k)
	{
		k--;
		udelay(5);
		reg = READREG(offsetreg);
	}
	if(!k) { 
    	result = -1;
    	LOGPRINT;
    	goto failed;
   	}
   	WRITEREG(offsetreg, offset);
    	
	k = BAP_BUSY_TIMEOUT;
    /* Wait for offset[busy] to clear */
    reg=READREG(offsetreg);
   	while (PRISM_OFFSET_ISBUSY(reg) &&k )
   	{
   		k--;
		udelay(5);
		reg=READREG(offsetreg);	
	}
    if (PRISM_OFFSET_ISERR(reg)) { //??
    	//result = reg;
    	LOGPRINT;
    	result = -1;
    	return result;
    }
    /* Read even(len) buf contents from data reg */
    for ( i = 0; i < (len>>1); i++ ) {
    	*dw++ = READREG(datareg);
    }
    /* If len odd, handle last byte */
    if ( len % 2 )
    {
    	reg = READREG(datareg);
    	d[len-1] = ((UINT8*)(&reg))[0];
    }

    if (result) {
    	LOGPRINT;
    	printk("copy_from_bap failed\r\n");
    }
failed:
    return result;

}


static int prism_hw_get_essid(struct w740prism_priv *priv, int *active,
			      char buf[IW_ESSID_MAX_SIZE+1])
{
#if 0
	hermes_t *hw = &priv->hw;
	int err = 0;
	struct hermes_idstring essidbuf;
	char *p = (char *)(&essidbuf.val);
	int len;


	orinoco_lock(priv);

	if (strlen(priv->desired_essid) > 0) {
		/* We read the desired SSID from the hardware rather
		   than from priv->desired_essid, just in case the
		   firmware is allowed to change it on us. I'm not
		   sure about this */
		/* My guess is that the OWNSSID should always be whatever
		 * we set to the card, whereas CURRENT_SSID is the one that
		 * may change... - Jean II */
		u16 rid;

		*active = 1;

		rid = (priv->port_type == 3) ? HERMES_RID_CNFOWNSSID :
			HERMES_RID_CNFDESIREDSSID;
		
		err = hermes_read_ltv(hw, USER_BAP, rid, sizeof(essidbuf),
				      NULL, &essidbuf);
		if (err)
			goto fail_unlock;
	} else {
		*active = 0;

		err = hermes_read_ltv(hw, USER_BAP, HERMES_RID_CURRENTSSID,
				      sizeof(essidbuf), NULL, &essidbuf);
		if (err)
			goto fail_unlock;
	}

	len = le16_to_cpu(essidbuf.len);

	memset(buf, 0, IW_ESSID_MAX_SIZE+1);
	memcpy(buf, p, len);
	buf[len] = '\0';

 fail_unlock:
	orinoco_unlock(priv);

	return err;       
#endif
	return 0;
}

/* Get current connected AP's essid */
int prism_ioctl_getessid(struct net_device *dev, struct iw_point *erq)
{
	struct w740prism_priv *priv = dev->priv;
	char essidbuf[IW_ESSID_MAX_SIZE+1];
	prism_cnfDesireSSID_t our_ssid;
	int err = 0;
	
	err = Read_RID_Config(PRISM_RID_CNFDESIREDSSID, &our_ssid);
	DEBUG("essid len: %d\n", our_ssid.ssidLen);
	if(our_ssid.ssidLen <= 0) {
		erq->flags = 0;
		return 0;
	}
	
	erq->flags = 1;
	erq->length = our_ssid.ssidLen;
	memcpy(essidbuf, our_ssid.ssidName, our_ssid.ssidLen);
	if (erq->pointer)
		if ( copy_to_user(erq->pointer, essidbuf, erq->length) )
			return -EFAULT;
	return 0;
}

/* it should get net working freq or channel, we only get channel */
/* if AD-Hoc mode, return OWNCHANNEL register's value			  */
/* else if infrastructure mode, return CURRENT regiester's value  */
long prism_hw_get_freq(struct w740prism_priv *priv)
{
	int err = 0;
	u16 channel;
	long freq = 0;
	prism_lock(priv);
	DEBUG("Current Mode: %d\n", priv->iw_mode);
	if(priv->iw_mode == IW_MODE_INFRA) {
		err = Read_RID_Config(PRISM_RID_CURRENTCHANNEL, &channel);
	}
	else if(priv->iw_mode == IW_MODE_ADHOC) {
		err = Read_RID_Config(PRISM_RID_CNFOWNCHANNEL, &channel);
	}
	else
		err = -1;
	if(err < 0)
	{
		printk("%s: read channel erro\n");
		err = -EBUSY;
		goto out;
	}
	
	if ( (channel < 1) || (channel > NUM_CHANNELS) ) {
		struct net_device *dev = priv->ndev;

		printk("%s: Channel out of range (%d)!\n", dev->name, channel);
		err = -EBUSY;
		goto out;

	}
	freq = channel;
	//printk("channel: %d, %d\n", channel, freq);
	err = 0;
out:
	prism_unlock(priv);
	return err ? err : freq;
}

/* Set essid */
int prism_ioctl_setessid(struct net_device *dev, struct iw_point *essid)
{
	int err = 0;
	prism_lock(dev->priv);
	memcpy(((struct w740prism_priv *)(dev->priv))->desired_essid, (unsigned char *)essid->pointer, essid->length);
	DEBUG("essid: %s, len: %d\n", ((struct w740prism_priv *)dev->priv)->desired_essid, essid->length);
	prism_unlock(dev->priv);
	
	return err;
}

/* set freq(it refers to channel here) */
int prism_ioctl_setfreq(struct net_device *dev, struct iw_freq *freq) 
{
	int err = 0;
	struct w740prism_priv *priv = dev->priv;
	u16 reg;
	
	//printk("value: %x\n", freq->m);
	if( (freq->m < 1) || (freq->m > NUM_CHANNELS)) {
			printk("%s: Channel out of range (%d)!\n", dev->name, freq->m);
			err = -EFAULT;
			goto out;
	}
	priv->channel = freq->m;
out:
	return err;
}

/* get Wep key */
int prism_ioctl_getiwencode(struct net_device *dev, struct iw_point *erq)
{
	struct w740prism_priv *priv = dev->priv;
	int index = (erq->flags & IW_ENCODE_INDEX) - 1;
	u16 xlen = 0;
	char keybuf[PRISM_MAX_KEY_SIZE];
	int i, j;
	u16 defaultkeyid, wepflags, authentication;
	
	prism_lock(priv);
	if ((index < 0) || (index >= PRISM_MAX_KEYS))
		index = priv->tx_key;

	erq->flags = 0;
	if (! priv->wep_on)
	erq->flags |= IW_ENCODE_DISABLED;
	erq->flags |= index + 1;
	
	xlen = le16_to_cpu(priv->keys[index].len);
	erq->length = xlen;

	if (erq->pointer) {
		memcpy(keybuf, priv->keys[index].data, PRISM_MAX_KEY_SIZE);
	}
	
	prism_unlock(priv);
	
	if (erq->pointer) {
		if (copy_to_user(erq->pointer, keybuf, xlen))
			return -EFAULT;
	}
//	for(i = 0; i < xlen; i++) {
//		printk("%x", keybuf[i]);
//	}
//	printk("\n");
	for(j = 0; j < 4; j++) {
		Read_RID_Config(PRISM_RID_CNFDEFAULTKEY0+j, keybuf);
//		for(i = 0; i < xlen; i++) {
//			printk("%x", keybuf[i]);
//		}
//		printk("\n");
	}
	Read_RID_Config(PRISM_RID_CNFWEPDEFAULTKEYID, &defaultkeyid);
	Read_RID_Config(PRISM_RID_CNFWEPFLAGS, &wepflags);
	Read_RID_Config(PRISM_RID_CNFAUTHENTICATION, &authentication);
//	printk("defaultkeyid: %x, wepflags: %x, authentication: %x\n", defaultkeyid, wepflags, authentication);
	return 0;
}

/* set Wep key */
int prism_ioctl_setiwencode(struct net_device *dev, struct iw_point *erq)
{
	int i;
	struct w740prism_priv *priv = dev->priv;
	char buf[IW_ENCODING_TOKEN_MAX+1];
	int index = (erq->flags & IW_ENCODE_INDEX) - 1;
	int setindex = priv->tx_key;
	int enable = priv->wep_on;
	int restricted = priv->wep_restrict;
	int xlen = 0;
	int err = 0;
//printk("index: %d, len: %d, nowlen: %d\n", index, erq->length, priv->keys[index].len);	
	if (erq->length > IW_ENCODING_TOKEN_MAX)
	{
		printk("Too long key\n");
		return -EFAULT;
	}
	
	if (erq->pointer) {
		if(copy_from_user(buf, erq->pointer, erq->length))
			return -EFAULT;
	}
	prism_lock(priv);
	if (erq->pointer) {
		if (erq->length > PRISM_MAX_KEY_SIZE) {
			printk("Too long key string\n");
			err = -E2BIG;
			goto out;
		}
		
//		if ( (erq->length > LARGE_KEY_SIZE)
//		     || ( ! priv->has_big_wep && (erq->length > SMALL_KEY_SIZE))  ) {
//			err = -EINVAL;
//			goto out;
//		}
		
		if ((index < 0) || (index >= PRISM_MAX_KEYS))
			index = priv->tx_key;
		
		if (erq->length > SMALL_KEY_SIZE) {
			LOGPRINT;
			xlen = LARGE_KEY_SIZE;
		} else if (erq->length > 0) {
			LOGPRINT;
			xlen = SMALL_KEY_SIZE;
		} else
			xlen = 0;
		
		/* Switch on WEP if off */
		if ((!enable) && (xlen > 0)) {
			setindex = index;
			enable = 1;
		}
	} else {
		/* Important note : if the user do "iwconfig eth0 enc off",
		 * we will arrive there with an index of -1. This is valid
		 * but need to be taken care off... Jean II */
		if ((index < 0) || (index >= PRISM_MAX_KEYS)) {
			if((index != -1) || (erq->flags == 0)) {
				err = -EINVAL;
				goto out;
			}
		} else {
			/* Set the index : Check that the key is valid */
			if(priv->keys[index].len == 0) {
				err = -EINVAL;
				goto out;
			}
			setindex = index;
		}
	}
	
	if (erq->flags & IW_ENCODE_DISABLED)
		enable = 0;
	/* Only for Prism2 & Symbol cards (so far) - Jean II */
	if (erq->flags & IW_ENCODE_OPEN)
		restricted = 0;
	if (erq->flags & IW_ENCODE_RESTRICTED)
		restricted = 1;

	if (erq->pointer) {
		priv->keys[index].len = cpu_to_le16(xlen);
		memset(priv->keys[index].data, 0, sizeof(priv->keys[index].data));
		memcpy(priv->keys[index].data, buf, erq->length);
	}
	priv->tx_key = setindex;
	priv->wep_on = enable;
	priv->wep_restrict = restricted;
	
 out:
	buf[erq->length] = '\0';
//	for(i = 0; i < erq->length; i++)
//		printk("%x", buf[i]);
//	printk("\n");
//	printk("tx_key: %d, wep_on: %d, wep_restrict: %d\n", priv->tx_key, priv->wep_on, priv->wep_restrict);
//	printk("tx_key's length: %d\n", priv->keys[priv->tx_key].len);
	prism_unlock(priv);
	return err;
}

/* real setup the Wep key to hardware */
int prism_hw_setup_wep(struct w740prism_priv *priv)
{
	int err = 0;
	int keylen;
	int i, j;
	int auth_flag;
	int master_wep_flag = 0;		//is off
	
//	printk("prism_hw_setup_wep\n");
	if (priv->wep_on) {
		/* Write all 4 keys */
		keylen = le16_to_cpu(priv->keys[priv->tx_key].len);
		for(i = 0; i < PRISM_MAX_KEYS; i++) {
//			printk("keylen: %d, reallen: %d\n", keylen, priv->keys[i].len);
			if (keylen > LARGE_KEY_SIZE) {
				printk("%s: Key %d has oversize length %d.\n",
			       		priv->ndev->name, i, keylen);
					return -E2BIG;
			}
//			printk("%dkey's len: %d\n", i, keylen);
//			for( j = 0; j < keylen; j++)
//				printk("%x", priv->keys[i].data[j]);
//			printk("\n");
			err = Write_RID_Config(PRISM_RID_CNFDEFAULTKEY0 + i, priv->keys[i].data, keylen);
			if (err)
				return err;
		}

	err = Write_RID_Config(PRISM_RID_CNFWEPDEFAULTKEYID, &priv->tx_key, PRISM_RID_CNFWEPDEFAULTKEYID_LEN);
	if (err)
		return err;
	
	if (priv->wep_restrict) {
		auth_flag = 2;
		master_wep_flag = 3;
	} else {
		auth_flag = 1;
		master_wep_flag = 1; /* Intersil */
	}

	err = Write_RID_Config(PRISM_RID_CNFAUTHENTICATION, &auth_flag, PRISM_RID_CNFAUTHENTICATION_LEN); 
	if (err)
		return err;
	}
	Read_RID_Config(PRISM_RID_CNFAUTHENTICATION, &auth_flag);
//	printk("Auth_flag: %x\n", auth_flag);
	/* Master WEP setting : on/off */
	err = Write_RID_Config(PRISM_RID_CNFWEPFLAGS, &master_wep_flag, PRISM_RID_CNFWEPFLAGS_LEN);
	if (err)
		return err;	
	return 0;
}

int prism_ioctl_setscan(struct net_device *dev, struct iw_point *erq)
{
	int err = 0;
	unsigned short len;
	unsigned int i, j, cnt;
	struct w740prism_priv *priv = dev->priv;
//	printk("erq->length: %d\n", erq->length);
	prism_lock(priv);
#if 1
	prism_cmd_inquiry(PRISM_INQ_SCAN);
	if(down_interruptible(&priv->sema))
			err = -1;
	
	
	DEBUG("erq addr: %x\n", erq);
	prism_ioctl_getaplist(dev, erq);
#endif
	prism_unlock(priv);
	return err;
}

int prism_hw_get_bssid(struct net_device *dev, char buf[ETH_ALEN])
{
	struct w740prism_priv *priv = dev->priv;
	int err = 0;
	int i = 0;
	
	err = Read_RID_Config(PRISM_RID_CURRENTBSSID, buf);
	
//	for(i = 0; i < 6; i++)
//		printk("%x.", buf[i]);
//	printk("\n");
	return err;
}

int prism_ioctl_getstat(struct net_device *dev, struct iw_point *erq)
{
	int err = 0;
	int status = 0;
//	printk("%s: %d\n", __FILE__,__LINE__);
	prism_cmd_inquiry(PRISM_INQ_ASSOCIATIONSTATUS);
//	printk("%s: %d\n", __FILE__,__LINE__);
	return err;
}


int prism_ioctl_getlinkstat(struct net_device *dev, struct iw_point *erq)
{
	int err = 0;
	int status = 0;
	struct w740prism_priv *priv = (struct w740prism_priv *)dev->priv;
//	printk("%s: %d\n", __FILE__,__LINE__);
	prism_lock(priv);
	if (erq->pointer) {
		if (copy_to_user(erq->pointer, &(priv->status), erq->length))
			return -EFAULT;
	}
	prism_unlock(priv);
	//prism_cmd_inquiry(PRISM_INQ_LINKSTATUS);
//	printk("%s: %d\n", __FILE__,__LINE__);
	return err;
}

int prism_ioctl_getiwrate(struct net_device *dev, struct iw_param *iwp)
{
	int err = 0;
	int rate = 0;
	int i;
	err = Read_RID_Config(PRISM_RID_CURRENTTXRATE, &rate);
	
	for(i = 0; i < BITRATE_TABLE_SIZE; i++)
	{
		if (bitrate_table[i].prism_txratectrl == rate) {
				break;
		}
	}
	if(i >= BITRATE_TABLE_SIZE)
	{
		printk("Can't get correct rate\n");
		i = 0;
	}
	
	iwp->value = bitrate_table[i].bitrate * 100000;
//	printk("Current rate: %d\n", iwp->value);
	iwp->disabled = 0;
	return err;
}

int prism_ioctl_setiwrate(struct net_device *dev, struct iw_param *iwp)
{
	struct w740prism_priv *priv = dev->priv;
	int err = 0;
	int ratemode = -1;
	int bitrate; /* 100s of kilobits */
	int i;
//printk("rate: %d\n", iwp->value);	
	if (iwp->value == -1)
		bitrate = 110;
	else {
		if (iwp->value % 100000)
			return -EINVAL;
		bitrate = iwp->value / 100000;
	}

	if ( (bitrate != 10) && (bitrate != 20) &&
	     (bitrate != 55) && (bitrate != 110) )
		return -EINVAL;

	for (i = 0; i < BITRATE_TABLE_SIZE; i++)
		if ( (bitrate_table[i].bitrate == bitrate)) {
			ratemode = i;
			break;
		}
	
	if (ratemode == -1)
		return -EINVAL;

	prism_lock(priv);
	priv->bitratemode = ratemode;
	prism_unlock(priv);

	return err;
}

int prism_hw_setiwrate(struct w740prism_priv *priv)
{
	int err = 0;
//	printk("ratemode: %x\n", priv->bitratemode);
//
	if (priv->bitratemode >= BITRATE_TABLE_SIZE) {
		printk("%s: Invalid bitrate mode %d\n",
		       "wlan", priv->bitratemode);
		return -EINVAL;
	}
	err = Write_RID_Config(PRISM_RID_TXRATECNTL, &bitrate_table[priv->bitratemode].prism_txratectrl, PRISM_RID_TXRATECNTL_LEN);
	return err;	
}

int prism_ioctl_setrts(struct net_device *dev, struct iw_param *iwp)
{
	int err = 0;
	struct w740prism_priv *priv = dev->priv;
	int val = iwp->value;

	if (iwp->disabled)
		val = 2347;

	if ( (val < 0) || (val > 3000) )
		return -EINVAL;

	prism_lock(priv);
	priv->rts_thresh = val;
	prism_unlock(priv);
	
	return err;
}

int prism_hw_setrts(struct w740prism_priv *priv)
{
	int err = 0;
	
	/* Set RTS threshold */
	err = Write_RID_Config(PRISM_RID_RTSTHRESHOLD, &priv->rts_thresh, PRISM_RID_RTSTHRESHOLD_LEN);
	
	return err;
}

int prism_ioctl_getsens(struct net_device *dev, struct iw_param *iwp)
{
	int err = 0;
	int val = 0;
	
	Read_RID_Config(PRISM_RID_CNFSYSTEMSCALE, &val);
//	printk("Current sense: %d\n", val);
	iwp->value = val;
	iwp->fixed = 0; /* auto */

	return 0;
}

int prism_ioctl_setsens(struct net_device *dev, struct iw_param *iwp)
{
	int err = 0;
	struct w740prism_priv *priv = dev->priv;
	int val = iwp->value;

	if ((val < 1) || (val > 3))
		return -EINVAL;
	
	prism_lock(priv);
	priv->ap_density = val;
	prism_unlock(priv);

	return err;
}

int prism_hw_setapdensity(struct w740prism_priv *priv)
{
	int err = 0;
	err = Write_RID_Config(PRISM_RID_CNFSYSTEMSCALE, &priv->ap_density, PRISM_RID_CNFSYSTEMSCALE_LEN);
	if (err)
		return err;
	return err;
}

int prism_ioctl_getpower(struct net_device *dev, struct iw_param *iwp)
{
	int err = 0;
	struct w740prism_priv *priv = dev->priv;
	u16 enable, period, timeout, mcast;
	
	prism_lock(priv);
	
	Read_RID_Config(PRISM_RID_CNFPMENABLED, &enable);
	Read_RID_Config(PRISM_RID_CNFMAXSLEEPDURATION, &period);
	Read_RID_Config(PRISM_RID_CNFPMHOLDOVERDURATION, &timeout);
	Read_RID_Config(PRISM_RID_CNFMULTICASTRECEIVE, &mcast);
	
	iwp->disabled = !enable;
	/* Note : by default, display the period */
	if ((iwp->flags & IW_POWER_TYPE) == IW_POWER_TIMEOUT) {
		iwp->flags = IW_POWER_TIMEOUT;
		iwp->value = timeout * 1000;
	} else {
		iwp->flags = IW_POWER_PERIOD;
		iwp->value = period * 1000;
	}
	if (mcast)
		iwp->flags |= IW_POWER_ALL_R;
	else
		iwp->flags |= IW_POWER_UNICAST_R;

 out:
	prism_unlock(priv);
	return err;
}

int prism_ioctl_setpower(struct net_device *dev, struct iw_param *iwp)
{
	int err = 0;
	struct w740prism_priv *priv = dev->priv;
	
	prism_lock(priv);
	if (iwp->disabled) {
		priv->pm_on = 0;
	} else {
		switch (iwp->flags & IW_POWER_MODE) {
		case IW_POWER_UNICAST_R:
			priv->pm_mcast = 0;
			priv->pm_on = 1;
			break;
		case IW_POWER_ALL_R:
			priv->pm_mcast = 1;
			priv->pm_on = 1;
			break;
		case IW_POWER_ON:
			break;
		default:
			err = -EINVAL;
		}
		
		if (err)
			goto out;
		
		if (iwp->flags & IW_POWER_TIMEOUT) {
			priv->pm_on = 1;
			priv->pm_timeout = iwp->value / 1000;
		}
		if (iwp->flags & IW_POWER_PERIOD) {
			priv->pm_on = 1;
			priv->pm_period = iwp->value / 1000;
		}
		if(!priv->pm_on) {
			err = -EINVAL;
			goto out;
		}			
	}
out:
	prism_unlock(priv);
	return err;
}


int prism_hw_setpower(struct w740prism_priv *priv)
{
	int err = 0;
	err = Write_RID_Config(PRISM_RID_CNFPMENABLED, &priv->pm_on, PRISM_RID_CNFPMENABLED_LEN);
	if (err)
		return err;
	err = Write_RID_Config(PRISM_RID_CNFMAXSLEEPDURATION, &priv->pm_period, PRISM_RID_CNFMAXSLEEPDURATION_LEN);
	if (err)
		return err;
	err = Write_RID_Config(PRISM_RID_CNFPMHOLDOVERDURATION, &priv->pm_timeout, PRISM_RID_CNFPMHOLDOVERDURATION_LEN);
	if (err)
		return err;		
	err = Write_RID_Config(PRISM_RID_CNFMULTICASTRECEIVE, &priv->pm_mcast, PRISM_RID_CNFMULTICASTRECEIVE_LEN);
	if (err)
		return err;	
	return err;
}

int get_scan_result(UINT16 infoid, int lens, struct w740prism_priv *priv)
{
	int result = 0;
	int len, j, i, cnt;
#ifdef SCAN_ENABLE
	struct prism_ScanResult * scan;
	prism_lock(priv);
	 
	len = Read_CT_InfoData(infoid, &priv->scan_res, lens);
	scan = &priv->scan_res;
//	printk("len=%d\n", len);
	len = len/2;
	cnt = (len - 2) / 31;
	priv->scan_res_len = cnt;
//	printk("reason[%d], cnt: %d\n", scan->scanreason, cnt);
	for (j=0; j<cnt; j++)
	{
#if 1
    	if (scan->result[j].chid > 14) {
    		LOGPRINT;
    		break;
    	}
//		printk("off: %d, j: %d\n", scan->result+j, j);
//   		printk("channel[%d]\n", scan->result[j].chid);
//
//		printk("bssid [");
//   		for (i=0; i<6; i+=2)
//   			printk(" %02x %02x", scan->result[j].bssid[i+1], scan->result[j].bssid[i]);
//		printk("]\n");
//		printk("ssid [");
//		len = scan->result[j].ssid.ssidLen;
//		if (len % 2)
//		{
//			for (i=0; i<len-1; i+=2)
//				printk(" %02x %02x", scan->result[j].ssid.ssidName[i+1], scan->result[j].ssid.ssidName[i]);
//			printk(" %02x", scan->result[j].ssid.ssidName[len]);
//		}
//		else
//		{
//			for (i=0; i<len; i+=2)
//				printk(" %02x %02x", scan->result[j].ssid.ssidName[i+1], scan->result[j].ssid.ssidName[i]);
//		}
//		printk("]\n");
#endif
	}	
	up(&priv->sema);
	prism_unlock(priv);
#endif
	return result;
}

int prism_ioctl_getaplist(struct net_device *dev, struct iw_point * iwp)
{
	int err = 0;
	struct w740prism_priv *priv = dev->priv;
	int num = 0;
	int i, len;
#if 1
	int j;
#endif
	prism_ScanResultSub_t *resultp = &(priv->scan_res.result[0]);
DEBUG("iwp: %x, iwp->length: %d\n", iwp, iwp->length);	
	prism_lock(priv);

	if(iwp->length == 0)
	{
		iwp->length = priv->scan_res_len;
		DEBUG("iwp->length: %d\n", iwp->length);
	}
#if 1
	else
	{
		len = (iwp->length > priv->scan_res_len ? priv->scan_res_len:iwp->length);
		for(i = 0; i < len; i++)
		{
			if (iwp->pointer)
				if ( copy_to_user(iwp->pointer + sizeof(prism_cnfDesireSSID_t)*i, 
									&((resultp+i)->ssid), 
									sizeof(prism_cnfDesireSSID_t) ) 
					)
					return -EFAULT;	
#if 0
			for(j = 0; j < ((resultp+i)->ssid).ssidLen; j++)
			{
				printk("%c", ((resultp+i)->ssid).ssidName[j]);
			}
			printk("\n");
#endif
		}
	}
#endif
	prism_unlock(priv);
	return err;
}

void set_port_type(struct w740prism_priv *priv)
{
	switch (priv->iw_mode) {
	case IW_MODE_INFRA:
		priv->port_type = 1;
		priv->allow_ibss = 0;
		break;
	case IW_MODE_ADHOC:
		if (priv->prefer_port3) {
			priv->port_type = 3;
			priv->allow_ibss = 0;
		} else {
			priv->port_type = 0;//priv->ibss_port;
			priv->allow_ibss = 1;
		}
		break;
	default:
		printk("%s: Invalid priv->iw_mode\n",
		       priv->ndev->name);
	}
}

int prism_ioctl_setspy(struct net_device *dev, struct iw_point *iwp)
{
	int result = 0;
	struct w740prism_priv *priv = dev->priv;
	struct sockaddr address[IW_MAX_SPY];
	int number = iwp->length;
	int i;
	
	if (number > IW_MAX_SPY)
		return -E2BIG;

	if (iwp->pointer) {
		if (copy_from_user(address, iwp->pointer,
				   sizeof(struct sockaddr) * number))
			return -EFAULT;
	}

	prism_lock(priv);
	priv->spy_number = 0;

	if (number > 0) {
		for (i = 0; i < number; i++)
			memcpy(priv->spy_address[i], address[i].sa_data,
			       ETH_ALEN);
		memset(priv->spy_stat, 0,
		       sizeof(struct iw_quality) * IW_MAX_SPY);
		priv->spy_number = number;
	}

	DEBUG("%s: New spy list:\n", dev->name);
	for (i = 0; i < number; i++) {
		DEBUG("%s: %d - %02x:%02x:%02x:%02x:%02x:%02x\n",
		      dev->name, i+1,
		      priv->spy_address[i][0], priv->spy_address[i][1],
		      priv->spy_address[i][2], priv->spy_address[i][3],
		      priv->spy_address[i][4], priv->spy_address[i][5]);
	}

	prism_unlock(priv);
	return result;
}

int prism_ioctl_getspy(struct net_device *dev, struct iw_point *iwp)
{
	struct w740prism_priv *priv = dev->priv;
	struct sockaddr address[IW_MAX_SPY];
	struct iw_quality spy_stat[IW_MAX_SPY];
	int number;
	int i;

	prism_lock(priv);

	number = priv->spy_number;
	if ((number > 0) && (iwp->pointer)) {
		for (i = 0; i < number; i++) {
			memcpy(address[i].sa_data, priv->spy_address[i],
			       ETH_ALEN);
			address[i].sa_family = AF_UNIX;
		}
		/* Copy stats */
		memcpy(&spy_stat, priv->spy_stat,
		       sizeof(struct iw_quality) * IW_MAX_SPY);
		for (i=0; i < number; i++)
			priv->spy_stat[i].updated = 0;
	}

	prism_unlock(priv);

	iwp->length = number;
	if(copy_to_user(iwp->pointer, address,
			 sizeof(struct sockaddr) * number))
		return -EFAULT;
	if(copy_to_user(iwp->pointer + (sizeof(struct sockaddr)*number),
			&spy_stat, sizeof(struct iw_quality) * number))
		return -EFAULT;

	return 0;
}
