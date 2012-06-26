/*
 * $Log: flcustom.c,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.7   Jan 20 2002 12:03:34   oris
 * Removed flVerifyWrite initialization.   
 * 
 *    Rev 1.6   Nov 16 2001 00:31:50   oris
 * Added flVerifyWrite = 1 (set).
 * 
 *    Rev 1.5   Jun 17 2001 16:43:46   oris
 * Improved documentation.
 * 
 *    Rev 1.4   Jun 17 2001 08:12:46   oris
 * Changed registration in order to allow DiskOnChip Millennium Plus to be the first device 
 * of a multi-doc volume.
 * 
 *    Rev 1.3   May 09 2001 00:47:26   oris
 * Update for Millennium Plus MTD and Translation Layer.
 * 
 *    Rev 1.2   Feb 18 2001 23:42:04   oris
 * Moved flPolicy, flUseMultiDoc and flMaxUnitChain to blockdev.c.
 *
 *    Rev 1.1   Feb 14 2001 02:19:28   oris
 * Added flMaxUnitChain environment variable.
 * Changed flUseMultiDoc and flPolicy variables type and names.
 *
 *    Rev 1.0   Feb 04 2001 13:31:02   oris
 * Initial revision.
 *
 */

/************************************************************************/
/*                                                                      */
/*              FAT-FTL Lite Software Development Kit                   */
/*              Copyright (C) M-Systems Ltd. 1995-2001                  */
/*                                                                      */
/************************************************************************/

#include "stdcomp.h"

/* environment variables */
#ifdef ENVIRONMENT_VARS

unsigned char flUse8Bit;
unsigned char flUseNFTLCache;
unsigned char flUseisRAM;
unsigned int  flPolicyPar; /* obsolete */

#ifndef MULTI_DOC
unsigned char flUseMultiDoc;
#endif
/*-----------------------------------------------------------------------*/
/*                 f l s e t E n v V a r                                 */
/*  Sets the value of all environment variables                          */
/*  Parameters : None                                                    */
/*-----------------------------------------------------------------------*/
void flSetEnvVar(void)
{
  flGetEnvVarFromParam( &flUseNFTLCache, &flPolicyPar, &flUseisRAM,
                        ((int *)&flUseMultiDoc), &flUse8Bit );
}

#endif /* ENVIRONMENT_VARS */

/*----------------------------------------------------------------------*/
/*                f l R e g i s t e r C o m p o n e n t s               */
/*                                                                      */
/* Register socket, MTD and Translation Layer components for usage      */
/*                                                                      */
/* This function is called by FLite once only, at initialization of the */
/* FLite system.                                                        */
/*                                                                      */
/* Parameters:                                                          */
/*      None                                                            */
/*                                                                      */
/*----------------------------------------------------------------------*/

FLStatus flRegisterComponents(void)
{
    /* Registering socket interface */

    flprintk(1,"flRegisterComponents: from %x to %x\n",flGetWinL(), flGetWinH());

    if (flRegisterDOCPLUSSOC(flGetWinL(), flGetWinH()) == flOK)
    { /* Register DiskOnChip socket interface */

        /* Registering MTD */
        flRegisterDOCPLUS();

        /* Registering translation layer */
        flRegisterINFTL();

        return flOK;
    }
    
    if (flRegisterDOCSOC(flGetWinL(), flGetWinH()) == flOK) { /* Register DiskOnChip socket interface */

        /* Registering MTD */
        flRegisterDOC2000();

        /* Registering translation layer */
        flRegisterINFTL();	/* Register MCP */

        /* Registering translation layer */
        flRegisterNFTL();

        return flOK;
    }
    return flAdapterNotFound;
}
