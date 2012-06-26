/*
 *  linux/drivers/mtd/nand/w90n745_nand.c
 *
 *  Copyright (c) 2006  Winbond (http://www.winbond.com.tw)
 *
 * History:
 *	2006/08/24    Created by NS24 zswan  v1.0 for nand 512b per page
 *	2006/12/12    Created by NS24 zswan  v1.1 for nand 2kb per page
 *	mail:zswan@winbond.com.tw
 */

#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include "w90n745_nand.h"

#define write_cmd_reg(cmd)		outb(cmd, REG_SMCMD)
#define write_addr_reg(addr)		outb(addr, REG_SMADDR)
#define read_data_reg()			inb(REG_SMDATA)
#define write_data_reg(value)		outb(value, REG_SMDATA)



typedef unsigned int      UINT32;
typedef unsigned char UINT8;
UINT32 blockaddr;
UINT8 _fmi_ucBaseAddr1, _fmi_ucBaseAddr2;
/*
 * MTD structure for NAND controller
 */
static struct mtd_info *w90n745_mtd = NULL;
//static u_char data_buf[2048 + 64];
//static u_char oob_buf[64*64];
/*
 * Define partitions for flash device
 */
 #define NUM_PARTITIONS	 2
const static struct mtd_partition partition_info[NUM_PARTITIONS] = {

	{ 
		.name = "w90n745 512B NAND PAR1",
	  	.offset = 0,
	  	.size = 16*1024*1024 
	},
	{ 
		.name = "w90n745 512B NAND PAR2",
	  	.offset = 16*1024*1024,
	  	.size = 16*1024*1024 
	},
};


static int w90n745_device_ready(struct mtd_info *mtd)
{
	#ifdef _DEBUG_NAND
	printk("w90n745_device_ready\n");
	#endif
	int ready;
	ready=(ebiCheckRB())?1:0;	// rb# status
	return ready;
}

static u_char w90n745_nand_read_byte(struct mtd_info *mtd)
{
#ifdef _DEBUG_NAND
printk("w90n745_nand_read_byte\n");
#endif
	u_char ret;
	ret = (u_char)read_data_reg();
	return ret;
}

static void w90n745_nand_write_byte(struct mtd_info *mtd, u_char byte)
{
	//printk("write_byte : %02x\n", byte);
#ifdef _DEBUG_NAND
printk("w90n745_nand_write_byte\n");
#endif
	write_data_reg(byte);
}
int ebiCheckRB()
{
	return (inl(0xfff8305c) & 0x10);
}
static void  nand_reset(void)
{
#ifdef _DEBUG_NAND
printk("nand_reset\n");
#endif
	UINT32 volatile i;
	outb(0xff,REG_SMCMD);
	for (i=100; i>0; i--);
	while(!ebiCheckRB());
}
static void 
w90n745_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{

#ifdef _DEBUG_NAND
printk("w90n745_nand_write_buf\n");
#endif

	int i;
	for (i=0; i<len; i++){
		write_data_reg(buf[i]);
	}
}


static void 
w90n745_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{

#ifdef _DEBUG_NAND
printk("w90n745_nand_read_buf\n");
#endif

	int i;
	for(i = 0; i < len; i++){

		buf[i] =(u_char)read_data_reg();
		}
}

static int 
w90n745_nand_verify_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
#ifdef _DEBUG_NAND
printk("w90n745_nand_verify_buf\n");
#endif

	int i;
	for (i=0; i<len; i++)
		if (buf[i] != (u_char)read_data_reg())
		return -EFAULT;
	return 0;
}

static void w90n745_nand_select_chip(struct mtd_info *mtd, int chip)
{
#ifdef _DEBUG_NAND
printk("w90n745_nand_select_chip\n");
#endif

}
static void w90n745_nand_command (struct mtd_info *mtd, unsigned command, 
		int column, int page_addr)
{
#ifdef _DEBUG_NAND
printk("w90n745_nand_command\n");
#endif
	register struct nand_chip *this = mtd->priv;


	if (command == NAND_CMD_SEQIN) {
		int readcmd;

		if (column >= mtd->oobblock) {
			/* OOB area */
			column -= mtd->oobblock;
			readcmd = NAND_CMD_READOOB;
		} else if (column < 256) {
			/* First 256 bytes --> READ0 */
			readcmd = NAND_CMD_READ0;
		} else {
			column -= 256;
			readcmd = NAND_CMD_READ1;
		}
		write_cmd_reg(readcmd);
	}

	write_cmd_reg(command);

	if (column != -1 || page_addr != -1) {

		/* Serially input address */
		if (column != -1)
			write_addr_reg(column);
		if (page_addr != -1){
			write_addr_reg((unsigned char)(page_addr )& 0xff);	
			write_addr_reg((unsigned char) ((page_addr>>8 )& 0xff));
		}	
			/* One more address cycle for higher density devices */
		if (mtd->size & 0x0c000000) {
			//write_addr_reg((unsigned char) ((page_addr >> 16) & 0x0f));
		}
	}

	switch (command) {
			
	case NAND_CMD_PAGEPROG:
	case NAND_CMD_ERASE1:
	case NAND_CMD_ERASE2:
	case NAND_CMD_SEQIN:
	case NAND_CMD_STATUS:
		break;
	case NAND_CMD_RESET:
		write_cmd_reg(command);
		//udelay(this->chip_delay);
		//while(!(inl(REG_SMISR) & 0x10));	// rb# status
		nand_reset();
		break;
	default:
		udelay(this->chip_delay);
		break;
}
	while(!this->dev_ready(mtd));
}

/*
 * Main initialization routine
 */
static int __init w90n745_init (void)
{
#ifdef _DEBUG_NAND
printk("w90n745_init\n");
#endif
	struct nand_chip *this;
	// set EBI
#if 1
	#ifdef EBI_BANK2
	Setup_EXTIO2_Base();
	#else
	Setup_EXTIO3_Base();
	#endif

	// set GPIO
	outpw(0xfff83050, inpw(0xfff83050) & 0xfffff0ff);	// GPIO5-4 and GPIO5-5
	outpw(0xfff83054, (inpw(0xfff83054) & 0xffffffcf) | 0x20);
	outpw(0xfff83058, inpw(0xfff83058) | 0x20);
#endif
	/* Allocate memory for MTD device structure and private data */
	w90n745_mtd = kmalloc (sizeof(struct mtd_info) + 
			sizeof (struct nand_chip), GFP_KERNEL);
	//w90n745_mtd=kmalloc (1024, GFP_KERNEL);
	//kfree (w90n745_mtd);
	if (!w90n745_mtd) {
		printk ("Unable to allocate NAND MTD dev structure.\n");
		return -ENOMEM;
	}

	/* Get pointer to private data */
	this = (struct nand_chip *) (&w90n745_mtd[1]);

	/* Initialize structures */
	memset((char *) w90n745_mtd, 0, sizeof(struct mtd_info));
	memset((char *) this, 0, sizeof(struct nand_chip));

	/* Link the private data with the MTD structure */
	w90n745_mtd->priv = this;


	this->dev_ready = w90n745_device_ready;
	/* 30 us command delay time */
	this->chip_delay = 100;		

	this->cmdfunc = w90n745_nand_command;
	this->select_chip = w90n745_nand_select_chip;
	//this->select_chip = NULL;
	this->write_byte = w90n745_nand_write_byte;
	this->read_byte = w90n745_nand_read_byte;
	this->write_buf = w90n745_nand_write_buf;
	this->read_buf = w90n745_nand_read_buf;
	this->verify_buf = w90n745_nand_verify_buf;

	this->eccmode = NAND_ECC_NONE;
	this->data_buf = NULL;
	this->oob_buf = NULL;
	//w90n745_nand_Reset();
	/* Scan to find existence of the device */
	if (nand_scan (w90n745_mtd, 1)) {
		kfree (w90n745_mtd);
		return -ENXIO;
	}

	/* Register the partitions */
	add_mtd_partitions(w90n745_mtd, partition_info, NUM_PARTITIONS);

	return 0;
}

static void __exit w90n745_exit(void)
{
#ifdef _DEBUG_NAND
printk("w90n745_exit\n");
#endif
	struct nand_chip *this = (struct nand_chip *) &w90n745_mtd[1];

	/* Unregister partitions */
	del_mtd_partitions(w90n745_mtd);

	/* Unregister the device */
	del_mtd_device (w90n745_mtd); 

	/* Free the MTD device structure */
	kfree (w90n745_mtd);

}

module_init(w90n745_init);
module_exit(w90n745_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NS24 zswan, zswan@Winbond.com.tw");
MODULE_DESCRIPTION("Board-specific glue layer for NAND flash on w90n745 board");

