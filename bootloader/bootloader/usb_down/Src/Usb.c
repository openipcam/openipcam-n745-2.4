/*
 *  W9986 USB Control
 *
 *
 *  Winbond Electronics Corp. 2000
 */
#include <string.h>
#include "u710def.h"
#include "usb.h"
#include "uprintf.h"

typedef void (*usbfptr)();  // function pointer
static usbfptr IUSB_HandlerTable[31];

UINT32 volatile Bulk_Out_Transfer_Size=0;
UINT8 volatile USBModeFlag=0;
UINT8 volatile ResetFlag=0;
UINT8 volatile Bulk_First_Flag=0;
UINT8 volatile DMA_CInt_A_Flag=0;
UINT8 volatile DMA_CInt_B_Flag=0;
UINT8 volatile DMA_CInt_C_Flag=0;
UINT8 volatile Token_Input_A_Flag=0;
UINT8 volatile Token_Input_B_Flag=0;
UINT8 volatile Token_Input_C_Flag=0;

UINT8 volatile hbi_read_flg=1;

UINT8 volatile DEV_RES_Flag;
UINT8 volatile GET_DEV_Flag;
UINT8 volatile GET_CFG_Flag;
UINT8 volatile GET_STR_Flag;
UINT8 volatile CLA_CMD_Oflag;
UINT8 volatile VEN_CMD_Oflag;
UINT8 volatile VEN_CMD_Iflag;
UINT8 volatile MCCI_CMD_Oflag;
UINT8 volatile MCCI_CMD_Iflag;
UINT8 volatile CLA_CMD_Iflag;
UINT8 volatile CTL_BKI_Eflag;
extern UINT8 volatile USB_Power_Flag; // 0: bus power de-attached; 1: attached

//Mem start sector
UINT32 volatile USB_MEM_START;

//bulk set
UINT32 volatile Bulk_set_length=0;
UINT8 volatile bulksetflag=0;
UINT32 volatile Bulkout_set_length=0;
UINT8 volatile bulkoutsetflag=0;

/////////way
UINT8 USB_Setup_Endpoint(UINT8 epname,UINT8 epnum,UINT8 epdir,UINT8 eptype,
                         UINT8 epcon,UINT8 epinf,UINT16 epmaxps );

void USB2SDRAM_Bulk(UINT32 DRAM_Addr ,UINT32 Tran_Size);
void SDRAM2USB_Bulk(UINT32 DRAM_Addr ,UINT32 Tran_Size);
void SDRAM2USB_Int(UINT32 DRAM_Addr ,UINT32 Tran_Size);
void Outp_Byte(UINT32 addr,UINT8 value);

void USB_Int_ISR(void);
void USBHandler(void);

void USB_All_Flag_Clear(void);
void USB_Cmd_Parsing(void);
void Vender_Data_Out(void);
void Vender_Data_In(void);
//void MCCI_Data_Out(void);
//void MCCI_Data_In(void);
void Class_Data_Out(void);
void Class_Data_In(void);
void Get_Dev_Dpr_In(void);
/*
UINT8 readgpio10(void);//usb-detect
*/
////////
void USB_ISR_Reset_Start(void);
void USB_ISR_Reset_End(void);
void USB_ISR_Suspend(void);
void USB_ISR_Resume(void);
void USB_ISR_Error(void);
void USB_ISR_Dev_Des(void);
void USB_ISR_Conf_Des(void);
void USB_ISR_Str_Des(void);
void USB_ISR_Class_Cmd(void);
void USB_ISR_Vendor_Cmd(void);
void USB_ISR_CtlOut(void);
void USB_ISR_CtlIn(void);
/*
void USB_ISR_Power(void);
*/
void USB_ISR_EpA_Stall(void);
void USB_ISR_EpA_Token_Input(void);
void USB_ISR_EpA_DMA_Complete(void);
void USB_ISR_EpA_Bus_Err(void);
void USB_ISR_EpB_Stall(void);
void USB_ISR_EpB_Token_Input(void);
void USB_ISR_EpB_DMA_Complete(void);
void USB_ISR_EpB_Bus_Err(void);
void USB_ISR_EpC_Stall(void);
void USB_ISR_EpC_Token_Input(void);
void USB_ISR_EpC_DMA_Complete(void);
void USB_ISR_EpC_Bus_Err(void);

USB_Vender_Cmd_Format_T volatile USB_CtlOut_Format;

USB_EP_Inf_T volatile USB_EP_Inf_Array[3];

static USB_Device[5] = {
0x01100112, 0x10000000, 0x70210416/*pid & vid*/, 0x02010100, 0x00000100 };

static USB_Config[8] = {
0x00200209, 0x40000101, 0x00040932, 0x00000200 , 0x05070000, 0x00400281,
0x02050701, 0x01004002 };


static USB_Lang_Id_String[1] = {
0x04090304 };

static USB_Vender_Id_String[6] = {
0x00550316, 0x00420053, 0x00440020, 0x00760065, 0x00630069 ,0x00000065};

static USB_Device_Id_String[12] = {
0x0057032E, 0x00390039, 0x00300037, 0x00200032, 0x00530055, 0x00200042, 0x00690056,
0x00740072, 0x00610075, 0x0020006C, 0x004F0043, 0x0000004D};


static USB_Version[1] = { 0x00000001 };

static USB_INIT_T USBInit[6] = 
{
    0x00,0x000000e4,
    0x08,0x000047f7,
    0x14,0x00000030,
    0x38,0x00000000,
    0x44,0x00000001,
    0x48,0x00000000,
};


UINT8 USB_Setup_Endpoint(UINT8 epname,UINT8 epnum,UINT8 epdir,UINT8 eptype,
                         UINT8 epcon,UINT8 epinf,UINT16 epmaxps )
{
    UINT32 tmp;
    
    if (epname == EP_A)
    {
    	if ((epdir==Ep_In)&&(eptype==Ep_Bulk)&&(epcon==1))
    	    outpw(REG_USB_EPA_INFO,0x30000010);
    	else if ((epdir==Ep_In)&&(eptype==Ep_Int)&&(epcon==1))
    	    outpw(REG_USB_EPA_INFO,0x50000010);
    	else if ((epdir==Ep_Out)&&(eptype==Ep_Bulk)&&(epcon==1))
    	    outpw(REG_USB_EPA_INFO,0x20000010);
    	else
    	     return 0;
    	tmp = epinf;
    	tmp = tmp << 8;
    	outpw(REG_USB_EPA_INFO,(inpw(REG_USB_EPA_INFO)|tmp));
    	tmp = epmaxps;
    	tmp = tmp << 16;
    	outpw(REG_USB_EPA_INFO,(inpw(REG_USB_EPA_INFO)|tmp));
    	outpw(REG_USB_EPA_INFO,(inpw(REG_USB_EPA_INFO)|(epnum&0x0f)));
        USB_EP_Inf_Array[EP_A].EP_Num = epnum;
        USB_EP_Inf_Array[EP_A].EP_Dir = epdir;
        USB_EP_Inf_Array[EP_A].EP_Type = eptype;
    	outpw(REG_USB_EPA_CTL,0x00000001);
    }
    else if (epname == EP_B)
    {
    	if ((epdir==Ep_In)&&(eptype==Ep_Bulk)&&(epcon==1))
    	    outpw(REG_USB_EPB_INFO,0x30000010);
    	else if ((epdir==Ep_In)&&(eptype==Ep_Int)&&(epcon==1))
    	    outpw(REG_USB_EPB_INFO,0x50000010);
    	else if ((epdir==Ep_Out)&&(eptype==Ep_Bulk)&&(epcon==1))
    	    outpw(REG_USB_EPB_INFO,0x20000010);
    	else
    	     return 0;
    	tmp = epinf;
    	tmp = tmp << 8;
    	outpw(REG_USB_EPB_INFO,(inpw(REG_USB_EPB_INFO)|tmp));
    	tmp = epmaxps;
    	tmp = tmp << 16;
    	outpw(REG_USB_EPB_INFO,(inpw(REG_USB_EPB_INFO)|tmp));
    	outpw(REG_USB_EPB_INFO,(inpw(REG_USB_EPB_INFO)|(epnum&0x0f)));
        USB_EP_Inf_Array[EP_B].EP_Num = epnum;
        USB_EP_Inf_Array[EP_B].EP_Dir = epdir;
        USB_EP_Inf_Array[EP_B].EP_Type = eptype;
    	outpw(REG_USB_EPB_CTL,0x00000001);
    }
    else if (epname == EP_C)
    {
    	if ((epdir==Ep_In)&&(eptype==Ep_Bulk)&&(epcon==1))
    	    outpw(REG_USB_EPC_INFO,0x30000010);
    	else if ((epdir==Ep_In)&&(eptype==Ep_Int)&&(epcon==1))
    	    outpw(REG_USB_EPC_INFO,0x50000010);
    	else if ((epdir==Ep_Out)&&(eptype==Ep_Bulk)&&(epcon==1))
    	    outpw(REG_USB_EPC_INFO,0x20000010);
    	else
    	     return 0;
    	tmp = epinf;
    	tmp = tmp << 8;
    	outpw(REG_USB_EPC_INFO,(inpw(REG_USB_EPC_INFO)|tmp));
    	tmp = epmaxps;
    	tmp = tmp << 16;
    	outpw(REG_USB_EPC_INFO,(inpw(REG_USB_EPC_INFO)|tmp));
    	outpw(REG_USB_EPC_INFO,(inpw(REG_USB_EPC_INFO)|(epnum&0x0f)));
        USB_EP_Inf_Array[EP_C].EP_Num = epnum;
        USB_EP_Inf_Array[EP_C].EP_Dir = epdir;
        USB_EP_Inf_Array[EP_C].EP_Type = eptype;
    	outpw(REG_USB_EPC_CTL,0x00000001);
    }
    else
        return 0;
    return 1;
}


void USB_Init(void)
{
    int j;

    for (j=0 ; j<6 ; j++)
		outpw((USB_BA+USBInit[j].dwAddr),USBInit[j].dwValue);
		
		

    for (j=0 ; j<3 ; j++)
    {
        USB_EP_Inf_Array[j].EP_Num = 0xff;
        USB_EP_Inf_Array[j].EP_Dir = 0xff;
        USB_EP_Inf_Array[j].EP_Type = 0xff;
    }
    USB_Setup_Endpoint(EP_A,1,Ep_In,Ep_Bulk,1,0,64);
    USB_Setup_Endpoint(EP_B,2,Ep_Out,Ep_Bulk,1,0,64);

    outpw(REG_USB_EPA_IE,(inpw(REG_USB_EPA_IE)|0x00000008));//enable DMA interrupt
    outpw(REG_USB_EPB_IE,(inpw(REG_USB_EPB_IE)|0x00000008));//enable DMA interrupt

    outpw(REG_USB_CTL,(inpw(REG_USB_CTL)|0x00000001));//D+ high

}

static void USBSetInterrupt(volatile UINT32 vector, void (*handler)())
{
 	IUSB_HandlerTable[vector] = handler;
}

// Interrupt Service Routine                                                                         */
__irq void USBIRQ_IntHandler(void)
{
// 	UINT32 IPER, ISNR;
	UINT32 irq;

// 	IPER = AIC_IPER >> 2;
// 	ISNR = AIC_ISNR;
	irq=REG_AIC_ISR;
	if( irq & (0x1<<IRQ_USBD) )
		(*IUSB_HandlerTable[IRQ_USBD])();
	
}

void USB_Int_Init(void)
{
	{
		INT tmp;
		*((volatile UINT32 *)0x38)=(UINT32)USBIRQ_IntHandler;
		__asm
		{
			MRS	tmp, CPSR
			BIC	tmp, tmp, 0x80
			MSR	CPSR_c, tmp
		}
	}
    //WB_InstallISR(IRQ_LEVEL_1, IRQ_USBD, (PVOID)USBHandler);
    USBSetInterrupt(IRQ_USBD, USBHandler) ;
    
    /* enable USB interrupt */
   // WB_EnableInterrupt(IRQ_USBD);
    Enable_Int(IRQ_USBD);
    
    outpw(REG_USB_EPA_IE,(inpw(REG_USB_EPA_IE)|0x00000008));//enable DMA interrupt
    outpw(REG_USB_EPB_IE,(inpw(REG_USB_EPB_IE)|0x00000008));//enable DMA interrupt
}

void USBHandler(void)
{
   /* clear USB interrupt state */
    USB_Int_ISR();
}

void USB_Int_ISR(void)
{
    UINT32 id;

    id = inpw(REG_USB_IS);
    if (id&0x00000001) 
        USB_ISR_Reset_Start();
    if (id&0x00000002) 
        USB_ISR_Suspend();
    if (id&0x00000004) 
        USB_ISR_Resume();
    if (id&0x00000008) 
        USB_ISR_Error();
    if (id&0x00000010)
        USB_ISR_Dev_Des();
    if (id&0x00000020)
        USB_ISR_Conf_Des();
    if (id&0x00000040) 
        USB_ISR_Str_Des();
    if (id&0x00000080)  
        USB_ISR_Class_Cmd();

    if (id&0x00000100) 
        USB_ISR_Vendor_Cmd();

    if (id&0x00000200) 
        USB_ISR_CtlOut();
    if (id&0x00000400)
        USB_ISR_CtlIn();
    if (id&0x00004000)
        USB_ISR_Reset_End();

    id = inpw(REG_USB_EPA_IS);
    if (id&0x00000001) 
        USB_ISR_EpA_Stall();
    if (id&0x00000002)
        USB_ISR_EpA_Token_Input();
    if (id&0x00000008)
        USB_ISR_EpA_DMA_Complete();
    if (id&0x00000010)
        USB_ISR_EpA_Bus_Err();

    id = inpw(REG_USB_EPB_IS);
    if (id&0x00000001) 
        USB_ISR_EpB_Stall();
    if (id&0x00000002) 
        USB_ISR_EpB_Token_Input();
    if (id&0x00000008)
        USB_ISR_EpB_DMA_Complete();
    if (id&0x00000010) 
        USB_ISR_EpB_Bus_Err();

    id = inpw(REG_USB_EPC_IS);
    if (id&0x00000001) 
        USB_ISR_EpC_Stall();
    if (id&0x00000002) 
        USB_ISR_EpC_Token_Input();
    if (id&0x00000008) 
        USB_ISR_EpC_DMA_Complete();
    if (id&0x00000010) 
        USB_ISR_EpC_Bus_Err();
}

void USB_ISR_Reset_Start(void)
{
    outpw(REG_USB_EPA_CTL,inpw(REG_USB_EPA_CTL)|0x00000002);
    outpw(REG_USB_EPB_CTL,inpw(REG_USB_EPB_CTL)|0x00000002);
    outpw(REG_USB_EPC_CTL,inpw(REG_USB_EPC_CTL)|0x00000002);
    outpw(REG_USB_EPA_CTL,inpw(REG_USB_EPA_CTL)&0xfffffffd);
    outpw(REG_USB_EPB_CTL,inpw(REG_USB_EPB_CTL)&0xfffffffd);
    outpw(REG_USB_EPC_CTL,inpw(REG_USB_EPC_CTL)&0xfffffffd);
    outpw(REG_USB_IC,0x00000001);
}

void USB_ISR_Reset_End(void)
{
	
    outpw(REG_USB_EPA_CTL,inpw(REG_USB_EPA_CTL)&0xfffffffb);
    outpw(REG_USB_EPB_CTL,inpw(REG_USB_EPB_CTL)&0xfffffffb);
    outpw(REG_USB_EPC_CTL,inpw(REG_USB_EPC_CTL)&0xfffffffb);
    ResetFlag=1;
    outpw(REG_USB_IC,0x00004000);
}
void USB_ISR_Suspend(void)
{
    if (DEV_RES_Flag == 0)
    {
    	DEV_RES_Flag = ~DEV_RES_Flag;
    }else
    {
#if 0
        regUPLLCtlCR = regUPLLCtlCR & 0x7f;
#endif        
    }
    outpw(REG_USB_IC,0x00000002);
}

void USB_ISR_Resume(void)
{
#if 0
    regUPLLCtlCR = regUPLLCtlCR | 0x80;
#endif
    outpw(REG_USB_IC,0x00000004);
}

void USB_ISR_Error(void)
{
    outpw(REG_USB_IC,0x00000008);
}

void USB_ISR_Dev_Des(void)
{
     USB_All_Flag_Clear();     	
     GET_DEV_Flag = 1;
     USBModeFlag=1;
     if ((inpw(REG_USB_CTLS)&0x0000001f) == 0x00000008)
     {
         USB_Cmd_Parsing();
         if (USB_CtlOut_Format.Length <=(USB_Device[0]&0x000000ff))
             outpw(REG_USB_ENG,0x00000008);
         else
         {
             USB_CtlOut_Format.Length = (USB_Device[0]&0x000000ff);
             outpw(REG_USB_ENG,0x00000008);
         }
     }    
     else {
        outpw(REG_USB_ENG,0x00000002);
     }
     outpw(REG_USB_IC,0x00000010);
}

void USB_ISR_Conf_Des(void)
{
     USB_All_Flag_Clear();     		
     GET_CFG_Flag = 1;
     if ((inpw(REG_USB_CTLS)&0x0000001f) == 0x00000008)
     {
         USB_Cmd_Parsing();
         if (USB_CtlOut_Format.Length <= ((USB_Config[0]&0x00ff0000)>>16))
             outpw(REG_USB_ENG,0x00000008);
         else
         {
             USB_CtlOut_Format.Length = ((USB_Config[0]&0x00ff0000)>>16);
             outpw(REG_USB_ENG,0x00000008);
         }
     }    
     else {
        outpw(REG_USB_ENG,0x00000002);
     }
     outpw(REG_USB_IC,0x00000020);
}

void USB_ISR_Str_Des(void)
{
     USB_All_Flag_Clear();     		
     GET_STR_Flag = 1;
     if ((inpw(REG_USB_CTLS)&0x0000001f) == 0x00000008)
     {
         USB_Cmd_Parsing();
         if ((inpw(REG_USB_ODATA0)&0x00ff0000) == 0x00000000)
         {
             if (USB_CtlOut_Format.Length <= (USB_Lang_Id_String[0]&0x000000ff))
                 outpw(REG_USB_ENG,0x00000008);
             else
             {
                 USB_CtlOut_Format.Length = (USB_Lang_Id_String[0]&0x000000ff);
                 outpw(REG_USB_ENG,0x00000008);
             }    
         }        	
         if ((inpw(REG_USB_ODATA0)&0x00ff0000) == 0x00010000)
         {
             if (USB_CtlOut_Format.Length <= (USB_Vender_Id_String[0]&0x000000ff))
                 outpw(REG_USB_ENG,0x00000008);
             else
             {
                 USB_CtlOut_Format.Length = (USB_Vender_Id_String[0]&0x000000ff);
                 outpw(REG_USB_ENG,0x00000008);
             }    
         }
         if ((inpw(REG_USB_ODATA0)&0x00ff0000) == 0x00020000)
         {
             if (USB_CtlOut_Format.Length <= (USB_Device_Id_String[0]&0x000000ff))
                 outpw(REG_USB_ENG,0x00000008);
             else
             {
                 USB_CtlOut_Format.Length = (USB_Device_Id_String[0]&0x000000ff);
                 outpw(REG_USB_ENG,0x00000008);
             }   
         }
     }    
     else {
        outpw(REG_USB_ENG,0x00000002);
     }
     outpw(REG_USB_IC,0x00000040);
}

void USB_ISR_Class_Cmd(void)
{
     USB_All_Flag_Clear();
     if ((inpb(USB_BA+0x1b) == 0xa1)&&(inpb(USB_BA+0x1a)==0xfe))
         CLA_CMD_Iflag = 1;
     outpw(REG_USB_ENG,0x00000008);
     outpw(REG_USB_IC,0x00000080);
}

void USB_ISR_Vendor_Cmd(void)
{
	
     USB_All_Flag_Clear();
     if ((inpw(REG_USB_CTLS)&0x0000001f) == 0x00000008)
     {
         USB_Cmd_Parsing();
         
         //printf("Value:%x\n",USB_CtlOut_Format.Value);
         
         if (USB_CtlOut_Format.Req_Type == 0x40)
         {
             VEN_CMD_Oflag = 1;
             Bulk_Out_Transfer_Size=0;
             if (USB_CtlOut_Format.Req == 0xA0)
             {
                 if (USB_CtlOut_Format.Value == 0x12)
                 {
					//VCOM PC driver Vendor CMD
                    Bulk_Out_Transfer_Size=USB_CtlOut_Format.Index;
                    Bulk_First_Flag=1;
                    outpw(REG_USB_IE,inpw(REG_USB_IE)&0xfffffeff);//disable vender interrupt enable
                 }
                 else if (USB_CtlOut_Format.Value == 0x13)
                 {
                 	 //printf("PC close port\n");
                 	 
                 	 outpw(REG_USB_IE,inpw(REG_USB_IE)|0x00000100);//enable vender interrupt enable
              
                    if ((inpw(REG_USB_EPB_CTL)&0x00000004)==0x00000004)
                    {
                        outpw(REG_USB_EPB_CTL,inpw(REG_USB_EPB_CTL)|0x00000002);//reset
                        outpw(REG_USB_EPB_CTL,inpw(REG_USB_EPB_CTL)&0xfffffffd);
                    }
                    if ((inpw(REG_USB_EPB_CTL)&0x00000004)==0x00000004)
                        outpw(REG_USB_EPB_CTL,inpw(REG_USB_EPB_CTL)&0xfffffffb);//clear ready
                 }
             }
         }
         else if (USB_CtlOut_Format.Req_Type == 0xc0)
             VEN_CMD_Iflag = 1;
         else if (USB_CtlOut_Format.Req_Type == 0x41)
             MCCI_CMD_Oflag = 1;
         else if (USB_CtlOut_Format.Req_Type == 0xc1)
             MCCI_CMD_Iflag = 1; 
         else
             outpw(REG_USB_ENG,0x00000002);
     }    
     else {
        outpw(REG_USB_ENG,0x00000002);
     }
     outpw(REG_USB_ENG,0x00000008);
     outpw(REG_USB_IC,0x00000100);
}

void USB_ISR_CtlOut(void)
{
     if (CLA_CMD_Oflag == 1)
         Class_Data_Out();
     else if (VEN_CMD_Oflag == 1)
         Vender_Data_Out();
     else
         outpw(REG_USB_ENG,0x00000002);
         
     outpw(REG_USB_ENG,0x00000008);
     outpw(REG_USB_IC,0x00000200);
}

void USB_ISR_CtlIn(void)
{
     if (CLA_CMD_Iflag == 1)
         Class_Data_In();
     else if (VEN_CMD_Iflag == 1)
         Vender_Data_In();
     else if (GET_DEV_Flag == 1)
         Get_Dev_Dpr_In();      
     else if (GET_CFG_Flag == 1)
         Get_Dev_Dpr_In();              
     else if (GET_STR_Flag == 1)
         Get_Dev_Dpr_In();     
     else
         outpw(REG_USB_ENG,0x00000002);

     if (CTL_BKI_Eflag == 1)
         outpw(REG_USB_ENG,0x00000001);
     else
         outpw(REG_USB_ENG,0x00000001);
     outpw(REG_USB_IC,0x00000400);
}

void USB_ISR_EpA_Stall(void)
{
     outpw(REG_USB_EPA_IC,0x00000001);
}

void USB_ISR_EpA_Token_Input(void)
{
     Token_Input_A_Flag=1;
     outpw(REG_USB_EPA_IC,0x00000002);
}

void USB_ISR_EpA_DMA_Complete(void)
{

     DMA_CInt_A_Flag=1;
     outpw(REG_USB_EPA_IC,0x00000008);
}

void USB_ISR_EpA_Bus_Err(void)
{
     outpw(REG_USB_EPA_IC,0x00000010);
}

void USB_ISR_EpB_Stall(void)
{
     outpw(REG_USB_EPB_IC,0x00000001);
}

void USB_ISR_EpB_Token_Input(void)
{
     outpw(REG_USB_EPB_IC,0x00000002);
}

void USB_ISR_EpB_DMA_Complete(void)
{
     if (Bulk_Out_Transfer_Size!=0)
     {
//         outpw(REG_HICPAR,Bulk_Out_Transfer_Size);
         //outpw(REG_HICPAR,0x00000000);  //hbi_plug-in
         //hbi_read_flg=0;
         
         //printf("REG_USB_EPB_XFER %d bytes\n",inpw(REG_USB_EPB_XFER));
         Bulk_Out_Transfer_Size=0;
         outpw(REG_USB_IE,inpw(REG_USB_IE)|0x00000100);//enable vender interrupt enable

     }

	 
     DMA_CInt_B_Flag=1;
     outpw(REG_USB_EPB_IC,0x00000008);
}

void USB_ISR_EpB_Bus_Err(void)
{
     outpw(REG_USB_EPB_IC,0x00000010);
}

void USB_ISR_EpC_Stall(void)
{
     outpw(REG_USB_EPC_IC,0x00000001);
}

void USB_ISR_EpC_Token_Input(void)
{
     Token_Input_C_Flag=1;
     outpw(REG_USB_EPC_IC,0x00000002);
}

void USB_ISR_EpC_DMA_Complete(void)
{
     DMA_CInt_C_Flag=1;
     outpw(REG_USB_EPC_IC,0x00000008);
}

void USB_ISR_EpC_Bus_Err(void)
{
     outpw(REG_USB_EPC_IC,0x00000010);
}

void USB_Cmd_Parsing(void)
{
     USB_CtlOut_Format.Req_Type = inpb(USB_BA+0x18);//A_U_CTOD0;
     USB_CtlOut_Format.Req = inpb(USB_BA+0x19);//A_U_CTOD1;     

     USB_CtlOut_Format.Value = inphw(USB_BA+0x1a);//2,3

     USB_CtlOut_Format.Index = inphw(USB_BA+0x1c);//4,5

     USB_CtlOut_Format.Length = inphw(USB_BA+0x1e);//6,7

     if ((USB_CtlOut_Format.Req == 0x04)||(USB_CtlOut_Format.Req == 0x05))
         USB_MEM_START = (USB_CtlOut_Format.Value<<16)+USB_CtlOut_Format.Index;

     if (GET_STR_Flag == 1)
         USB_CtlOut_Format.Index = 0;  
}

void Class_Data_Out(void)
{
}

void Vender_Data_Out(void)
{
     UINT16 i;
     UINT32 tmp;

     Bulk_set_length=0;
     Bulkout_set_length=0;     

     if ((inpw(REG_USB_CTLS)&0x0000001f) != 0)
     {
         if (USB_CtlOut_Format.Req == 0x01)     	 
         {
             tmp=REG_USB_ODATA0;
             for (i = 0 ; i < USB_CtlOut_Format.Length ; i++)
             {
                    Outp_Byte(USB_CtlOut_Format.Index++,inpb(tmp++));
             }
         }else if (USB_CtlOut_Format.Req == 0x13)     	 
         {  	
             USB_MEM_START = (UINT32)USB_CtlOut_Format.Index + (UINT32)0x10000 * USB_CtlOut_Format.Value ;
             tmp=REG_USB_ODATA0;
             for (i = USB_CtlOut_Format.Length-1 ; i > 0 ; i--)
                  Bulk_set_length = Bulk_set_length + (UINT32)(inpb(tmp+i)<<(i*8));
             Bulk_set_length = Bulk_set_length + (UINT32)(inpb(tmp));
             bulksetflag = 1;                 
         }else if (USB_CtlOut_Format.Req == 0x15)     	 
         {  	
             USB_MEM_START = (UINT32)USB_CtlOut_Format.Index + (UINT32)0x10000 * USB_CtlOut_Format.Value ;
             tmp=REG_USB_ODATA0;
             for (i = USB_CtlOut_Format.Length-1 ; i > 0 ; i--)
                  Bulkout_set_length = Bulkout_set_length + (UINT32)(inpb(tmp+i)<<(i*8));
             Bulkout_set_length = Bulkout_set_length + (UINT32)(inpb(tmp));
             bulkoutsetflag = 1;                 
         }
         else if (USB_CtlOut_Format.Req == 0x05)     	 
         { 
             USB_MEM_START = (UINT32)USB_CtlOut_Format.Index + (UINT32)0x10000 * USB_CtlOut_Format.Value ;
             tmp=REG_USB_ODATA0;
            // for (i = 0 ; i < USB_CtlOut_Format.Length ; i++)
                //  writeb(USB_MEM_START++,inpb(tmp++));
         }     
         USB_CtlOut_Format.Index = USB_CtlOut_Format.Index + (inpw(REG_USB_CTLS)&0x0000001f);
         USB_CtlOut_Format.Length = USB_CtlOut_Format.Length - (inpw(REG_USB_CTLS)&0x0000001f);; 
     }
}


void Class_Data_In(void)
{
     outpw(REG_USB_CVCMD,0x01);
     CTL_BKI_Eflag = 1;
     Outp_Byte(REG_USB_IDATA0,0x00);
}

void Vender_Data_In(void)
{
     UINT8 i;
     UINT32 tmp;

     if (USB_CtlOut_Format.Length > 0)
     {
     	 if (USB_CtlOut_Format.Length <= 16)
     	 {
     	     CTL_BKI_Eflag = 1;
     	     outpw(REG_USB_CVCMD,USB_CtlOut_Format.Length);     	     
     	 }
     	 else
     	 {
     	     USB_CtlOut_Format.Length = USB_CtlOut_Format.Length - 16;
     	     outpw(REG_USB_CVCMD,0x10);
     	 }
         if (USB_CtlOut_Format.Req == 0x00)     	 
         {
             tmp=REG_USB_IDATA0;
     	     for (i = 0 ; i < inpw(REG_USB_CVCMD) ; i++)
     	     {
                  Outp_Byte(tmp,inpb(USB_CtlOut_Format.Index));
                  tmp=tmp+1;
     	          USB_CtlOut_Format.Index = USB_CtlOut_Format.Index + 1;
     	     }
     	     
         }else if (USB_CtlOut_Format.Req == 0x10)
         {
             tmp=REG_USB_IDATA0;
     	     for (i = 0 ; i < inpw(REG_USB_CVCMD) ; i=i+2)
     	     {
     	          outphw(tmp,USB_Version[USB_CtlOut_Format.Index]);
     	     	  tmp=tmp+2;
     	          USB_CtlOut_Format.Index = USB_CtlOut_Format.Index + 1;       	      
     	     }             
         }else if (USB_CtlOut_Format.Req == 0x04)
         {
             tmp=REG_USB_IDATA0;
     	     for (i = 0 ; i < inpw(REG_USB_CVCMD) ; i++)
     	     {
                  Outp_Byte(tmp,readb(USB_MEM_START++));
     	     	  tmp=tmp+1;
             }
         }
     }
     else
         outpw(REG_USB_CVCMD,0x00); 
}

void Get_Dev_Dpr_In(void)
{
     UINT8 i;
     
     if (USB_CtlOut_Format.Length > 0)
     {
     	 if (USB_CtlOut_Format.Length <= 16)
     	 {
     	     CTL_BKI_Eflag = 1;
     	     outpw(REG_USB_CVCMD,USB_CtlOut_Format.Length);     	     
     	 }
     	 else
     	 {
     	     USB_CtlOut_Format.Length = USB_CtlOut_Format.Length - 16;
     	     outpw(REG_USB_CVCMD,0x10);
     	 }
     	 for (i = 0 ; i < inpw(REG_USB_CVCMD) ; i=i+4)
     	 {
     	      if (GET_DEV_Flag == 1)
     	          outpw(REG_USB_IDATA0+i,USB_Device[USB_CtlOut_Format.Index]);
              else if (GET_CFG_Flag == 1)
     	          outpw(REG_USB_IDATA0+i,USB_Config[USB_CtlOut_Format.Index]);
              else if (GET_STR_Flag == 1)
              {
                  if ((inpw(REG_USB_ODATA0)&0x00ff0000) == 0x00000000)
     	              outpw(REG_USB_IDATA0+i,USB_Lang_Id_String[USB_CtlOut_Format.Index]);
     	          if ((inpw(REG_USB_ODATA0)&0x00ff0000) == 0x00010000)
     	              outpw(REG_USB_IDATA0+i,USB_Vender_Id_String[USB_CtlOut_Format.Index]);
     	          if ((inpw(REG_USB_ODATA0)&0x00ff0000) == 0x00020000)
     	              outpw(REG_USB_IDATA0+i,USB_Device_Id_String[USB_CtlOut_Format.Index]);
     	      }
    	      USB_CtlOut_Format.Index = USB_CtlOut_Format.Index + 1;  
     	 }
     }
     else
         outpw(REG_USB_CVCMD,0x00); 
}

void USB_All_Flag_Clear(void)
{
    DEV_RES_Flag = 0;
    VEN_CMD_Oflag = 0;
    VEN_CMD_Iflag = 0;
    MCCI_CMD_Oflag = 0;
    MCCI_CMD_Iflag = 0;
    CLA_CMD_Oflag = 0;
    CLA_CMD_Iflag = 0;
    GET_DEV_Flag  = 0;
    GET_CFG_Flag  = 0;
    GET_STR_Flag  = 0;
    CTL_BKI_Eflag = 0;
//    USB_Power_Flag = 0;
}

void SDRAM_USB_Transfer(UINT8 epname,UINT32 DRAM_Addr ,UINT32 Tran_Size)
{
    if (epname == EP_A)
    {
        if(Tran_Size==0)
        {
            DMA_CInt_A_Flag=0;
            outpw(REG_USB_EPA_CTL,inpw(REG_USB_EPA_CTL)|0x00000040);
            while((USBModeFlag)&&(DMA_CInt_A_Flag==0));
//            while((USBModeFlag)&&((inpw(REG_USB_EPA_CTL)&0x00000040)==0x00000040));
        }
        else
        {
            outpw(REG_USB_EPA_ADDR,DRAM_Addr);
            outpw(REG_USB_EPA_LENTH,Tran_Size);
            DMA_CInt_A_Flag=0;
            outpw(REG_USB_EPA_CTL,inpw(REG_USB_EPA_CTL)|0x00000004);
            while((USBModeFlag)&&(DMA_CInt_A_Flag==0));

        }

    }
    else if (epname == EP_B)
    {
        if(Tran_Size==0)
        {
            DMA_CInt_B_Flag=0;
            outpw(REG_USB_EPB_CTL,inpw(REG_USB_EPB_CTL)|0x00000040);
            while((USBModeFlag)&&(DMA_CInt_B_Flag==0));

        }
        else
        {
            outpw(REG_USB_EPB_ADDR,DRAM_Addr);
            outpw(REG_USB_EPB_LENTH,Tran_Size);
            DMA_CInt_B_Flag=0;
            outpw(REG_USB_EPB_CTL,inpw(REG_USB_EPB_CTL)|0x00000004);
            while((USBModeFlag)&&(DMA_CInt_B_Flag==0));

        }
    }
    else if (epname == EP_C)
    {
        if(Tran_Size==0)
        {
            DMA_CInt_C_Flag=0;
            outpw(REG_USB_EPC_CTL,inpw(REG_USB_EPC_CTL)|0x00000040);
            while((USBModeFlag)&&(DMA_CInt_C_Flag==0));

        }
        else
        {
            outpw(REG_USB_EPC_ADDR,DRAM_Addr);
            outpw(REG_USB_EPC_LENTH,Tran_Size);
            DMA_CInt_C_Flag=0;
            outpw(REG_USB_EPC_CTL,inpw(REG_USB_EPC_CTL)|0x00000004);
            while((USBModeFlag)&&(DMA_CInt_C_Flag==0));

        }
    }
}

void Outp_Byte(UINT32 addr,UINT8 value)
{
    UINT32 tmp1,tmp2;
    
    tmp1=inpw(addr&0xfffffffc);
    tmp2=(UINT32)(value);
    if (addr%4==0)
    {
        tmp1=tmp1&0xffffff00;
        tmp1=tmp1|tmp2;
    }
    else if (addr%4==1)
    {
        tmp1=tmp1&0xffff00ff;
        tmp1=tmp1|(tmp2<<8);
    }
    else if (addr%4==2)
    {
        tmp1=tmp1&0xff00ffff;
        tmp1=tmp1|(tmp2<<16);
    }
    else if (addr%4==3)
    {
        tmp1=tmp1&0x00ffffff;
        tmp1=tmp1|(tmp2<<24);
    }
    outpw((addr&0xfffffffc),tmp1);
}

void SDRAM2USB_Bulk(UINT32 DRAM_Addr ,UINT32 Tran_Size)
{
    if ((USB_EP_Inf_Array[EP_A].EP_Dir==Ep_In)&&(USB_EP_Inf_Array[EP_A].EP_Type==Ep_Bulk))
        SDRAM_USB_Transfer(EP_A,DRAM_Addr,Tran_Size);
    else if ((USB_EP_Inf_Array[EP_B].EP_Dir==Ep_In)&&(USB_EP_Inf_Array[EP_B].EP_Type==Ep_Bulk))
    	SDRAM_USB_Transfer(EP_B,DRAM_Addr,Tran_Size);
    else if ((USB_EP_Inf_Array[EP_C].EP_Dir==Ep_In)&&(USB_EP_Inf_Array[EP_C].EP_Type==Ep_Bulk))
    	SDRAM_USB_Transfer(EP_C,DRAM_Addr,Tran_Size);
}
void USB2SDRAM_Bulk(UINT32 DRAM_Addr ,UINT32 Tran_Size)
{
    if ((USB_EP_Inf_Array[EP_A].EP_Dir==Ep_Out)&&(USB_EP_Inf_Array[EP_A].EP_Type==Ep_Bulk))
        SDRAM_USB_Transfer(EP_A,DRAM_Addr,Tran_Size);
    else if ((USB_EP_Inf_Array[EP_B].EP_Dir==Ep_Out)&&(USB_EP_Inf_Array[EP_B].EP_Type==Ep_Bulk))
        SDRAM_USB_Transfer(EP_B,DRAM_Addr,Tran_Size);
    else if ((USB_EP_Inf_Array[EP_C].EP_Dir==Ep_Out)&&(USB_EP_Inf_Array[EP_C].EP_Type==Ep_Bulk))
        SDRAM_USB_Transfer(EP_C,DRAM_Addr,Tran_Size);
}

void SDRAM2USB_Int(UINT32 DRAM_Addr ,UINT32 Tran_Size)
{
    if ((USB_EP_Inf_Array[EP_A].EP_Dir==Ep_In)&&(USB_EP_Inf_Array[EP_A].EP_Type==Ep_Int))
        SDRAM_USB_Transfer(EP_A,DRAM_Addr,Tran_Size);
    else if ((USB_EP_Inf_Array[EP_B].EP_Dir==Ep_In)&&(USB_EP_Inf_Array[EP_B].EP_Type==Ep_Int))
    	SDRAM_USB_Transfer(EP_B,DRAM_Addr,Tran_Size);
    else if ((USB_EP_Inf_Array[EP_C].EP_Dir==Ep_In)&&(USB_EP_Inf_Array[EP_C].EP_Type==Ep_Int))
    	SDRAM_USB_Transfer(EP_C,DRAM_Addr,Tran_Size);
}

int usb_recv(UINT8* buf,UINT32 len)
{
	SDRAM_USB_Transfer(EP_B,(UINT32)buf,len);
	return len;
}

int usb_send(UINT8* buf,UINT32 len)
{
	
	if ((len%64)==0)
    {
       SDRAM_USB_Transfer(EP_A,(UINT32)buf,len-1);
       SDRAM_USB_Transfer(EP_A,(UINT32)buf+len-1,1);
	}
    else
       SDRAM_USB_Transfer(EP_A,(UINT32)buf,len);
  
	return len;
}

