
Copyright WINBOND Limited 1999 - 2002. All rights reserved.

W90N740 boot loader
=============================

History:
========

version 1.0:	27th June 2003


1. Introduction
===============

This README introduces the firmware for the W90N740.

2. Building the Firmware
========================

General:

This firmware support the boot function for the system based on W90N740. 
There are three basic function in the firmware.
 a. System initialization code to initialize the system based on W90N740.
    If there is no boot code on the boot ROM of W90N740 deverlopment board, this initialization code can be 
    download into the internal SRAM of W90N740 to initialize the system by any ARM 7 debug tool (ex: ICE).
    The initialization code could be modified by user to fit their hardware design.

 b. W90N740 Boot Loader (WBL)
    WBL is a firmware to boot system, download program, write flash, and load the user application.
    It was designed to fit various systems based on W90N740. For more detail information, please refer to the 
    W90N740 Boot Loader User's Manual and W90N740 Boot Loader Reference Guide.

 c. Flash Library
    This library is used by boot loader to program the flash. This library could be modified by user to support 
    more flash ROM type.


Building using ARM Developer Suite 1.2:

There are three projects in this firmware. They are as follows:

 - init.mcp: The system initialization code.
 - bootloader.mcp: The W90N740 boot loader.
 - flash.mcp: The flash library



3. Supported Platforms
======================

The following platforms are supported:

 CHIP: 
	W90N740 version A, W90N740 version B

 SDRAM:
	1Mx16x2, 2Mx32, 4Mx32

 FLASH:
	AM29LV320DB	AM29LV320DT	AM29LV800BB	AM29LV800BT
	AM29LV160DB	AM29LV160DT	A29L160B	A29L160T
	SST39VF160	HY29LV160	MX28F160C3T	MX28F160C3B
	MX29LV160BT	MX29LV320AT	MX29LV320AB	MBM29LV160BE
	MBM29LV160TE	W19L320SB	W19L320ST	W28J800TT
	W28J800BT	W28J160TT	W28J160BT	W28J320TT
	W28J320BT	INTEL E28F320	INTEL E28F640	INTEL E28F128
	28F800C3-T	28F800C3-B	28F160C3-T	28F160C3-B
	28F320C3-T	28F320C3-B	W39L010		W29EE011
	
4. Revision Details
===================

version 1.1:
	Add new SDRAM type.
	Add new Flash type.
	Fix some bug.

version 1.0:
	The initial version.
	

5. Warnings
===========

See RELNOTES.TXT

