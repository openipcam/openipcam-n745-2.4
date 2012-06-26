
#ifndef FFS2_IO
#define FFS2_IO

struct ffs_read
{
   unsigned char *p;             // The buffer
   unsigned long ahead;          // Number of times p++ can be done
   unsigned long behind;         // Number of times p-- can be done
   unsigned long location;
   unsigned long block;
   unsigned long offset;
   struct super_block *super;
   unsigned char temp[128];
   struct buffer_head *bh[4];
   unsigned int age[4];
   unsigned int curage;
};

extern unsigned char *ffs2_read(struct ffs_read *r,unsigned long block,
				unsigned long offset,unsigned long count);
extern void ffs2_relse(struct ffs_read *r);
extern struct ffs2_entry *ffs2_find_entry(struct ffs_read *r,unsigned long loc);
extern int ffs2_find_boot_block(struct ffs_read *r,unsigned long blocks);
extern int ffs2_find_dirent(struct ffs_read *r,unsigned long loc,
			    struct qstr *name,unsigned long *pos);
extern int ffs2_find_blockalloc(struct ffs_read *r,unsigned long location,
				struct ffs2_blockalloc *blk);
extern int ffs2_copy_to_buff(struct ffs_read *r,unsigned char *buf,
			     struct ffs2_fileinfo *extent,unsigned long toread,
			     unsigned long offset);
extern int ffs2_prepare(struct ffs_read *r);

#endif
