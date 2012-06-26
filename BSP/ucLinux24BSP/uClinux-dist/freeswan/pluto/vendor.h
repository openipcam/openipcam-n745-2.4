/* FreeS/WAN ISAKMP VendorID
 * Copyright (C) 2002 Mathieu Lafon - Arkoon Network Security
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * RCSID $Id: vendor.h,v 1.1.1.1 2006-07-11 09:28:11 andy Exp $
 */

#ifndef _VENDOR_H_
#define _VENDOR_H_

/* 1 - 100 : Implementation names */
#define VID_OPENPGP                 1
#define VID_KAME_RACOON             2
#define VID_MS_NT5                  3
#define VID_SSH_SENTINEL            4
#define VID_SSH_SENTINEL_1_1        5
#define VID_SSH_SENTINEL_1_2        6
#define VID_SSH_SENTINEL_1_3        7
#define VID_SSH_IPSEC_1_1_0         8
#define VID_SSH_IPSEC_1_1_1         9
#define VID_SSH_IPSEC_1_1_2         10
#define VID_SSH_IPSEC_1_2_1         11
#define VID_SSH_IPSEC_1_2_2         12
#define VID_SSH_IPSEC_2_0_0         13
#define VID_SSH_IPSEC_2_1_0         14
#define VID_SSH_IPSEC_2_1_1         15
#define VID_SSH_IPSEC_2_1_2         16
#define VID_SSH_IPSEC_3_0_0         17
#define VID_SSH_IPSEC_3_0_1         18
#define VID_SSH_IPSEC_4_0_0         19
#define VID_SSH_IPSEC_4_0_1         20
#define VID_SSH_IPSEC_4_1_0         21
#define VID_SSH_IPSEC_4_2_0         22
#define VID_CISCO_UNITY             23

/* 101 - 200 : NAT-Traversal */
#define VID_NATT_STENBERG_01        101
#define VID_NATT_STENBERG_02        102
#define VID_NATT_HUTTUNEN           103
#define VID_NATT_IETF_00            104
#define VID_NATT_IETF_02            105
#define VID_NATT_IETF_03            106

/* 201 - 300 : Misc */
#define VID_MISC_XAUTH              201
#define VID_MISC_DPD                202
#define VID_MISC_HEARTBEAT_NOTIFY   203

void init_vendorid(void);

struct msg_digest;
void handle_vendorid (struct msg_digest *md, const char *vid, size_t len);

bool out_vendorid (u_int8_t np, pb_stream *outs, unsigned int vid);

#endif /* _VENDOR_H_ */

