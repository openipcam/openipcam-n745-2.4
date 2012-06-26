/*
 * $Log: idedoc.c,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:51  andy
 * W90N745 uCLinux kernel
 *
 */
/*-------------------------------------------------------------------*/
#include "idedoc.h"
/*-------------------------------------------------------------------*/
#ifdef EXIT
void docIdeClose(void)
{
}
#pragma exit docIdeClose
#endif /* EXIT */
/*------- Global Data Defines ---------------------------------------*/
IdeDocSocket ideDocSoc;
IdeDocSocket *ideDocSocPtr;

#define outportb(x,y)    outb(y,x)
#define inportb(x)       inb(x)
#define outport(x,y)     outw(y,x)
#define inport(x)        inw(x)

/*------- IdeDoc Register Basic Operations --------------------------*/
void ioInit( IdeDocReg *ioRegPtr, FLWORD portnum )
{ /* Init IO Register */
  ioRegPtr->port = portnum;
  ioRegPtr->value = 0;
}
/*---------------------------------------------------------*/
FLBYTE ioGetb( IdeDocReg *ioRegPtr )
{ /* Read 8-bit value from specific Register */
  ioRegPtr->value = (FLWORD)inportb( ioRegPtr->port );
  return( (FLBYTE)(ioRegPtr->value) );
}
/*---------------------------------------------------------*/
FLWORD ioGetw( IdeDocReg *ioRegPtr )
{ /* Read 16-bit value from specific Register */
  ioRegPtr->value = inport( ioRegPtr->port );
  return( ioRegPtr->value );
}
/*---------------------------------------------------------*/
FLBYTE ioPutb( IdeDocReg *ioRegPtr, FLBYTE data )
{ /* Write 8-bit 'data' into specific Register */
  ioRegPtr->value = (FLWORD)data;
  outportb( ioRegPtr->port, data );
  return( data );
}
/*---------------------------------------------------------*/
FLWORD ioPutw( IdeDocReg *ioRegPtr, FLWORD data )
{ /* Write 16-bit 'data' into specific Register */
  ioRegPtr->value = data;
  outport( ioRegPtr->port, data );
  return( data );
}

/*----- IdeDoc Socket functions -------------------------------------*/
FLStatus ideInit( FLWORD portbase )
{ /* Set Base port & init Socket Registers
   * Data flow order:
   * [ Address / Data ] >> [ ~CE ] >> [ ~OE / ~WR ] >> [ OE / WR ] >> [ CE ] */
  ideDocSocPtr = (IdeDocSocket *)&ideDocSoc;
  ideDocSocPtr->portBase = portbase;

/*  iopl(3); */
  
  ioInit(&(ideDocSocPtr->ioRegs[R_READ]),(portbase + I_READ));
  ioInit(&(ideDocSocPtr->ioRegs[R_ADDR]),(portbase + I_ADDR));
  ioInit(&(ideDocSocPtr->ioRegs[R_MASTER]),(portbase + I_MASTER));
  return( flOK );
}
/*---------------------------------------------------------*/
void ideSetAddr( FLWORD address )
{ /* Set Address in ADDR Register */
  FLBYTE addrReg;
#ifdef FL_IDE_DOC
  /* A0-A5,A11,A12 */
  addrReg = (((HIBYTE(address)) << 3) & 0xC0) | ((LOBYTE(address)) & 0x3F);
#else
  /* A0-A7 */
  addrReg = (FLBYTE)((address == Nio ) ? NflashData : address);
#endif /* FL_IDE_DOC */
  idePutb( R_ADDR, addrReg );
}
/*---------------------------------------------------------*/
FLBYTE ideReadb( FLWORD address )
{ /* Emulate read from DiskOnChip window */
  volatile FLBYTE val;

  ideSetAddr( address );
  val = ideGetb( R_READ );
  return( val );
}
/*---------------------------------------------------------*/
FLWORD ideReadw( FLWORD address )
{ /* Emulate read from DiskOnChip window */
  volatile FLWORD val;

  ideSetAddr( address );
  val = ideGetw( R_READ );
  return( val );
}
/*---------------------------------------------------------*/
FLBYTE ideWriteb( FLWORD address, FLBYTE data )
{ /* Emulate write to DiskOnChip window */
  ideSetAddr( address );
  idePutb( R_WRITE, data );
  return( data );
}
/*---------------------------------------------------------*/
FLWORD ideWritew( FLWORD address, FLWORD data)
{ /* Emulate write to DiskOnChip window */
  ideSetAddr( address );
  idePutw( R_WRITE, data );
  return( data );
}
/*------- Ide String Operations -------------------------------------*/
void ideCpyReadb( FLWORD address, FLBYTE far *des, FLWORD count )
{
  FLWORD readPort = ideDocSocPtr->ioRegs[R_READ].port;

  ideSetAddr(address);
  insb( readPort, (void *)des, (FLDWORD)count );
#if 0  
  asm {
      push dx
      push cx
      push di
      push es
      mov  dx, readPort
      mov  cx, count
      les  di, des
      cld
      rep  insb
      pop  es
      pop  di
      pop  cx
      pop  dx
  }
#endif  
}
/*---------------------------------------------------------*/
void ideCpyWriteb( FLWORD address, FLBYTE far *src, FLWORD count )
{
  FLWORD writePort = ideDocSocPtr->ioRegs[R_WRITE].port;

  ideSetAddr(address);
  outsb( writePort, (void *)src, (FLDWORD)count );
#if 0  
  asm {
      push dx
      push cx
      push si
      push ds
      mov  dx, writePort
      mov  cx, count
      lds  si, src
      cld
      rep  outsb
      pop  ds
      pop  si
      pop  cx
      pop  dx
  }
#endif  
}
/*---------------------------------------------------------*/
void ideCpyReadw( FLWORD address, FLBYTE far *des, FLWORD count )
{
  FLWORD readPort = ideDocSocPtr->ioRegs[R_READ].port;

  ideSetAddr(address);
  insw( readPort, (void *)des, (FLDWORD)(count>>1) );
#if 0  
  asm {
      push dx
      push cx
      push di
      push es
      mov  dx, readPort
      mov  cx, count
      shr  cx, 1
      les  di, des
      cld
      rep  insw
      pop  es
      pop  di
      pop  cx
      pop  dx
  }
#endif  
}
/*---------------------------------------------------------*/
void ideCpyWritew( FLWORD address, FLBYTE far *src, FLWORD count )
{
  FLWORD writePort = ideDocSocPtr->ioRegs[R_WRITE].port;

  ideSetAddr(address);
  outsw( writePort, (void *)src, (FLDWORD)(count>>1) );
#if 0  
  asm {
      push dx
      push cx
      push si
      push ds
      mov  dx, writePort
      mov  cx, count
      shr  cx, 1
      lds  si, src
      cld
      rep  outsw
      pop  ds
      pop  si
      pop  cx
      pop  dx
  }
#endif  
}
/*-------------------------------------------------------------------*/
