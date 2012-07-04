/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: bootproc.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: bootproc.c $
 * 
 * *****************  Version 5  *****************
 * User: Yachen       Date: 06/04/06   Time: 10:37a
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Removed 7-seg code to fix data abort problem
 * 
 * *****************  Version 4  *****************
 * User: Yachen       Date: 06/02/13   Time: 11:24a
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Disable USB before executing images, restore it afterward
 * 
 * *****************  Version 3  *****************
 * User: Yachen       Date: 06/01/20   Time: 7:30p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Add USB status flag, disable USB only if it's enabled
 * 
 * *****************  Version 2  *****************
 * User: Yachen       Date: 06/01/19   Time: 2:18p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Add USB support. User can chose between USB and MAC in shell.map
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/shell
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/shell
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:07p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add VSS header
 */
#include <string.h>
#include <errno.h>
#include "platform.h"
#include "flash.h"
#include "zlib.h"
#include "armzip.h"
#include "bib.h"
#include "uprintf.h"

#define PDEBUG
int enableUSB = 0;

INT BootProcess()
{
	tfooter * pfooter=0;
	INT volatile i,j;
	Bytef *dest;
	Bytef *src;
	uLongf len,slen;
	tbl_info info;

	if( FindImage(0, &pfooter) )
	{
		// Read boot loader configuration
		if( pfooter )
		{
			memcpy((CHAR *)&info, (CHAR *)pfooter->base, sizeof(info));
			if( info.type == BOOTLOADER_INFO )
			{
			/*
				// EBI/SDRAM setting
				if( info.ebicon != -1 )
					*((volatile unsigned int *)EBICON)=info.ebicon;
				if( info.romcon != -1 )
					*((volatile unsigned int *)ROMCON)=info.romcon;
				if( info.sdconf0 != -1 )
					*((volatile unsigned int *)SDCONF0)=info.sdconf0;
				if( info.sdconf1 != -1 )
					*((volatile unsigned int *)SDCONF1)=info.sdconf1;
				if( info.sdtime0 != -1 )
					*((volatile unsigned int *)SDTIME0)=info.sdtime0;
				if( info.sdtime1 != -1 )
					*((volatile unsigned int *)SDTIME1)=info.sdtime1;
			*/		
				// Cache
				if( info.cache )
				{
					CAHCON=0x87;
					while(CAHCON );
					CAHCNF=0x7;
				}
				else
				{
					CAHCNF=0x0;
				}
				if( CAHCNF==0 )uprintf("Cache disabed!\n");
				if( CAHCNF==0x7 )uprintf("Cache enabled!\n");
#ifdef __USB__				
				if(info.usb)
				{
					Enable_USB();
					enableUSB = 1;
					uprintf("USB enabled\n");
				}
				else
				{
					Disable_USB();
					enableUSB = 0;
					uprintf("USB disabled\n");
				}
#endif					
				
					
				
			}
		}
	}
	else
	{
		uprintf("ERROR: Image 0 not found\n");
		return -1;
	}
	

	 
	 	
	
	for(i=1;i<MAX_IMAGE_NUM;i++)
	{
#ifdef PDEBUG
		uprintf("Processing image %d ... \n",i);
#endif
		if( FindImage(i,&pfooter) )
		{
#if 0		
				if( info.cache )
				{
					CAHCON=0x87;
					while(CAHCON );
					CAHCNF=0x7;
				}
				else
				{
					CAHCNF=0x0;
				}		
#endif				
			// Only active images would be processed
			if( pfooter->type&IMAGE_ACTIVE )
			{
				if( (pfooter->type & IMAGE_COPY2RAM) && !(pfooter->type & IMAGE_COMPRESSED))
				{
					for(j=0;j<((pfooter->length&(~0x3))+4)/4;j++)
						*((unsigned int volatile*)pfooter->load_address+j)=*((unsigned int volatile*)pfooter->base+j);
				}
				if( pfooter->type & IMAGE_COMPRESSED )
				{
					dest = (Bytef *)(pfooter->load_address);
					src = (Bytef *)(pfooter->base);
					slen = (uLongf)(pfooter->length);
					len=-1;
					//========>
					if( validZipID(src) == Z_OK )
					{
						int status;
						#ifdef PDEBUG
							uprintf("Unzip image %d ... \n",pfooter->num);
						#endif
						if( (status=unzip(dest,&len,src,slen)) != Z_OK )
						{
							uprintf("\nERROR: unzip failed! %d\n",status);	
							return 0;
						}
					}
					else
					{
						uprintf("ERROR: Invalid zip file\n");
					}
					
				}
				if( pfooter->type & IMAGE_EXEC )
				{
#if 1				
					if( info.cache )
					{
						CAHCON=0x87;
						while(CAHCON );
						CAHCNF=0x7;
					}
					else
					{
						CAHCNF=0x0;
					}					
#endif					
					if( (pfooter->type & IMAGE_COPY2RAM) || (pfooter->type&IMAGE_COMPRESSED) )
					{
						#ifdef PDEBUG
							uprintf("Executing image %d ... \n",pfooter->num);
						#endif
						// Display image number on 7-seg.
//						*(unsigned char volatile *)(0xd0000000) = pfooter->num % 10;
//						if(pfooter->num > 9)
//							*(unsigned char volatile *)(0xd8000000) = 1;
//						else
//							*(unsigned char volatile *)(0xd8000000) = 0;

#ifdef __USB__
						if(enableUSB)	Disable_USB();
#endif
						swi_run(EXEC_SWI_NUM, pfooter->exec_address);
#ifdef __USB__
						if(enableUSB)	Enable_USB();
#endif
							//return(0);

					}
					else
					{
						if( pfooter->base != pfooter->load_address )
						{
							uprintf("ERROR: Executed before copy to load_address\n");
						}
						else
						{
						#ifdef PDEBUG
							uprintf("Executing image %d ... \n",pfooter->num);
						#endif
#ifdef __USB__
						if(enableUSB)	Disable_USB();
#endif						
							swi_run(EXEC_SWI_NUM, pfooter->exec_address);
#ifdef __USB__
						if(enableUSB)	Enable_USB();
#endif							

						}
					}
				}
			}
		}
	}




	return 0;
}