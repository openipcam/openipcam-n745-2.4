/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: flash_demo.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: flash_demo.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/WBLv1_1/Src
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:28p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/WBLv1_1/Src
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/12/03   Time: 5:16p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add MX29LV160BT/TT
 * Fixed MX28F160C3BT/TT.
 * The MX28F160C3 sectors are default to be locked, thus it needs unlock
 * it before write/erase it.
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/27   Time: 11:27a
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add ChangeImageType function
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:39p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 11:53a
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add VSS header
 */

#include <stdio.h>
#include <string.h>
#include "flash.h"
#include "bib.h"

#define BL_PHY 0
#define BL_IC_PLUS 1
#define BL_MARVELL6052 2




INT main(void)
{
	tfooter footer;
	UINT32 i,j;
	tfooter * pfooter;
	tbl_info info;

	// 1st step: find flash
	FindFlash();
	// The flash size ...
	printf("The flash size:0x%08x\n",FlashSize());
	
	if( FindImage(0,&pfooter) ) // find the image 0
	{
		// read the data in image 0 to tbl_info structure
		memcpy((CHAR *)&info,(CHAR *)pfooter->base, sizeof(tbl_info)); 
		// check if the data in image 0 is correct format of tbl_info. If correct ==> read all data
		if( info.type == BOOTLOADER_INFO )
		{
			printf("Boot Loader Configuration:\n\n");
			printf("\t%-20s: MAC %d\n","TFTP server port",info.net_mac);
			printf("\t%-20s: ","Network phy chip");
			switch( info.phy )
			{
				case BL_PHY:
					printf("PHY");
					break;
				case BL_IC_PLUS:
					printf("IC PLUS");
					break;
				case BL_MARVELL6052:
					printf("MARVELL 6052");
					break;
				default:
					break;			
			}
			printf("\n");
			printf("\t%-20s: %02x:%02x:%02x:%02x:%02x:%02x\n","MAC 0 Address",info.mac0[0],info.mac0[1],info.mac0[2],info.mac0[3],info.mac0[4],info.mac0[5]);
			printf("\t%-20s: %d.%d.%d.%d\n","IP 0 Address",info.ip0[0],info.ip0[1],info.ip0[2],info.ip0[3]);
			printf("\t%-20s: %02x:%02x:%02x:%02x:%02x:%02x\n","MAC 1 Address",info.mac1[0],info.mac1[1],info.mac1[2],info.mac1[3],info.mac1[4],info.mac1[5]);
			printf("\t%-20s: %d.%d.%d.%d\n","IP 1 Address",info.ip1[0],info.ip1[1],info.ip1[2],info.ip1[3]);
			if( info.dhcp )
				printf("\t%-20s: Enabled\n","DHCP Client");	
			else
				printf("\t%-20s: Disabled\n","DHCP Client");	
			if( info.cache )
				printf("\t%-20s: Enabled\n","CACHE");	
			else
				printf("\t%-20s: Disabled\n","CACHE");	
		}
		else
		{
			printf("ERROR: Unknown data\n");
			return -1;		
		}
	}
	else
		printf("Can't find image 0\n");


	
	// Before you update your image, you must delete it first
	printf("WARNING: The flash data will be erased, continue? ");
	if( getchar() != 'y' )return 0;
	printf("\n");
	for(i=2;i<FlashSize()/FLASH_BLOCK_SIZE;i++)	
	{
		printf("Deleting block %d ... ",i);
		DelBlock(i);
		printf("Deleted!\n");
	}
	
	//return 0;

	for( j=1;j< MAX_FOOTER_NUM ;j++)
	{
	
		footer.num=j; // image number
		memcpy(footer.name,"test image",11);
		footer.base=0x7F010000+j*FLASH_BLOCK_SIZE;	// image base address 
		footer.length=0x100;	// image size
		footer.load_address=0x8000;	// image load address
		footer.exec_address=0x8000; // image exec address
		footer.signature=SIGNATURE_WORD;// This is a key word to identify the image footer location
		footer.type=IMAGE_ACTIVE|IMAGE_COPY2RAM|IMAGE_EXEC; // Setting the attribution of the image
		// IMAGE_ACTIVE => It will be processed by Boot Loader
		// IMAGE_COPY2RAM => It will be copy from footer->base to footer->load_address with size footer->length
		// IMAGE_EXEC => It will be executed by Boot Loader with entry point at footer->exec_address
		// IMAGE_FILE => Identified as a file system
		// NOTE: Only the image with IMAGE_ACTIVE attribution will be processed by Boot Loader
		// If you want to let Boot Loader execute an image automatically, you should set the 
		// attributions as "-acx", where 'a' indicating IMAGE_ACTIVE, 'c' indicating IMAGE_COPY2RAM,
		// 'x' indicating IMAGE_EXEC.
	
		// Prepare some data to write	
		for(i=0x0200000;i<0x020FF00;i++)
		{	
			*((UCHAR*)i)=i;
		}
	
		// Write the data into flash. The relative image information is in footer structer.
		printf("Writing image %d base=0x%08x ... \n",footer.num,footer.base);

		if( WriteImage(&footer, 0x200000)==0 )
			printf("Write ok!\n");	
		else
			printf("Write failed!\n");	

		// Read image and compare its data
		if( FindImage(j,&pfooter) )
		{
			printf("Found image %d, checking data ... ",j);
			for(i=pfooter->base;i<pfooter->base+pfooter->length;i++)
			{
				if( (i&0xF)==0 )printf("\n[%08x] ",i);
				printf("%02x ",*((UCHAR *)i));
				if( *((UCHAR *)i) != *((UCHAR *)(i-pfooter->base+0x200000)) )
				{
					printf("\nERROR: A:0x%08x W:0x%08x R:0x%08x\n",i,*((UCHAR *)(i-pfooter->base+0x200000)),*((UCHAR *)i));
					return -1;
				}
			}
		printf("\n >> ok!\n");
		}
		else
		{
			printf("ERROR: Can't found image %d ... \n",j);
			return -1;
		}
		
		printf("Deleting image %d ... ",j);
		DelImage( j );
		printf("Deleted!\n");

	}//end for j loop

}

