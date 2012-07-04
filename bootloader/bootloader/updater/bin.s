;/******************************************************************************
; *
; * Copyright (c) 2003 Windond Electronics Corp.
; * All rights reserved.
; *
; * $Workfile: bin.s $
; *
; * $Author: Yachen $
; ******************************************************************************/
;/*
; * $History: bin.s $
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/10   Time: 10:55a
; Created in $/W90P710/Applications/710bootloader/updater
; 710 Bootloader, without USB support
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/04   Time: 2:27p
; Created in $/W90P710/Module Test Programs/FIRMWARE_710/updater
; Module test bootloader, removed decompress function in order to save
; memory space for LCD control
; 
; *****************  Version 4  *****************
; User: Wschang0     Date: 04/03/03   Time: 1:44p
; Updated in $/W90P710/FIRMWARE/updater
; remove bootrom image
; add bootrom update check
; 
; *****************  Version 3  *****************
; User: Wschang0     Date: 03/08/27   Time: 11:32a
; Updated in $/W90P710/FIRMWARE/updater
; Modify the source path to VSSWORK
; 
; *****************  Version 2  *****************
; User: Wschang0     Date: 03/08/20   Time: 1:33p
; Updated in $/W90P710/FIRMWARE/updater
; Add VSS header
; */

	EXPORT	bootrom_entry
	EXPORT	bootrom_tail
	EXPORT	bootloader_entry
	EXPORT	bootloader_tail

	AREA bin_section, CODE, READONLY
bootrom_entry
 IF {ENDIAN} = "little"
;	INCBIN	..\WBRv1_1\image\bootrom\little\bootrom.bin
 ELSE
;	INCBIN	..\WBRv1_1\image\bootrom\big\bootrom.bin
 ENDIF
bootrom_tail	



bootloader_entry
 IF {ENDIAN} = "little"
	INCBIN	..\WBLv1_1\image\bootloader\little\bootloader.bin
;	INCBIN  E:\image\bootloader\little\bootloader.bin
 ELSE
	INCBIN	..\WBLv1_1\image\bootloader\big\bootloader.bin
;	INCBIN  E:\image\bootloader\big\bootloader.bin
 ENDIF
bootloader_tail

	
	END