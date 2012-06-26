// -*- mode: cpp; mode: fold -*-
// Description                                                          /*{{{*/
// $Id: io.c,v 1.1.1.1 2006-07-11 09:31:07 andy Exp $
/* ######################################################################

   Microsoft Flash File System 2 IO Operations
   
   These are some generic IO operations that are not depenend on the 
   linux kernel. The boot loader uses this file to get FFS2 read 
   capability.
   
   ##################################################################### */
									/*}}} */
#include "local.h"
#include "io.h"

// ffs2_find_blockalloc - Locate the block allocation structure		/*{{{*/
// ---------------------------------------------------------------------
/* Once located the information will be returned in blk and the reader will
   be preloaded with the first chunk of the block data itself. Location is
   a 'pointer', the top 16 bits are the block number and the lower 16 are
   the allocation number. */
int ffs2_find_blockalloc(struct ffs_read *r,unsigned long location,
			 struct ffs2_blockalloc *blk)
{
   struct ffs2_blockalloc *alloc;
   unsigned long block = getFFS2_sb(r->super).BlockMap[location >> 16];
   signed long offset = getFFS2_sb(r->super).EraseSize - FFS_SIZEOF_BLOCK;
   offset -= FFS_SIZEOF_BLOCKALLOC*((location & 0xFFFF) + 1);

   if (offset <= 0)
      return -1;
   
   // Fetch it..
   if (ffs2_read(r,block,offset,FFS_SIZEOF_BLOCKALLOC) == 0)
      return -1;
   
   alloc = (struct ffs2_blockalloc *)r->p;
   
   // Make sure it is allocated
#ifndef SMALLER
   if (isflagset(alloc->Status,FFS_ALLOC_SMASK,FFS_ALLOC_ALLOCATED) == 0)
   {
      printk("ffs2: Failed to read read %lu %lu %x %x %x %x\n",block,
	     offset,alloc->Status,alloc->Offset[0],alloc->Offset[1],
	     alloc->Offset[2]);
      return -1;
   }
#endif
   
   if (blk != 0)
      memcpy(blk,alloc,FFS_SIZEOF_BLOCKALLOC);
   
   // Prepare the reader
   offset = (alloc->Offset[2] << 16) + (alloc->Offset[1] << 8) + alloc->Offset[0];
#ifndef SMALLER   
   if (offset >= getFFS2_sb(r->super).EraseSize ||
       offset + alloc->Len >= getFFS2_sb(r->super).EraseSize)
   {
      printk("ffs2: Corrupted allocation block %lx %u %u %u\n",location,alloc->Offset[0],alloc->Offset[1],alloc->Offset[2]);
      return -1;
   }
#endif
   
   r->location = location;
   
   // Preload it with the block
   if (alloc->Len < 100)
   {
      if (ffs2_read(r,block,offset,alloc->Len) == 0)
	 return -1;
   }   
   else
   {
      if (ffs2_read(r,block,offset,100) == 0)
	 return -1;
   }
   
   return 0;
}
									/*}}}*/
// ffs2_find_entry - Find an entry					/*{{{*/
// ---------------------------------------------------------------------
/* This follows the secondary pointers to resolve the superceding 
   mechanism. It will work on any type of entry structure 
   (dir/file/extent) */
struct ffs2_entry *ffs2_find_entry(struct ffs_read *r,unsigned long loc)
{
   struct ffs2_entry *entry;
   struct ffs2_blockalloc alloc;
   
   while (1)
   {
      if (ffs2_find_blockalloc(r,loc,&alloc) != 0)
	 break;
      entry = (struct ffs2_entry *)r->p;
      if (isFNULL(entry->SecondaryPtr) == 0 &&
	  (entry->Status & FFS_ENTRY_SECONDARY) != FFS_ENTRY_SECONDARY)
	 loc = entry->SecondaryPtr;
      else
      {
	 unsigned long size = alloc.Len;
	 
	 /* Range check the structure to make sure that it is the right 
	    size */
	 if (isflagset(entry->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEDIR) ||
	     isflagset(entry->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEFILE))
	    size = FFS_SIZEOF_ENTRY + entry->VarStructLen + entry->NameLen;
	 if (isflagset(entry->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEEXTENT))
	    size = FFS_SIZEOF_FILEINFO + entry->VarStructLen;

	 // Overflows the block (bad!)
#ifndef SMaLLER	 
	 if (size > alloc.Len)
	 {
	    printk("ffs2: Invalid entry %lu (%lu %u)\n",loc,size,alloc.Len);
	    return 0;
	 }	    
	 
	 if (size != alloc.Len)
	    printk("ffs2: Size off %lu %u\n",size,alloc.Len);
#endif
	 
	 // See if we need a larger read-ahead
	 if (size < r->ahead)
	 {
	    if (ffs2_read(r,r->block,r->offset,alloc.Len) == 0)
	       return 0;
	    entry = (struct ffs2_entry *)r->p;
	 }	 
	 
	 return entry;	 
      }      
   }
   printk("ffs2: Failed to find entry\n");
   return 0;
}
									/*}}}*/
// ffs2_copy_to_buff - Copy from an extent into a buffer		/*{{{*/
// ---------------------------------------------------------------------
/* This is the only routine that reads file extents. Any sort of 
   compression algorithm can be placed here. */
int ffs2_copy_to_buff(struct ffs_read *r,unsigned char *buf,
		      struct ffs2_fileinfo *extent,unsigned long toread,
		      unsigned long offset)
{
   if (ffs2_find_blockalloc(r,extent->ExtentPtr,0) != 0)
      return -1;
      
   /* Read in the block by repeatedly repositioning the memory window
      at consecutive bytes and reading the maximum possible length */
   while(toread != 0)
   {
      if (ffs2_read(r,r->block,r->offset + offset,10) == 0)
	 return -1;
      offset = 0;

      if (r->ahead > toread)
	 r->ahead = toread;
      
      memcpy(buf,r->p,r->ahead);
      r->offset += r->ahead;
      buf += r->ahead;
      toread -= r->ahead;
   }
   
   return 0;
}
									/*}}}*/
// ffs2_find_boot_block - Find the boot block				/*{{{*/
// ---------------------------------------------------------------------
/* Search the media for the first block and the boot block. This is a 
   simple search looking for valid block records and then one with a boot
   block pointer */
int ffs2_find_boot_block(struct ffs_read *r,unsigned long blocks)
{
   unsigned I;
   unsigned long firstvalid = 0xFFFFF;
   struct ffs2_sb_info *sb = &getFFS2_sb(r->super);
      
   for (I = 0; I != blocks; I++)
   {
      struct ffs2_block *block;
      sb->ZeroBlock = 0;

      if (ffs2_read(r,I,sb->EraseSize - FFS_SIZEOF_BLOCK,
		    FFS_SIZEOF_BLOCK) == 0)
	 return -1;
      
      block = (struct ffs2_block *)r->p;

      // It is either an erased block or junk..
      if (isflagset(block->Status,FFS_STATE_MASK,FFS_STATE_SPARE) &&
	  block->BlockSeqChecksum == 0xFFFF)
      {
	 if (firstvalid == 0xFFFFF)
	    firstvalid = I;
	 continue;
      }
      
      // Check the status field
#ifndef SMALLER      
      if ((block->BlockSeq ^ block->BlockSeqChecksum) != 0xFFFF ||
	  ((block->Status >> 3) & FFS_STATE_RESERVED) != FFS_STATE_RESERVED ||
	  block->BlockSeq >= blocks)
      {
	 printk("ffs2: Caution, invalid block %u\n",I);
	 continue;
      }
#endif
      
      if (firstvalid == 0xFFFFF)
	 firstvalid = I;

      // See if there is a boot block pointer..
      if (isflagset(block->Status,FFS_STATE_MASK,FFS_STATE_READY) && 
	  isflagset(block->Status,FFS_BOOTP_MASK,FFS_BOOTP_CURRENT) &&
	  (block->BootRecordPtr >> 16) == block->BlockSeq)
      {
	 struct ffs2_blockalloc alloc;
	 struct ffs2_bootrecord *boot;

	 // Now we index the boot block, repositioning the reader
	 sb->ZeroBlock = firstvalid;
	 sb->BlockMap[block->BootRecordPtr >> 16] = I - firstvalid;
	 if (ffs2_find_blockalloc(r,block->BootRecordPtr,&alloc) != 0)
	 {
	    printk("ffs2: Couldn't read the boot record %u\n",I);
	    continue;
	 }

	 // Verify it
	 boot = (struct ffs2_bootrecord *)r->p;
	 if (boot->Signature == 0xF1A5 && boot->FFSWriteVersion == 0x0200 &&
	     boot->FFSReadVersion == 0x0200 && 
	     boot->TotalBlockCount <= blocks - sb->ZeroBlock &&
	     boot->SpareBlockCount < boot->TotalBlockCount &&
	     (boot->BlockLen % sb->EraseSize) == 0)
	 {
	    memcpy(&sb->Boot,boot,sizeof(*boot));
	    return 0;
	 }
      }
   }   
   return -1;
}
									/*}}}*/
// ffs2_prepare - Loads in the block mapping table			/*{{{*/
// ---------------------------------------------------------------------
/* Generate a mapping of block sequence numbers to real block numbers and
   make sure that we have no holes. */
int ffs2_prepare(struct ffs_read *r)
{
   unsigned I;
   struct ffs2_sb_info *sb = &getFFS2_sb(r->super);
   unsigned Blocks = sb->Boot.TotalBlockCount - sb->Boot.SpareBlockCount;
   
   // Create the block mapping
   for (I = 0; I != sb->Boot.TotalBlockCount; I++)
   {
      struct ffs2_block *block;
      if (ffs2_read(r,I,sb->EraseSize - FFS_SIZEOF_BLOCK,
		    FFS_SIZEOF_BLOCK) == 0)
	 return -1;
      block = (struct ffs2_block *)r->p;

      if (isflagset(block->Status,FFS_STATE_MASK,FFS_STATE_SPARE) &&
	  block->BlockSeqChecksum == 0xFFFF)
	 continue;

      // Check the status field
      if ((block->BlockSeq ^ block->BlockSeqChecksum) != 0xFFFF ||
	  ((block->Status >> 3) & FFS_STATE_RESERVED) != FFS_STATE_RESERVED ||
	  block->BlockSeq >= Blocks)
      {
	 printk("ffs2: Fatal, invalid block %lu within FS\n",I+sb->ZeroBlock);
	 return -1;
      }

      if (!isflagset(block->Status,FFS_STATE_MASK,FFS_STATE_READY))
	 continue;
      sb->BlockMap[block->BlockSeq] = I;
   }
   
   // Verify that every block is accounted for
#ifndef SMALLER   
   for (I = 0; I != Blocks; I++)
   {
      if (sb->BlockMap[I] == 0xFFFF)
      {
	 printk("ffs2: Fatal, block sequence numbers do not work out %d\n",I);
	 return -1;
      }
   }
#endif
   
   return 0;
}
									/*}}}*/
// find_dirent - Lookup a file in a directory by name			/*{{{*/
// ---------------------------------------------------------------------
/* This is the same as ffs2_readdir, but doesnt do adds, just compares.
   If name is null then this returns 0 if the directory has anything 
   in it */
int ffs2_find_dirent(struct ffs_read *r,unsigned long loc,
		     struct qstr *name,unsigned long *pos)
{
   struct ffs2_entry *entry;
   unsigned long cur;
   
   // Locate the inode
   entry = ffs2_find_entry(r,loc);
   if (entry == 0)
      return -2;

   // Iterate over all of the extents for the directory
   if (!isFNULL(entry->PrimaryPtr) &&
       (entry->Status & FFS_ENTRY_PRIMARY) != FFS_ENTRY_PRIMARY)
   {
      cur = entry->PrimaryPtr;
      while (1)
      {
	 struct ffs2_entry *extent = ffs2_find_entry(r,cur);
	 if (extent == 0)
	 {
	    printk("ffs2: Failure reading directory component\n");
	    break;
	 }

	 // This should not happen.
#ifndef SMALLER	 
	 if (isflagset(extent->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEEXTENT))
	 {
	    printk("ffs2: Filesystem corruption, an extent in the directory list\n");
	    break;
	 }
#endif
	 
	 // Match?
	 if ((extent->Status & FFS_ENTRY_EXISTS) == FFS_ENTRY_EXISTS)
	 {
	    if (name == 0 || (name->len == extent->NameLen &&
		strncmp(extent->Name,name->name,name->len) == 0))
	    {
	       *pos = cur;
	       return 0;
	    }
	 }
	 
	 // Skip to the next one
	 if (isFNULL(extent->SiblingPtr) == 0 &&
	     (extent->Status & FFS_ENTRY_SIBLING) != FFS_ENTRY_SIBLING)
	    cur = extent->SiblingPtr;
	 else
	    break;
      }
   }
   *pos = 0;
   return -1;
}
									/*}}}*/
