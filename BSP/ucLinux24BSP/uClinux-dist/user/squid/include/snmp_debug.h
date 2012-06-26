/*
 * $Id: snmp_debug.h,v 1.1.1.1 2006-07-11 09:33:06 andy Exp $
 */

#ifndef SNMP_DEBUG_H
#define SNMP_DEBUG_H

#if STDC_HEADERS
extern void snmplib_debug(int, char *,...);
#else
extern void snmplib_debug(va_alist);
#endif

#endif
