/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 * Winbond W90P710 Boot Loader
 *
 * Module Name: FLASH_FU.C
 *
 * Created by : PC31 WSChang
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "flash.h"



#define MAX_NUM 8
#define ALLOC_BUFFER 0
#define BUFFER 0x200000		// static memory assigned
#define BUFFER_SIZE 0x200000 // for allocate memory
//#define BLOCK_LOCK_ENABLE

char * GetAddress(char *string);
unsigned int s2hex(char *s);
int validhex(char *s);


#if 0
INT main(void)
{
	tfooter footer;
	UINT32 i,sum,*p;
	short data[16];
	
	for(i=0;i<16;i++)
		data[i]=i+1;
	
	BlockLock_E28F640(0x7f000000,BLOCK_UNLOCK);
	BlockErase_E28F640(0x7f000000,0x10000);
	BlockWrite_E28F640(0x7f000000, (UCHAR *)data, 32);	
	DelImage(0);	
	
	footer.num=0;
	footer.base=0x7F180000;
	footer.length=0x18000;
	footer.load_address=0x8000;
	footer.exec_address=0x8000;
	footer.signature=SIGNATURE_WORD;
	footer.type=IMAGE_ACTIVE|IMAGE_COPY2RAM|IMAGE_EXEC;
	
	p=(UINT32 *)&footer;
	sum=0;
	for(i=0;i<sizeof(tfooter)/4-1;i++)
		sum+=*(p+i);
	footer.checksum=sum;
	
	
	for(i=0;i< footer.length/4;i++)
		*((volatile unsigned int *)0x200000+i)=i;
		
	if( WriteImage(&footer, 0x200000)==0 )
		printf("Write ok!\n");	
	else
		printf("Write failed!\n");	
	
	
	
	

}


#else
INT main(void)
{
	UINT32 srcAddress;
	UINT32 destAddress=FLASH_BASE;
	UINT32 fileSize;
	UINT32 flash_type;
	UINT32 blockSize,src,dest;
	UCHAR pid0,pid1;
	INT i;
	CHAR *p;
	FILE *rfp;
	//CHAR *image_name="C:\\PC30VSS\\Source\\Emulation Programs\\Chip Verification Board\\BOOT_IMAGE\\bootMonitor_LT_standalone_6052_04.bin";
	//CHAR *image_name="c:\\PC30VSS\\Source\\booter\\booter_Data\\ubooter\\booter.bin";
	CHAR image_name[256];

	/* Disable cache */
	*((volatile unsigned int *)0xFFF02000)=0x0;
	*((volatile unsigned int *)0xFFF01004)=0xFE030040;
	

	printf("Please input the image file name:");
	scanf("%s",&image_name[0]);
	/*flush the UART*/
	getchar();
	/* open file */
	if( (rfp=fopen(image_name,"rb"))==NULL )
	{
		printf("ERROR: Can't open file: %s\n",image_name);
		return -1;
	}

#if ALLOC_BUFFER
	if ( (srcAddress= (UINT32)malloc(BUFFER_SIZE))==NULL )
	{
		printf("ERROR: Can't allocate memory for download buffer!\n");
		return -1;
	}
#else
	srcAddress=BUFFER;
#endif

	/* download the data for programming to srcAddress by TFTP, Xmodem, ... */
	/* read file */
	fseek(rfp,0,SEEK_END);
	fileSize=ftell(rfp);
	rewind(rfp);
	if( fread((char *)srcAddress,fileSize,1,rfp)==1 )
		printf("File %s load ok!\n",image_name);
	else
	{
		printf("File %s read failed!\n",image_name);
		return -1;
	}


	printf("\n");
	printf("Flash Detecting ... \n");
	// Detect the flash type
	flash_type=-1;
	for(i=0;i<FLASH_TYPE_NUM;i++)
	{
		flash[i].ReadPID(FLASH_BASE, &pid0, &pid1);
		if( (flash[i].PID0 == pid0) && (flash[i].PID1 == pid1) )
		{
			flash_type=i;
			printf("Flash type is: %s\n",flash[i].name);
			break;		
		}
	}
	if( flash_type==-1 )
	{
		printf("Unsupported flash type!!\n");
		return -1;
	}


	/* Get the flash address written */
	p=GetAddress("Enter image load address (Enter for default):");
	if( validhex( p ) )
		destAddress = s2hex(p);
	else
		printf("Image load address default to 0x%08x\n",destAddress);
	
	
	printf("Flash programming ");
	// Write program
	i=(fileSize&(~0x3))+0x4; //word aligment
	src=srcAddress;
	dest=destAddress;
#ifdef BLOCK_LOCK_ENABLE	
	flash[flash_type].BlockLock(dest, BLOCK_UNLOCK);// Block unlock!
#endif
	while(i)
	{
		putchar('.');
		blockSize=flash[flash_type].BlockSize(dest);
		if( (dest&(blockSize-1)) )
			blockSize=blockSize-(dest&(blockSize-1));
		else
			flash[flash_type].BlockErase(dest, blockSize);
		if( i < blockSize )blockSize=i; // Check if > a block size
		flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
#ifdef BLOCK_LOCK_ENABLE	
		flash[flash_type].BlockLock(dest, BLOCK_LOCK);// Block lock!
#endif
		src+=blockSize;
		dest+=blockSize;
		i-=blockSize;
	}
	printf(" OK!\n");
	printf("Verifing ");
	// Verify program
	i=(fileSize&(~0x3))+0x4; //word aligment
	src=srcAddress;
	dest=destAddress;
	blockSize=flash[flash_type].BlockSize(destAddress);
	blockSize=blockSize-1;
	while(i)
	{
		if( (i&blockSize)==0x0 )putchar('.');
		if( inpw(src) != inpw(dest) )
		{
			printf("\nERROR: A:0x%08x W:0x%08x R:0x%08x!!\n",dest,inpw(src),inpw(dest));
			return -1;
		}
		src+=4;
		dest+=4;
		i-=4;
	}
	printf(" OK!\n");	
	printf("Programming finished!!\n");	
	return 0;
}
#endif


char * GetAddress(char *string)
{
	static char str[16];
	int i;
	char ch;
	
	for(i=0;i<16;i++)str[i]=0;
	printf("%s ",string);
	i=0;
	while(1)
	{
		ch=getchar();
		if( ch == '\n' || ch == '\r' )
		{
			putchar('\n');
			return str;
		}
		else
		{
			if( ch == '\b' )
			{
			  if(  i >= 1 )
			  {
				str[--i]='\0';
				printf("\r                                                             \r%s %s",string,str);
			  }	
			}
			else
			{
			
				if( i<15 )
				{
					str[i++]=ch;
					putchar(ch);
				}
			}		
		}
	}
}


int chartohex(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;
    return -1;
}

unsigned int s2hex(char *s)
{
    char nStack[MAX_NUM];
    int n;
    unsigned int result;
    int shift;

    // hop over the 0x start
    s += 2;

    // stack the number in reverse order
    n = 0;
    while ((*s != '\0') && (n < MAX_NUM))
    {
        nStack[n++] = *s++;
    }

    // Pull characters off the stack, converting to hex on the way
    result = 0;
    shift = 0;
    while (n > 0)
    {
        result += (chartohex(nStack[--n]) << shift);
        shift += 4;
    }

    return result;
}


int validhex(char *s)
{
    // Is this string a valid hex number?
    // For example, of the form 0xNNNNNNNN

    // valid length?
    if (strlen(s) > 10)
        return 0;
    if (strlen(s) <= 2)
        return 0;

    // valid start? (0x or 0X)
    if ((s[0] != '0') || ((s[1] != 'X') && (s[1] != 'x')))
        return 0;

    s += 2;

    // valid characters?
    while (*s != '\0')
    {
        if (chartohex(*s) < 0)
            // this character not valid
            return 0;

        // go onto the next character
        s++;
    }

    // success
    return 1;
}
