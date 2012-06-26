// -*- mode: cpp; mode: fold -*-
// Description                                                          /*{{{*/
// $Id: super.c,v 1.1.1.1 2006-07-11 09:31:07 andy Exp $
/* ######################################################################

   Microsoft Flash File System 2 
   
   Information for the FFS2.0 was found in Microsoft's knowledge base,
   http://msdn.microsoft.com/isapi/msdnlib.idc?theURL=/library/specs/S346A.HTM
   Try searching for "Flash File System" if it has been moved
   
   The filesystem can run in two modes, directly on a MTD or indirectly
   through the block system. The FFS2 filesystem was really designed for 
   linearly mapped flash, it has no block alignment aside from the
   erase unit size which makes it difficult to fit into the block 
   device model.

   While operating the filesystem runs in such a way that a crash will not
   damage the consistency of the filesystem. All crashes will result in 
   allocated but unreferenced blocks that can be easially reclaimed by
   probing the entire allocation tree. There is a risk that a file entry 
   pointer could be partialy written to. In this case that entry pointer is
   useless - the simplest way out is to reclaim that block and set the 
   pointer back to FF. There is, unfortunately, no really good mechanism to
   do that directly :<
   
   TODO:
     - Byte swapping (Carefull, the structures are returned directly from
       bh's!)
     - Compression check should be equal sizes not by compress ID
     - Write routine needs to handle more cases, particularly overwriting,
       not just append
     - Directory additions should check and reclaim very long directories
       that are mostly empty by using superceeds
     - Use a constant inode scheme like the latest FAT driver does (2.2.11)
     - Rename
     - Block reclimation
     - Built in fsck on load - search for unreferenced blocks
     - Look at extensions to handle symlinks, device nodes and ownership.
       Can be done using the extension mechanism that is built in quite 
       directly.
     - Setting the new mtimes isn't right, it needs to go back over the list
       and reset the mtime valid flags. Mtime on empty newly created things
       is wrong too, it needs to use the value of the item itself.
     - Get support for QNX's BPE decompressor.
     - Split this foolish thing up into smaller modules
     - unlink seems to not flush the dentry cache?
   
   ##################################################################### */
									/*}}} */
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/malloc.h>
#include <linux/locks.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include "ffs2_fs.h"
#include "ffs2_fs_sb.h"
#include "ffs2_fs_i.h"
#include "local.h"
#include "io.h"

#include <asm/uaccess.h>

static struct super_operations ffs2_ops;
static struct inode_operations *ffs2_inoops[];

// ffs2_getbh - Get a bh, consulting our cache first			/*{{{*/
// ---------------------------------------------------------------------
/* Because of the random jumps in operations like reading a directory we
   keep a small cache of 4 most recent requests. This prevents BH trashing
   when we lookup block allocation then block then the next allocation
   etc. The speed gain on block devices is substantial. */
static struct buffer_head *ffs2_getbh(struct ffs_read *r,unsigned long loc)
{
   unsigned int I;
   unsigned int old = 0;
   unsigned int age = r->curage;
   
   // Search
   for (I = 0; I != sizeof(r->bh)/sizeof(*r->bh); I++)
   {
      if (r->bh[I] == 0)
	 continue;
      
      if (r->bh[I]->b_blocknr == loc)
	 break;
      if (age > r->age[I])
      {
	 old = I;
	 age = r->age[I];
      }	 
   }   
   
   // Miss
   if (I >= sizeof(r->bh)/sizeof(*r->bh))
   {
      I = old;
      
      // Free the old bh
      if (r->bh[I] != 0)
	 brelse(r->bh[I]);
      if ((r->bh[I] = bread(r->super->s_dev,loc,BLOCK_SIZE)) == NULL)
	 return 0;
   }   
   
   r->age[I] = r->curage++;
   return r->bh[I];
}
									/*}}}*/
// ffs2_read - Perform a read operation					/*{{{*/
// ---------------------------------------------------------------------
/* This performs a read operation with no alignment constraints. If it is 
   possible to return the entire requested block in someone elses memory
   (a MTD window or a buffer head) then that will be done, otherwise a
   copy is performed to the given temporary memory and that is returned
   instead. This will automatically free the ffs_read structure as it
   executes. Count must be a small number, below 128. Using the 
   ahead/behind values it is possible to make multiple requests to 
   do reading */
unsigned char *ffs2_read(struct ffs_read *r,unsigned long block,
			 unsigned long offset,unsigned long count)
{
   unsigned long loc;
   unsigned long left;
   unsigned char *pos;

   if (getFFS2_sb(r->super).Boot.TotalBlockCount != 0 &&
       block >= getFFS2_sb(r->super).Boot.TotalBlockCount)
      return 0;

   r->block = block;
   r->offset = offset;

   block += getFFS2_sb(r->super).ZeroBlock;
   loc = getFFS2_sb(r->super).EraseSize*block + offset;

   // We can return a bh..
   if ((loc >> BLOCK_SIZE_BITS) == ((loc + count - 1) >> BLOCK_SIZE_BITS))
   {
      struct buffer_head *bh = ffs2_getbh(r,loc >> BLOCK_SIZE_BITS);
      if (bh == 0)
	 return 0;
      
      r->behind = loc & (BLOCK_SIZE-1);
      r->ahead = BLOCK_SIZE - r->behind;
      r->p = bh->b_data + r->behind;
      return r->p;
   }
   
   // Doomed :>
   if (count > sizeof(r->temp))
   {
      printk("ffs2: Reading too much");
      return 0;
   }
   
   // Need to use the temp buffer
   pos = r->temp;
   memset(r->temp,0,sizeof(r->temp));
   left = count;
   while (left != 0)
   {
      unsigned long behind;
      unsigned long ahead;
      struct buffer_head *bh = ffs2_getbh(r,loc >> BLOCK_SIZE_BITS);
      if (bh == 0)
	 return 0;
      
      behind = loc & (BLOCK_SIZE-1);
      ahead = BLOCK_SIZE - behind;
      if (left < ahead)
	 ahead = left;
      memcpy(pos,bh->b_data + behind,ahead);
      pos += ahead;
      loc += ahead;
      left -= ahead;
   }
   r->behind = 0;
   r->ahead = count;
   r->p = r->temp;
   return r->p;
}
									/*}}}*/
// ffs2_relse - Release the read structure				/*{{{*/
// ---------------------------------------------------------------------
/* This releases any cached BHs, it is important to call it! */
void ffs2_relse(struct ffs_read *r)
{
   unsigned I;
   for (I = 0; I != sizeof(r->bh)/sizeof(*r->bh); I++)
   {
      if (r->bh[I] != 0)
	 brelse(r->bh[I]);
   }
   memset(r,0,sizeof(*r));
}
									/*}}}*/

// ffs2_write - Write data to the flash					/*{{{*/
// ---------------------------------------------------------------------
/* Copy the given data into the buffer cache */
int ffs2_write(struct ffs_read *r,unsigned long block,
	       unsigned long offset,unsigned char *from,unsigned long count)
{
   unsigned long loc;
   unsigned long left;

   if (getFFS2_sb(r->super).Boot.TotalBlockCount != 0 &&
       block >= getFFS2_sb(r->super).Boot.TotalBlockCount)
      return 0;

   r->block = block;
   r->offset = offset;

   block += getFFS2_sb(r->super).ZeroBlock;
   loc = getFFS2_sb(r->super).EraseSize*block + offset;

   /* Copy over the new data, this should probably use a mechansim that
      doesnt read in the old contents if the write buffer is the full 
      length */
   left = count;
   while (left != 0)
   {
      unsigned long behind;
      unsigned long ahead;
      struct buffer_head *bh = ffs2_getbh(r,loc >> BLOCK_SIZE_BITS);
      if (bh == 0)
	 return -1;
      
      /*printk("Writing %lu to %lu\n",left,loc);
      {
	 unsigned long I;
	 for (I = 0; I < left; I++)
	    printk("%x ",from[I]);
	 printk("\n");
      }*/
      
      behind = loc & (BLOCK_SIZE-1);
      ahead = BLOCK_SIZE - behind;
      if (left < ahead)
	 ahead = left;
      memcpy(bh->b_data + behind,from,ahead);
      from += ahead;
      loc += ahead;
      left -= ahead;

      // Dirty the buffer
      mark_buffer_uptodate(bh, 1);
      mark_buffer_dirty(bh,0);
   }
   r->behind = 0;
   r->ahead = 0;
   r->p = 0;
   return 0;
}
									/*}}}*/

// insert_region - Add a new region to the list of regions		/*{{{*/
// ---------------------------------------------------------------------
/* The purpose here is to take a region and add it to our sorted list of
   regions and merge it with the other regions in the list. */
static int insert_region(struct ffs2_free_space *spaces,unsigned int max,
			 unsigned long start,unsigned long len)
{
   unsigned int I;
   unsigned long end = start + len;
   for (I = 0; I < max-2; I++)
   {
      // Merge it with the last one
      if (spaces[I].Stop == start && spaces[I].Stop != 0)
      {
	 spaces[I].Stop = end;
	 
	 // Join with the next one too
	 if (end == spaces[I+1].Start)
	 {
	    spaces[I].Stop = spaces[I+1].Stop;
	    memmove(spaces + I+1,spaces + I+2,(max - I - 1)*sizeof(*spaces));
	 }	 
	 return 0;
      }
      
      // Merge it with the next one
      if (spaces[I].Start == end)
      {
	 spaces[I].Start = start;
	 return 0;
      }
      	 
      // Insert a new one
      if (spaces[I].Start > start || spaces[I].Stop == 0)
      {	 
	 memmove(spaces + I+1,spaces + I,(max - I - 1)*sizeof(*spaces));
	 spaces[I].Start = start;
	 spaces[I].Stop = end;
	 return 0;
      }
   }   
   
   printk("ffs2: Unable to sort allocation list");
   return -1;
}
									/*}}}*/
// ffs2_make_free_list - Generate the list of free regions		/*{{{*/
// ---------------------------------------------------------------------
/* This scans the block allocations and builds up a list of the free 
   regions. Due to the allocation model used this list should be 
   -short- but it is expensive to calculate the first time. */
static int ffs2_make_free_list(struct ffs_read *r,unsigned block)
{
   enum {max = 100};
   struct ffs2_sb_info *sb = &getFFS2_sb(r->super);
   struct ffs2_free_space spaces[max];
   struct ffs2_blockalloc *alloc;
   struct ffs2_sb_block *info = &sb->Blocks[block];
   unsigned long cur;
   unsigned long offset;
   
   memset(spaces,0,sizeof(spaces));
   
   // Record the end block
   if (insert_region(spaces,max, sb->EraseSize - FFS_SIZEOF_BLOCK,
		     FFS_SIZEOF_BLOCK) != 0)
      return -1;

   info->ReclaimableSpace = 0;
   for (cur = 0; ; cur++)
   {	  
      offset = sb->EraseSize - FFS_SIZEOF_BLOCK - 
	       (cur + 1)*FFS_SIZEOF_BLOCKALLOC;
      if (offset <= 100)
      {
	 printk("ffs2: Allocation list too long");
	 return -1;
      }
      
      // XX can be advoided
      if (ffs2_read(r,block,offset,FFS_SIZEOF_BLOCKALLOC) == 0)
	 return -1;
      alloc = (struct ffs2_blockalloc *)r->p;

      // Record the block alloc structure
      if (insert_region(spaces,max,offset,FFS_SIZEOF_BLOCKALLOC) != 0)
	 return -1;

      // Make sure it is allocated
      if (isflagset(alloc->Status,FFS_ALLOC_SMASK,FFS_ALLOC_ALLOCATED) == 0 &&
	  isflagset(alloc->Status,FFS_ALLOC_SMASK,FFS_ALLOC_DEALLOCATED) == 0)
      {
	 // End of the list
	 if (isflagset(alloc->Status,FFS_ALLOC_EMASK,FFS_ALLOC_END) != 0)
	    break;
	 continue;
      }      
      if (isflagset(alloc->Status,FFS_ALLOC_SMASK,FFS_ALLOC_DEALLOCATED) != 0)
	 info->ReclaimableSpace += alloc->Len;

      // Record the block data
      offset = (alloc->Offset[2] << 16) + (alloc->Offset[1] << 8) + alloc->Offset[0];
      if (insert_region(spaces,max,offset,alloc->Len) != 0)
	 return -1;

      // End of the list
      if (isflagset(alloc->Status,FFS_ALLOC_EMASK,FFS_ALLOC_END) != 0)
	 break;
   }

/*   printk("free list for %lu - %lu %lu\n",block,cur,r->location);
   for (cur = 0; spaces[cur].Stop != 0 || spaces[cur].Start != 0; cur++)
      printk("  at %lu %lu\n",spaces[cur].Start,spaces[cur].Stop);*/
   
   // Invert the list to get a list of free spaces
   cur = 0;
   if (spaces[0].Start != 0)
   {
      memmove(spaces+1,spaces,(max - 1)*sizeof(*spaces));
      spaces[0].Start = 1;
      spaces[0].Stop = 0;
   }
   info->FreeSpace = 0;
   info->LargestSpace = 0;
   for (; spaces[cur].Stop != 0 || spaces[cur].Start != 0; cur++)
   {
      spaces[cur].Start = spaces[cur].Stop;
      spaces[cur].Stop = spaces[cur+1].Start;
      if (spaces[cur].Stop == 0)
      {
	 spaces[cur].Start = 0;
	 continue;
      }
      
      info->FreeSpace += spaces[cur].Stop - spaces[cur].Start;
      if (info->LargestSpace < spaces[cur].Stop - spaces[cur].Start)
	 info->LargestSpace = spaces[cur].Stop - spaces[cur].Start;
   }

   // Store the allocation list
   info->FreeList = kmalloc(sizeof(*info->FreeList)*cur,GFP_KERNEL);
   memmove(info->FreeList,spaces,sizeof(*info->FreeList)*cur);
   
   return 0;
}
									/*}}}*/
// ffs2_prepare_info - Prepares the block info structures		/*{{{*/
// ---------------------------------------------------------------------
/* Generate a mapping of block sequence numbers to real block numbers and
   make sure that we have no holes. This also should repair any damage
   from an abortive reclimation process */
static int ffs2_prepare_info(struct ffs_read *r)
{
   unsigned I;
   struct ffs2_sb_info *sb = &getFFS2_sb(r->super);
   unsigned Blocks = sb->Boot.TotalBlockCount - sb->Boot.SpareBlockCount;

   sb->Blocks = kmalloc(sizeof(*sb->Blocks)*sb->Boot.TotalBlockCount,
			GFP_KERNEL);
   if (sb->Blocks == 0)
      return -1;
   memset(sb->Blocks,0,sizeof(*sb->Blocks)*sb->Boot.TotalBlockCount);
      
   // Generate the reverse mapping
   for (I = 0; I != Blocks; I++)
   {
      if (ffs2_make_free_list(r,sb->BlockMap[I]) != 0)
      {
	 kfree(sb->Blocks);
	 sb->Blocks = 0;
	 return -1;
      }            
      sb->Blocks[sb->BlockMap[I]].VirtualBlock = I;
   }
   
   return 0;
}
									/*}}}*/

// find_free_alloc - Find and allocate some space			/*{{{*/
// ---------------------------------------------------------------------
/* This tries to allocate some space for writing new data too. Block 
   acts as a hint of where to place the allocation or -1 if it does not
   matter. Writing consists of 
     1 Locate a free allocation block
     2 Extend the list
     3 Write the offset and length
     4 Indicate the block is allocated
   Failures can occure at any point. The only tricky failure is betten 3
   and 4, this is detected by ensuring the fields of an unused entry are
   actually FF. Once point 4 is passed then the block is permanently 
   allocated and a failure before it is linked into any lists will leave
   it hanging around forever. */
static unsigned long find_free_alloc(struct ffs_read *r,
				     struct ffs2_blockalloc *outalloc,
				     unsigned long len,long block)
{
   struct ffs2_sb_info *sb = &getFFS2_sb(r->super);
   struct ffs2_sb_block *info;
   unsigned int I;
   unsigned int writeblock;
   unsigned long cur;
   unsigned long offset;
   long best = -1;
   struct ffs2_blockalloc *alloc;

   if (block > 0)
      block = sb->BlockMap[block];
      
   // Select a block that has enough free space to insert the data
   for (I = 0; I != sb->Boot.TotalBlockCount; I++)
   {
      if (sb->Blocks[I].LargestSpace < len + FFS_SIZEOF_BLOCKALLOC)
	 continue;
      if (best == -1)
	 best = I;
      
      if (sb->Blocks[I].FreeSpace > sb->Blocks[best].FreeSpace)
	 best = I;
   }

   /* If there is no space in the requested block or none was given use
      the best one. This should possibly try to reclaim the best if that
      would help */
   if (block < 0 || 
       sb->Blocks[block].LargestSpace < len + FFS_SIZEOF_BLOCKALLOC)
      block = best;
   
   // No block found
   if (block == -1)
      return -1;
   
   // Locate the block we are going to use
   info = &sb->Blocks[block];
   for (writeblock = 0; info->FreeList[writeblock].Stop != 0; writeblock++)
      if (info->FreeList[writeblock].Stop - info->FreeList[writeblock].Start >= len)
	 break;
   if (info->FreeList[writeblock].Stop == 0)
      return -1;
   
   // Look for a free allocation entry to use
   for (cur = 0; ; cur++)
   {	  
      offset = sb->EraseSize - FFS_SIZEOF_BLOCK - 
	       (cur + 1)*FFS_SIZEOF_BLOCKALLOC;
      if (offset <= 100)
      {
	 printk("ffs2: Allocation list too long");
	 return -1;
      }
      
      // XX can be advoided
      if (ffs2_read(r,block,offset,FFS_SIZEOF_BLOCKALLOC) == 0)
	 return -1;
      alloc = (struct ffs2_blockalloc *)r->p;
      
      // It is free..
      if (isflagset(alloc->Status,FFS_ALLOC_SMASK,FFS_ALLOC_FREE) != 0)
      {
	 // Make sure that this was not an abortive write
	 if (alloc->Len == 0xFFFF && alloc->Offset[0] == 0xFF &&
	     alloc->Offset[1] == 0xFF && alloc->Offset[2] == 0xFF)
	    break;
      }
      
      // End of the list
      if (isflagset(alloc->Status,FFS_ALLOC_EMASK,FFS_ALLOC_END) != 0)
	 break;
   }
   
   // End of the list, allocate a new entry past the very end.
   if (isflagset(alloc->Status,FFS_ALLOC_EMASK,FFS_ALLOC_END) != 0)
   {
      __u8 newStatus;
      
      // Update the allocation table
      for (I = 0; info->FreeList[I].Stop != 0; I++)
      {
	 if (info->FreeList[I].Stop == offset)
	    break;
      }      
      if (info->FreeList[I].Stop == 0)
      {
	 printk("ffs2: Corrupted allocation list");
	 return -1;
      }
      
      // Rewrite the status bit on the last entry
      newStatus = alloc->Status & (~FFS_ALLOC_END);
      if (ffs2_write(r,block,offset,&newStatus,sizeof(newStatus)) != 0)
	 return -1;
      
      cur++;
      offset = sb->EraseSize - FFS_SIZEOF_BLOCK - 
	       (cur + 1)*FFS_SIZEOF_BLOCKALLOC;
      info->FreeList[I].Stop = offset;
   }

   // Double check
   if (info->FreeList[writeblock].Stop - info->FreeList[writeblock].Start < len)
   {
      printk("ffs2: Logic error allocation new free block\n");
      return -1;
   }
 
   if (ffs2_read(r,block,offset,FFS_SIZEOF_BLOCKALLOC) == 0)
      return -1;
   alloc = (struct ffs2_blockalloc *)r->p;

   // The region that is supposed to be FF isnt!
   if (alloc->Len != 0xFFFF || alloc->Offset[0] != 0xFF ||
       alloc->Offset[1] != 0xFF || alloc->Offset[2] != 0xFF)
   {
      printk("ffs2: Block allocations corrupted\n");
      return -1;
   }
   
   // Now write the offset and length bytes
   outalloc->Status = (alloc->Status & (~FFS_ALLOC_SMASK)) | FFS_ALLOC_ALLOCATED;
   outalloc->Len = len;
   outalloc->Offset[2] = (__u8)(info->FreeList[writeblock].Start >> 16);
   outalloc->Offset[1] = (__u8)(info->FreeList[writeblock].Start >> 8);
   outalloc->Offset[0] = (__u8)(info->FreeList[writeblock].Start);
   
   if (ffs2_write(r,block,offset + sizeof(outalloc->Status),
		  outalloc->Offset,FFS_SIZEOF_BLOCKALLOC - 
		  sizeof(outalloc->Status)) != 0)
      return -1;
   if (ffs2_write(r,block,offset,&outalloc->Status,sizeof(outalloc->Status)) != 0)
      return -1;

   // Remove it from the free list
   offset = info->FreeList[writeblock].Start;
   info->FreeList[writeblock].Start += len;

   // Recalculate the available space.
   info->LargestSpace = 0;
   info->FreeSpace = 0;
   for (I = 0; info->FreeList[I].Stop != 0; I++)
   {
      if (info->FreeList[I].Stop - info->FreeList[I].Start > info->LargestSpace)
	 info->LargestSpace = info->FreeList[I].Stop - info->FreeList[I].Start;
      info->FreeSpace += info->FreeList[I].Stop - info->FreeList[I].Start;
   }

   r->block = block;
   r->offset = offset;
   return (info->VirtualBlock << 16) + cur;
}
									/*}}}*/
// update_pointer - Update a pointer in a block				/*{{{*/
// ---------------------------------------------------------------------
/* This does some small writes to update a single pointer to point to a new
   location. */
#define FFS_PTR_SIBLING 2
#define FFS_PTR_PRIMARY 6
#define FFS_PTR_SECONDARY 10
static int update_pointer(struct ffs_read *r,unsigned long from,
			  unsigned long target,unsigned type)
{
   struct ffs2_entry *extent = ffs2_find_entry(r,from);
   unsigned long block = r->block;
   unsigned long offset = r->offset;
   __u16 status;
   
   if (extent == 0)
      return -1;   
   status = extent->Status;
   
   // First update the pointer
   if (ffs2_write(r,block,offset + type,(unsigned char *)&target,4) != 0)
      return -1;
   
   // Then the status
   if (type == FFS_PTR_PRIMARY)
      status = status & (~FFS_ENTRY_PRIMARY);
   if (type == FFS_PTR_SECONDARY)
      status = status & (~FFS_ENTRY_SECONDARY);
   if (type == FFS_PTR_SIBLING)
      status = status & (~FFS_ENTRY_SIBLING);
   
   if (ffs2_write(r,block,offset,(unsigned char *)&status,sizeof(status)) != 0)
      return -1;
   
   return 0;
}
									/*}}}*/
// ffs2_entry_add - Add a new entry to a directory			/*{{{*/
// ---------------------------------------------------------------------
/* This adds a new entry to a directory. The steps are,
    1 - Find allocation space
    2 - Mark as allocated, write the data to it
    3 - Update the directory pointer to point to the new allocation 
   Flash space will be lost if there is a failure between 2 and 3. */
int ffs2_entry_add(struct ffs_read *r,struct ffs2_entry *dir,
		   struct ffs2_entry *new,unsigned long *loc)
{
   unsigned long cur;
   unsigned long allocloc;
   int Primary = 0;
   int res;
   struct ffs2_blockalloc alloc;
      
   // The directory is empty, we need to add the new entry below the head.
   if (isFNULL(dir->PrimaryPtr) ||
       (dir->Status & FFS_ENTRY_PRIMARY) == FFS_ENTRY_PRIMARY)
   {
      cur = r->location;
      Primary = 1;
      
      // XXX Perform block reclimation instead
      if (isFNULL(dir->PrimaryPtr) == 0)
      {
	 return -1;
      }
   }   
   else
   {
      struct ffs2_entry *extent;
      
      // Walk the directory and find the end point of the linked list
      cur = dir->PrimaryPtr;
      while (1)
      {
	 extent = ffs2_find_entry(r,cur);
	 if (extent == 0)
	    return -1;
	 
	 // This should not happen.
	 if (isflagset(extent->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEEXTENT))
	    return -1;
	 
	 // Skip to the next one
	 if (isFNULL(extent->SiblingPtr) == 0 &&
	     (extent->Status & FFS_ENTRY_SIBLING) != FFS_ENTRY_SIBLING)
	    cur = extent->SiblingPtr;
	 else
	    break;
      }
    
      // XXX Perform block reclimation instead
      if (isFNULL(extent->SiblingPtr) == 0)
      {
	 return -1;
      }
   }
   
   /* Cur is now the location that we will be linking too. If this is a new
      primary link then we choose a block at random, otherwise we try to 
      get a new allocation in the same block */
   if (Primary == 0)
      allocloc = find_free_alloc(r,&alloc,FFS_SIZEOF_ENTRY + new->VarStructLen +
				 new->NameLen,cur >> 16);
   else
      allocloc = find_free_alloc(r,&alloc,FFS_SIZEOF_ENTRY + new->VarStructLen +
				 new->NameLen,-1);
   if (allocloc == (unsigned long)-1)
      return -1;

   // Write the block (find_free_alloc resets r->block/r->offset)
   if (ffs2_write(r,r->block,r->offset,(unsigned char *)new,alloc.Len) != 0)
      return -1;
   
   // Update the directory entry
   if (Primary == 0)
      res = update_pointer(r,cur,allocloc,FFS_PTR_SIBLING);
   else
      res = update_pointer(r,cur,allocloc,FFS_PTR_PRIMARY);
      
   *loc = allocloc;
   
   return res;
}
									/*}}}*/
// ffs2_new_entry - Create an arbitary new entry			/*{{{*/
// ---------------------------------------------------------------------
/* This goes through the process of creating a new sub entry to a
   directory. Type may either be a file or a directory */
static int ffs2_new_entry(struct inode *dir,struct dentry *dentry,
			   struct qstr *name,__u8 type)
{
   struct inode *inode;
   struct ffs2_entry *entry;
   struct ffs2_entry *direntry;
   struct ffs_read r;
   unsigned char tmp[300];
   unsigned long now = CURRENT_TIME;
   unsigned long cur;

   memset(&r,0,sizeof(r));   
   memset(tmp,0xFF,sizeof(tmp));
   r.super = dir->i_sb;

   if (name->len >= sizeof(tmp) - sizeof(*entry))
      return -ENAMETOOLONG;
       
   // Fill in the entry structure
   entry = (struct ffs2_entry *)tmp;  
   entry->Status = type | (0xFFFF & (~FFS_ENTRY_TYPEMASK));
   entry->Attrib = 0;
   entry->Time = (__u16)now;
   entry->Date = (__u16)(now >> 16);
   entry->VarStructLen = 0;
   entry->NameLen = name->len;
   memcpy(entry->Name,name->name,entry->NameLen);

   // Get the directory inode
   direntry = ffs2_find_entry(&r,dir->i_ino);
   if (direntry == 0)
   {
      ffs2_relse(&r);
      return -EIO;
   }
		  
   // Append the entry
   if (ffs2_entry_add(&r,direntry,entry,&cur) != 0)
   {
      ffs2_relse(&r);
      return -EIO;
   }
   
   ffs2_relse(&r);
   
   if (dentry == 0)
      return 0;
   
   // Update the dentry cache
   if ((inode = iget(dir->i_sb,cur))==NULL)
      return -EACCES;
   d_instantiate(dentry,inode);
   return 0;
}
									/*}}}*/
// ffs2_new_extent - Create a new extent				/*{{{*/
// ---------------------------------------------------------------------
/* This creates a new, unlinked extent. The caller must link the extent to
   something. */
static int ffs2_new_extent(struct ffs_read *r,unsigned char *buf,
			   unsigned long length,unsigned long *loc)
{
   struct ffs2_fileinfo *entry;
   struct ffs2_blockalloc alloc;
   unsigned char tmp[300];
   unsigned long now = CURRENT_TIME;
   unsigned long cur;
   
   memset(tmp,0xFF,sizeof(tmp));
   
   // Fill in the entry structure
   entry = (struct ffs2_fileinfo *)tmp;  
   entry->Status = FFS_ENTRY_TYPEEXTENT | (0xFFFF & (~FFS_ENTRY_TYPEMASK));
   entry->Attrib = 0;
   entry->Time = (__u16)now;
   entry->Date = (__u16)(now >> 16);
   entry->VarStructLen = 0;
   entry->UncompressedExtentLen = length;
   entry->CompressedExtentLen = length;

   // Allocate the data block
   entry->ExtentPtr = find_free_alloc(r,&alloc,length,-1);
   if (entry->ExtentPtr == (unsigned long)-1 ||
       ffs2_write(r,r->block,r->offset,buf,alloc.Len) != 0)
      return -1;
   entry->Status = entry->Status & (~FFS_ENTRY_SIBLING);
   
   /* Allocate the entry for the extent info in the same block as the data
      if possible */
   cur = find_free_alloc(r,&alloc,FFS_SIZEOF_FILEINFO +
			 entry->VarStructLen,entry->ExtentPtr >> 16);
   if (cur == (unsigned long)-1 ||
       ffs2_write(r,r->block,r->offset,(unsigned char *)entry,alloc.Len) != 0)
      return -1;
   
   *loc = cur;
   return 0;
}
									/*}}}*/
// free_alloc - Set an allocation entry to be reclaimable		/*{{{*/
// ---------------------------------------------------------------------
/* */
static int free_alloc(struct ffs_read *r,unsigned long location)
{
   struct ffs2_blockalloc *alloc;
   unsigned long length;
   __u8 status;
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
   if (isflagset(alloc->Status,FFS_ALLOC_SMASK,FFS_ALLOC_ALLOCATED) == 0)
      return -1;
   
   // Deallocate
   length = alloc->Len;
   status = (alloc->Status & (~FFS_ALLOC_SMASK)) | FFS_ALLOC_DEALLOCATED;
   if (ffs2_write(r,block,offset,&status,sizeof(status)) != 0)
      return -1;
   
   getFFS2_sb(r->super).Blocks[block].ReclaimableSpace += length;
   return 0;
}
									/*}}}*/
// ffs2_erase - Erase an arbitary entry					/*{{{*/
// ---------------------------------------------------------------------
/* This does a very unsafe unlink operation. It goes over the linked list
   of extents and marks them all as erased. If this routine is interrupted
   the filesystem will have unreclaimable blocks but will still be 
   consistent */
int ffs2_erase(struct ffs_read *r,unsigned long loc,int isdir)
{
   struct ffs2_blockalloc alloc;
   struct ffs2_entry *entry;
   unsigned long top;
   unsigned long oldloc;
   
   // Go along the linked list and destroy it.
   top = loc;
   while (loc != 0xFFFFFFFF)
   {
      unsigned long block;
      unsigned long offset;
      
      oldloc = loc;
      if (ffs2_find_blockalloc(r,loc,&alloc) != 0)
	 break;
      entry = (struct ffs2_entry *)r->p;
      block = r->block;
      offset = r->offset;
      
      if (isFNULL(entry->SecondaryPtr) == 0 &&
	  (entry->Status & FFS_ENTRY_SECONDARY) != FFS_ENTRY_SECONDARY)
      {
	 loc = entry->SecondaryPtr;
      }      
      else
      {
	 // Skip cur to the next one
	 if (isdir == 0)
	 {
	    if (isFNULL(entry->PrimaryPtr) == 0 &&
		(entry->Status & FFS_ENTRY_PRIMARY) != FFS_ENTRY_PRIMARY)
	       loc = entry->PrimaryPtr;
	    else
	       loc = 0xFFFFFFFF;
	 }
	 else
	 {
	    if (isFNULL(entry->SiblingPtr) == 0 &&
		(entry->Status & FFS_ENTRY_SIBLING) != FFS_ENTRY_SIBLING)
	       loc = entry->SiblingPtr;
	    else
	       loc = 0xFFFFFFFF;
	 }
	 
	 if (isflagset(entry->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEEXTENT))
	 {
	    // Purge the datablock
	    if ((entry->Status & FFS_ENTRY_EXISTS) == FFS_ENTRY_EXISTS)
	       free_alloc(r,entry->SiblingPtr);	    
	 }
      }

      // Erase it
      if (top != oldloc)
	 free_alloc(r,oldloc);
      else
      {
	 // The first entry we set to deleted and do not erase
	 __u8 status = entry->Status;
	 status = status & (~FFS_ENTRY_EXISTS);
	 if (ffs2_write(r,block,offset,&status,sizeof(status)) != 0)
	    return -EIO;
      }      
   }
   return 0;
}
									/*}}}*/

// ffs2_read_super - Read the superblock (mount)			/*{{{*/
// ---------------------------------------------------------------------
/* The process for locating the super block involves looking at every block
   in the filesystem for a block record that has a valid boot record pointer,
   following that pointer and then verifying that it is the boot block. */
static struct super_block *ffs2_read_super(struct super_block *s,
					   void *data,int silent)
{
   struct ffs2_sb_info *sb;
   struct ffs_read r;
   unsigned long blocks;
   
   MOD_INC_USE_COUNT;

   // Setup the super block
   lock_super(s);
   set_blocksize(s->s_dev, BLOCK_SIZE);
   s->s_blocksize = BLOCK_SIZE;
   s->s_blocksize_bits = BLOCK_SIZE_BITS;
   sb = &getFFS2_sb(s);
   memset(sb,0,sizeof(*sb));
   sb->EraseSize = 0x20000;        // Temp
   
   memset(&r,0,sizeof(r));
   r.super = s;

   // Allocate the block mapping structure
   blocks = blk_size[MAJOR(s->s_dev)][MINOR(s->s_dev)]*BLOCK_SIZE/sb->EraseSize;
   sb->BlockMap = kmalloc(sizeof(*sb->BlockMap)*blocks,GFP_KERNEL);
   if (sb->BlockMap == 0)
   {
      s->s_dev = 0;
      unlock_super(s);
      MOD_DEC_USE_COUNT;
      return 0;      
   }
   memset(sb->BlockMap,0xFF,sizeof(*sb->BlockMap)*blocks);
   
   /* Perform the filesystem startup, locate the boot block, do
      a cleanup, etc. */
   if (ffs2_find_boot_block(&r,blocks) != 0 || ffs2_prepare(&r) != 0 || 
       ffs2_prepare_info(&r))
   {
      printk("ffs2: Could not find the superblock\n");
      kfree(sb->BlockMap);
      ffs2_relse(&r);
      s->s_dev = 0;
      unlock_super(s);
      MOD_DEC_USE_COUNT;
      return 0;      
   }
   ffs2_relse(&r);
   
   s->s_op = &ffs2_ops;
   s->s_root = d_alloc_root(iget(s,sb->Boot.RootDirectoryPtr),NULL);
   
   unlock_super(s);
   
   return s;
}
									/*}}}*/
// ffs2_put_super - Put back the superblock (unmount)			/*{{{*/
// ---------------------------------------------------------------------
/* Not much to do here..*/
static void ffs2_put_super(struct super_block *s)
{
   unsigned I;
   
   kfree(getFFS2_sb(s).BlockMap);   
   for (I = 0; I != getFFS2_sb(s).Boot.TotalBlockCount; I++)
      kfree(getFFS2_sb(s).Blocks[I].FreeList);
   kfree(getFFS2_sb(s).Blocks);
   
   MOD_DEC_USE_COUNT;
   return;
}
									/*}}}*/
// ffs2_statfs - Stat the filesystem get free blocks, etc		/*{{{*/
// ---------------------------------------------------------------------
/* */
static int ffs2_statfs(struct super_block *s, struct statfs *buf, int bufsize)
{
   struct statfs tmp;
   struct ffs2_sb_info *sb = &getFFS2_sb(s);
   unsigned int I;
   
   memset(&tmp, 0, sizeof(tmp));
   tmp.f_type = FFS2_MAGIC;
   tmp.f_bsize = 1;
   tmp.f_blocks = sb->Boot.TotalBlockCount - sb->Boot.SpareBlockCount;
   tmp.f_blocks *= sb->Boot.BlockLen;
   tmp.f_namelen = FFS2_MAXFN;
   
   // Count up free space
   for (I = 0; I != sb->Boot.TotalBlockCount; I++)
   {
      tmp.f_bfree += sb->Blocks[I].FreeSpace  +
	 sb->Blocks[I].ReclaimableSpace;
   }
   tmp.f_bavail = tmp.f_bfree;
      
   return copy_to_user(buf, &tmp, bufsize) ? -EFAULT : 0;
}
									/*}}}*/
// ffs2_readdir - Read a list of files from a directory			/*{{{*/
// ---------------------------------------------------------------------
/* read a directory. offset is the number of extents into the directory,
   I am not sure exactly how the calling code uses this, but it would be
   more effecient to store a pointer to the current entry in offset, but 
   that is basically a random value.. */
static int ffs2_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
   struct ffs2_entry *entry;
   struct inode *i = filp->f_dentry->d_inode;
   struct ffs_read r;
   unsigned int stored = 0;
   unsigned int offset = 1;
   unsigned long cur;
   
   memset(&r,0,sizeof(r));   
   r.super = i->i_sb;
   
   // Locate the inode
   entry = ffs2_find_entry(&r,i->i_ino);
   if (entry == 0)
   {
      ffs2_relse(&r);
      return 0;
   }

   if (offset > filp->f_pos)
   {
      filp->f_pos = offset;
      if (filldir(dirent,".",1,filp->f_pos,i->i_ino) < 0)
      {
	 ffs2_relse(&r);
	 return stored;
      }
      stored++;
   }
   offset++;
       
   if (offset > filp->f_pos)
   {
      unsigned long node = 0;
      if (i->i_ino == getFFS2_sb(i->i_sb).Boot.RootDirectoryPtr)
	 node = i->i_ino;
      
      filp->f_pos = offset;
      if (filldir(dirent,"..",2,filp->f_pos,node) < 0)
      {
	 ffs2_relse(&r);
	 return stored;
      }
      stored++;
   }
   offset++;
   
   // Iterate over all of the extents for the directory
   if (!isFNULL(entry->PrimaryPtr) &&
       (entry->Status & FFS_ENTRY_PRIMARY) != FFS_ENTRY_PRIMARY)
   {
      cur = entry->PrimaryPtr;
      while (1)
      {
	 struct ffs2_entry *extent = ffs2_find_entry(&r,cur);
	 if (extent == 0)
	 {
	    printk("ffs2: Failure reading directory component\n");
	    break;
	 }

	 // This should not happen.
	 if (isflagset(extent->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEEXTENT))
	 {
	    printk("ffs2: Filesystem corruption, an extent in the directory list\n");
	    break;
	 }
	 
	    
	 // Fill in the dirent structure
	 if ((extent->Status & FFS_ENTRY_EXISTS) == FFS_ENTRY_EXISTS)
	 {
	    if (offset > filp->f_pos)
	    {
	       char S[100];
	       strncpy(S,extent->Name,extent->NameLen);
	       S[extent->NameLen] = 0;
	       filp->f_pos = offset;
	       if (filldir(dirent,extent->Name,extent->NameLen,filp->f_pos,cur) < 0)
		  break;
	       stored++;
	    }
	    
	    offset++;
	 }
	 
	 // Skip to the next one
	 if (isFNULL(extent->SiblingPtr) == 0 &&
	     (extent->Status & FFS_ENTRY_SIBLING) != FFS_ENTRY_SIBLING)
	    cur = extent->SiblingPtr;
	 else
	    break;
      }
   }
   
   ffs2_relse(&r);
   
   return stored;
}
									/*}}}*/
// ffs2_lookup - Lookup a file in a directory by name			/*{{{*/
// ---------------------------------------------------------------------
/* This is the same as ffs2_readdir, but doesnt do adds, just compares */
static struct dentry *ffs2_lookup(struct inode *dir, struct dentry *dentry)
{
   struct inode *inode;
   struct ffs_read r;
   unsigned long pos;
   int res;
   
   memset(&r,0,sizeof(r));   
   r.super = dir->i_sb;
   
   res = ffs2_find_dirent(&r,dir->i_ino,&dentry->d_name,&pos);
   if (res != 0)
   {
      ffs2_relse(&r);
      if (res == -1)
      {
	 d_add(dentry,0);
	 return ERR_PTR(0);
      }
      
      return ERR_PTR(-EIO);
   }

   ffs2_relse(&r);
   
   if ((inode = iget(dir->i_sb,pos)) == NULL)
      return ERR_PTR(-EACCES);
   d_add(dentry, inode);
   return 0;
}
									/*}}}*/
// ffs2_readpage - Read a page from the file				/*{{{*/
// ---------------------------------------------------------------------
/* This is the 'read' function, it works in PAGE_SIZE blocks, reading 
   from the file system directly into the page. It is forced to iterate over
   many of the extent headers which will slow things down.. */
static int ffs2_readpage(struct file *file, struct page *page)
{
   struct dentry *dentry = file->f_dentry;
   struct inode *inode = dentry->d_inode;
   unsigned char *buf;
   unsigned long offset, avail, readlen;
   int result = -EIO;
   struct ffs2_entry *entry;
   struct ffs_read r;
   unsigned long cur;
   
   memset(&r,0,sizeof(r));   
   r.super = inode->i_sb;
   
   // Setup the page
   atomic_inc(&page->count);
   set_bit(PG_locked, &page->flags);
   buf = (unsigned char *)page_address(page);
   clear_bit(PG_uptodate, &page->flags);
   clear_bit(PG_error, &page->flags);
   
   // Past then end is == 0
   readlen = 1;
   while (page->offset < inode->i_size) 
   {
      avail = inode->i_size - page->offset;
      readlen = PAGE_SIZE;
      if (avail < readlen)
	 readlen = avail;

      // Blank out the end of the page
      if (readlen < PAGE_SIZE) 
	 memset(buf + readlen,0,PAGE_SIZE-readlen);

      // Get the inode and follow to find the first extent
      if ((entry = ffs2_find_entry(&r,inode->i_ino)) == 0 ||
	  isFNULL(entry->PrimaryPtr) ||
	  (entry->Status & FFS_ENTRY_PRIMARY) == FFS_ENTRY_PRIMARY)
	 break;
      
      // Check for compression
      if ((entry->Status >> FFS_ENTRY_COMPIP_SHIFT) != 0xFF)
      {
	 printk("ffs2: No support for compressed format %x\n",
		entry->Status >> FFS_ENTRY_COMPIP_SHIFT);
	 break;
      }
            
      cur = entry->PrimaryPtr;
      offset = 0;
      while (readlen > 0)
      {
	 unsigned long length = 0;
	 
	 struct ffs2_fileinfo *extent = (struct ffs2_fileinfo *)ffs2_find_entry(&r,cur);
	 if (extent == 0)
	    break;
	 
	 if (!isflagset(extent->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEEXTENT))
	    break;
	 
	 // Skip cur to the next one
	 if (isFNULL(extent->PrimaryPtr) == 0 &&
	     (extent->Status & FFS_ENTRY_PRIMARY) != FFS_ENTRY_PRIMARY)
	    cur = extent->PrimaryPtr;
	 else
	    cur = 0xFFFFFFFF;
	 
	 if ((extent->Status & FFS_ENTRY_EXISTS) != FFS_ENTRY_EXISTS)
	    continue;
	 
	 // See if we have entered the copy region
	 length = extent->UncompressedExtentLen;
	 if (offset + extent->UncompressedExtentLen > page->offset)
	 {
	    unsigned long toread = extent->UncompressedExtentLen;
	    int res;

	    if (toread > readlen)
	       toread = readlen;
	    
	    if (page->offset >= offset)
	       res = ffs2_copy_to_buff(&r,buf,extent,toread,page->offset - offset);
	    else
	       res = ffs2_copy_to_buff(&r,buf + offset - page->offset,extent,toread,0);
	    if (res != 0)
	       break;
	    readlen -= toread;
	 }

	 if (length == 0)
	    break;
	 
	 offset += length;
	 if (cur == 0xFFFFFFFF)
	    break;
      }
      
      break;
   }
   
   ffs2_relse(&r);

   if (readlen == 0)
   {
      set_bit(PG_uptodate, &page->flags);
      result = 0;
   }      
   else
   {      
      set_bit(PG_error, &page->flags);
      memset((void *)buf, 0, PAGE_SIZE);
   }
      
   clear_bit(PG_locked, &page->flags);
   wake_up(&page->wait);
   free_page((unsigned long)buf);
   
   return result;
}
									/*}}}*/
// ffs2_readlink - Read a symlink					/*{{{*/
// ---------------------------------------------------------------------
/* */
static int ffs2_readlink(struct dentry *dentry, char *buffer, int len)
{
   printk("readlink\n");
   return -EIO;   
}
									/*}}}*/
// ffs2_follow_link - ??						/*{{{*/
// ---------------------------------------------------------------------
/* */
static struct dentry *ffs2_follow_link(struct dentry *dentry,
					struct dentry *base,
					unsigned int follow)
{
   printk("followlink\n");
   return ERR_PTR(EAGAIN);
}
									/*}}}*/
// ffs2_read_inode - Fill in an inode					/*{{{*/
// ---------------------------------------------------------------------
/* This function fills in the information for an inode. For FFS an inode
   is defined to be the DirEntry/FileEntry for the object in question. The
   inode number is the pointer this structure. */
static void ffs2_read_inode(struct inode *i)
{
   struct ffs2_entry *entry;
   struct ffs_read r;
   unsigned long cur;
   
   memset(&r,0,sizeof(r));   
   r.super = i->i_sb;
   // Setup the super block
   //lock_super(i->i_sb);

   // Locate the real inode
   entry = ffs2_find_entry(&r,i->i_ino);
   if (entry == 0)
   {
      ffs2_relse(&r);
      return;
   }
   
   i->i_mode = 0;
   i->i_nlink = 1;    // FFS does not have a link concept
   i->i_size = 0;
   i->i_mtime = i->i_atime = i->i_ctime = 0;
   i->i_uid = i->i_gid = 0;
   i->i_op = ffs2_inoops[1];
   
   // Decide on the mode
   if (isflagset(entry->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEDIR))
   {
      i->i_op = ffs2_inoops[1];
      i->i_mode = S_IRWXUGO | S_IFDIR;
   }
   else
   {
      i->i_op = ffs2_inoops[2];
      // i->i_mode = (S_IRWXUGO & (~S_IXUGO)) | S_IFREG;
      i->i_mode = S_IRWXUGO | S_IFREG;
   }
   
   // Compute the size and locate the date/time
   if (!isFNULL(entry->PrimaryPtr) &&
       (entry->Status & FFS_ENTRY_PRIMARY) != FFS_ENTRY_PRIMARY)
   {
      cur = entry->PrimaryPtr;
      while (1)
      {
	 struct ffs2_entry *extent = ffs2_find_entry(&r,cur);
	 if (extent == 0)
	 {
	    ffs2_relse(&r);
	    return;
	 }
	 
	 /* Grab the correct time. Technically the spec says this should use
	    the stupid DOS format, but QNX's File System stores a sensible
	    unix date in here, so we use that instead. Maybe make this an
	    option, the fat code in linux has the decoding routines. */
	 if ((extent->Status & FFS_ENTRY_TIME) == FFS_ENTRY_TIME)
	    i->i_mtime = i->i_atime = i->i_ctime = extent->Time + (extent->Date << 16);

	 /* Add in the size and step to the next entry. For some crazed
	    reason the meanings of primary+sibling swap for entries :< */
	 if (isflagset(extent->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEEXTENT))
	 {
	    i->i_size += ((struct ffs2_fileinfo *)extent)->UncompressedExtentLen;
	    // Skip to the next one
	    if (isFNULL(extent->PrimaryPtr) == 0 &&
		(extent->Status & FFS_ENTRY_PRIMARY) != FFS_ENTRY_PRIMARY)
	       cur = extent->PrimaryPtr;
	    else
	       break;
	 }	 
	 else
	 {
	    i->i_size += sizeof(*extent) + extent->NameLen;
	 
	    // Skip to the next one
	    if (isFNULL(extent->SiblingPtr) == 0 &&
		(extent->Status & FFS_ENTRY_SIBLING) != FFS_ENTRY_SIBLING)
	       cur = extent->SiblingPtr;
	    else
	       break;
	 }	 
      }
   }
   
   ffs2_relse(&r);
}
									/*}}}*/

// ffs2_mkdir - Make a new directory					/*{{{*/
// ---------------------------------------------------------------------
/* */
static int ffs2_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
   return ffs2_new_entry(dir,dentry,&dentry->d_name,FFS_ENTRY_TYPEDIR);
}
									/*}}}*/
// ffs2_create - Create a new file					/*{{{*/
// ---------------------------------------------------------------------
/* */
static int ffs2_create(struct inode *dir,struct dentry *dentry,int mode)
{
   return ffs2_new_entry(dir,dentry,&dentry->d_name,FFS_ENTRY_TYPEFILE);
}
									/*}}}*/
// ffs2_unlink - Unlink a file						/*{{{*/
// ---------------------------------------------------------------------
/* This does a very unsafe unlink operation. It goes over the linked list
   of extents and marks them all as erased. If this routine is interrupted
   the filesystem will have unreclaimable blocks but will still be 
   consistent */
static int ffs2_unlink(struct inode *dir,struct dentry *dentry)
{
   struct ffs_read r;
   struct ffs2_entry *entry;
   struct ffs2_blockalloc alloc;
   unsigned long loc;
   int res;
   
   memset(&r,0,sizeof(r));   
   r.super = dir->i_sb;
   
   // Locate the entry in the directory
   res = ffs2_find_dirent(&r,dir->i_ino,&dentry->d_name,&loc);
   if (res != 0 || ffs2_find_blockalloc(&r,loc,&alloc) != 0)
   {
      ffs2_relse(&r);
      if (res == -1)
	 return -ENOENT;
      return -EIO;
   }
   entry = (struct ffs2_entry *)r.p;
   
   // Dont unlink directories, use rmdir for that
   if (isflagset(entry->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEDIR))
   {
      ffs2_relse(&r);
      return -EISDIR;
   }

   if (ffs2_erase(&r,loc,0) != 0)
   {
      ffs2_relse(&r);
      return -EIO;
   }
   
   ffs2_relse(&r);
   return 0;
}
									/*}}}*/
// ffs2_rmdir - Remove a directory					/*{{{*/
// ---------------------------------------------------------------------
/* This is similar to ffs2_unlink, except that it checks to make sure the
   directory is empty before trying to erase */
static int ffs2_rmdir(struct inode *dir,struct dentry *dentry)
{
   struct ffs_read r;
   struct ffs2_entry *entry;
   unsigned long loc;
   unsigned long jnk;
   int res;
   
   memset(&r,0,sizeof(r));   
   r.super = dir->i_sb;
   
   // Locate the entry in the directory
   res = ffs2_find_dirent(&r,dir->i_ino,&dentry->d_name,&loc);
   if (res != 0 || ffs2_find_blockalloc(&r,loc,0) != 0)
   {
      ffs2_relse(&r);
      if (res == -1)
	 return -ENOENT;
      return -EIO;
   }
   entry = (struct ffs2_entry *)r.p;

   // Dont unlink a file, use rmdir for that
   if (!isflagset(entry->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEDIR))
   {
      ffs2_relse(&r);
      return -ENOTDIR;
   }

   // Check for emptyness
   if (ffs2_find_dirent(&r,loc,0,&jnk) == 0)
   {
      ffs2_relse(&r);
      return -ENOTEMPTY;
   }
   
   if (ffs2_erase(&r,loc,1) != 0)
   {
      ffs2_relse(&r);
      return -EIO;
   }
		     
   ffs2_relse(&r);
   return 0;
}
									/*}}}*/
// ffs2_rename - Rename a file 						/*{{{*/
// ---------------------------------------------------------------------
/* */
static int ffs2_rename(struct inode *old_dir, struct dentry *old_dentry,
		       struct inode *new_dir, struct dentry *new_dentry)
{
   return -ENOENT;
}
									/*}}}*/
// ffs2_file_write - Write a file					/*{{{*/
// ---------------------------------------------------------------------
/* */
static ssize_t ffs2_file_write(struct file *filp,const char *buf,
			       size_t count,loff_t *ppos)
{
   struct inode *inode = filp->f_dentry->d_inode;
   off_t pos;
   ssize_t written;
   char *tmp;
   struct ffs2_entry *entry;
   struct ffs_read r;
   unsigned long cur;
   unsigned long offset;
   ssize_t res = 0;
   
   memset(&r,0,sizeof(r));   
   r.super = inode->i_sb;

   // Make sure the inode is sensible
   if (!inode || !S_ISREG(inode->i_mode))
      return -EINVAL;

   // Get the writing position
   if (filp->f_flags & O_APPEND)
      pos = inode->i_size;
   else
      pos = *ppos;
   written = 0;

   // Get the inode and follow to find the first extent
   if ((entry = ffs2_find_entry(&r,inode->i_ino)) == 0 ||
       isFNULL(entry->PrimaryPtr) ||
       (entry->Status & FFS_ENTRY_PRIMARY) == FFS_ENTRY_PRIMARY)
   {
      cur = inode->i_ino;
   }
   else
   {
      cur = entry->PrimaryPtr;
   }
   
   // Check for compression (this isnt quite right..)
   if ((entry->Status >> FFS_ENTRY_COMPIP_SHIFT) != 0xFF)
   {
      printk("ffs2: No support for compressed foramt %x\n",
	     entry->Status >> FFS_ENTRY_COMPIP_SHIFT);
      ffs2_relse(&r);
      return -EINVAL;
   }
   
   /* Search for the first block that overlaps this block, or the end of
      the list */
   offset = 0;
   written = 0;
   tmp = kmalloc(PAGE_SIZE,GFP_KERNEL);
   while (written < count)
   {
      unsigned long oldcur = cur;
      struct ffs2_fileinfo *extent = (struct ffs2_fileinfo *)ffs2_find_entry(&r,cur);
      if (extent == 0)
	 break;

      // This is invoked if there is no extents at all
      if (cur != inode->i_ino)
      {
	 if (!isflagset(extent->Status,FFS_ENTRY_TYPEMASK,FFS_ENTRY_TYPEEXTENT))
	    break;
	 
	 // Skip cur to the next one
	 if (isFNULL(extent->PrimaryPtr) == 0 &&
	     (extent->Status & FFS_ENTRY_PRIMARY) != FFS_ENTRY_PRIMARY)
	    cur = extent->PrimaryPtr;
	 else
	    cur = 0xFFFFFFFF;
	 
	 if ((extent->Status & FFS_ENTRY_EXISTS) != FFS_ENTRY_EXISTS)
	    continue;
	 
	 // See if we have entered the copy region
	 if (offset + extent->UncompressedExtentLen > pos)
	 {
	    printk("ffs2: No support for overwriting\n");
	    res = -EINVAL;
	    break;
	 }	 
      }
      else
	 cur = 0xFFFFFFFF;
      
      // Check if the end of this block is the start of the insertion region
      if (cur == 0xFFFFFFFF && offset + extent->UncompressedExtentLen >= pos)
      {	 
	 /* Allocate a new block, write the data and link it to the 
	    last block, repeatedly growing the length of the chain */
	 while (written < count)
	 {
	    unsigned long ext;
	    ssize_t c = count - written;
	    if (c > PAGE_SIZE)
	       c = PAGE_SIZE;

	    if (copy_from_user(tmp,buf,c) != 0)
	    {
	       res = -EFAULT;
	       break;
	    }
	    
	    written += c;
	    if (ffs2_new_extent(&r,tmp,c,&ext) != 0)
	    {
	       res = -EIO;
	       break;
	    }
	    
	    res = update_pointer(&r,oldcur,ext,FFS_PTR_PRIMARY);
	    if (res != 0)
	       break;
	    
	    update_vm_cache(inode,pos,tmp,c);
	    oldcur = ext;
	    pos += c;
	    offset += c;
	 }
	 break;
      }
      
      offset += ((struct ffs2_fileinfo *)extent)->UncompressedExtentLen;
      if (cur == 0xFFFFFFFF)
      {
	 printk("ffs2: Sparse files not supported\n");
	 res = -EINVAL;
	 break;
      }      
   }
   
   kfree(tmp);
   ffs2_relse(&r);
   
   if (res != 0)
      return res;
   *ppos = pos;
   if (pos > inode->i_size)
      inode->i_size = pos;   
   return written;
}
									/*}}}*/

// Kernel Binding							/*{{{*/
static struct file_operations ffs2_file_operations =
{
   NULL,			/* lseek - default */
   generic_file_read,		/* read */
   ffs2_file_write,		/* write */
   NULL,			/* readdir */
   NULL,			/* poll - default */
   NULL,			/* ioctl */
   generic_file_mmap,		/* mmap */
   NULL,			/* open */
   NULL,			/* flush */
   NULL,			/* release */
   NULL,			/* fsync */
   NULL,			/* fasync */
   NULL,			/* check_media_change */
   NULL				/* revalidate */
};

static struct inode_operations ffs2_file_inode_operations =
{
   &ffs2_file_operations,
   NULL,			/* create */
   NULL,			/* lookup */
   NULL,			/* link */
   NULL,			/* unlink */
   NULL,			/* symlink */
   NULL,			/* mkdir */
   NULL,			/* rmdir */
   NULL,			/* mknod */
   NULL,			/* rename */
   NULL,			/* readlink */
   NULL,			/* follow_link */
   ffs2_readpage,		/* readpage */
   NULL,			/* writepage */
   NULL,			/* bmap -- not really */
   NULL,			/* truncate */
   NULL,			/* permission */
   NULL,			/* smap */
};

static struct file_operations ffs2_dir_operations =
{
   NULL,			/* lseek - default */
   NULL,			/* read */
   NULL,			/* write - bad */
   ffs2_readdir,		/* readdir */
   NULL,			/* poll - default */
   NULL,			/* ioctl */
   NULL,			/* mmap */
   NULL,			/* open */
   NULL,			/* flush */
   NULL,			/* release */
   NULL,			/* fsync */
   NULL,			/* fasync */
   NULL,			/* check_media_change */
   NULL				/* revalidate */
};

static struct inode_operations ffs2_dir_inode_operations =
{
   &ffs2_dir_operations,
   ffs2_create,			/* create */
   ffs2_lookup,			/* lookup */
   NULL,			/* link */
   ffs2_unlink,			/* unlink */
   NULL,			/* symlink */
   ffs2_mkdir,			/* mkdir */
   ffs2_rmdir,			/* rmdir */
   NULL,			/* mknod */
   ffs2_rename,			/* rename */
   NULL,			/* readlink */
   NULL,			/* follow_link */
   NULL,			/* readpage */
   NULL,			/* writepage */
   NULL,			/* bmap */
   NULL,			/* truncate */
   NULL,			/* permission */
   NULL,			/* smap */
};

static struct inode_operations ffs2_link_inode_operations =
{
   NULL,			/* no file operations on symlinks */
   NULL,			/* create */
   NULL,			/* lookup */
   NULL,			/* link */
   NULL,			/* unlink */
   NULL,			/* symlink */
   NULL,			/* mkdir */
   NULL,			/* rmdir */
   NULL,			/* mknod */
   NULL,			/* rename */
   ffs2_readlink,		/* readlink */
   ffs2_follow_link,		/* follow_link */
   NULL,			/* readpage */
   NULL,			/* writepage */
   NULL,			/* bmap */
   NULL,			/* truncate */
   NULL,			/* permission */
   NULL,			/* smap */
};

static struct inode_operations *ffs2_inoops[] =
{
   NULL,			/* hardlink, handled elsewhere */
   &ffs2_dir_inode_operations,
   &ffs2_file_inode_operations,
   &ffs2_link_inode_operations,
   &blkdev_inode_operations,	/* standard handlers */
   &chrdev_inode_operations,
   NULL,			/* socket */
   NULL,			/* fifo */
};

static struct super_operations ffs2_ops =
{
   ffs2_read_inode,		/* read inode */
   NULL,			/* write inode */
   NULL,			/* put inode */
   NULL,			/* delete inode */
   NULL,			/* notify change */
   ffs2_put_super,		/* put super */
   NULL,			/* write super */
   ffs2_statfs,		/* statfs */
   NULL				/* remount */
};

static struct file_system_type ffs2_fs_type =
{
   "ffs2",
   FS_REQUIRES_DEV,
   ffs2_read_super,
   NULL
};
									/*}}}*/
// Init Stuff								/*{{{*/
int __init init_ffs2_fs()
{
   return register_filesystem(&ffs2_fs_type);
}

#ifdef MODULE

EXPORT_NO_SYMBOLS;

int init_module(void)
{
   return init_ffs2_fs();
}

void cleanup_module()
{
   unregister_filesystem(&ffs2_fs_type);
}
#endif
									/*}}}*/
