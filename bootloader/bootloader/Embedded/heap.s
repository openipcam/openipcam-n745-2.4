;/******************************************************************************
; *
; * Copyright (c) 2003 Windond Electronics Corp.
; * All rights reserved.
; *
; * $Workfile: heap.s $
; *
; * $Author: Yachen $
; ******************************************************************************/
;/*
; * $History: heap.s $
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/10   Time: 10:55a
; Created in $/W90P710/Applications/710bootloader/Embedded
; 710 Bootloader, without USB support
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/04   Time: 2:27p
; Created in $/W90P710/Module Test Programs/FIRMWARE_710/Embedded
; Module test bootloader, removed decompress function in order to save
; memory space for LCD control
; 
; *****************  Version 2  *****************
; User: Wschang0     Date: 03/08/20   Time: 1:22p
; Updated in $/W90P710/FIRMWARE/Embedded
; Add VSS header
; */

        AREA    Heap, DATA, NOINIT

        EXPORT bottom_of_heap

; Create dummy variable used to locate bottom of heap

bottom_of_heap    SPACE   1

        END
