#include <string.h>
#include "u710def.h"
#include "usb.h"
#include "uprintf.h"
#include "uxmodem.h"

int loop=1;

void usleep(int count)
{
	int i=0;
	
	for(i=0;i<count;i++);
	
}


void dump(unsigned char* base)
{
	int i=0;
	
	for(i=0;i<1024;i++)
		uprintf("%c",base[i]);
		
	return;
}


void UXmodem(UINT32 _base,unsigned int* fileSize)
{
	int len;
	unsigned char* ptr;
	unsigned int file_len=0;
	unsigned char* _ch;
	unsigned int* _ack;
	
	_ch=(unsigned char*)(_base);
	_ch=((unsigned char*)(((unsigned int)_ch)|NON_CACHE));

    usleep(10000);//give usb device init time
 	
 	//uprintf("Start USB download...%x,%x\n",_base,_ch);
 	uprintf("Waiting for usb download ...\n");
 	
    	
    	ptr=_ch;
    	file_len=0;
    	
	    while(1)
	    {
		
	    	if(Bulk_Out_Transfer_Size>0)
	    	{
	       		usb_recv((unsigned char*)ptr,4);
	    
	    		file_len=*((unsigned int*)ptr);
	    	
	    		break;
	    	}
	    }
	  
	    if(((UINT32)_ch+file_len)%ALIGN!=0)
	    	_ack=(unsigned int*)(_ch+file_len+(ALIGN-((UINT32)_ch+file_len)%ALIGN));
	    else
	    	_ack=(unsigned int*)(_ch+file_len);

		_ack=((unsigned int*)(((unsigned int)_ack)|NON_CACHE));
		
		*fileSize=file_len;
		
		//uprintf("\nfile len:%d bytes,ack addr:0x%x\n",file_len,_ack);
	    
	    do
	    {
	    	if(Bulk_Out_Transfer_Size>0)
	    	{
	    		len=Bulk_Out_Transfer_Size;
	    		usb_recv(ptr,len);//recv data from PC
	    		ptr+=len;
	
	    		*_ack=len;
	    		usb_send((unsigned char*)_ack,4);//send ack to PC
	    	}
			//printf("download :%d bytes\r",ptr-_ch);
		}while((ptr-_ch)<file_len);
		
		//uprintf("USB download over...\n");
		
		//dump(_ch);
	
	//}
	

    
}