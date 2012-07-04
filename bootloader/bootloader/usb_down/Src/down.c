#include <string.h>
#include "u710def.h"
#include "usb.h"
#include "uprintf.h"

void UXmodem(UINT32 _base,unsigned int* fileSize);

//#define DOWN_MEM	0x400000 //start at 4M

int Disable_USB()
{
	Disable_Int(IRQ_USBD);
   	
   	outpw(REG_USB_CTL,0x0);//disable usb enginee
   	
   	return 0;
	
}

int Enable_USB()
{
	 USB_Init();
    //WB_SetGlobalInterrupt(DISABLE_ALL_INTERRUPTS);
    
    USB_Int_Init();
    
    return 0;
	
}

void usb_down(UINT32 _base,unsigned int* fileSize)
{
	
   
    /* enable CPSR I bit */
    //WB_SetLocalInterrupt(ENABLE_IRQ);

   	UXmodem(_base,fileSize);
   	
   	//Disable_USB();

}