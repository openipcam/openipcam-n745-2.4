// -*- mode: cpp; mode: fold -*-
// Description                                                          /*{{{*/
// $Id: ffs2_fs.h,v 1.1.1.1 2006-07-11 09:31:07 andy Exp $
/* ######################################################################

   Microsoft Flash File System 2 

   File System block structures as defined the MS ffs2 spec, names are
   taken directly from there. 
   
   The structure of the filesystem follows a linked list design, their
   are no blocking constraints like in normal FS's, structures start
   anyplace on the flash. Directories are linked lists of files and files
   are linked lists of extents, the whole thing forms a tree on the flash.

   Every structure is permanently tied to being in a single block, but 
   the offset in the block is flexable. All accesses are indirected 
   through a block allocation scheme. Inodes numbrs are assigned to the 
   pointer location values of the head of the 'extent' list. 
     
   ##################################################################### */
									/*}}} */
#ifndef __LINUX_FFS2FS_FS_H
#define __LINUX_FFS2FS_FS_H

#define FFS2_MAGIC 0x2757
#define FFS2_MAXFN 128

#include <asm/types.h>

// Probably should change this to use __attribute__ 
#pragma pack(1)

/* The boot record is hidden inside the media and pointed too by one of
   the block allocation structures. We store a copy of it in the super
   block */
struct ffs2_bootrecord
{
   __u16 Signature;
   __u32 SerialNumber;
   __u16 FFSWriteVersion;
   __u16 FFSReadVersion;
   __u16 TotalBlockCount;
   __u16 SpareBlockCount;
   __u32 BlockLen;
   __u32 RootDirectoryPtr;
   __u16 Status;
   __u16 BootCodeLen;
   __u8 BootCode[1];
};
#define FFS_SIZEOF_BOOT sizeof(struct ffs2_bootrecord)

/* Generic Entry for Files/Dirs and almost Extents. Extents are similar
   yet have a different end section */
struct ffs2_entry
{
   __u16 Status;
   __u32 SiblingPtr;
   __u32 PrimaryPtr;
   __u32 SecondaryPtr;
   __u8 Attrib;
   __u16 Time;
   __u16 Date;
   __u16 VarStructLen;
   __u8 NameLen;
   __u8 Name[1];              // Can be bigger..
};
#define FFS_SIZEOF_ENTRY 22
#define FFS_ENTRY_EXISTS     (1 << 0)
#define FFS_ENTRY_TIME       (1 << 1)
#define FFS_ENTRY_TYPEMASK   (3 << 2)
#define FFS_ENTRY_TYPEDIR    (0 << 2)
#define FFS_ENTRY_TYPEFILE   (1 << 2)
#define FFS_ENTRY_TYPEEXTENT (2 << 2)
#define FFS_ENTRY_PRIMARY    (1 << 4)
#define FFS_ENTRY_SECONDARY  (1 << 5)
#define FFS_ENTRY_SIBLING    (1 << 6)
#define FFS_ENTRY_COMPIP_SHIFT (8)

/* This is an 'extent', it contains the pointer to the raw file data, 
   foolishly they store the compression type in the file entry 
   record <grr> */
struct ffs2_fileinfo
{
   __u16 Status;
   __u32 ExtentPtr;
   __u32 PrimaryPtr;
   __u32 SecondaryPtr;
   __u8 Attrib;
   __u16 Time;
   __u16 Date;
   __u16 VarStructLen;
   __u16 UncompressedExtentLen;
   __u16 CompressedExtentLen;   
};
#define FFS_SIZEOF_FILEINFO sizeof(struct ffs2_fileinfo)

/* Allocation structure that preceeds the block structure. It provides
   for constant pointers into the segment */
struct ffs2_blockalloc
{
   __u8 Status;
   __u8 Offset[3];
   __u16 Len;
};
#define FFS_SIZEOF_BLOCKALLOC 6
#define FFS_ALLOC_EMASK       (1 << 7)
#define FFS_ALLOC_END         (1 << 7)
#define FFS_ALLOC_SMASK       (7 << 4)
#define FFS_ALLOC_FREE        (7 << 4)
#define FFS_ALLOC_ALLOCATED   (3 << 4)
#define FFS_ALLOC_DEALLOCATED (1 << 4)
#define FFS_ALLOC_NULL        0

/* The block structure placed at the end of each block */
struct ffs2_block
{
   __u32 BootRecordPtr;
   __u32 EraseCount;
   __u16 BlockSeq;
   __u16 BlockSeqChecksum;
   __u16 Status;
};
#define FFS_SIZEOF_BLOCK 14
// Bits 15-10, block state
#define FFS_STATE_MASK    (63 << 10)
#define FFS_STATE_READY   (48 << 10)
#define FFS_STATE_ERASED  (63 << 10)
#define FFS_STATE_ECOUNT  (62 << 10)
#define FFS_STATE_SPARE   (60 << 10)
#define FFS_STATE_RECLAIM (56 << 10)
#define FFS_STATE_RETIRED (0 << 10)
// Bits 9-3, reserved
#define FFS_STATE_RESERVED   127
// Bits 2-0, BootRecordPtr status
#define FFS_BOOTP_MASK    7
#define FFS_BOOTP_NONE    7
#define FFS_BOOTP_CURRENT 6
#define FFS_BOOTP_SUPERSEDED 0

#pragma pack()

#define isflagset(value,mask,expect) ((value & mask) == expect)
#define isFNULL(value) (value == 0xFFFFFFFF)

#ifdef __KERNEL__

extern int init_ffs2fs_fs(void);

#endif

#endif
