/* This is used to pack YAFFS2 tags, not YAFFS1tags. */

#ifndef __YAFFS_PACKEDTAGS2_H__
#define __YAFFS_PACKEDTAGS2_H__

#include "yaffs_guts.h"
#include "yaffs_ecc.h"

typedef struct __attribute__((packed)){
	unsigned int sequenceNumber;
	unsigned int objectId;
	unsigned int chunkId;
	unsigned int byteCount;//unsigned short byteCount;
} yaffs_PackedTags2TagsPart;

typedef struct __attribute__((packed)) {
	//unsigned char badblockmark;//zswan debug it
	yaffs_PackedTags2TagsPart t;
	yaffs_ECCOther ecc;
} yaffs_PackedTags2;

void yaffs_PackTags2(yaffs_PackedTags2 * pt, const yaffs_ExtendedTags * t);
void yaffs_UnpackTags2(yaffs_ExtendedTags * t, yaffs_PackedTags2 * pt);
#endif
