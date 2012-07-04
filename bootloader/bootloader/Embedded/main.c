/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: main.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: main.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/Embedded
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/Embedded
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:22p
 * Updated in $/W90P710/FIRMWARE/Embedded
 * Add VSS header
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "platform.h"
#include "serial.h"

#pragma import(__use_no_semihosting_swi)	

//extern void * vector_table;

void demo_printf(void)
{
  printf("Hello World\n");
}


void demo_sprintf(void)
{
  int x;
  char buf[20];

  for (x=1; x<=5; x++)
  {
    sprintf(buf, "Hello Again %d\n", x);
    printf("%s", buf);
  }
  
  
}


float f1=3.1415926535898, f2=1.2345678;


void demo_float_print(void)
{
  double f3=3.1415926535898, f4=1.2345678;

  printf("Float: f1 x f2 = %f x %f = %f\n", f1, f2, f1*f2);
  printf("Double: f3 x f4 = %14.14f x %14.14f = %14.14f\n", f3, f4, f3*f4);
}


int  *p;

void demo_malloc(void)
{
  p = (int *)malloc(0x1000);

  if (p==NULL)
  {
    printf("Out of memory\n");
  }
  else
  {
    printf("Allocated p at %p\n", (void *)p);
  }

  if (p)
  {
    free(p);
    printf("Freed p\n");
  }
}


#ifdef EMBEDDED
extern void init_serial_A(void);
#endif

int main(void)
{
	char * p;

#ifdef EMBEDDED
  #pragma import(__use_no_semihosting_swi)  // ensure no functions that use semihosting 
                                            // SWIs are linked in from the C library 
#ifdef USE_SERIAL_PORT
  init_serial(0, ARM_BAUD_115200);            // initialize serial port A
#endif

#endif




  printf("C Library Example\n");

  demo_printf();
  demo_sprintf();
  demo_float_print();
  demo_malloc();
  
  p=malloc(0x1000);
  printf("p = 0x%08x\n",(int)p);
  p=malloc(0x1000);
  printf("p = 0x%08x\n",(int)p);
  p=malloc(0x1000);
  printf("p = 0x%08x\n",(int)p);
  p=malloc(0x1000);
  printf("p = 0x%08x\n",(int)p);
  
  

  return 0;
}

