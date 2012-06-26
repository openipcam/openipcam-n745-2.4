/*
 * $Log: idedoc.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:44  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 */
/*-------------------------------------------------------------------*/
#ifndef _IDEDOC_H_
#define _IDEDOC_H_

/* #include <unistd.h> */
#include <asm/io.h>

#include "flstatus.h"

#define far

/* #include <dos.h> */
/*---------------------------------------------------------*/
typedef unsigned short  FLWORD;
typedef unsigned char   FLBYTE;
typedef unsigned long   FLDWORD;
/*---------------------------------------------------------*/
#define LOBYTE(w)     (FLBYTE)(w)
#define HIBYTE(w)     (FLBYTE)((FLWORD)(w) >> 8)
#define LOWORD(l)     (FLWORD)(l)
#define HIWORD(l)     (FLWORD)((FLDWORD)(l) >> 16)
#define MKDWORD(h,l)  (FLDWORD)(((FLWORD)l) | (((FLDWORD)((FLWORD)h)) << 16))
#define MKWORD(h,l)   (FLWORD)((FLBYTE)l | ((FLWORD)((FLBYTE)h) << 8))

/*------ IDE Ports ----------------------------------------*/
#define IDE_PORT_PRM  0x1F0
#define IDE_PORT_SEC  0x170
/*------ Registers of IDE ---------------------------------*/
#ifdef FL_IDE_DOC
#define I_READ    3
#define I_ADDR    4
#else
#define I_READ    0
#define I_ADDR    2
#define NflashData    0x1028
#define Nio           0x0800
#endif /* FL_IDE_DOC */
#define I_MASTER  6
/*------ Registers of Socket ------------------------------*/
#define R_READ    0
#define R_WRITE   0
#define R_ADDR    1
#define R_MASTER  2
#define R_REGSNUM 3

/*------- IdeDoc Register Structure -----------------------*/
typedef struct { /* IdeDoc Register Basic Operations */
  FLWORD value;
  FLWORD port;
} IdeDocReg;

/*------- IdeDoc Register Basic Operations ----------------*/
void ioInit( IdeDocReg *ioRegPtr, FLWORD portnum );
FLWORD ioGetw( IdeDocReg *ioRegPtr );
FLBYTE ioGetb( IdeDocReg *ioRegPtr );
FLWORD ioPutw( IdeDocReg *ioRegPtr, FLWORD data );
FLBYTE ioPutb( IdeDocReg *ioRegPtr, FLBYTE data );

/*------ IdeDoc Socket Structure -------------------------*/
typedef struct { /* ideDoc Socket Basic Operations */
  IdeDocReg ioRegs[R_REGSNUM];
  FLWORD portBase;
} IdeDocSocket;

/*------- IdeDoc Socket Basic Operations ------------------*/
#define ideGetVal(reg)          ideDocSocPtr->ioRegs[reg].value
#define ideGetb(reg)		ioGetb(&(ideDocSocPtr->ioRegs[reg]))
#define ideGetw(reg)		ioGetw(&(ideDocSocPtr->ioRegs[reg]))
#define idePutb(reg,val)	ioPutb(&(ideDocSocPtr->ioRegs[reg]),(val))
#define idePutw(reg,val)	ioPutw(&(ideDocSocPtr->ioRegs[reg]),(val))

FLStatus ideInit( FLWORD portbase );
void ideSetAddr( FLWORD address );
FLBYTE ideReadb( FLWORD address );
FLWORD ideReadw( FLWORD address );
FLBYTE ideWriteb( FLWORD address, FLBYTE data );
FLWORD ideWritew( FLWORD address, FLWORD data);

void ideCpyReadb( FLWORD address, FLBYTE far *des, FLWORD count );
void ideCpyWriteb( FLWORD address, FLBYTE far *src, FLWORD count );
void ideCpyReadw( FLWORD address, FLBYTE far *des, FLWORD count );
void ideCpyWritew( FLWORD address, FLBYTE far *src, FLWORD count );

/*------- Global Data Defines -------------------*/
extern IdeDocSocket ideDocSoc;
extern IdeDocSocket *ideDocSocPtr;

#endif /* _IDEDOC_H_ */
