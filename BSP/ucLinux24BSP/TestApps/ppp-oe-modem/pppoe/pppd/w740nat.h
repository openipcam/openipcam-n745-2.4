#ifndef _W740NAT_H_
#define _W740NAT_H_

#ifdef __KERNEL__
#include <linux/netfilter_ipv4/ip_conntrack.h>
#endif

//#include <linux/netfilter_ipv4/ip_conntrack_core.h>

//static int do_ioctl(struct   nata_req *req);//mcli

#define _NATA_BASE	0xfff06000

//NATA Control and status Register
#define NATCMD		(_NATA_BASE+0)  //NAT Command Register
#define NATCCLR		(_NATA_BASE+4)  //NAT Counter clear Register
#define NATCFG0		(_NATA_BASE+40) //NAT Entry 0 Configuration Register
//Address lookup and Replacement Registers
#define EXMACM		(_NATA_BASE+100) 
#define EXMACL		(_NATA_BASE+104)
#define INMACM		(_NATA_BASE+108)
#define INMACL		(_NATA_BASE+10c)
#define NATA_ENTRY_START (_NATA_BASE+200)
#define NATA_ENTRY_SIZE	 64
//NAT command REgister 
#define NATCMD_NATEN     1

//NAT Entry x Configuration Registers
#define NATCFG_ExEn     1  //Entry x Comparison Enble bit
#define NATCFG_AxCE		(1<<1) //ip Address Comparion  Enable at entry x
#define NATCFG_PxCE		(1<<2)
#define NATCFG_AxRE		(1<<3)
#define NATCFG_PxRE		(1<<4) //Port Number Repacement Enalbe at Entry x
#define NATCFG_Inverse	(1<<5) //Inverse Comparion adn Replacement bit
#define NATCFG_Discard	(1<<6)
#define NATCFG_NOP		(1<<7)
#define NATCFG_CNTx		(1<<8)
#define NATCFG_EHCNTx	(1<<16)


#define NATEN       0x00000001  // NAT Enable bit
// NAT Entry Configuration Register(NATCFG)
#define NAT_NOP     0x00000080  // No Operation bit
#define NAT_Discard 0x00000040  // Packet Discard bit
#define NAT_Inverse 0x00000020  // Inverse Comparison and Replacement bit
#define NAT_PxRE    0x00000010  // Port Number Replacement Enable
#define NAT_AxRE    0x00000008  // IP Address Replacement Enable
#define NAT_PxCE    0x00000004  // Port Number Comparison Enable
#define NAT_AxCE    0x00000002  // IP Address Comparison Enable
#define NAT_ExEN    0x00000001  // Entry Enable bit


/* PPPoE Header Struct */
struct NATA_PPPOE_PPP_HEADER_REQ
{
	 
	unsigned char  ver : 4;
	unsigned char  type : 4;
	unsigned char  code;
	unsigned short  sid;//mcli
	unsigned short length;
    unsigned short protocl;
    unsigned  long PPPoE_rmad;
    char  remote_mac[8];
    unsigned long  start_pppoe;
};

/*
*  NATA Entry struct 
*/
#define NATA_ENTRY_FLAG_EMPTY 0
#define NATA_ENTRY_FLAG_HEAD  1
#define NATA_ENTRY_FLAG_RESERVE 2
#define NATA_ENTRY_FLAG_NONE	3
typedef struct _NATA_ENTRY
{
	unsigned long flag;
	unsigned long index;
	unsigned long time;
	
	unsigned long rsad;
	unsigned long rspn;
	
	unsigned long msad;
	unsigned long mspn ; //NAT Masquerading Port Number Entry 	
	
	unsigned long lsad  ;	
	unsigned long lspn  ;//NAT  local Port Number Entry
	
	//masq ip;
		
	char 		  LocMAC[8];	
	char		  RsMAC[8];	
	struct _NATA_ENTRY  * next;
	struct _NATA_EMTRY  * prev;
	struct ip_conntrack  *ct;	
}NATA_ENTRY;

#ifdef __KERNEL__
struct   _NATA_Table
{
			 int  size;  
	unsigned long num_use;     
	NATA_ENTRY  * head;
	
	NATA_ENTRY  * empty_list;
	NATA_ENTRY  * reserve_list;
	
	unsigned long valid;   //表可用？
	unsigned long LanIp;
	unsigned long WanIp;
	unsigned char Lanmac[ETH_ALEN]; //内部MAC 
	unsigned char Wanmac[ETH_ALEN]; //外部MAC
	
	unsigned short PPPoE_SessionID;
	unsigned short PPPoE_PPP_Protocol;
	unsigned long PPPoE_rmad;
	unsigned long  start_pppoe;
	struct timer_list timer;	
	NATA_ENTRY    table[64];        //Entry data
};
extern  struct _NATA_Table  NATA_Table; 
#endif

struct nata_req
{
    int cmd;   
    union{
    struct NATA_PPPOE_PPP_HEADER_REQ pppoe_header;
    NATA_ENTRY  entry;
    unsigned long wanip;
    unsigned long lanip;
    char buf[0];
    }type1;
};
/* NATA command type */
#define NATA_ADD_ENTRY 0x01
#define NATA_DEL_ENTRY 0x02
#define NATA_SET_PPPOE_HEADER 0x03
#define NATA_GET_STATUS	0x04
#define NATA_START		0x05
#define NATA_STOP		0x06
#define NATA_RESET		0x07

#define LAN 0
#define WAN 1
#define MAC_LEN 6
#define PPPOE_HEADER_LEN 8

extern int NATA_Stop (void);
extern int NATA_Start (void);
extern int NATA_Add_Entry(NATA_ENTRY *entry);
extern int NATA_Del_Entry(NATA_ENTRY *entry);

//int NATA_Stop();
//int NATA_Start();
extern  int do_ioctl(struct   nata_req *req);
extern int NATA_StopPPPoE();
extern int NATA_Reset();
extern int NATA_StartPPPoE(unsigned short session ,unsigned long rmad);

#endif
