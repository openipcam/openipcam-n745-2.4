/*
 * $Log: _flflash.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:48  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.2   May 14 2002 15:02:46   oris
 * Moved the needed defintions for the Boot SDK (under MTD_STANDALONE compilation flag) from public header.
 * 
 *    Rev 1.1   03 May 2002 20:23:48   andreyk
 * typedef FLFlash definition moved from _FLFLASH.H to FLFLASH.H
 * 
 *    Rev 1.0   May 02 2002 19:58:58   oris
 * Initial revision.
 */

/*********************************************************************************** 
 *                                                                                 * 
 *                        M-Systems Confidential                                   * 
 *           Copyright (C) M-Systems Flash Disk Pioneers Ltd. 1995-2002            * 
 *                         All Rights Reserved                                     * 
 *                                                                                 * 
 *********************************************************************************** 
 *                                                                                 * 
 *                            NOTICE OF M-SYSTEMS OEM                              * 
 *                           SOFTWARE LICENSE AGREEMENT                            * 
 *                                                                                 * 
 *      THE USE OF THIS SOFTWARE IS GOVERNED BY A SEPARATE LICENSE                 * 
 *      AGREEMENT BETWEEN THE OEM AND M-SYSTEMS. REFER TO THAT AGREEMENT           * 
 *      FOR THE SPECIFIC TERMS AND CONDITIONS OF USE,                              * 
 *      OR CONTACT M-SYSTEMS FOR LICENSE ASSISTANCE:                               * 
 *      E-MAIL = info@m-sys.com                                                    * 
 *                                                                                 * 
 ***********************************************************************************/


#ifndef _FLFLASH_H
#define _FLFLASH_H


#ifdef MTD_STANDALONE

typedef struct tSocket FLSocket;

struct tSocket
{
    unsigned      volNo;   /* Volume no. of socket */
    void FAR0 *   base;    /* Pointer to window base */
    Sdword        size;    /* Window size (must by power of 2) */
};

#if (defined (VERIFY_WRITE) || defined(VERIFY_ERASE) || defined(MTD_RECONSTRUCT_BBT))
extern byte globalReadBack[SOCKETS][READ_BACK_BUFFER_SIZE];
#endif /* VERIFY_WRITE */

extern FLSocket *flSocketOf(unsigned volNo);
extern FLBuffer  globalMTDBuffer;
extern int       noOfMTDs;

/* Replacement for various TrueFFS typedefs */

typedef unsigned long CardAddress;        /* Physical offset on card */

#endif /* MTD_STANDALONE */


/*----------------------------------------------------------------------*/
/*                 Flash array identification structure                 */
/*                                                                      */
/* This structure contains a description of the Flash array and         */
/* routine pointers for the map, read, write & erase functions.         */
/*                                                                      */
/* The structure is initialized by the MTD that identifies the Flash    */
/* array.                                                               */
/* On entry to an MTD, the Flash structure contains default routines    */
/* for all operations. This routines are sufficient forread-only access */
/* to NOR Flash on a memory-mapped socket. The MTD should override the  */
/* default routines with MTD specific ones when appropriate.            */
/*----------------------------------------------------------------------*/

/* Flash array identification structure */

struct tFlash {
  FlashType type;                 /* Flash device type (JEDEC id)           */
  byte      mediaType;            /* see media types obove                  */
  byte      ppp;                  /* number of allowed PPP                  */
  dword busAccessType;            /* saves bus access type                  */
  dword maxEraseCycles;           /* erase cycles limit per erase block     */
  dword changeableProtectedAreas; /* areas capable of changing protection   */
                                  /* attribute with no danger of loosing    */
                                  /* the entire chip                        */
  byte   totalProtectedAreas;     /* total number of protection arweas      */
  dword  erasableBlockSize;       /* Smallest physically erasable size      */
                                  /* (with interleaving taken into account) */
  byte      erasableBlockSizeBits;/* Bits representing the erasable block   */
  dword     chipSize;          /* chip size                                 */
  byte      noOfFloors;        /* no of controllers in array                */
  word      pageSize;          /* size of flash page in bytes               */
  word      noOfChips;         /* no of chips in array                      */
  dword     firstUsableBlock;  /* Some devices may not use all of the media */
                               /* blocks. For example mdocplus can not use  */
                               /* the first 3 blocks.                       */
  Sword     interleaving;      /* chip interleaving (The interleaving is    */
                               /* defined as the address difference between */
                               /* two consecutive bytes on a chip)          */
  word      flags;             /* Special capabilities & options Bits 0-7   */
                               /* may be used by FLite. Bits 8-15 are not   */
                               /* used bt FLite and may beused by MTD's for */
                               /* MTD-specific purposes.                    */
  void *    mtdVars;           /* Points to MTD private area for this socket.*/
                               /* This field, if used by the MTD, is         */
                               /* initialized bythe MTD identification       */
                               /* routine.                                   */
  FLSocket * socket;           /* Socket of this drive. Note that 2 diffrent */
                               /* records are used. One for OSAK and the     */
                               /* other forstandalone applications.          */
  NDOC2window win;             /* DiskOnChip memory windows                  */

/*----------------------------------------------------------------------*/
/*                        f l a s h . m a p                             */
/*                                                                      */
/* MTD specific map routine                                             */
/*                                                                      */
/* The default routine maps by socket mapping, and is suitable for all  */
/* NOR Flash.                                                           */
/* NAND or other type Flash should use map-through-copy emulation: Read */
/* a block of Flash to an internal buffer and return a pointer to that  */
/* buffer.                                                              */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      address            : Card address to map                        */
/*      length             : Length to map                              */
/*                                                                      */
/* Returns:                                                             */
/*        Pointer to required card address                              */
/*----------------------------------------------------------------------*/
  void FAR0 * (*map)(FLFlash *, CardAddress, int);

/*----------------------------------------------------------------------*/
/*                        f l a s h . r e a d                           */
/*                                                                      */
/* MTD specific Flash read routine                                      */
/*                                                                      */
/* The default routine reads by copying from a mapped window, and is    */
/* suitable for all NOR Flash.                                          */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      address            : Card address to read                       */
/*      buffer             : Area to read into                          */
/*      length             : Length to read                             */
/*      modes              : See write mode flags definition above      */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*read)(FLFlash *, CardAddress, void FAR1 *, dword, word);

/*----------------------------------------------------------------------*/
/*                       f l a s h . w r i t e                          */
/*                                                                      */
/* MTD specific Flash write routine                                     */
/*                                                                      */
/* The default routine returns a write-protect error.                   */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      address            : Card address to write to                   */
/*      buffer             : Address of data to write                   */
/*      length             : Number of bytes to write                   */
/*      modes              : See write mode flags definition above      */
/*                                                                      */
/* Returns:                                                             */
/*        FLStatus        : 0 on success, failed otherwise              */
/*----------------------------------------------------------------------*/
  FLStatus (*write)(FLFlash *, CardAddress, const void FAR1 *, dword, word);

/*----------------------------------------------------------------------*/
/*                       f l a s h . e r a s e                          */
/*                                                                      */
/* Erase one or more contiguous Flash erasable blocks                   */
/*                                                                      */
/* The default routine returns a write-protect error.                   */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                 : Pointer identifying drive                 */
/*      firstErasableBlock  : Number of first block to erase            */
/*      numOfErasableBlocks : Number of blocks to erase                 */
/*                                                                      */
/* Returns:                                                             */
/*        FLStatus        : 0 on success, failed otherwise              */
/*----------------------------------------------------------------------*/
  FLStatus (*erase)(FLFlash *, word, word);

/*----------------------------------------------------------------------*/
/*               f l a s h . s e t P o w e r O n C a l l b a c k        */
/*                                                                      */
/* Register power on callback routine. Default: no routine is           */
/* registered.                                                          */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*                                                                      */
/*----------------------------------------------------------------------*/
  void (*setPowerOnCallback)(FLFlash *);

/*----------------------------------------------------------------------*/
/*                        f l a s h . r e a d B B T                     */
/*                                                                      */
/* MTD specific Flash routine returning the media units status          */
/* Note that a unit can contain more then 1 erase block                 */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      unitNo             : Number of the first unit to check          */
/*      unitsToRead        : Number of units to check                   */
/*      blockMultiplier    : Number of blocks per erase unit            */
/*      buffer             : Buffer to return the units status          */
/*      reconstruct        : TRUE for reconstruct BBT from virgin card  */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*readBBT)(FLFlash *, dword unitNo, dword unitsToRead,
              byte blockMultiplier,byte FAR1 * buffer, FLBoolean reconstruct);

/*----------------------------------------------------------------------*/
/*                    f l a s h . w r i t e I P L                       */
/*                                                                      */
/* MTD specific Flash write IPL area routine                            */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      buffer             : Buffer containing the data to write        */
/*      length             : Length to write                            */
/*      flags              : Flags of write IPL operation (see obove)   */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*writeIPL)(FLFlash *, const void FAR1 * buffer, word length, 
                       byte offset , unsigned flags);
/*----------------------------------------------------------------------*/
/*                     f l a s h . r e a d I P L                        */
/*                                                                      */
/* MTD specific Flash read area IPL routine                             */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      buffer             : Area to read into                          */
/*      length             : Length to read                             */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*readIPL)(FLFlash *, void FAR1 * buffer, word length);

#ifdef HW_OTP

/*----------------------------------------------------------------------*/
/*                        f l a s h . w r i t e O T P                   */
/*                                                                      */
/* MTD specific Flash write and lock OTP area routine                   */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      buffer             : buffer containing the data to write        */
/*      length             : Length to write                            */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*writeOTP)(FLFlash *, const void FAR1 * buffer,word length);

/*----------------------------------------------------------------------*/
/*                        f l a s h . r e a d O T P                     */
/*                                                                      */
/* MTD specific Flash read OTP area routine                             */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      offset             : Offset from the begining of the OTP arae   */
/*      buffer             : Area to read into                          */
/*      length             : Length to read                             */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*readOTP)(FLFlash *, word offset, void FAR1 * buffer, word length);

/*----------------------------------------------------------------------*/
/*                        f l a s h . otpSize                           */
/*                                                                      */
/* MTD specific Flash get OTP area size and state                       */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      sectionSize        : total size of the OTP area                 */
/*      usedSize           : Used (and locked) size of the OTP area     */
/*      locked             : LOCKED_OTP flag stating the locked state   */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*otpSize)(FLFlash *,  dword FAR2* sectionSize,
             dword FAR2* usedSize, word FAR2* locked);
#endif /* HW_OTP */
/*----------------------------------------------------------------------*/
/*                  f l a s h . g e t U n i q u e I d                   */
/*                                                                      */
/* MTD specific Flash get the chip unique ID                            */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      buffer             : byte buffer to read unique ID into         */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*getUniqueId)(FLFlash *, void FAR1 * buffer);
#ifdef  HW_PROTECTION
/*----------------------------------------------------------------------*/
/*        f l a s h . p r o t e c t i o n B o u n d r i e s             */
/*                                                                      */
/* MTD specific Flash get protection boundries  routine                 */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      areaNo             : Protection area number to work on          */
/*      addressLow         : Low boundary Address of protected area     */
/*      addressHigh        : High boundary Address of protected area    */
/*      floorNo            : The floor to work on.                      */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*protectionBoundries)(FLFlash *, byte areaNo,
            CardAddress* addressLow ,CardAddress* addressHigh, byte floorNo);

/*----------------------------------------------------------------------*/
/*        f l a s h . p r o t e c t i o n K e y I n s e r t             */
/*                                                                      */
/* MTD specific Flash insert the protection key routine                 */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Note the key is inserted only to protected areas and to all floors   */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      areaNo             : Protection area number to work on          */
/*      key                : protection key buffer                      */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*protectionKeyInsert)(FLFlash *, byte areaNo, byte FAR1* key);

/*----------------------------------------------------------------------*/
/*        f l a s h . p r o t e c t i o n K e y R e m o v e             */
/*                                                                      */
/* MTD specific Flash remove the protection key routine                 */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Note the key is removed from all floors.                             */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      areaNo             : Protection area number to work on          */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*protectionKeyRemove)(FLFlash *,byte areaNo);

/*----------------------------------------------------------------------*/
/*        f l a s h . p r o t e c t i o n T y p e                       */
/*                                                                      */
/* MTD specific Flash get protection type routine                       */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Note the type is the combined attributes of all the floors.          */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      areaNo             : Protection area number to work on          */
/*      areaType           : returnining the protection type            */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*protectionType)(FLFlash *,byte areaNo, word* areaType);

/*----------------------------------------------------------------------*/
/*              f l a s h . p r o t e c t i o n S e t                   */
/*                                                                      */
/* MTD specific Flash get protection type routine                       */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol           : Pointer identifying drive                       */
/*      areaNo        : Protection area number to work on               */
/*      areaType      : Protection area type                            */
/*      addressLow    : Low boundary Address of protected area          */
/*      addressHigh   : High boundary Address of protected area         */
/*      key           : protection key buffer                           */
/*      modes         : Either COMMIT_PROTECTION will cause the new     */
/*                      values to take affect immidiatly or             */
/*                      DO_NOT_COMMIT_PROTECTION for delaying the new   */
/*                      values to take affect only after the next reset.*/
/*      floorNo       : The floor to work on.                           */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*protectionSet )( FLFlash *,byte areaNo, word areaType ,
        CardAddress addressLow, CardAddress addressHigh,
            byte FAR1* key , byte modes , byte floorNo);

#endif /* HW_PROTECTION */

/*----------------------------------------------------------------------*/
/*      f l a s h . e n t e r D e e p P o w e r D o w n M o d e         */
/*                                                                      */
/* MTD specific Flash enter deep power down mode routine                */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*      state              : DEEP_POWER_DOWN                            */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*enterDeepPowerDownMode)(FLFlash *,word state);

/*----------------------------------------------------------------------*/
/*                    f l a s h . d o w n l o a d                       */
/*                                                                      */
/* MTD specific - Reset download mechanizm to download IPL and          */
/*                protection attributes.                                */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*                                                                      */
/*----------------------------------------------------------------------*/
  FLStatus (*download)(FLFlash *);

/*----------------------------------------------------------------------*/
/* DiskOnChip memory access routines type defintions                    */
/*----------------------------------------------------------------------*/

#ifndef FL_NO_USE_FUNC
  FLMemWindowSize FAR1* memWindowSize; /* Doc memory window size          */
  FLMemRead       FAR1* memRead;       /* Doc memory read routine         */
  FLMemWrite      FAR1* memWrite;      /* Doc memory write routine        */
  FLMemSet        FAR1* memSet;        /* Doc memory set routine          */
  FLMemRead8bit   FAR1* memRead8bit;   /* Doc memory 8 bit read routine   */
  FLMemWrite8bit  FAR1* memWrite8bit;  /* Doc memory 8 bit write routine  */
  FLMemRead16bit  FAR1* memRead16bit;  /* Doc memory 16 bit read routine  */
  FLMemWrite16bit FAR1* memWrite16bit; /* Doc memory 16 bit write routine */
  FLMemSetGetMode FAR1* memSetGetMode; /* Interleave change event -       */
                                       /* call back to plant new routines */
#endif /* FL_NO_USE_FUNC */
};

typedef FLStatus (*MTDidentifyRoutine) (FLFlash *);

extern MTDidentifyRoutine mtdTable[MTDS];

/* Returns specific flash structure of the socket */

extern FLFlash * flFlashOf(unsigned volNo);

#ifdef MTD_STANDALONE
typedef FLStatus (*SOCKETidentifyRoutine) (FLSocket * ,
          dword lowAddress, dword highAddress);
typedef void     (*FREEmtd) (FLSocket vol);

extern SOCKETidentifyRoutine socketTable[MTDS];
extern FREEmtd               freeTable[MTDS];

#else

/* The address of this, if returned from map, denotes a data error */

extern FLStatus dataErrorObject;

#define dataErrorToken ((void FAR0 *) &dataErrorObject)

/* See interface documentation of functions in flflash.c        */

extern void flIntelIdentify(FLFlash *,
                void (*)(FLFlash *, CardAddress, byte, FlashPTR),
                CardAddress);

extern FLStatus        flIntelSize(FLFlash *,
                void (*)(FLFlash *, CardAddress, byte, FlashPTR),
                CardAddress);

extern FLStatus        flIdentifyFlash(FLSocket *socket, FLFlash *flash);

#endif /* MTD_STANDALONE */
/*----------------------------------------------------------------------*/
/*              f l a s h . r e s e t I n t e r r u p t                 */
/*                                                                      */
/* MTD specific Flash reset the interrupt signal routine                */
/*                                                                      */
/* No default routine is implemented for this routine.                  */
/*                                                                      */
/* Parameters:                                                          */
/*      vol                : Pointer identifying drive                  */
/*                                                                      */
/*----------------------------------------------------------------------*/
/*void (*resetInterrupt)(FLFlash vol); */

#endif /* _FLFLASH_H */

