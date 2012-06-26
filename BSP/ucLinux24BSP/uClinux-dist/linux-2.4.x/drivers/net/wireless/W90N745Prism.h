#ifndef __W90N745PRISM_H__
#define __W90N745PRISM_H__


#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <linux/skbuff.h>
#include <linux/wireless.h>

#if LINUX_VERSION_CODE < 132116
	#define SIOCSIWSCAN	0x8B18		/* trigger scanning */
	#define SIOCGIWSCAN	0x8B19		/* get scanning results */
#endif

//#define WDEBUG

#define UINT16 	short
#define INT16 	short
#define UINT8 	unsigned char
#define UINT32 	unsigned int

#define __WLAN_ATTRIB_PACK__		__attribute__ ((packed))

/* read / write prism register macro */
#define WRITEREG(reg, data) 	(*(volatile unsigned short *)(EBI_BASE + reg)) = data
#define READREG(reg)        	*(volatile unsigned short *)(EBI_BASE + reg)

// ASIC Address Definitions
#define VPint   	*(volatile unsigned int *)
#define VPshort 	*(volatile unsigned short *)
#define VPchar  	*(volatile unsigned char *)

// external I/O 2 control register
#define EXT0CON_REG		0xFFF01018
#define EXT1CON_REG  	0xFFF0101C
#define EXT2CON_REG  	0xFFF01020

// Special Register Start Address After System Reset
#define EBI_BASE     	0xC0800000   // on W90N745 board PCMCIA

// Advanced Interrupt Controller Registers
#define AIC_SCR_nIRQ0  	(VPint(Base_Addr+0x00082008))
#define AIC_SCR_nIRQ2  	(VPint(Base_Addr+0x00082010))

// MAC Interrupt Source
#define nIRQ0	2
#define nIRQ2	4

#define DIAG_PATTERNA	((UINT16)0xaaaa)
#define DIAG_PATTERNB 	((UINT16)~0xaaaa)

#define HERMES_MAGIC	(0x7d1f)

#define USER_BAP		0
#define IRQ_BAP			1

/* operation mode */
#define IW_MODE_AUTO	0	/* Let the driver decides */
#define IW_MODE_ADHOC	1	/* Single cell network */
#define IW_MODE_INFRA	2	/* Multi cell network, roaming, ... */
/*---- follow no support now -----*/
#define IW_MODE_MASTER	3	/* Synchronisation master or Access Point */
#define IW_MODE_REPEAT	4	/* Wireless Repeater (forwarder) */
#define IW_MODE_SECOND	5	/* Secondary master/repeater (backup) */

/*------------------------------------------------------------------------*/
/*                         PRISM MAC registers                            */
/*	                          Command/Status                              */
/*------------------------------------------------------------------------*/
#define  PRISM_CMD				0x00
#define  PRISM_PARAM0           0x02
#define  PRISM_PARAM1           0x04
#define  PRISM_PARAM2           0x06
#define  PRISM_STATUS			0x08
#define  PRISM_RESP0            0x0A
#define  PRISM_RESP1            0x0C
#define  PRISM_RESP2            0x0E
// FID Management
#define  PRISM_INFOFID          0x10
#define  PRISM_RXFID            0x20
#define  PRISM_ALLOCFID			0x22
#define  PRISM_TXCOMPLFID		0x24
// BAP0
#define  PRISM_SELECT0			0x18
#define  PRISM_OFFSET0			0x1C
#define  PRISM_DATA0            0x36
// BAP1
#define  PRISM_SELECT1			0x1A
#define  PRISM_OFFSET1			0x1E
#define  PRISM_DATA1				0x38
// Event
#define  PRISM_EVSTAT 			0x30
#define  PRISM_INTEN 				0x32
#define  PRISM_EVACK				0x34
// Control
#define  PRISM_CONTROL			0x14
// Host Software
#define  PRISM_SWSUPPORT0		0x28
#define  PRISM_SWSUPPORT1		0x2A
#define  PRISM_SWSUPPORT2		0x2C
// Auxiliary Port
#define  PRISM_AUXPAGE			0x3A
#define  PRISM_AUXOFFSET		0x3C
#define  PRISM_AUXDATA			0x3E

// Command Code Constants
// Controller Commands
#define  PRISM_CMDCODE_INIT		(0x00)
#define  PRISM_CMDCODE_ENABLE	(0x01)
#define  PRISM_CMDCODE_DISABLE	(0x02)
#define  PRISM_CMDCODE_DIAG		(0x03)

// Buffer Mgmt Commands
#define  PRISM_CMDCODE_ALLOC	(0x0A)
#define  PRISM_CMDCODE_TX		(0x0B)

// Regulate Commands
#define  PRISM_CMDCODE_NOTIFY	(0x10)
#define  PRISM_CMDCODE_INQ		(0x11)

// Configure Commands
#define  PRISM_CMDCODE_ACCESS	(0x21)
#define  PRISM_CMDCODE_DOWNLD	(0x22)

// Serial I/O Commands
#define  PRISM_CMDCODE_READMIF	(0x30)
#define  PRISM_CMDCODE_WRITEMIF	(0x31)

/*--- Result Codes --------------------------*/
#define  PRISM_SUCCESS			((0x00))
#define  PRISM_CARD_FAIL		((0x01))
#define  PRISM_NO_BUFF			((0x05))
#define  PRISM_CMD_ERR			((0x7F))



/*------------- Programming Modes ------------------------*/
/*	MODE 0: Disable programming		    	  */
/*	MODE 1: Enable volatile memory programming 	  */
/*	MODE 2: Enable non-volatile memory programming    */
/*	MODE 3: Program non-volatile memory section  	  */
/*--------------------------------------------------------*/
#define PRISM_PROGMODE_DISABLE		(0x00)		//no used
#define PRISM_PROGMODE_RAM			(0x01)		//no used
#define PRISM_PROGMODE_NV			(0x02)		//no used
#define PRISM_PROGMODE_NVWRITE		(0x03)		//no used

/*------------------ Register Test/Get/Set Field macros ------------------------*/
#define  PRISM_CMD_ISBUSY(value)         	((UINT16)(((UINT16)value) & 0x8000))
#define  PRISM_CMD_AINFO_GET(value)      	((UINT16)(((UINT16)(value) & 0x7F00) >> 8))
#define  PRISM_CMD_AINFO_SET(value)      	((UINT16)((UINT16)(value) << 8))
#define  PRISM_CMD_MACPORT_GET(value)    	((UINT16)(PRISM_CMD_AINFO_GET((UINT16)(value) & 0x0700)))
#define  PRISM_CMD_MACPORT_SET(value)    	((UINT16)PRISM_CMD_AINFO_SET(value))
#define  PRISM_CMD_ISRECL(value)         	((UINT16)(PRISM_CMD_AINFO_GET((UINT16)(value) & 0x0100)))
#define  PRISM_CMD_RECL_SET(value)       	((UINT16)PRISM_CMD_AINFO_SET(value))
#define  PRISM_CMD_ISWRITE(value)        	((UINT16)(PRISM_CMD_AINFO_GET((UINT16)(value) & 0x0100)))
#define  PRISM_CMD_WRITE_SET(value)      	((UINT16)PRISM_CMD_AINFO_SET((UINT16)value))
#define  PRISM_CMD_PROGMODE_GET(value)   	((UINT16)(PRISM_CMD_AINFO_GET((UINT16)(value) & 0x0300)))
#define  PRISM_CMD_PROGMODE_SET(value)   	((UINT16)PRISM_CMD_AINFO_SET((UINT16)value))
#define  PRISM_CMD_CMDCODE_GET(value)    	((UINT16)(((UINT16)(value)) & 0x003F))
#define  PRISM_CMD_CMDCODE_SET(value)    	((UINT16)(value))

#define  PRISM_STATUS_RESULT_GET(value)  	((UINT16)((((UINT16)(value)) & 0x7F00) >> 8))
#define  PRISM_STATUS_RESULT_SET(value)  	(((UINT16)(value)) << 8)
#define  PRISM_STATUS_CMDCODE_GET(value) 	(((UINT16)(value)) & 0x003F)
#define  PRISM_STATUS_CMDCODE_SET(value) 	((UINT16)(value))

#define  PRISM_OFFSET_ISBUSY(value)      	((UINT16)(((UINT16)(value)) & 0x8000))
#define  PRISM_OFFSET_ISERR(value)       	((UINT16)(((UINT16)(value)) & 0x4000))
#define  PRISM_OFFSET_DATAOFF_GET(value) 	((UINT16)(((UINT16)(value)) & 0x0FFE))
#define  PRISM_OFFSET_DATAOFF_SET(value) 	((UINT16)(value))

#define  PRISM_EVSTAT_ISTICK(value)      	((UINT16)(((UINT16)(value)) & 0x8000))
#define  PRISM_EVSTAT_ISWTERR(value)     	((UINT16)(((UINT16)(value)) & 0x4000))
#define  PRISM_EVSTAT_ISINFDROP(value)   	((UINT16)(((UINT16)(value)) & 0x2000))
#define  PRISM_EVSTAT_ISINFO(value)      	((UINT16)(((UINT16)(value)) & 0x0080))
#define  PRISM_EVSTAT_ISDTIM(value)      	((UINT16)(((UINT16)(value)) & 0x0020))
#define  PRISM_EVSTAT_ISCMD(value)       	((UINT16)(((UINT16)(value)) & 0x0010))
#define  PRISM_EVSTAT_ISALLOC(value)     	((UINT16)(((UINT16)(value)) & 0x0008))
#define  PRISM_EVSTAT_ISTXEXC(value)     	((UINT16)(((UINT16)(value)) & 0x0004))
#define  PRISM_EVSTAT_ISTX(value)        	((UINT16)(((UINT16)(value)) & 0x0002))
#define  PRISM_EVSTAT_ISRX(value)        	((UINT16)(((UINT16)(value)) & 0x0001))

#define  PRISM_EVENT_MASK					((UINT16)0xFFFF)
#define  PRISM_INTEN_ENABLEBITS				((UINT16)0x008F)
#define	 PRISM_INTEN_DISABLE				((UINT16)0x0000)

#define  PRISM_INTEN_ISTICK(value)       	((UINT16)(((UINT16)(value)) & 0x8000))
#define  PRISM_INTEN_TICK_SET(value)     	((UINT16)(((UINT16)(value)) << 15))
#define  PRISM_INTEN_ISWTERR(value)      	((UINT16)(((UINT16)(value)) & 0x4000))
#define  PRISM_INTEN_WTERR_SET(value)    	((UINT16)(((UINT16)(value)) << 14))
#define  PRISM_INTEN_ISINFDROP(value)    	((UINT16)(((UINT16)(value)) & 0x2000))
#define  PRISM_INTEN_INFDROP_SET(value)		((UINT16)(((UINT16)(value)) << 13))
#define  PRISM_INTEN_ISINFO(value)       	((UINT16)(((UINT16)(value)) & 0x0080))
#define  PRISM_INTEN_INFO_SET(value)     	((UINT16)(((UINT16)(value)) << 7))
#define  PRISM_INTEN_ISDTIM(value)       	((UINT16)(((UINT16)(value)) & 0x0020))
#define  PRISM_INTEN_DTIM_SET(value)     	((UINT16)(((UINT16)(value)) << 5))
#define  PRISM_INTEN_ISCMD(value)        	((UINT16)(((UINT16)(value)) & 0x0010))
#define  PRISM_INTEN_CMD_SET(value)      	((UINT16)(((UINT16)(value)) << 4))
#define  PRISM_INTEN_ISALLOC(value)      	((UINT16)(((UINT16)(value)) & 0x0008))
#define  PRISM_INTEN_ALLOC_SET(value)    	((UINT16)(((UINT16)(value)) << 3))
#define  PRISM_INTEN_ISTXEXC(value)      	((UINT16)(((UINT16)(value)) & 0x0004))
#define  PRISM_INTEN_TXEXC_SET(value)    	((UINT16)(((UINT16)(value)) << 2))
#define  PRISM_INTEN_ISTX(value)         	((UINT16)(((UINT16)(value)) & 0x0002))
#define  PRISM_INTEN_TX_SET(value)       	((UINT16)(((UINT16)(value)) << 1))
#define  PRISM_INTEN_ISRX(value)         	((UINT16)(((UINT16)(value)) & 0x0001))
#define  PRISM_INTEN_RX_SET(value)       	((UINT16)(((UINT16)(value)) << 0))

#define  PRISM_EVACK_ISTICK(value)       	((UINT16)(((UINT16)(value)) & 0x8000))
#define  PRISM_EVACK_TICK_SET(value)     	((UINT16)(((UINT16)(value)) << 15))
#define  PRISM_EVACK_ISWTERR(value)      	((UINT16)(((UINT16)(value)) & 0x4000))
#define  PRISM_EVACK_WTERR_SET(value)    	((UINT16)(((UINT16)(value)) << 14))
#define  PRISM_EVACK_ISINFDROP(value)    	((UINT16)(((UINT16)(value)) & 0x2000))
#define  PRISM_EVACK_INFDROP_SET(value)  	((UINT16)(((UINT16)(value)) << 13))
#define  PRISM_EVACK_ISINFO(value)       	((UINT16)(((UINT16)(value)) & 0x0080))
#define  PRISM_EVACK_INFO_SET(value)     	((UINT16)(((UINT16)(value)) << 7))
#define  PRISM_EVACK_ISDTIM(value)       	((UINT16)(((UINT16)(value)) & 0x0020))
#define  PRISM_EVACK_DTIM_SET(value)     	((UINT16)(((UINT16)(value)) << 5))
#define  PRISM_EVACK_ISCMD(value)        	((UINT16)(((UINT16)(value)) & 0x0010))
#define  PRISM_EVACK_CMD_SET(value)      	((UINT16)(((UINT16)(value)) << 4))
#define  PRISM_EVACK_ISALLOC(value)      	((UINT16)(((UINT16)(value)) & 0x0008))
#define  PRISM_EVACK_ALLOC_SET(value)    	((UINT16)(((UINT16)(value)) << 3))
#define  PRISM_EVACK_ISTXEXC(value)      	((UINT16)(((UINT16)(value)) & 0x0004))
#define  PRISM_EVACK_TXEXC_SET(value)    	((UINT16)(((UINT16)(value)) << 2))
#define  PRISM_EVACK_ISTX(value)         	((UINT16)(((UINT16)(value)) & 0x0002))
#define  PRISM_EVACK_TX_SET(value)       	((UINT16)(((UINT16)(value)) << 1))
#define  PRISM_EVACK_ISRX(value)         	((UINT16)(((UINT16)(value)) & 0x0001))
#define  PRISM_EVACK_RX_SET(value)       	((UINT16)(((UINT16)(value)) << 0))

#define  PRISM_CONTROL_AUXEN_SET(value) 	((UINT16)(((UINT16)(value)) << 14))
#define  PRISM_CONTROL_AUXEN_GET(value)  	((UINT16)(((UINT16)(value)) >> 14))

/* Communication Tallies' inquiry constants and data types */

#define PRISM_INQ_TALLIES					(0xF100)
#define PRISM_INQ_SCAN						(0xF101)
#define PRISM_INQ_LINKSTATUS				(0xF200)
#define PRISM_INQ_ASSOCIATIONSTATUS			(0xF201)
/*-------------------- Record ID Constants ----------------------------------------*/

/*----------------------------------------------------------------------------------*/
/*     Configuration RIDs: Network Parameters, Static Configuration Entities	    */
/*----------------------------------------------------------------------------------*/
#define PRISM_RID_CNFPORTTYPE            	((UINT16)0xFC00)
#define PRISM_RID_CNFPORTTYPE_LEN        	((UINT16)2)
#define PRISM_RID_CNFOWNMACADDR          	((UINT16)0xFC01)
#define PRISM_RID_CNFOWNMACADDR_LEN      	((UINT16)6)
#define PRISM_RID_CNFDESIREDSSID         	((UINT16)0xFC02)
#define PRISM_RID_CNFDESIREDSSID_LEN     	((UINT16)34)
#define PRISM_RID_CNFOWNCHANNEL          	((UINT16)0xFC03)
#define PRISM_RID_CNFOWNCHANNEL_LEN      	((UINT16)2)
#define PRISM_RID_CNFOWNSSID             	((UINT16)0xFC04)
#define PRISM_RID_CNFOWNSSID_LEN         	((UINT16)34)
#define PRISM_RID_CNFSYSTEMSCALE			((UINT16)0xFC06)
#define PRISM_RID_CNFSYSTEMSCALE_LEN		((UINT16)2)
#define PRISM_RID_CNFMAXDATALEN          	((UINT16)0xFC07)
#define PRISM_RID_CNFMAXDATALEN_LEN      	((UINT16)2)
#define PRISM_RID_CNFPMENABLED				((UINT16)0xFC09)
#define PRISM_RID_CNFPMENABLED_LEN			((UINT16)2)
#define PRISM_RID_CNFMULTICASTRECEIVE		((UINT16)0xFC0B)
#define PRISM_RID_CNFMULTICASTRECEIVE_LEN	((UINT16)2)
#define PRISM_RID_CNFMAXSLEEPDURATION		((UINT16)0xFC0C)
#define PRISM_RID_CNFMAXSLEEPDURATION_LEN	((UINT16)2)
#define PRISM_RID_CNFPMHOLDOVERDURATION		((UINT16)0xFC0D)
#define PRISM_RID_CNFPMHOLDOVERDURATION_LEN	((UINT16)2)
#define PRISM_RID_CNFWEPDEFAULTKEYID		((UINT16)0xFC23)
#define PRISM_RID_CNFWEPDEFAULTKEYID_LEN	((UINT16)sizeof(UINT16))
#define PRISM_RID_CNFDEFAULTKEY0			((UINT16)0xFC24)
#define PRISM_RID_CNFDEFAULTKEY1			((UINT16)0xFC25)
#define PRISM_RID_CNFDEFAULTKEY2			((UINT16)0xFC26)
#define PRISM_RID_CNFDEFAULTKEY3			((UINT16)0xFC27)
#define PRISM_RID_CNFWEPFLAGS				((UINT16)0xFC28)
#define PRISM_RID_CNFWEPFLAGS_LEN			((UINT16)sizeof(UINT16))
#define PRISM_RID_CNFAUTHENTICATION      	((UINT16)0xFC2A)
#define PRISM_RID_CNFAUTHENTICATION_LEN  	((UINT16)sizeof(UINT16))
#define PRISM_RID_CNFBASICRATES          	((UINT16)0xFCB3)
#define PRISM_RID_CNFBASICRATES_LEN      	((UINT16)2)
#define PRISM_RID_CNFSUPPRATES           	((UINT16)0xFCB4)
#define PRISM_RID_CNFSUPPRATES_LEN       	((UINT16)2)
#define PRISM_RID_TXRATECNTL0            	((UINT16)0xFC9E)
#define PRISM_RID_TXRATECNTL0_LEN        	((UINT16)2)

#define PRISM_RID_CNFBEACONINTERVAL      	((UINT16)0xFC33)
#define PRISM_RID_CNFBEACONINTERVAL_LEN  	((UINT16)2)
#define PRISM_RID_CNFOWNDTIMPERIOD       	((UINT16)0xFC10)
#define PRISM_RID_CNFOWNDTIMPERIOD_LEN   	((UINT16)2)


/* Power management flags available (along with the value, if any) */
#define IW_POWER_ON				0x0000	/* No details... */
#define IW_POWER_TYPE			0xF000	/* Type of parameter */
#define IW_POWER_PERIOD			0x1000	/* Value is a period/duration of  */
#define IW_POWER_TIMEOUT		0x2000	/* Value is a timeout (to go asleep) */
#define IW_POWER_MODE			0x0F00	/* Power Management mode */
#define IW_POWER_UNICAST_R		0x0100	/* Receive only unicast messages */
#define IW_POWER_MULTICAST_R	0x0200	/* Receive only multicast messages */
#define IW_POWER_ALL_R			0x0300	/* Receive all messages though PM */
#define IW_POWER_FORCE_S		0x0400	/* Force PM procedure for sending unicast */
#define IW_POWER_REPEATER		0x0800	/* Repeat broadcast messages in PM period */
#define IW_POWER_MODIFIER		0x000F	/* Modify a parameter */
#define IW_POWER_MIN			0x0001	/* Value is a minimum  */
#define IW_POWER_MAX			0x0002	/* Value is a maximum */
#define IW_POWER_RELATIVE		0x0004	/* Value is not in seconds/ms/us */

/*--------------------------------------------------------------------------------*/
/*  Configuration RIDs: Network Parameters, Dynamic Configuration Entities	  */
/*--------------------------------------------------------------------------------*/
#define PRISM_RID_GROUPADDRESSES      	((UINT16)0xFC80)
#define PRISM_RID_NFCREATEIBSS			((UINT16)0xFC81)
#define PRISM_RID_NFCREATEIBSS_LEN		((UINT16)2)
#define PRISM_RID_RTSTHRESHOLD			((UINT16)0xFC83)
#define PRISM_RID_RTSTHRESHOLD_LEN		((UINT16)2)
#define PRISM_RID_TXRATECNTL          	((UINT16)0xFC84)
#define PRISM_RID_TXRATECNTL_LEN      	((UINT16)2)

#define PRISM_RID_PROMISCUOUS         	((UINT16)0xFC85)
#define PRISM_RID_PROMISCUOUS_LEN     	((UINT16)2)
#define PRISM_Promoscuous_Enable      	((UINT16)0x0001)
#define PRISM_Promoscuous_Disable     	((UINT16)0x0000)


/*------------------------------------------------------------------------------*/
/*				Information RIDs: NIC Information		*/
/*------------------------------------------------------------------------------*/
#define PRSIM_NIC_DOWNLOADBUFFER	((UINT16)0xFD01)
#define PRISM_RID_CIS         		((UINT16)0xFD13)
#define PRISM_RID_CIS_LEN     		((UINT16)480)


/*-------------------------------------------------------------------------*/
/*				Information RIDs:  MAC Information         */
/*-------------------------------------------------------------------------*/
#define PRISM_RID_PORTSTATUS         	((UINT16)0xFD40)
#define PRISM_RID_PORTSTATUS_LEN     	((UINT16)0)
#define PRISM_RID_CURRENTSSID        	((UINT16)0xFD41)
#define PRISM_RID_CURRENTSSID_LEN    	((UINT16)34)
#define PRISM_RID_CURRENTBSSID       	((UINT16)0xFD42)
#define PRISM_RID_CURRENTBSSID_LEN   	((UINT16)6)
#define PRISM_RID_COMMSQUALITY       	((UINT16)0xFD43)
#define PRISM_RID_COMMSQUALITY_LEN   	((UINT16)6)
#define PRISM_RID_CURRENTTXRATE      	((UINT16)0xFD44)
#define PRISM_RID_CURRENTTXRATE_LEN  	((UINT16)2)
#define PRISM_RID_OWNMACADDRESS			((UINT16)0xFD86)
#define PRISM_RID_SCANRESULT			((UINT16)0xFD88)
/*------------------------------------------------------------------------------*/
/*				Information RIDs: Modem information 		*/
/*------------------------------------------------------------------------------*/
#define PRISM_RID_CURRENTCHANNEL		((UINT16)0xFDC1)	/* used */

/*----------------------------------------------------------------------*/
/*				Information Frames			*/
/*----------------------------------------------------------------------*/
#define PRISM_CT_LINKSTATUS        		((UINT16)0xF200)
#define PRISM_CT_SCANRESUTL				((UINT16)0xF101)

/*--------------------------------------------------------------------------------------*/
/*     Communication Frames: Test/Get/Set Field Values for Transmit Frames		*/
/*--------------------------------------------------------------------------------------*/
/*--------------- Status Field ---------------*/
#define PRISM_TXSTATUS_ISFORMERR(v)    	((UINT16)(((UINT16)(v)) & 0x0008))
#define PRISM_TXSTATUS_ISDISCON(v)     	((UINT16)(((UINT16)(v)) & 0x0004))
#define PRISM_TXSTATUS_ISAGEDERR(v)    	((UINT16)(((UINT16)(v)) & 0x0002))
#define PRISM_TXSTATUS_ISRETRYERR(v)   	((UINT16)(((UINT16)(v)) & 0x0001))

#define PRISM_TX_GET(v,m,s)           	((((UINT16)(v))&((UINT16)(m)))>>((UINT16)(s)))
#define PRISM_TX_SET(v,m,s)           	((((UINT16)(v))<<((UINT16)(s)))&((UINT16)(m)))

#define PRISM_TX_MACPORT_GET(v)        	PRISM_TX_GET(v, 0x0700, 8)
#define PRISM_TX_MACPORT_SET(v)        	PRISM_TX_SET(v, 0x0700, 8)
#define PRISM_TX_NOENCRYPT_GET(v)      	PRISM_TX_GET(v, 0x0080, 7)
#define PRISM_TX_NOENCRYPT_SET(v)      	PRISM_TX_SET(v, 0x0080, 7)
#define PRISM_TX_RETRYSTRAT_GET(v)     	PRISM_TX_GET(v, 0x0020, 5)
#define PRISM_TX_RETRYSTRAT_SET(v)     	PRISM_TX_SET(v, 0x0020, 5)
#define PRISM_TX_STRUCTYPE_GET(v)      	PRISM_TX_GET(v, 0x0018, 3)
#define PRISM_TX_STRUCTYPE_SET(v)      	PRISM_TX_SET(v, 0x0018, 3)
#define PRISM_TX_TXEX_GET(v)           	PRISM_TX_GET(v, 0x0004, 2)
#define PRISM_TX_TXEX_SET(v)           	PRISM_TX_SET(v, 0x0004, 2)
#define PRISM_TX_TXOK_GET(v)           	PRISM_TX_GET(v, 0x0002, 1)
#define PRISM_TX_TXOK_SET(v)           	PRISM_TX_SET(v, 0x0002, 1)


/*----------------------------------------------------------------------*/
/*		FRAME DESCRIPTORS AND FRAME STRUCTURES			*/
/*		FRAME DESCRIPTORS: Offsets				*/
/*----------------------------------------------------------------------*/
/*-------------Control Info---------------*/
#define  PRISM_FD_STATUS                ((UINT16)0x00)
#define  PRISM_FD_SWSUPPORT            	((UINT16)0x03)
#define  PRISM_FD_SILENCE               ((UINT16)0x03)
#define  PRISM_FD_SIGNAL                ((UINT16)0x03)
#define  PRISM_FD_RATE                  ((UINT16)0x04)
#define  PRISM_FD_TXRATE                ((UINT16)0x05)
#define  PRISM_FD_RETRYCOUNT            ((UINT16)0x05)
#define  PRISM_FD_TXCONTROL             ((UINT16)0x06)

/*-------------802.11 Header---------------*/
#define  PRISM_FD_FRAMECONTROL      	((UINT16)0x07)
#define  PRISM_FD_DURATIONID           	((UINT16)0x08)
#define  PRISM_FD_ADDRESS1              ((UINT16)0x09)
#define  PRISM_FD_ADDRESS2              ((UINT16)0x12)
#define  PRISM_FD_ADDRESS3              ((UINT16)0x15)
#define  PRISM_FD_SEQCONTROL           	((UINT16)0x18)
#define  PRISM_FD_ADDRESS4              ((UINT16)0x19)
#define  PRISM_FD_DATALEN               ((UINT16)0x22) 

/*--------------802.3 Header----------------*/
#define  PRISM_FD_DESTADDRESS         	((UINT16)0x23)
#define  PRISM_FD_SRCADDRESS           	((UINT16)0x26)
#define  PRISM_FD_DATALENGTH           	((UINT16)0x29)

/*--------------prism defines----------------*/
#define PRISM2_TX_FIDSTACKLEN_MAX  	6
#define PRISM2_FIDSTACKLEN_MAX  	16

#define HERMES_EV_CMD				(0x0010)
#define HERMES_STATUS_RESULT		(0x7f00)

#define TX_TIMEOUT  			(HZ)

/*-------------------------for encode---------------------------------*/
/* Maximum number of size of encoding token available
 * they are listed in the range structure */
#define IW_MAX_ENCODING_SIZES	8

/* Maximum size of the encoding token in bytes */
#define IW_ENCODING_TOKEN_MAX	32	/* 256 bits (for now) */

/* Flags for encoding (along with the token) */
#define IW_ENCODE_INDEX			0x00FF	/* Token index (if needed) */
#define IW_ENCODE_FLAGS			0xFF00	/* Flags defined below */
#define IW_ENCODE_MODE			0xF000	/* Modes defined below */
#define IW_ENCODE_DISABLED		0x8000	/* Encoding disabled */
#define IW_ENCODE_ENABLED		0x0000	/* Encoding enabled */
#define IW_ENCODE_RESTRICTED	0x4000	/* Refuse non-encoded packets */
#define IW_ENCODE_OPEN			0x2000	/* Accept non-encoded packets */
#define IW_ENCODE_NOKEY         0x0800  /* Key is write only, so not present */


/*---------------------------------------------------------------------------*/
/*		Communication Frame: Transmit Frame Structure		     */
/*---------------------------------------------------------------------------*/
struct hermes_tx_descriptor {
	u16 status;
	u16 reserved1;
	u16 reserved2;
	u32 sw_support;
	u8 retry_count;
	u8 tx_rate;
	u16 tx_control;	
} __attribute__ ((packed));

struct header_struct {
	/* 802.3 */
	u8 dest[ETH_ALEN];
	u8 src[ETH_ALEN];
	u16 len;
	/* 802.2 */
	u8 dsap;
	u8 ssap;
	u8 ctrl;
	/* SNAP */
	u8 oui[3];
	u16 ethertype;
} __attribute__ ((packed));

struct prism_tx_frame
{
	UINT16	status;
	UINT16	reserved1;
	UINT16	reserved2;
	UINT16	sw_support0;
	UINT16	sw_support1;
	UINT16	reserved3;
	UINT16	tx_control;

	/*-- 802.11 Header Information --*/

	UINT16	frame_control;
	UINT16	duration_id;
	UINT8	address1[6];
	UINT8	address2[6];
	UINT8	address3[6];
	UINT16	sequence_control;
	UINT8	address4[6];
	UINT16	data_len;             /* little endian format */

	/*-- 802.3 Header Information --*/

	UINT8	dest_addr[6];
	UINT8	src_addr[6];
	UINT16	data_length;          /* big endian format */
	UINT8   llc_data[6];
} __attribute__ ((packed));;

typedef struct prism_tx_frame prism_tx_frame_t;

/*----------------------------------------------------------------*/
/*	Communication Frame: Receive Frame Structure 		  */
/*----------------------------------------------------------------*/
struct prism_rx_frame
{
	/*-- MAC rx descriptor (prism byte order) --*/
	UINT16	status;
	UINT16	reserved0;
	UINT16	reserved1;
	UINT8	silence;
	UINT8	signal;
	UINT8	rate;
	UINT8	reserved2;
	UINT16	reserved3;
	UINT16	reserved4;

	/*-- 802.11 Header Information (802.11 byte order) --*/
	UINT16	frame_control;
	UINT16	duration_id;
	UINT8	address1[6];
	UINT8	address2[6];
	UINT8	address3[6];
	UINT16	sequence_control;
	UINT8	address4[6];
	UINT16	data_len;             /* little endian format */

	/*-- 802.3 Header Information --*/
	UINT8	dest_addr[6];
	UINT8	src_addr[6];
	UINT16	data_length;          /* big endian format */
} __attribute__ ((packed));
typedef struct prism_rx_frame prism_rx_frame_t;

/*------------------------------------------------------------------*/
/*	MAC state structure, argument to all functions		    */
/*------------------------------------------------------------------*/
/*--prism hardware record----*/
typedef struct prism_record 
{
 		UINT16 reclen;
    	UINT16 rid;
} prism_rec_t;

/*------ RID Structure---------*/
typedef struct prism_cnfDesireSSID
{
    	UINT16 ssidLen __WLAN_ATTRIB_PACK__;
    	UINT8  ssidName[32] __WLAN_ATTRIB_PACK__;
}__WLAN_ATTRIB_PACK__ prism_cnfDesireSSID_t;


typedef struct prism_pdaRec
{
    	unsigned short len;
    	unsigned short id;
    	unsigned short buf[60];
} prism_pdaRec_t;

#define SCAN_ENABLE
#ifdef SCAN_ENABLE

typedef struct prism_ScanResultSub
{
	UINT16	chid __WLAN_ATTRIB_PACK__;				
	UINT16	anl __WLAN_ATTRIB_PACK__;
	UINT16	sl __WLAN_ATTRIB_PACK__;
	UINT8	bssid[6] __WLAN_ATTRIB_PACK__;
	UINT16	bcnint __WLAN_ATTRIB_PACK__;
	UINT16	capinfo __WLAN_ATTRIB_PACK__;
	struct prism_cnfDesireSSID ssid __WLAN_ATTRIB_PACK__;
	UINT8	supprates[10] __WLAN_ATTRIB_PACK__; /* 802.11 info element */
	UINT16	proberesp_rate __WLAN_ATTRIB_PACK__;
}__WLAN_ATTRIB_PACK__ prism_ScanResultSub_t;

typedef struct prism_ScanResult
{
	UINT16	rsvd __WLAN_ATTRIB_PACK__;
	UINT16	scanreason __WLAN_ATTRIB_PACK__;
	struct prism_ScanResultSub result[35] __WLAN_ATTRIB_PACK__;
}__WLAN_ATTRIB_PACK__ prism_ScanResult_t;

#endif


/*-------prism cmd responses-------*/
typedef struct hermes_response {
	UINT16 status, resp0, resp1, resp2;
} hermes_response_t;

#define PRISM_MAX_KEY_SIZE	14
#define PRISM_MAX_KEYS	4

#define SMALL_KEY_SIZE		5
#define LARGE_KEY_SIZE		13

typedef struct prism_key {
	u16 len;	/* always store little-endian */
	char data[PRISM_MAX_KEY_SIZE];
} __attribute__ ((packed)) prism_key_t;

typedef prism_key_t prism_keys_t[PRISM_MAX_KEYS];

/*-----------------driver wireless default setting---------------------*/
#define DEFAULT_TXRATE_MODE				0x00
#define DEFAULT_RXSTHRESHOLD			2432
#define DEFAULT_SENSE					1
#define DEFAULT_POWMANAGE				0
#define DEFAULT_MULITRECEIVE			1
#define DEFAULT_MAXSLEEP				100
#define DEFAULT_PMHOLDDURATION			100
/*----------------------------------------------------*/
/*	PRISM For W90N745 Definition		      */
/*----------------------------------------------------*/
#define WIRELESS_SPY		// enable iwspy support

#ifdef WIRELESS_SPY
#define SPY_NUMBER(priv)	(priv->spy_number)
#else
#define SPY_NUMBER(priv)	0
#endif /* WIRELESS_SPY */

struct w740prism_priv {
    	struct net_device_stats stats;
    	struct iw_statistics wstats;
    	int status;			// wireless link status; 
    						// -1: 	setup fail
    						// 0: 	setup ok
    						// 1: 	connected
    						// 2: 	disconnected
    						// 3:	Access Point Change
    						// 4:	Access Point Out of Range
    						// 5:	Access Point In Range
    						// 6: 	Association failed
    	unsigned char mac_address[ETH_ALEN] __attribute__ ((aligned (2)));
    	
    	int rx_packetlen;
    	u8 *rx_packetdata;
    	int tx_packetlen;
    	u8 *tx_packetdata;
    	struct sk_buff *skb;
    	
    	unsigned short txfid;			/* use for debug */
    	struct net_device * ndev;
    	
    	u16 wep_on, wep_restrict, tx_key;
    	prism_keys_t keys;
    	
    	/* Configuration paramaters */
		u32 iw_mode;
		int prefer_port3;
		int bitratemode;
 		char nick[IW_ESSID_MAX_SIZE+1] __attribute__ ((aligned (2)));
		char desired_essid[IW_ESSID_MAX_SIZE+1] __attribute__ ((aligned (2)));
		u16 frag_thresh, mwo_robust;
		u16 channel;
		u16 ap_density, rts_thresh;
		u16 pm_on, pm_mcast, pm_period, pm_timeout;
		u16 preamble;
#ifdef WIRELESS_SPY
		int			spy_number;
		u8			spy_address[IW_MAX_SPY][ETH_ALEN];
		struct iw_quality	spy_stat[IW_MAX_SPY];
#endif
#ifdef SCAN_ENABLE
		prism_ScanResult_t scan_res;
		int scan_res_len;
#endif
		/* Configuration dependent variables */
		int port_type, allow_ibss;
		int promiscuous, mc_count;
		
		struct semaphore	sema;		 /* to sleep on   */
    	spinlock_t lock;
};


#ifdef WDEBUG
/*------------------Debug use--------------------*/
#define LOGPRINT printk("%s: %d\n", __FILE__,__LINE__)
#define DEBUG printk

#else           
#define LOGPRINT
#define DEBUG
#endif
//#define DEBUGPRINT
//#define NEWDOCMD


/*-----------------------------------------*/
/* 	Inline functions		   */
/*-----------------------------------------*/
static inline void
prism_lock(struct w740prism_priv *priv)
{
	spin_lock_bh(&priv->lock);
}

static inline void
prism_unlock(struct w740prism_priv *priv)
{
	spin_unlock_bh(&priv->lock);
}

/*----------------------------------------------------*/
/*	PRISM For W90N745 Definition		      */
/*----------------------------------------------------*/
/*--------prism initialize set up value-------------*/
#define PRISM_TxRate	        ((UINT16)0x000F)
#define PRISM_Authentication  	((UINT16)0x0003)
#define PRISM_MacPortType     	((UINT16)0x0001)
#define PRISM_Default_Channel 	((UINT16)0x0009)
#define PRISM_Support_Rate    	((UINT16)0x000F)
#define PRISM_Beacon_Interval 	((UINT16)1000)
#define PRISM_DTIM_Period     	((UINT16)0x0003)
#define PRISM_MAX_DATA_LEN    	((UINT16)2304)

/*-------------transmit frame buffer declaration---------------*/
#define FRAME_DATA_MAXLEN     	2304
#define PRISM2_TXBUF_MAX      	(sizeof(prism_tx_frame_t) + FRAME_DATA_MAXLEN)

#if 0	//def _BIG_ENDIAN
#define PRISM_FrameControl   	((UINT16)0x0801)  // ToDS = 1, FromDS = 0 STA mode
#define PRISM_DurationID     	((UINT16)0x0000)
#else
#define PRISM_FrameControl   	((UINT16)0x0108)  // ToDS = 1, FromDS = 0 STA mode
#define PRISM_DurationID     	((UINT16)0x0000)
#endif //def _BIG_ENDIAN

/* These are maximum timeouts. Most often, card wil react much faster */
#define CMD_BUSY_TIMEOUT 	(100) 	/* In iterations of ~1us */
#define CMD_INIT_TIMEOUT 	(50000) /* in iterations of ~10us */
#define CMD_COMPL_TIMEOUT 	(1500) 	/* in iterations of ~10us */
//#define CMD_COMPL_TIMEOUT 	(2000) 	/* in iterations of ~10us */
#define ALLOC_COMPL_TIMEOUT 	(1000)  /* in iterations of ~10us */
#define BAP_BUSY_TIMEOUT 	(500)  	/* in iterations of ~1us */

#define MAX_IRQLOOPS_PER_IRQ 		10
#define MAX_IRQLOOPS_PER_JIFFY		(20000/HZ)	/* Based on a guestimate of how many events the
						   device can legitimately generate */
#define HERMES_DESCRIPTOR_OFFSET	0
#define HERMES_802_11_OFFSET		(14)
#define HERMES_802_3_OFFSET		(14+32)
#define HERMES_802_2_OFFSET		(14+32+14)

#define DUMMY_FID		0xFFFF

#define ENCAPS_OVERHEAD		(6*sizeof(char) + 2)

#define RUP_EVEN(a) ( (a) % 2 ? (a) + 1 : (a) )



/*-----------------------------------function declares-------------------------*/
/*------------ module routine------------- */
int prism_init_module(void);
void prism_cleanup(void);
int prism_init(struct net_device *dev);  

/*------------------------------driver routine------------------------------*/
/* open */
int prism_open(struct net_device *dev);
/* as close */
int prism_release(struct net_device *dev);
/* upper level software call to send package */
int prism_start_tx(struct sk_buff *skb, struct net_device *dev);
/* send buffer */
static int send_frame(struct net_device *dev ,unsigned char *data,int length);
/* prism get package from internal to upper level software */
void prism_rx(struct net_device *dev);
/* prism get communication information */
int prism_info(struct net_device *dev);
/* interrupt event handle function */
void prism_interrupt(int irq, void *dev_id, struct pt_regs *regs);
/* mac driver's ioctl */
int prism_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
/* multicast function */
static void prism_set_multicast_list(struct net_device *dev);
/* check after tx */
int prism_tx(struct net_device *dev); 
/* timeout handle */
void prism_tx_timeout (struct net_device *dev);
/* set mac address */
static int prism_set_mac_address(struct net_device *dev, void *addr);
/* net device stat */
struct net_device_stats *prism_stats(struct net_device *dev);
/* net device reset */
int prism_reset(struct w740prism_priv *priv);

/* ------------------------------prism's operation ------------------------------*/
/* prism init in W90N745 */
int prism_initmac(struct net_device *dev);
/* init command operation */
int prism_cmd_initialize();
/* copy content to prism though buffer access path */
int prism_copy_to_bap(UINT16 bap, UINT16 id, UINT16 offset, void *buf, UINT32 len);
/* copy buffer from prsim though buffer access path */
int prism_copy_from_bap(UINT16 bap, UINT16 id, UINT16 offset, void *buf, UINT32 len);
/* alloc buffer for tx */
int prism_cmd_allocate(UINT16 len, unsigned short *txfid);
/* reset this card */
int prism_reset_device(struct net_device *dev);
/* send command to prism and wait */
#ifndef NEWDOCMD
int prism_docmd_wait(UINT16 cmd, UINT16 parm0, UINT16 parm1, UINT16 parm2);
int prism_docmd_nowait(UINT16 cmd, UINT16 parm0, UINT16 parm1, UINT16 parm2);
#else
//int prism_docmd_wait(UINT16 cmd, UINT16 parm0, UINT16 parm1, UINT16 parm2, hermes_response_t *resp);
int prism_docmd_wait(UINT16 cmd, UINT16 parm0, UINT16 parm1, UINT16 parm2);
int hermes_issue_cmd(UINT16 cmd, UINT16 param0, UINT16 param1, UINT16 param2);
#endif
int prism_cmd_access(UINT16 write, UINT16 rid);
int prism_cmd_diagnose();
int prism_config(struct net_device *dev, struct ifmap *map);
int prism_cmd_enable(UINT16 macport);
int prism_cmd_disable(UINT16 macport);
/* command to start transmit which parameter is fid */
int prism_cmd_transmit(UINT16 reclaim, UINT16 fid);
int prism_cmd_inquiry(UINT16 infoType);
int Write_RID_Config(UINT16 rid, void *buf, UINT16 len);
int Read_RID_Config(UINT16 rid, void *buf);
int Read_CT_InfoType(UINT16 Infofid, UINT16 *len);
int Read_CT_InfoData(UINT16 Infofid, void *buf, int len);
void prism_txexc();

/* ioctl extends functions */
int prism_ioctl_getessid(struct net_device *dev, struct iw_point *erq);
int prism_ioctl_setessid(struct net_device *dev, struct iw_point *essid);
long prism_hw_get_freq(struct w740prism_priv *priv);
int prism_ioctl_setfreq(struct net_device *dev, struct iw_freq *freq); 
int prism_ioctl_setiwencode(struct net_device *dev, struct iw_point *erq);
int prism_hw_setup_wep(struct w740prism_priv *priv);
int prism_hw_get_bssid(struct net_device *dev, char buf[ETH_ALEN]);
int prism_ioctl_getlinkstat(struct net_device *dev, struct iw_point *erq);
int prism_ioctl_getiwrate(struct net_device *dev, struct iw_param *iwp);
int prism_ioctl_setiwrate(struct net_device *dev, struct iw_param *iwp);
int prism_hw_setiwrate(struct w740prism_priv *priv);
int prism_ioctl_setrts(struct net_device *dev, struct iw_param *iwp);
int prism_ioctl_getsens(struct net_device *dev, struct iw_param *iwp);
int prism_ioctl_setsens(struct net_device *dev, struct iw_param *iwp);
int prism_hw_setapdensity(struct w740prism_priv *priv);
int prism_ioctl_getpower(struct net_device *dev, struct iw_param *iwp);
int prism_ioctl_setpower(struct net_device *dev, struct iw_param *iwp);
int prism_hw_setpower(struct w740prism_priv *priv);
int get_scan_result(UINT16 infoid, int lens, struct w740prism_priv *priv);
int prism_ioctl_getaplist(struct net_device *dev, struct iw_point * iwp);
void set_port_type(struct w740prism_priv *priv);
int prism_ioctl_setspy(struct net_device *dev, struct iw_point *iwp);
int prism_ioctl_getspy(struct net_device *dev, struct iw_point *iwp);
/*------------------Swap macro------------------*/
static inline unsigned short Swap16(unsigned short val)
{
    	return ((val & 0xFF) <<8) | ((val &0xFF00) >>8);
}

/*----------------Test card present--------------*/
static inline int hermes_present()
{
	UINT32 value = READREG(PRISM_SWSUPPORT0);
	printk("hermes_present: %x\n", value);
	return (value == HERMES_MAGIC);
}
#endif
