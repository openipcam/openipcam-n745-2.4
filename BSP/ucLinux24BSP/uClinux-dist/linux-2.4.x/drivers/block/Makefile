#
# Makefile for the kernel block device drivers.
#
# 12 June 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
# 
# Note : at this point, these files are compiled on all systems.
# In the future, some of these should be built conditionally.
#

O_TARGET := block.o

export-objs	:= ll_rw_blk.o blkpg.o loop.o DAC960.o genhd.o

obj-y	:= ll_rw_blk.o blkpg.o genhd.o elevator.o

obj-$(CONFIG_MAC_FLOPPY)	+= swim3.o
obj-$(CONFIG_BLK_DEV_FD)	+= floppy.o
obj-$(CONFIG_AMIGA_FLOPPY)	+= amiflop.o
obj-$(CONFIG_ATARI_FLOPPY)	+= ataflop.o
obj-$(CONFIG_BLK_DEV_SWIM_IOP)	+= swim_iop.o
obj-$(CONFIG_ATARI_ACSI)	+= acsi.o
obj-$(CONFIG_ATARI_SLM)		+= acsi_slm.o
obj-$(CONFIG_AMIGA_Z2RAM)	+= z2ram.o
obj-$(CONFIG_BLK_DEV_BLKMEM)+= blkmem.o
obj-$(CONFIG_BLK_DEV_RAM)	+= rd.o
obj-$(CONFIG_BLK_DEV_LOOP)	+= loop.o
obj-$(CONFIG_BLK_DEV_PS2)	+= ps2esdi.o
obj-$(CONFIG_BLK_DEV_XD)	+= xd.o
obj-$(CONFIG_BLK_CPQ_DA)	+= cpqarray.o
obj-$(CONFIG_BLK_CPQ_CISS_DA)  += cciss.o
obj-$(CONFIG_BLK_DEV_DAC960)	+= DAC960.o
obj-$(CONFIG_BLK_DEV_UMEM)	+= umem.o
obj-$(CONFIG_BLK_DEV_NBD)	+= nbd.o
obj-$(CONFIG_WBFLASH)	+= flash/wbflash.o
obj-y+= export.o

subdir-$(CONFIG_PARIDE) += paride
subdir-$(CONFIG_WBFLASH) += flash

ifeq ($(CONFIG_ARCH_ACORN),y)
mod-subdirs	+= ../acorn/block
subdir-y	+= ../acorn/block
obj-y		+= ../acorn/block/acorn-block.o
endif

subdir-$(CONFIG_BLK_DEV_MSYS_DOC) += doc
obj-$(CONFIG_BLK_DEV_MSYS_DOC) += doc/doc.o

include $(TOPDIR)/Rules.make
