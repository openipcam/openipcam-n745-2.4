/*
 *  drivers/mtd/ssfdc.c
 *
 *  Copyright (C) 2003 Simon Haynes (simon@baydel.con)
 *                     Baydel Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This module provides a translation layer, via mtd, for smart
 * media card access. It essentially enables the possibility 
 * of using cards on a hardware which does not have a hardware translation
 * layer and interchanging them with hardware that does ie: PC card readers
 *
 * I had to write this module for a specific task and in a short timeframe
 * for this reason I have imposed some restricions to make the job easier.
 *
 * To build an compile the driver I added the following lines
 * to mtd/Config.in
 *
 *  dep_tristate '  SSFDC support' CONFIG_SSFDC $CONFIG_MTD
 *
 * to /mtd/Makefile
 *
 * obj-$(CONFIG_SSFDC)             += ssfdc.o
 *
 * and compiled the kernel via the usual methods.
 *
 * I am sure that there are many problems I don't know about but here are
 * some that I know of
 *
 * Currently the driver uses MAJOR number 44 which I think is FTL or NFTL
 * I did this because I wanted a static number and I didn't know
 * how to go about getting a new one. This needs addressing
 * The dev nodes required are like standard. I only use minor 0
 * (/dev/ssfdca), and minor 1 (/dev/ssfdca1).
 * You should be able to run fdisk on /dev/ssfdca and the first partition
 * is /dev/ssfdca1. There is no working code in the module for changing the
 * SMC and rebuilding the maps so the card should not be changed once the
 * module is loaded. At present I only look for 1 partition. But this is a
 * small commented hack.
 *
 * There is no support cards which do not have a 512 byte page size with 16
 * bytes of oob and an erase size of 16K.
 * There are no checks for this at present. In addition the MTD reported size
 * must be 16M or a multiple.
 *
 * Code to handle multiple partitions or multiple cards is incomplete
 * Need to allocate data buffer and oob buffer on a per partition basis.
 * As I am only concerned with one partition I will do this if I ever need to.
 * The cached physical address variable also needs this attention.
 *
 * Recently I have started to work on media changes. Some of this is specific
 * to my hardware and you will see references to pt_ssfdc_smc and smc_status.
 * This code is incomplete and does not work. I have commented it for the moment
 * but it should give an indication of what I think is required. Maybe there is
 * something it mtd that can help
 *
 * 17th August 2004 MHB
 *
 * Following updating CVS I noticed some single bit data corruption. I believe
 * that this was down to the fact that I was using mtd->read instead of mtd->read_ecc
 * and that mtd->read was applying it's own error corretion from the wrong ecc bytes
 * I have now corrected this.
 *
 * During this time I noticed that while in allocate new I only seem to look for blocks
 * in 1 zone. So this limits the partition size to 16MB with all the other SMC size
 * restrictions


*/

#include <linux/config.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/blktrans.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/major.h>
#include <linux/ioctl.h>
#include <linux/hdreg.h>
#include <linux/list.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>


#if (LINUX_VERSION_CODE >= 0x20100)
#include <linux/vmalloc.h>
#endif
#if (LINUX_VERSION_CODE >= 0x20303)
#include <linux/blkpg.h>
#endif

#include <asm/semaphore.h>

#define SSFDC_FORMAT 1

#define PDEBUG(fmt, args...)

#define BLK_INC_USE_COUNT MOD_INC_USE_COUNT
#define BLK_DEC_USE_COUNT MOD_DEC_USE_COUNT

#if (LINUX_VERSION_CODE < 0x20320)
#define BLK_DEFAULT_QUEUE(n)    blk_dev[n].request_fn
#define blk_init_queue(q, req)  q = (req)
#define blk_cleanup_queue(q)    q = NULL
#define request_arg_t           void
#else
#define request_arg_t           request_queue_t *q
#endif

#define TRUE 1
#define FALSE 0

#define SSFDC_MAJOR	44

#define MAJOR_NR		SSFDC_MAJOR
#define DEVICE_NAME		"ssfdc"
#define DEVICE_REQUEST		do_ssfdc_request
#define DEVICE_ON(device)
#define DEVICE_OFF(device)

#include <linux/blk.h>

#include "/home/simon/ebony/dbwhatu/dbwhatu/smccontrol.h"



#define ZONE_SIZE 		(16 * 1024 * 1024)
#define SMC_BLOCK_SIZE 		(16 * 1024)
#define SECTOR_SIZE 		512
#define SECTORS_PER_ZONE  	(ZONE_SIZE / SECTOR_SIZE)
#define BLOCKS_PER_ZONE	    	(ZONE_SIZE / SMC_BLOCK_SIZE)
#define SECTORS_PER_BLOCK	(SMC_BLOCK_SIZE / SECTOR_SIZE)
#define OOB_SIZE		16


#define MAX_DEVICES 	4
#define MAX_PARTITIONS 	8
#define PARTITION_BITS 	3
#define MAX_ZONES 	8


int ssfdc_major = SSFDC_MAJOR;
unsigned int ssfdc_cached = 0xFFFFFFFF;
static unsigned char ssfdc_scratch[16384];
static unsigned char ssfdc_buffer[16];
static unsigned char ssfdc_ffoob_buf[OOB_SIZE * SECTORS_PER_BLOCK];
static unsigned char ssfdc_oob_buf[OOB_SIZE * SECTORS_PER_BLOCK];


static struct nand_oobinfo ssfdc_ffoob_info = {
	.useecc = 0,
};


typedef struct minor_t {
	atomic_t open;
	int cached;
	unsigned char * pt_data;
	unsigned char * pt_oob;
} minor_t;



typedef struct partition_t {
	int type;
    	struct mtd_info	*mtd;
    	int count;
	unsigned int *zone;
	unsigned int zoneCount;
	minor_t minor[MAX_PARTITIONS];
	unsigned int last_written[MAX_ZONES];
} partition_t;

partition_t SMCParts[MAX_DEVICES];


static unsigned char ssfdc_ecc[] = {14, 13, 15, 9, 8, 10};

static struct hd_struct ssfdc_hd[MAX_DEVICES * MAX_PARTITIONS];
static int ssfdc_sizes[MAX_DEVICES * MAX_PARTITIONS];
static int ssfdc_blocksizes[MAX_DEVICES * MAX_PARTITIONS];
smc_control * pt_ssfdc_smc;


static struct gendisk ssfdc_gendisk = {
    major:		SSFDC_MAJOR,
    major_name:		"ssfdc",
    minor_shift:	PARTITION_BITS,
    max_p:		MAX_PARTITIONS,
    part:		ssfdc_hd,
    sizes:		ssfdc_sizes,
};


static int	ssfdc_ioctl(struct inode *inode, struct file *file, u_int cmd, u_long arg);
static int 	ssfdc_open(struct inode *inode, struct file *file);
static int 	ssfdc_close(struct inode *inode, struct file *file);
static int 	ssfdc_write(partition_t *part, caddr_t buffer, u_long sector, u_long nblocks);
static int 	ssfdc_read(partition_t *part, caddr_t buffer, u_long sector, u_long nblocks);
static int 	ssfdc_physical(partition_t * pt_smcpart, int zone, int block);
static int 	ssfdc_erase(partition_t *pt_smcpart, unsigned int offset);
static int 	ssfdc_read_partitions(partition_t * pt_smcpart);
static void 	ssfdc_notify_add(struct mtd_info *mtd);
static void 	ssfdc_notify_remove(struct mtd_info *mtd);
static void 	ssfdc_tables(partition_t * pt_smcpart);
static int 	ssfdc_sector_blank(partition_t * pt_smcpart, int sc);
static int  	ssfdc_allocate_new(partition_t * pt_smcpart, int zone);
int 		ssfdc_parity(int number);
static void 	ssfdc_erase_callback(struct erase_info *erase);



static DECLARE_WAIT_QUEUE_HEAD(ssfdc_wq);


static struct mtd_notifier ssfdc_notifier = {
	add:		ssfdc_notify_add,
	remove:		ssfdc_notify_remove,
};



static struct block_device_operations ssfdc_fops = {
    open:	ssfdc_open,
    release:	ssfdc_close,
    ioctl:	ssfdc_ioctl,
};
                                               
static struct semaphore ssfdc_semaphore;

static void ssfdc_notify_add(struct mtd_info *mtd) {



	
	if(mtd->index >= 1) return;   // Hack to limit SSFDC to 1 partition

	if( ((mtd->size % ZONE_SIZE) != 0) && (mtd->size < (ZONE_SIZE * MAX_ZONES)) ){
		PDEBUG("ssfdc_notify_add : mtd partition %d is not modulus 16M, not SSFDC\n", mtd->index);	
	}
	else {
		memset((void *)&SMCParts[mtd->index].type, 0, sizeof(partition_t));	
		SMCParts[mtd->index].mtd = mtd;
		SMCParts[mtd->index].count = mtd->index;
		SMCParts[mtd->index].type = 1;
		SMCParts[mtd->index].zoneCount = mtd->size / ZONE_SIZE;
		SMCParts[mtd->index].zone = kmalloc(SMCParts[mtd->index].zoneCount * 8192, GFP_KERNEL);
		

		if(!SMCParts[mtd->index].zone) {
			printk(KERN_NOTICE "ssfdc_notify_add : mtd partition %d, failed to allocate mapping table\n", mtd->index);
			SMCParts[mtd->index].type = 0;
		}
		else {
			memset((void *)SMCParts[mtd->index].zone, 0xFF, SMCParts[mtd->index].zoneCount * 8192);
		}
	
		ssfdc_read_partitions((partition_t *)&SMCParts[mtd->index].type);
	}
	return;

}
static int ssfdc_read_partitions(partition_t * pt_smcpart) {

	int whole, i, j, size;

//=printk("ssfdc_read_partitions : start\n");

	for(i=0; i<MAX_PARTITIONS; i++)
	   	if ((atomic_read(&pt_smcpart->minor[i].open) > 1)) {
//=printk("ssfdc_read_partitions : part %d busy\n", i);

    		return -EBUSY;
   		}


//=printk("ssfdc_read_partitions : tables start\n");
	ssfdc_tables(pt_smcpart);
//=printk("ssfdc_read_partitions : tables end\n");

   	whole = pt_smcpart->count << PARTITION_BITS;         		


   	j = MAX_PARTITIONS - 1;
   	while (j-- > 0) {
		if (ssfdc_hd[whole+j].nr_sects > 0) {
    			kdev_t rdev = MKDEV(SSFDC_MAJOR, whole+j);
    			invalidate_device(rdev, 1);
		}
		ssfdc_hd[whole+j].start_sect = 0;
		ssfdc_hd[whole+j].nr_sects = 0;
   	}


	size = (((pt_smcpart->mtd->size / 16384) * 1000) / 1024) * 32;
	size /= (0x8 * 0x20);
	size = size * (0x8 * 0x20);

//=printk("ssfdc_read_partitions : register start\n");

    register_disk(&ssfdc_gendisk, whole >> PARTITION_BITS, MAX_PARTITIONS,
		  &ssfdc_fops, size);

//=printk("ssfdc_read_partitions : register end\n");


    	return 0;
}


static void ssfdc_notify_remove(struct mtd_info *mtd) {
int i, j, whole;

	i=mtd->index;
	whole = i << PARTITION_BITS;
   	if(SMCParts[i].mtd == mtd) {
       		if(SMCParts[i].zone)kfree(SMCParts[i].zone);
		memset((void *)&SMCParts[i].type, 0, sizeof(partition_t));
    		for (j = 0; j < MAX_PARTITIONS; j++) {
			if (ssfdc_hd[whole+j].nr_sects > 0) {
		   		ssfdc_hd[whole+j].start_sect = 0;
	   			ssfdc_hd[whole+j].nr_sects=0;
			}
    		}
		return;
	}
	return;
}



static int ssfdc_ioctl(struct inode *inode, struct file *file,
		u_int cmd, u_long arg) {

    int minor = MINOR(inode->i_rdev);
    int ret = -EINVAL;
    partition_t * pt_smcpart = (partition_t *)&SMCParts[(minor & ~(MAX_PARTITIONS -1)) >> PARTITION_BITS].type;
    struct hd_geometry geo;
    int size;
/*
	unsigned char smc_status;

  	smc_status = in_8((void *)&pt_ssfdc_smc->smc_status);
  	if(!(smc_status & SMC_PRESENT)) {
		printk("ssfdc : media not present\n");
		ret = 1;
		goto ssfdc_ioctl_error;
	}

  	if(smc_status & SMC_CHANGED) {
   		out_8((void *)&pt_ssfdc_smc->smc_status, smc_status);
		if(minor & ((1<< PARTITION_BITS) - 1)) return -ENOTTY;
			ssfdc_read_partitions(pt_smcpart);
		printk("ssfdc : media change\n");
  	}
*/
	switch(cmd) {

	    case HDIO_GETGEO:
		   	memset(&geo, 0, sizeof(geo));
			size = (((pt_smcpart->mtd->size / 16384) * 1000) / 1024) * 32;
			size /= (0x8 * 0x20);
			geo.heads = 0x8;
			geo.sectors = 0x20;
			geo.cylinders = size;
			geo.start = ssfdc_hd[minor].start_sect;
//			printk(KERN_WARNING "ssfdc : HDIO_GETGEO heads %d, sectors %d, cylinders %d, start %lu\n",
//				geo.heads, geo.sectors, geo.cylinders, geo.start);
			copy_to_user((void *)arg, &geo, sizeof(geo));
			ret = 0;
		break;

	    case BLKGETSIZE64:
   	 	case BLKGETSIZE:
			size = (((pt_smcpart->mtd->size / 16384) * 1000) / 1024) * 32;
			//=printk(KERN_WARNING "ssfdc : BLKGETSIZE %d, minor %d\n", size, minor);
			ret = copy_to_user((unsigned long *)arg, &size, sizeof(size));
		break;
		case BLKSSZGET:
			size = 512;
			ret = copy_to_user((unsigned long *)arg, &size, sizeof(size));
		break;
		break;

    	case BLKRRPART:
				if(minor & ((1<< PARTITION_BITS) - 1)) return -ENOTTY;
				ssfdc_read_partitions(pt_smcpart);
				ret=0;
		break;
   		case BLKFLSBUF:
			printk(KERN_WARNING "ssfdc : block ioctl 0x%x\n", cmd);
		break;

   		default:
			printk(KERN_WARNING "ssfdc: unknown ioctl 0x%x\n", cmd);
    }

//ssfdc_ioctl_error:
    return(ret);

}
static int ssfdc_open(struct inode *inode, struct file *file)
{
    int minor = MINOR(inode->i_rdev);
    partition_t *pt_smcpart;
	int index;

    if (minor >= MAX_MTD_DEVICES)
	return -ENODEV;

    index = (minor & ~(MAX_PARTITIONS -1)) >> PARTITION_BITS;


    if(SMCParts[index].type != SSFDC_FORMAT)
	return -ENXIO;

	pt_smcpart = &SMCParts[index];


	if(!pt_smcpart->zone)
	return -ENXIO;
 

    BLK_INC_USE_COUNT;

    if (!get_mtd_device(pt_smcpart->mtd, -1)) {
	    BLK_DEC_USE_COUNT;
	    return -ENXIO;
    }

    if ((file->f_mode & 2) && !(pt_smcpart->mtd->flags & MTD_CLEAR_BITS) ) {
	    put_mtd_device(pt_smcpart->mtd);
	    BLK_DEC_USE_COUNT;
            return -EROFS;
    }


    atomic_inc(&pt_smcpart->minor[minor & ~(MAX_PARTITIONS -1)].open);

	PDEBUG("ssfdc_open : device %d\n", minor);

	return(0);
}

static void ssfdc_tables(partition_t * pt_smcpart) {

	int * logical, * physical;
	int offset = 0;
	int zone, block;
	int i, retlen;
	int block_address, parity;
	int h, l;

	for(zone=0; zone<pt_smcpart->zoneCount; zone++) {	
		logical  =  pt_smcpart->zone + (2048 * zone);
		memset((void *)logical, 0xFF, 1024 * sizeof(int));
		physical =  pt_smcpart->zone + (2048 * zone) + 1024;
		memset((void *)physical, 0xFF, 1024 * sizeof(int));

		for(block=0; block < 1024; block++) {
			offset = (zone * ZONE_SIZE) + (block * SMC_BLOCK_SIZE);
	    		pt_smcpart->mtd->read_oob(pt_smcpart->mtd, offset, sizeof(ssfdc_buffer), &retlen, ssfdc_buffer);
			if(retlen != sizeof(ssfdc_buffer)) {
				printk(KERN_WARNING "ssfdc_tables : failed to read OOB\n");
				pt_smcpart->type = 0;
				return;
			}

			l = (ssfdc_buffer[7] & 0xFF);
			h = (ssfdc_buffer[6] & 0xFF);
            		block_address = l + (h << 8L);

			if((block_address & ~0x7FF) != 0x1000) {
					continue;
			}

			parity = block_address & 0x01;
			
			block_address &= 0x7FF;
			block_address >>= 1;


			if(ssfdc_parity(block_address) != parity) {
 				printk(KERN_WARNING "ssfdc_tables : parity error offset 0x%x, block 0x%x, parity 0x%x\nOOB : "
						, offset, block_address, parity);
			    	for(i=0; i<16; i++) {
                 			printk("0x%02x ", (unsigned char)ssfdc_buffer[i]);
			    	}
				printk("\n");
				pt_smcpart->type = 0;
				return;          		
			}


            		/* Ok we have a valid block number so insert it */
            		*(logical + block_address) = (offset/SMC_BLOCK_SIZE);
			PDEBUG("ssfdc_tables : logical 0x%x + 0x%x = 0x%x\n", 
					(unsigned int)logical, block_address, (offset/SMC_BLOCK_SIZE));
			*(physical + block) = block_address;
			PDEBUG("ssfdc_tables : physical 0x%x + 0x%x = 0x%x\n", (unsigned int)physical, block, block_address);
			

    		}
    	}
    	return;
}
int ssfdc_parity(int number) {
 	int i;
	int parity = 1; // the 0x1000 bit

	for(i=0; i<10; i++) {
      		parity += ((number >> i) & 1);
	}
	PDEBUG("ssfdc_parity : number 0x%x, parity 0x%x\n", number, parity);
	return(parity % 2);
}
static int ssfdc_physical(partition_t * pt_smcpart, int zone, int block) {

	unsigned int * logical;

	logical = pt_smcpart->zone + (zone * 2048);

	logical += block;

	if(*logical == 0xFFFFFFFF) {
		PDEBUG("ssfdc_physical : physical for zone %d, block %d invalid\n", zone, block);
		return(-1);
	}

	PDEBUG("ssfdc_physical : physical for zone %d, block %d, 0x%x\n", zone, block, (*logical * SMC_BLOCK_SIZE));
	return(*logical * SMC_BLOCK_SIZE);
}

static int ssfdc_close(struct inode *inode, struct file *file)
{                       
    int minor = MINOR(inode->i_rdev);
    partition_t *pt_smcpart;
	int index = (minor & ~(MAX_PARTITIONS -1)) >> PARTITION_BITS;

    if (minor >= MAX_MTD_DEVICES)
	return -ENODEV;

    if(SMCParts[index].type != SSFDC_FORMAT)
	return -ENXIO;

    pt_smcpart = &SMCParts[index];
    atomic_dec(&pt_smcpart->minor[minor & ~(MAX_PARTITIONS -1)].open);
    put_mtd_device(pt_smcpart->mtd);
    BLK_DEC_USE_COUNT;

    return(0);
} 


static void do_ssfdc_request(request_arg_t)
{
    int ret, minor;
    partition_t *pt_smcpart;
	int index;
    do {

	INIT_REQUEST;



	minor = MINOR(CURRENT->rq_dev);
	index = (minor & ~(MAX_PARTITIONS -1)) >> PARTITION_BITS;

	pt_smcpart = &SMCParts[index];
	if (pt_smcpart->type == SSFDC_FORMAT) {
 		ret = 0;
		switch (CURRENT->cmd) {
			case READ:
	  			ret = ssfdc_read(pt_smcpart, CURRENT->buffer,
					CURRENT->sector + ssfdc_hd[minor].start_sect,
					CURRENT->current_nr_sectors);
		    break;

		  	case WRITE:
		    	ret = ssfdc_write(pt_smcpart, CURRENT->buffer,
				    CURRENT->sector	+ ssfdc_hd[minor].start_sect,
				    CURRENT->current_nr_sectors);
		    break;

		  default:
		    panic("do_ssfdc_request : unknown block command!\n");
		  }
	
	} else {
	  ret = 1;
	  PDEBUG("not ssfdc partition type\n");
	}

	if (!ret) {
	  CURRENT->sector += CURRENT->current_nr_sectors;
	}

	end_request((ret == 0) ? 1 : 0);
    } while (1);
}

static int ssfdc_write(partition_t *pt_smcpart, caddr_t buffer,
		     u_long sector, u_long nblocks)
{
	int zone, block, offset;
	int sectors_written = 0;
    	int physical;
	int * pt_logical;
	int * pt_physical;
	int new = -1;
	int size;
	int retlen;
	int i;
	int sc;
	int ptr_done = 0;
	unsigned char * ptr = (unsigned char *)buffer;
	unsigned char ecc_code[6], ecc_calc[6];
	int do_erase;
//	unsigned char smc_status;



	offset = (sector % SECTORS_PER_ZONE) % SECTORS_PER_BLOCK ;

    	PDEBUG("write device %d, sector %d, count %d\n",
			pt_smcpart->count, sector, nblocks);
/*
  	smc_status = in_8((void *)&pt_ssfdc_smc->smc_status);
	if(!(smc_status & SMC_PRESENT)) {
		printk("ssfdc : media not present\n");
		return -ENXIO;
	}

    if(smc_status & SMC_CHANGED) {
   		out_8((void *)&pt_ssfdc_smc->smc_status, smc_status);
		ssfdc_read_partitions(pt_smcpart);
		printk("ssfdc : media change\n");
	}
*/
	while(sectors_written < nblocks) {

		new = -1;
		do_erase = FALSE;
    
		zone = (sector + sectors_written) / SECTORS_PER_ZONE;
		block = ((sector + sectors_written) % SECTORS_PER_ZONE) / SECTORS_PER_BLOCK ;
		offset = ((sector + sectors_written) % SECTORS_PER_ZONE) % SECTORS_PER_BLOCK ;

		pt_logical = pt_smcpart->zone + (zone * 2048);
		pt_physical = pt_smcpart->zone + (zone * 2048) + 1024;

		size = ((SECTORS_PER_BLOCK - offset) < (nblocks - sectors_written)) ?
				(SECTORS_PER_BLOCK - offset) : (nblocks - sectors_written);
		size *= SECTOR_SIZE;

		PDEBUG("write device %d, sector %d, count %d, zone %d, block %d, offset %d, done %d, size %d, address 0x%x\n",
				pt_smcpart->count, sector, nblocks, zone, block, offset, sectors_written, size, (unsigned int)ptr);

		physical = ssfdc_physical(pt_smcpart, zone, block);


		if(physical >= 0) {
			if(ssfdc_cached != physical) {
	   			pt_smcpart->mtd->read_ecc(pt_smcpart->mtd, physical, SMC_BLOCK_SIZE, &retlen, ssfdc_scratch,
					 ssfdc_oob_buf, &ssfdc_ffoob_info);
				if(retlen != SMC_BLOCK_SIZE) {
					printk(KERN_WARNING "ssfdc_write : failed to read physical\n");
					return -ENXIO;
				}

				for(sc=0; sc<SECTORS_PER_BLOCK; sc++) {
	   				pt_smcpart->mtd->read_oob(pt_smcpart->mtd, physical + (sc * SECTOR_SIZE), sizeof(ssfdc_buffer), &retlen, ssfdc_buffer);
					if(retlen != sizeof(ssfdc_buffer)) {
						printk(KERN_WARNING "ssfdc_write : failed to read physical oob\n");
						return -ENXIO;
					}

					nand_calculate_ecc (pt_smcpart->mtd, &ssfdc_scratch[sc * SECTOR_SIZE], &ecc_calc[0]);
					nand_calculate_ecc (pt_smcpart->mtd, &ssfdc_scratch[(sc * SECTOR_SIZE) + 256], &ecc_calc[3]);
					for(i=0; i<6; i++) ecc_code[i] = ssfdc_buffer[ssfdc_ecc[i]];
					nand_correct_data(pt_smcpart->mtd, &ssfdc_scratch[sc * SECTOR_SIZE], &ecc_code[0], &ecc_calc[0]);
					nand_correct_data(pt_smcpart->mtd, &ssfdc_scratch[(sc * SECTOR_SIZE) + 256], &ecc_code[3], &ecc_calc[3]);
				}

			}
			
			for(sc=0; sc<SECTORS_PER_BLOCK; sc++) {
				if(offset > sc) {
					PDEBUG("offset %d, sector %d\n", offset, sc);
					continue;
				}
   				pt_smcpart->mtd->read_oob(pt_smcpart->mtd, physical + (sc * SECTOR_SIZE), sizeof(ssfdc_buffer), &retlen, ssfdc_buffer);
				if(retlen != sizeof(ssfdc_buffer)) {
					printk(KERN_WARNING "ssfdc_write : failed to read physical oob\n");
					return -ENXIO;
				}

				nand_calculate_ecc (pt_smcpart->mtd, &ssfdc_scratch[sc * SECTOR_SIZE], &ecc_calc[0]);
				nand_calculate_ecc (pt_smcpart->mtd, &ssfdc_scratch[(sc * SECTOR_SIZE) + 256], &ecc_calc[3]);
				for(i=0; i<6; i++) ecc_code[i] = ssfdc_buffer[ssfdc_ecc[i]];
				nand_correct_data(pt_smcpart->mtd, &ssfdc_scratch[sc * SECTOR_SIZE], &ecc_code[0], &ecc_calc[0]);
				nand_correct_data(pt_smcpart->mtd, &ssfdc_scratch[(sc * SECTOR_SIZE) + 256], &ecc_code[3], &ecc_calc[3]);
				
				/* find out if the block is being used */


				if(ssfdc_sector_blank(pt_smcpart, sc)) {
					PDEBUG("ssfdc_write : zone %d, block %d, sector %d, lbn %d, blank, physical 0x%x\n",
						zone, block, sc, sector, physical);
					memcpy(&ssfdc_scratch[(sc * SECTOR_SIZE)], ptr+ptr_done, SECTOR_SIZE);
					nand_calculate_ecc (pt_smcpart->mtd, (ptr + ptr_done), &ecc_calc[0]);
					nand_calculate_ecc (pt_smcpart->mtd, (ptr + ptr_done + 256), &ecc_calc[3]);
					for(i=0; i<6; i++) ssfdc_buffer[ssfdc_ecc[i]] = ecc_calc[i];
					i = (block << 1) | 0x1000;
					i |= ssfdc_parity(block);
       				        ssfdc_buffer[7] = ssfdc_buffer[12] = i & 0xFF;
                    			ssfdc_buffer[6] = ssfdc_buffer[11] = (i & 0xFF00) >> 0x08;

                    pt_smcpart->mtd->write_ecc(pt_smcpart->mtd, physical + (sc * SECTOR_SIZE), SECTOR_SIZE, &retlen,
								ptr + ptr_done, ssfdc_buffer, &ssfdc_ffoob_info);
					if(retlen != SECTOR_SIZE) {
						printk(KERN_WARNING "ssfdc_write : failed to write physical 0x%x, sector 0x%x, blank, retlen %d\n"
								, physical, sc, retlen);
						return -ENXIO;
					}

                    ptr_done += SECTOR_SIZE;
					if(ptr_done >= size) break;
				}
				else {
					new = ssfdc_allocate_new(pt_smcpart, zone);
					/* erase the old block */
		            *(pt_physical + ((physical % ZONE_SIZE) / SMC_BLOCK_SIZE)) = 0xFFFFFFFF;

					PDEBUG("ssfdc_write : physical 0x%x + 0x%x = 0x%x\n",
						(unsigned int)pt_physical, ((physical % ZONE_SIZE) / SMC_BLOCK_SIZE), 0xFFFFFFFF);
					do_erase = TRUE;
					PDEBUG("ssfdc_write : zone %d, block %d, sector %d, lbn %d, written, physical 0x%x, new 0x%x\n",
						zone, block, sc, sector, physical, new);
					break;
				}
			}
		}
		else {
			ssfdc_cached = 0xFFFFFFFF;
			memset(ssfdc_scratch, 0xFF, sizeof(ssfdc_scratch));
			new = ssfdc_allocate_new(pt_smcpart, zone);
			PDEBUG("ssfdc_write : zone %d, block %d, lbn %d, physical 0x%x, unallocated, new 0x%x\n",
				zone, block, sector, physical, new);
		}



		if(new != -1) {


			memcpy(&ssfdc_scratch[(offset * SECTOR_SIZE)], ptr, size);
			PDEBUG("ssfdc_write : new 0x%x, offset 0x%x, size 0x%x, block 0x%x\n", new, offset, size, block);
       		for(sc=0; sc<SECTORS_PER_BLOCK; sc++) {
				memset(ssfdc_buffer, 0xFF, OOB_SIZE);
				nand_calculate_ecc (pt_smcpart->mtd, &ssfdc_scratch[sc * SECTOR_SIZE], &ecc_calc[0]);
				nand_calculate_ecc (pt_smcpart->mtd, &ssfdc_scratch[(sc * SECTOR_SIZE) + 256], &ecc_calc[3]);
				for(i=0; i<6; i++) ssfdc_buffer[ssfdc_ecc[i]] = ecc_calc[i];
				i = (block << 1) | 0x1000;
				i |= ssfdc_parity(block);
                		ssfdc_buffer[7] = ssfdc_buffer[12] = i & 0xFF;
                		ssfdc_buffer[6] = ssfdc_buffer[11] = (i & 0xFF00) >> 0x08;
                memcpy(&ssfdc_oob_buf[sc * OOB_SIZE], ssfdc_buffer, OOB_SIZE);
			}


	   		pt_smcpart->mtd->write_ecc(pt_smcpart->mtd, new, SMC_BLOCK_SIZE, &retlen, ssfdc_scratch,
                        ssfdc_oob_buf, &ssfdc_ffoob_info);
			if(retlen != SMC_BLOCK_SIZE) {
				printk(KERN_WARNING "ssfdc_write : failed to write block, physical 0x%x, returned 0x%x\n", new, retlen);
				return -ENXIO;
			}
			/* change the mapping table to reflect the new block placement */

			*(pt_logical + block) = (new % ZONE_SIZE) / SMC_BLOCK_SIZE;
			PDEBUG("ssfdc_write : logical 0x%x + 0x%x = 0x%x\n",
						(unsigned int)pt_logical, block, (new % ZONE_SIZE) / SMC_BLOCK_SIZE);

            		*(pt_physical + ((new % ZONE_SIZE) / SMC_BLOCK_SIZE)) = block;
			PDEBUG("ssfdc_write : physical 0x%x + 0x%x = 0x%x\n",
				(unsigned int)pt_physical, ((new % ZONE_SIZE) / SMC_BLOCK_SIZE), block);


			ssfdc_cached = new;
   	    }


		ptr += size;
		ptr_done = 0;
		sectors_written += (size / SECTOR_SIZE);
		if(do_erase) ssfdc_erase(pt_smcpart, physical);

	}




	return(0);
}
static int ssfdc_sector_blank(partition_t * pt_smcpart, int sc) {
int b;

	for(b=0; b<SECTOR_SIZE; b++) {
		if(ssfdc_scratch[b + (sc * SECTOR_SIZE)] != 0xFF) return(0);
	}
	for(b=0; b<OOB_SIZE; b++) {
		if((b==6) || (b==7) || (b==11) || (b==12)) continue;   // Block address fields
		if(ssfdc_buffer[b] != 0xFF) return(0);
	}
    return(1);
}
static int ssfdc_allocate_new(partition_t * pt_smcpart, int zone) {

	int new = pt_smcpart->last_written[zone] + 1;
	int * pt_physical;
	int physical;
    	int block;
	int retlen;
	unsigned char oob[16];
	

	if(new >= BLOCKS_PER_ZONE) new = 0;


	while (new != pt_smcpart->last_written[zone]) {
       	block = new % BLOCKS_PER_ZONE;
		pt_physical = pt_smcpart->zone + (zone * 2048) + 1024 + block;
		physical = (zone * ZONE_SIZE) + (block * SMC_BLOCK_SIZE);

	  	PDEBUG("ssfdc_allocate_new : zone %d, block %d, address 0x%08x, data 0x%08x\n",
			zone, block, (unsigned int)pt_physical, *pt_physical);
     		if(*pt_physical == 0xFFFFFFFF) {
			PDEBUG("ssfdc_allocate_new : physical 0x%x = 0x%x\n", (unsigned int)pt_physical, *pt_physical);
			memset(oob, 0, OOB_SIZE);
			pt_smcpart->mtd->read_oob(pt_smcpart->mtd, physical, OOB_SIZE, &retlen, oob);
			if((oob[5] == 0xFF) && (retlen == OOB_SIZE)) {   // If not a bad block
				pt_smcpart->last_written[zone] = new;
				return((new * SMC_BLOCK_SIZE) + (zone * ZONE_SIZE));
			}
			else {
             			PDEBUG("ssfdc_allocate_new : new 0x%x, physical 0x%x, block status 0x%x, oob length 0x%x\n", new, physical, oob[5], retlen);
			}
		}
		new++;
		if(new >= BLOCKS_PER_ZONE) new = 0;
	}

   	panic("ssfdc_allocate_new : cant find free block\n");

}
	


static int ssfdc_read(partition_t *pt_smcpart, caddr_t buffer,
		    u_long sector, u_long nblocks)
{
	int zone, block, offset;
	int sectors_read = 0;
    int physical;
	int size;
	int retlen;
	int i;
	int sc;
	unsigned char * ptr = (unsigned char *)buffer;
	unsigned char ecc_code[6], ecc_calc[6];
/*
    unsigned char smc_status;

  	smc_status = in_8((void *)&pt_ssfdc_smc->smc_status);
	if(!(smc_status & SMC_PRESENT)) {
		printk("ssfdc : media not present\n");
		return -ENXIO;
	}



    if(smc_status & SMC_CHANGED) {
   		out_8((void *)&pt_ssfdc_smc->smc_status, smc_status);
		ssfdc_read_partitions(pt_smcpart);
		printk("ssfdc : media change\n");
	}
*/
	while(sectors_read < nblocks) {

		zone = (sector + sectors_read) / SECTORS_PER_ZONE;
		block = ((sector + sectors_read) % SECTORS_PER_ZONE) / SECTORS_PER_BLOCK ;
		offset = ((sector + sectors_read) % SECTORS_PER_ZONE) % SECTORS_PER_BLOCK ;


		if(offset) {
			size = ((SECTORS_PER_BLOCK - offset) < (nblocks - sectors_read)) ?
					(SECTORS_PER_BLOCK - offset) : (nblocks - sectors_read);
		}
		else {
			size = (SECTORS_PER_BLOCK < (nblocks - sectors_read)) ? SECTORS_PER_BLOCK : nblocks - sectors_read;
		}
		size *= SECTOR_SIZE;

	    PDEBUG("ssfdc_read :  device %d, sector %d, count %d, zone %d, block %d, offset %d, done %d, size %d, address 0x%x\n",
			pt_smcpart->count, sector, nblocks, zone, block, offset, sectors_read, size, (unsigned int)ptr);

			
		physical = ssfdc_physical(pt_smcpart, zone, block);
		if(physical >=  0) {
			if(ssfdc_cached != physical) {
           		pt_smcpart->mtd->read_ecc(pt_smcpart->mtd, physical, SMC_BLOCK_SIZE, &retlen, ssfdc_scratch,
													 ssfdc_oob_buf, &ssfdc_ffoob_info);
				if(retlen != SMC_BLOCK_SIZE) {
					printk(KERN_WARNING "ssfdc_read : failed to read physical\n");
					return -ENXIO;
				}
				for(sc=0; sc<SECTORS_PER_BLOCK; sc++) {
	    			pt_smcpart->mtd->read_oob(pt_smcpart->mtd, physical + (sc * SECTOR_SIZE), sizeof(ssfdc_buffer), &retlen, ssfdc_buffer);
					if(retlen != sizeof(ssfdc_buffer)) {
						printk(KERN_WARNING "ssfdc_read : failed to read physical oob\n");
						return -ENXIO;
					}
					nand_calculate_ecc (pt_smcpart->mtd, &ssfdc_scratch[sc * SECTOR_SIZE], &ecc_calc[0]);
					nand_calculate_ecc (pt_smcpart->mtd, &ssfdc_scratch[(sc * SECTOR_SIZE) + 256], &ecc_calc[3]);
					for(i=0; i<3; i++) ecc_code[i] = ssfdc_buffer[ssfdc_ecc[i]];
					for(i=3; i<6; i++) ecc_code[i] = ssfdc_buffer[ssfdc_ecc[i]];
					nand_correct_data(pt_smcpart->mtd, &ssfdc_scratch[sc * SECTOR_SIZE], &ecc_code[0], &ecc_calc[0]);
					nand_correct_data(pt_smcpart->mtd, &ssfdc_scratch[(sc * SECTOR_SIZE) + 256], &ecc_code[3], &ecc_calc[3]);
				}

				/* Get the ecc bytes and check that they are ok */


			}
			ssfdc_cached = physical;
			
			
		}
		else {
			memset(ssfdc_scratch, 0xFF, sizeof(ssfdc_scratch));
			ssfdc_cached = 0xFFFFFFFF;
		}
			

		memcpy(ptr, &ssfdc_scratch[(offset * SECTOR_SIZE)], size);
		ptr += size;
		sectors_read += (size / SECTOR_SIZE);	
	}


	                                                    
	return(0);
}

static void ssfdc_erase_callback(struct erase_info *erase) {

	PDEBUG("ssfdc_erase_callback : wake erase\n");
    	up(&ssfdc_semaphore);
	PDEBUG("ssfdc_erase_callback : woken erase\n");
}

static int ssfdc_erase(partition_t *pt_smcpart, unsigned int offset)
{
	int ret = 0;
    	struct erase_info *erase;
	unsigned char * junk;
	unsigned char * oob;
	int retlen;
	int b, sc;


	PDEBUG("ssfdc_erase : offset 0x%08x\n", offset);

	erase=kmalloc(sizeof(struct erase_info), GFP_KERNEL);
    	junk=kmalloc(pt_smcpart->mtd->erasesize + 16, GFP_KERNEL);
    	oob = junk + pt_smcpart->mtd->erasesize;

    	if (!erase)
        	 return -ENOMEM;
    	if (!junk)
        	 return -ENOMEM;

    	erase->addr = offset;
    	erase->len = pt_smcpart->mtd->erasesize;
	erase->callback = ssfdc_erase_callback;
    	ret = pt_smcpart->mtd->erase(pt_smcpart->mtd, erase);
	if(ret) {
     		printk(KERN_WARNING "ssfdc_erase : failed status 0x%x\n", ret);
		goto end;

	}

	down(&ssfdc_semaphore);

	pt_smcpart->mtd->read_ecc(pt_smcpart->mtd, offset, SMC_BLOCK_SIZE, &retlen, junk,
							 ssfdc_oob_buf, &ssfdc_ffoob_info);
	if(retlen != SMC_BLOCK_SIZE) {
           	printk(KERN_WARNING "ssfdc_erase : offset 0x%x, read returned length %d\n", offset, retlen);
		goto end;
	}


	for(sc=0; sc < SECTORS_PER_BLOCK; sc++) {
		for(b=0; b<SECTOR_SIZE; b++) {
			if(*(junk + (b + (sc * SECTOR_SIZE))) != 0xFF) {
             			printk(KERN_WARNING "ssfdc_erase : offset 0x%x, sector 0x%x, byte 0x%x, data 0x%02x, expected 0xff\n"
						, offset, sc, b, *(junk + (b + (sc * SECTOR_SIZE))));
				goto end;
			}
		}
		pt_smcpart->mtd->read_oob(pt_smcpart->mtd, offset + (sc * SECTOR_SIZE), OOB_SIZE, &retlen, oob);
		if(retlen != OOB_SIZE) {
           		printk(KERN_WARNING "ssfdc_erase : offset 0x%x, read oob returned length %d\n", offset, retlen);
			goto end;
		}
		for(b=0; b<OOB_SIZE; b++) {
			if(*(oob+b) != 0xFF) {
             			printk(KERN_WARNING "ssfdc_erase : offset 0x%x, byte 0x%x, oob got 0x%02x, expected 0xff\n", 
						offset, b, *(oob+b));
				goto end;
			}
		}
	}

end:
	
    kfree(erase);
	kfree(junk);

    return ret;
} /* erase_xfer */





int init_ssfdc(void)
{
	int result, i;

//	unsigned char smc_status;
//	#define B01159_FIO_PBASE 0x0000000148000000  /* Physical Base address of SMC control chip  */

	printk(KERN_INFO "SSFDC block device translation layer V1.0\n");
/*
	pt_ssfdc_smc = ioremap64(B01159_FIO_PBASE, 1024);
	if(!pt_ssfdc_smc){
     	printk("ssfdc : failed to map SMC control device\n");
        return(-EFAULT);
	}
	
  	smc_status = in_8((void *)&pt_ssfdc_smc->smc_status);
*/	
    memset(ssfdc_ffoob_buf, 0xFF, sizeof(ssfdc_ffoob_buf));
    
	for (i = 0; i < MAX_DEVICES*MAX_PARTITIONS; i++) {
		ssfdc_hd[i].nr_sects = 0;
		ssfdc_hd[i].start_sect = 0;
		ssfdc_blocksizes[i] = 4096;
    	}
   	blksize_size[SSFDC_MAJOR] = ssfdc_blocksizes;
   	ssfdc_gendisk.major = SSFDC_MAJOR;


	memset(ssfdc_scratch, 0xFF, sizeof(ssfdc_scratch));

	result = register_blkdev(ssfdc_major, "ssfdc", &ssfdc_fops);
	if(result != 0) {
		printk(KERN_WARNING "ssfdc : failed to get a major number\n");
		return(result);
	}
//	if(ssfdc_major == 0) ssfdc_major = result;
	
    	blk_init_queue(BLK_DEFAULT_QUEUE(ssfdc_major), &do_ssfdc_request);

    	add_gendisk(&ssfdc_gendisk);



    	register_mtd_user(&ssfdc_notifier);


	init_MUTEX_LOCKED(&ssfdc_semaphore);



    	return 0;
}

static void __exit cleanup_ssfdc(void)
{
	int i;

	for(i=0; i<MAX_DEVICES; i++) {
       		if(SMCParts[i].zone)kfree(SMCParts[i].zone);
	}


    	unregister_mtd_user(&ssfdc_notifier);
    	unregister_blkdev(ssfdc_major, "ssfdc");
    	blk_cleanup_queue(BLK_DEFAULT_QUEUE(ssfdc_major));



    	blksize_size[SSFDC_MAJOR] = NULL;
    	del_gendisk(&ssfdc_gendisk);

}

module_init(init_ssfdc);
module_exit(cleanup_ssfdc);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Simon Haynes <simon@baydel.com>");
MODULE_DESCRIPTION("SSFDC translation layer support for MTD");




