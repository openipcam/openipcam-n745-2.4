/*
 * $Log: fllnx.c,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 */

/***********************************************************************************/
/*                        M-Systems Confidential                                   */
/*           Copyright (C) M-Systems Flash Disk Pioneers Ltd. 1995-2001            */
/*                         All Rights Reserved                                     */
/***********************************************************************************/
/*                            NOTICE OF M-SYSTEMS OEM                              */
/*                           SOFTWARE LICENSE AGREEMENT                            */
/*                                                                                 */
/*      THE USE OF THIS SOFTWARE IS GOVERNED BY A SEPARATE LICENSE                 */
/*      AGREEMENT BETWEEN THE OEM AND M-SYSTEMS. REFER TO THAT AGREEMENT           */
/*      FOR THE SPECIFIC TERMS AND CONDITIONS OF USE,                              */
/*      OR CONTACT M-SYSTEMS FOR LICENSE ASSISTANCE:                               */
/*      E-MAIL = info@m-sys.com                                                    */
/***********************************************************************************/

/*
 * fllnx.c
 *
 * Linux driver support
 *
 * This file implements a generic abstration layer between the Linux block
 * device layer and the M-Systems OSAK library.
 *
 * This peforms two functions. 
 * 
 * First it allows us to move all linux specific code up into the driver
 * implementation where it can be compiled with knowledge of the kernel
 * implementation. This includes some low level functions such as memcpy and
 * memory access. 
 *
 * Second it keeps all OSAK specific knowledge localized into the libosak
 * code base. The top level driver needs only a minimal amount of
 * information, none of which comes from the OSAK headers files. 
 *
 * The OSAK library can be built without reference to any kernel files.
 * Removing any requirement to compile it on a target machine simply to know
 * what the kernel include files are for the target environment.
 *
 * Two files are exported into the public area to help build the linux
 * driver. 
 *      
 *      fllnx.h         defines the generic access methods
 *      libosak.a       the unlinked OSAK code and fllnx.o (this file)
 *
 */

#include "flcustom.h"
#include "flstatus.h"
#include "flreq.h"
#include "flsocket.h"
#include "flsystem.h"
#include "fatfilt.h"
#include "flioctl.h"

#include "fllnx.h"
#include "blockdev.h"

/*
 * The following are all of the calls to M-Systems OSAK. 
 *
 */


/* Initialize M-Systems OSAK. */
int fl_doc_init()
{
	DEBUG_PRINT("fllnx version: %s\n", FLLNX_VERSION);
	if (flInit())
	{
		flprintk(0,"M-Systems DiskOnChip not found\n");
		return (-1);
	}
	return (0);
}

/* fl_doc_mount()
 *
 * Mount DOC volume so we can use it.
 */
int fl_doc_mount(int VOL)
{
	IOreq           ioreq;
	int             status;

	ioreq.irHandle = VOL;

	if ((status = flAbsMountVolume(&ioreq)) != 0)
	{
		return (-1);
	}
	return (0);
}

/* Dismount DOC volume */
int fl_doc_dismount(int VOL)
{
	IOreq           ioreq;
	int             status;

	ioreq.irHandle = VOL;
	if ((status = flDismountVolume(&ioreq)) != 0)
	{
		flprintk(0,"fl_doc_dismount: Cannot dismount DOC volume: %d\n", VOL);
		return (-1);
	}
	return (0);
}

/* test mount upto DRIVES volumes to see how many we can find */

extern unsigned  noOfDrives;
int fl_doc_count(int max)
{
	return (noOfDrives);
}

unsigned char fl_partition_count(int socket)
{
	IOreq ioreq;

	DEBUG_PRINT("In fl_partition_count\n");

	ioreq.irHandle=socket;
	if(!flCountVolumes(&ioreq))
	{
	    return ioreq.irFlags;
	}
	else
	{
	    return 0xff;
	}
}

/* Get the number of sectors in the DOC volume */
int fl_doc_sectors(int VOL)
{
	IOreq           ioreq;
	int             status;

	ioreq.irHandle = VOL;
	if ((status = flSectorsInVolume(&ioreq)) != 0)
	{
		flprintk(0,"Cannot get sectors in volume: %d\n", VOL);
		return (0);
	}
	return (ioreq.irLength);
}

/* return 1 if volume read only)*/
int fl_protType(int VOL)
{
	IOreq           ioreq;
	int stat=0;

	ioreq.irHandle = VOL;
	stat=flIdentifyProtection(&ioreq);
	if(!stat)
	{
		if( (ioreq.irFlags&READ_PROTECTED || ioreq.irFlags&WRITE_PROTECTED) && (!(ioreq.irFlags&KEY_INSERTED)) )
		{
			flprintk(1,"fl_protType:%x: Volume protected\n", VOL);
			return 1;
		}
	}
	else
		flprintk(1,"fl_protType:%x: flIdentifyProtection returns %d\n", VOL,stat);
	return 0;
}

/* Perform read or write of appropriate number of sectors from a DOC volume */
int fl_doc_read(int VOL, void *data, int block, int count)
{
	IOreq ioreq;
	static int status=0;

	ioreq.irHandle = VOL;
	ioreq.irData = data;
	ioreq.irSectorNo = block;
	ioreq.irSectorCount = count;
	if ((status = flAbsRead(&ioreq)))
		flprintk(0,"%d: flAbsRead() status %d, block=%d count=%d\n", VOL, status, block, count);
	return status;
}

/* Perform read or write of appropriate number of sectors from a DOC volume */
int fl_doc_write(int VOL, void *data, int block, int count)
{
	IOreq           ioreq;
	int             status=0;

	ioreq.irHandle = VOL;
	ioreq.irData = data;
	ioreq.irSectorNo = block;
	ioreq.irSectorCount = count;

#ifdef FL_FAT_FILTER
	if ((status = ffCheckBeforeWrite(&ioreq)))
		flprintk(0,"VOL: %d ffCheckBeforeWrite() status %d, block=%d count=%d\n", VOL, status, block, count);
#endif /* FL_FAT_FILTER */		
	if ((status = flAbsWrite(&ioreq)))
		flprintk(0,"VOL: %d flAbsWrite() status %d, block=%d count=%d\n", VOL, status, block, count);
	return (status);
}

unsigned char flRemoveProtectionFromParm(unsigned char bPart,unsigned char*pKey)
{
	IOreq ioreq;
	ioreq.irHandle=bPart;
	ioreq.irData=pKey;
	if(flInsertProtectionKey(&ioreq))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* non-OSAK IOCTLs defenitions */
#define FL_IOCTL_LNX	(FL_IOCTL_DEEP_POWER_DOWN_MODE+1)
typedef struct
{
	unsigned long command;
	unsigned long data;
} flInputLnxRecord;

typedef struct
{
	unsigned long status;
	unsigned long data;
} flOutputLnxRecord;
/* end of non-OSAK IOCTLs defenitions */

int fl_doc_ioctl(int bdtlVolume, int cmd,unsigned long arg)
{
	unsigned char	bdkVolume=0;
	IOreq           ioreq;
	int             status;

	DEBUG_PRINT("In fl_doc_ioctl: bdtlVolume %d bdkVolume %d command %d\n",bdtlVolume,bdkVolume,cmd);

/* maybe arg doesn't correspond exactly to what irData should be, and so we have to do some unpickling */
	switch (cmd)
	{
	case FL_IOCTL_GET_INFO:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_GET_INFO;
		break;
	case FL_IOCTL_DEFRAGMENT:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_DEFRAGMENT;
		break;
	case FL_IOCTL_WRITE_PROTECT:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_WRITE_PROTECT;
		break;
	case FL_IOCTL_MOUNT_VOLUME:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_MOUNT_VOLUME;
		break;
	case FL_IOCTL_FORMAT_VOLUME:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_FORMAT_VOLUME;
		break;
	case FL_IOCTL_BDK_OPERATION:
		ioreq.irHandle = bdkVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_BDK_OPERATION;
		break;
	case FL_IOCTL_DELETE_SECTORS:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_DELETE_SECTORS;
		break;
	case FL_IOCTL_READ_SECTORS:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_READ_SECTORS;
		break;
	case FL_IOCTL_WRITE_SECTORS:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_WRITE_SECTORS;
		break;
	case FL_IOCTL_FORMAT_PHYSICAL_DRIVE:
		ioreq.irHandle = bdtlVolume&0x0f;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_FORMAT_PHYSICAL_DRIVE;
		break;
	case FL_IOCTL_FORMAT_LOGICAL_DRIVE:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_FORMAT_LOGICAL_DRIVE;
		break;
	case FL_IOCTL_BDTL_HW_PROTECTION:
		ioreq.irHandle = bdtlVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_BDTL_HW_PROTECTION;
		break;
	case FL_IOCTL_BINARY_HW_PROTECTION:
		ioreq.irHandle = bdkVolume;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_BINARY_HW_PROTECTION;
		break;
	case FL_IOCTL_OTP:
		ioreq.irHandle = bdtlVolume&0x0f;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_OTP;
		break;
	case FL_IOCTL_CUSTOMER_ID:
		ioreq.irHandle = bdtlVolume&0x0f;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_CUSTOMER_ID;
		break;
	case FL_IOCTL_UNIQUE_ID:
		ioreq.irHandle = bdtlVolume&0x0f;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_UNIQUE_ID;
		break;
	case FL_IOCTL_NUMBER_OF_PARTITIONS:
		ioreq.irHandle = bdtlVolume&0x0f;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_NUMBER_OF_PARTITIONS;
		break;
	case FL_IOCTL_INQUIRE_CAPABILITIES:
		ioreq.irHandle = 0;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_INQUIRE_CAPABILITIES;
		break;
	case FL_IOCTL_SET_ENVIRONMENT_VARIABLES:
		ioreq.irHandle = 0;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_SET_ENVIRONMENT_VARIABLES;
		break;
	case FL_IOCTL_PLACE_EXB_BY_BUFFER:
		ioreq.irHandle = bdtlVolume&0x0f;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_PLACE_EXB_BY_BUFFER;
		break;
	case FL_IOCTL_WRITE_IPL:
		ioreq.irHandle = bdtlVolume&0x0f;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_WRITE_IPL;
		break;
	case FL_IOCTL_DEEP_POWER_DOWN_MODE:
		ioreq.irHandle = bdtlVolume&0x0f;
		ioreq.irData = (void *) arg;
		ioreq.irFlags = FL_IOCTL_DEEP_POWER_DOWN_MODE;
		break;
	case FL_IOCTL_LNX:
		{
			flInputLnxRecord*inRec=(flInputLnxRecord*)(((flIOctlRecord*)arg)->inputRecord);
			flOutputLnxRecord*outRec=(flOutputLnxRecord*)(((flIOctlRecord*)arg)->outputRecord);
			switch( inRec -> command )
			{
			case 0:	// set BKD volume
				bdkVolume=inRec->data;
				return (outRec->status=flOK);
			default:
				return (outRec->status=flBadParameter);
			}
		}
	default:
		return flBadParameter;
	}

	if ((status = flIOctl(&ioreq)))
	{
		flprintk(0,"fl: bdtlVolume: %d flIOctl() status %d, cmd=%d", bdtlVolume, status, cmd);
	}
	return (status);
}
