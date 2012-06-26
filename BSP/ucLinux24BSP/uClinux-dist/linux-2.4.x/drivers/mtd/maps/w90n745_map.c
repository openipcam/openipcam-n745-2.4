#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/config.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <asm/io.h>


#if 1
#define DEBUGMTD		printk
#else
#define DEBUGMTD
#endif

#define WINDOW_ADDR		0xFF000000 //lsshi
#define WINDOW_SIZE		0x400000 //flash size (4M)

#define BUSWIDTH 2

 

static struct mtd_info *mymtd;

__u8 W90N745_read8(struct map_info *map, unsigned long ofs)
{

	return __raw_readb(map->map_priv_1 + ofs);

}
struct map_info W90N745_map;
__u16 W90N745_read16(struct map_info *map, unsigned long ofs)
{

	return __raw_readw(map->map_priv_1 + ofs);
}

map_word W90N745_read(struct map_info *map, unsigned long ofs)
{
	map_word r;

	r.x[0] = __raw_readw(map->map_priv_1 + ofs);	
	return r;	
}

__u32 W90N745_read32(struct map_info *map, unsigned long ofs)
{

	return __raw_readl(map->map_priv_1 + ofs);

}

void W90N745_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{

	memcpy_fromio(to, map->map_priv_1 + from, len);

}

void W90N745_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	__raw_writeb(d, map->map_priv_1 + adr);
	mb();
}

void W90N745_write16(struct map_info *map, __u16 d, unsigned long adr)
{

	__raw_writew(d, map->map_priv_1 + adr);

	mb();

}


void W90N745_write(struct map_info *map, const map_word datum, unsigned long ofs)
{
	__raw_writew(datum.x[0], map->map_priv_1 + ofs);

	mb();

}

void W90N745_write32(struct map_info *map, __u32 d, unsigned long adr)

{

	__raw_writel(d, map->map_priv_1 + adr);

	mb();

}

void W90N745_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{

	memcpy_toio(map->map_priv_1 + to, from, len);

}

struct map_info W90N745_map = {

	name: "POS-TAX flash device", //lsshi W90N745 flash device
	
	size: WINDOW_SIZE,
	
	bankwidth: BUSWIDTH,

	read: W90N745_read,
	
	read8: W90N745_read8,
	
	read16: W90N745_read16,
	
	read32: W90N745_read32,
	
	copy_from: W90N745_copy_from,
	
	write8: W90N745_write8,
	
	write16: W90N745_write16,
	
	write: W90N745_write, 
	
	write32: W90N745_write32,
	
	copy_to: W90N745_copy_to

};

/*

* MTD 'PARTITIONING' STUFF 

*/

static struct mtd_partition W90N745_partitions[] = {

	{
		name: "images 3M",
		size: 0x300000,
		offset: 0
	},
	{
		/*if jffs2 can run on this partition the size can not less than 6 sectors*/
		name: "user 1M",
		size: 0x100000,
		offset: 0x00300000	//offset
	}


};

int __init init_POS_FLASH(void)
{
	
	W90N745_map.map_priv_1 = (unsigned long)ioremap(WINDOW_ADDR, WINDOW_SIZE);
	
	if (!W90N745_map.map_priv_1) {	
		printk("Failed to ioremap\n");	
		return -EIO;	
	}
	
	mymtd = do_map_probe("cfi_probe", &W90N745_map);

	if (mymtd) {	
		mymtd->owner = THIS_MODULE;		
		return add_mtd_partitions(mymtd, W90N745_partitions, sizeof(W90N745_partitions) / sizeof(struct mtd_partition));
	
	}
	
	iounmap((void *)W90N745_map.map_priv_1);
	
	return -ENXIO;

}

static void __exit cleanup_POS_FLASH(void)
{
	
	if (mymtd) {
	
		del_mtd_partitions(mymtd);
		map_destroy(mymtd);

	}
	
	if (W90N745_map.map_priv_1) {
	
		iounmap((void *)W90N745_map.map_priv_1);
		W90N745_map.map_priv_1 = 0;
	}
	
}

module_init(init_POS_FLASH);
module_exit(cleanup_POS_FLASH);

 
