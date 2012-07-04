#ifndef __USB_H
#define __USB_H




#define  USB_BA  0x6000 /* USB Device */

#define REG_USB_CTL	  		(USB_BA+0x00)    /* USB control register */
#define REG_USB_CVCMD		(USB_BA+0x04)    /* USB class or vendor command register */
#define REG_USB_IE	  		(USB_BA+0x08)    /* USB interrupt enable register */
#define REG_USB_IS	  		(USB_BA+0x0c)    /* USB interrupt status register */
#define REG_USB_IC	  		(USB_BA+0x10)    /* USB interrupt status clear register */
#define REG_USB_IFSTR		(USB_BA+0x14)    /* USB interface and string register */
#define REG_USB_ODATA0		(USB_BA+0x18)    /* USB control transfer-out port 0 register */
#define REG_USB_ODATA1		(USB_BA+0x1C)    /* USB control transfer-out port 1 register */
#define REG_USB_ODATA2		(USB_BA+0x20)    /* USB control transfer-out port 2 register */
#define REG_USB_ODATA3		(USB_BA+0x24)    /* USB control transfer-out port 3 register */
#define REG_USB_IDATA0		(USB_BA+0x28)    /* USB control transfer-in data port 0 register */
#define REG_USB_IDATA1		(USB_BA+0x2C)    /* USB control transfer-in data port 1 register */
#define REG_USB_IDATA2		(USB_BA+0x30)    /* USB control transfer-in data port 2 register */
#define REG_USB_IDATA3		(USB_BA+0x34)    /* USB control transfer-in data port 2 register */
#define REG_USB_SIE			(USB_BA+0x38)    /* USB SIE status Register */
#define REG_USB_ENG			(USB_BA+0x3c)    /* USB Engine Register */
#define REG_USB_CTLS		(USB_BA+0x40)    /* USB control transfer status register */
#define REG_USB_CONFD		(USB_BA+0x44)    /* USB Configured Value register */
#define REG_USB_EPA_INFO	(USB_BA+0x48)    /* USB endpoint A information register */
#define REG_USB_EPA_CTL		(USB_BA+0x4c)    /* USB endpoint A control register */
#define REG_USB_EPA_IE		(USB_BA+0x50)    /* USB endpoint A Interrupt Enable register */
#define REG_USB_EPA_IC		(USB_BA+0x54)    /* USB endpoint A interrupt clear register */
#define REG_USB_EPA_IS		(USB_BA+0x58)    /* USB endpoint A interrupt status register */
#define REG_USB_EPA_ADDR	(USB_BA+0x5c)    /* USB endpoint A address register */
#define REG_USB_EPA_LENTH	(USB_BA+0x60)    /* USB endpoint A transfer length register */
#define REG_USB_EPB_INFO	(USB_BA+0x64)    /* USB endpoint B information register */
#define REG_USB_EPB_CTL		(USB_BA+0x68)    /* USB endpoint B control register */
#define REG_USB_EPB_IE		(USB_BA+0x6c)    /* USB endpoint B Interrupt Enable register */
#define REG_USB_EPB_IC		(USB_BA+0x70)    /* USB endpoint B interrupt clear register */
#define REG_USB_EPB_IS		(USB_BA+0x74)    /* USB endpoint B interrupt status register */
#define REG_USB_EPB_ADDR	(USB_BA+0x78)    /* USB endpoint B address register */
#define REG_USB_EPB_LENTH	(USB_BA+0x7c)    /* USB endpoint B transfer length register */
#define REG_USB_EPC_INFO	(USB_BA+0x80)    /* USB endpoint C information register */
#define REG_USB_EPC_CTL		(USB_BA+0x84)    /* USB endpoint C control register */
#define REG_USB_EPC_IE		(USB_BA+0x88)    /* USB endpoint C Interrupt Enable register */
#define REG_USB_EPC_IC		(USB_BA+0x8c)    /* USB endpoint C interrupt clear register */
#define REG_USB_EPC_IS		(USB_BA+0x90)    /* USB endpoint C interrupt status register */
#define REG_USB_EPC_ADDR	(USB_BA+0x94)    /* USB endpoint C address register */
#define REG_USB_EPC_LENTH	(USB_BA+0x98)    /* USB endpoint C transfer length register */
#define REG_USB_EPA_XFER	(USB_BA+0x9c)    /* USB endpoint A remain transfer length register */
#define REG_USB_EPA_PKT		(USB_BA+0xa0)    /* USB endpoint A remain packet length register */
#define REG_USB_EPB_XFER	(USB_BA+0xa4)    /* USB endpoint B remain transfer length register */
#define REG_USB_EPB_PKT		(USB_BA+0xa8)    /* USB endpoint B remain packet length register */
#define REG_USB_EPC_XFER	(USB_BA+0xac)    /* USB endpoint C remain transfer length register */
#define REG_USB_EPC_PKT		(USB_BA+0xb0)    /* USB endpoint C remain packet length register */
 

extern UINT8 volatile USB_Power_Flag; // 0: bus power de-attached; 1: attached
extern UINT8 volatile Bulk_First_Flag;
extern UINT8 volatile USBModeFlag;    //way
extern UINT8 volatile bulkonlycmd;
extern UINT32 volatile USB_MEM_START;

//bulk set
extern UINT32 volatile Bulk_set_length;
extern UINT8 volatile bulksetflag;
/* Define Endpoint feature */
#define Ep_In        0x01
#define Ep_Out       0x00
#define Ep_Bulk      0x01
#define Ep_Int       0x02
#define Ep_Iso       0x03
#if 0
#define EP_A         0x01
#define EP_B         0x02
#define EP_C         0x03
#else
#define EP_A         0x00
#define EP_B         0x01
#define EP_C         0x02
#endif
/* Define USB Interrupt */
#define USB_RSTX  0x01
#define USB_SUSX  0x02
#define USB_RUMX  0x04
#define USB_ERRX  0x08
#define USB_IRQX  0x10
#define USB_DEVX  0x20
#define USB_CFGX  0x40
#define USB_STRX  0x80

#define USB_CLAX  0x01
#define USB_VENX  0x02
#define USB_CDOX  0x04
#define USB_CDIX  0x08
#define USB_CVKX  0x10
#define USB_BIEX  0x20
#define USB_VASX  0x40
#define USB_AASX  0x80

#define USB_SWBKIX 0x01
#define USB_SWBKOX 0x02
#define USB_SBIERX 0x04
#define USB_SBOERX 0x08
#define USB_POWERX 0x10

/*  Class and Vender command control*/
#define RET_CV_DAT 0x01
#define RET_CV_STL 0x02
#define RET_CV_LDA 0x04
#define RET_SDO_RD 0x08
#define RET_IRQ_DAT 0x10
#define RET_IRQ_STL 0x20


typedef struct 
{
    UINT32 dwAddr;
    UINT32 dwValue;
}USB_INIT_T;

typedef struct 
{
 UINT8 Req_Type;
 UINT8 Req;
 UINT16 Value;
 UINT16 Index;
 UINT16 Length;
}USB_Vender_Cmd_Format_T;   // each field of vendor command

typedef struct {
    UINT8 EP_Num;
    UINT8 EP_Dir;
    UINT8 EP_Type;
} USB_EP_Inf_T;

void USB_Init(void);
void USB_Int_Init(void);
void SDRAM2USB_Bulk(UINT32 DRAM_Addr ,UINT32 Tran_Size);
void USB2SDRAM_Bulk(UINT32 DRAM_Addr ,UINT32 Tran_Size);
void SDRAM2USB_Int(UINT32 DRAM_Addr ,UINT32 Tran_Size);


extern UINT8 volatile USBModeFlag;



#endif /* __USB_H */