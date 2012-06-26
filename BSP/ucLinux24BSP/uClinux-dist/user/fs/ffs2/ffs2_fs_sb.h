// -*- mode: cpp; mode: fold -*-
// Description                                                          /*{{{*/
// $Id: ffs2_fs_sb.h,v 1.1.1.1 2006-07-11 09:31:07 andy Exp $
/* ######################################################################

   Microsoft Flash File System 2
      
   The super block stores some misc information that is occasionally 
   needed when reading/writing the FS.
   
   ##################################################################### */
									/*}}} */
#ifndef __LINUX_FFS2FS_FS_SB_H
#define __LINUX_FFS2FS_FS_SB_H

#include "ffs2_fs.h"

enum ffs2_block_state {ffs2_ready, ffs2_erased, ffs2_ecount, ffs2_spare,
                       ffs2_reclaim, ffs2_retired};

struct ffs2_free_space
{
   unsigned long Start;
   unsigned long Stop;
};

// Stored information about each block
struct ffs2_sb_block
{
   unsigned long FreeSpace;
   unsigned long ReclaimableSpace;
   unsigned long LargestSpace;
   struct ffs2_free_space *FreeList;   
   enum ffs2_block_state Sate;
   unsigned short VirtualBlock;
};

// Superblock in-core data
struct ffs2_sb_info
{
   unsigned long EraseSize;      // The block size in bytes
   unsigned long ZeroBlock;      /* The first flash block in the file system 
                                    (hack to advoid needing paritions sometimes) */
   struct ffs2_bootrecord Boot;
   unsigned short *BlockMap;     // Mapping of virtual blocks to real blocks
   struct ffs2_sb_block *Blocks; // boot.TotalBlocks long array
};

#endif
