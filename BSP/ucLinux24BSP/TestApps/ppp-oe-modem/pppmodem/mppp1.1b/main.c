/*
 *  main for mppp
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2006-08-14 02:32:36 $
 *
 *  Description:
 *
 *      Program to make a ppp connection.
 *
 *  Limitations and Comments:
 *
 *  N/A
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   May-03-1997    first cut
 */


#ifdef HAVE_MOTIF

#include "xhead.h"
#include "xmppp.h"

#endif  /* HAVE_MOTIF */


#define INCLUDED_FROM_MAIN
#include "mppp.h"

int main(int argc,char **argv)
{

	unsigned int ii;
    /* initialize globals */
    g_verbose=0;        /* verbose off */
    g_quit=0;           /* quit for making the ppp connection is established*/
    g_quit_time=0;      /* stay up forever */
    *g_configfile='\0'; /* the config file */
#if 0
	*(unsigned int volatile *)(0xfff83050) = 0x15555055;
	*(unsigned int volatile *)(0xfff83054) = 0x30;
	*(unsigned int volatile *)(0xfff83058) = 0x00;
#endif

#if 1  //=============>  enable CTS/RTS pins
	*(unsigned int volatile *)(0xfff83050) &= 0xfffff0ff;
	*(unsigned int volatile *)(0xfff83050) |= 0xa00;
	*(unsigned int volatile *)(0xfff80110) = 0xf;
	
	//CSR_WRITE(COM_MCR_1, CSR_READ(COM_MCR_1)&0xf);
#endif

	//ii = *(unsigned int volatile *)(0xfff83000);
	//ii |= 4; // enable modem signal pins
	//*(unsigned int volatile *)(0xfff83000) = ii;
	//sleep(1);
	//printf("GPIO configured as %08x\n", *(unsigned int volatile *)(0xfff83050));
	//printf("UART configured as %08x\n", *(unsigned int volatile *)(0xfff80110));
	
#ifdef HAVE_MOTIF

    /* create the user interface */
    DoMotif(argc,argv);

#else
     parseCommandline(argc,argv);
     startPPP();
#endif      /* HAVE_MOTIF */
    exit(0);
}
