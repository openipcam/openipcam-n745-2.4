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
 * RCSID $Id: vendor.c,v 1.1.1.1 2006-07-11 09:28:11 andy Exp $
 */

#include <stdlib.h>
#include <string.h>

#include <freeswan.h>

#include "constants.h"
#include "defs.h"
#include "log.h"
#include "md5.h"
#include "id.h"
#include "x509.h"
#include "connections.h"
#include "packet.h"
#include "demux.h"
#include "whack.h"
#include "vendor.h"
#include "state.h"

#ifdef NAT_TRAVERSAL
#include "nat_traversal.h"
#endif

/**
 * Unknown VID:
 *
 * SafeNet SoftRemote 8.0.0:
 *  47bbe7c993f1fc13b4e6d0db565c68e5010201010201010310382e302e3020284275696c6420313029000000
 *  da8e937880010000
 *
 * Netscreen:
 *  cf49908791073fb46439790fdeb6aeed981101ab0000000500000300
 *  4865617274426561745f4e6f74696679386b0100  (HeartBeat_Notify + 386b0100)
 *
 * Cisco:
 *  12f5f28c457168a9702d9fe274cc0100 (Cisco-Unity)
 *  c32364b3b4f447eb17c488ab2a480a57
 *  1f07f70eaa6514d3b0fa96542a500305
 *  1f07f70eaa6514d3b0fa96542a500300
 *  1f07f70eaa6514d3b0fa96542a500301 (VPN 3000 version 3.1 ??)
 *  afcad71368a1f1c96b8696fc77570100 (Dead Peer Detection ?)
 *  6d761ddc26aceca1b0ed11fabbb860c4
 *
 * If someone know what they mean, mail me.
 */

#define MAX_LOG_VID_LEN    8

struct vid_struct {
	unsigned int id;
	char *vid;
	unsigned int vid_len;
	int substring;
	const char *vid_string;
	const char *vid_string_to_hash;
	const char *info_str;
};

static struct vid_struct _vid_tab[] = {

	/* Implementation names */

	{ VID_OPENPGP, NULL, 0, 0, "OpenPGP10171", NULL, "OpenPGP" },
	{ VID_KAME_RACOON, NULL, 0, 0, NULL, "KAME/racoon", "KAME (BSD)" },
	{ VID_MS_NT5, NULL, 0, 1, NULL, "MS NT5 ISAKMPOAKLEY", NULL },

	{ VID_SSH_SENTINEL, NULL, 0, 0, NULL, "SSH Sentinel", NULL },
	{ VID_SSH_SENTINEL_1_1, NULL, 0, 0, NULL, "SSH Sentinel 1.1", NULL },
	{ VID_SSH_SENTINEL_1_2, NULL, 0, 0, NULL, "SSH Sentinel 1.2", NULL },
	/*
	 * I don't know why but my version of SSH Sentinel 1.3 use
	 * 'SSH Communications Security IPSEC Express version 4.1.0' instead
	 * of 'SSH Sentinel 1.3'
	 */
	{ VID_SSH_SENTINEL_1_3, NULL, 0, 0, NULL, "SSH Sentinel 1.3", NULL },

	/* These ones come from SSH vendors.txt */
	{ VID_SSH_IPSEC_1_1_0, NULL, 0, 0, NULL,
		"Ssh Communications Security IPSEC Express version 1.1.0", NULL },
	{ VID_SSH_IPSEC_1_1_1, NULL, 0, 0, NULL,
		"Ssh Communications Security IPSEC Express version 1.1.1", NULL },
	{ VID_SSH_IPSEC_1_1_2, NULL, 0, 0, NULL,
		"Ssh Communications Security IPSEC Express version 1.1.2", NULL },
	{ VID_SSH_IPSEC_1_2_1, NULL, 0, 0, NULL,
		"Ssh Communications Security IPSEC Express version 1.2.1", NULL },
	{ VID_SSH_IPSEC_1_2_2, NULL, 0, 0, NULL,
		"Ssh Communications Security IPSEC Express version 1.2.2", NULL },
	{ VID_SSH_IPSEC_2_0_0, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 2.0.0", NULL },
	{ VID_SSH_IPSEC_2_1_0, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 2.1.0", NULL },
	{ VID_SSH_IPSEC_2_1_1, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 2.1.1", NULL },
	{ VID_SSH_IPSEC_2_1_2, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 2.1.2", NULL },
	{ VID_SSH_IPSEC_3_0_0, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 3.0.0", NULL },
	{ VID_SSH_IPSEC_3_0_1, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 3.0.1", NULL },
	{ VID_SSH_IPSEC_4_0_0, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 4.0.0", NULL },
	{ VID_SSH_IPSEC_4_0_1, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 4.0.1", NULL },
	{ VID_SSH_IPSEC_4_1_0, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 4.1.0", NULL },
	{ VID_SSH_IPSEC_4_2_0, NULL, 0, 0, NULL,
		"SSH Communications Security IPSEC Express version 4.2.0", NULL },

	{ VID_CISCO_UNITY,
		"\x12\xf5\xf2\x8c\x45\x71\x68\xa9\x70\x2d\x9f\xe2\x74\xcc\x01\x00", 16,
		0, NULL, NULL, "Cisco-Unity" },

	/* NAT-Traversal */

	{ VID_NATT_STENBERG_01, NULL, 0, 0, NULL,
		"draft-stenberg-ipsec-nat-traversal-01", NULL },
	{ VID_NATT_STENBERG_02, NULL, 0, 0, NULL,
		"draft-stenberg-ipsec-nat-traversal-02", NULL },
	{ VID_NATT_HUTTUNEN, NULL, 0, 0, NULL, "ESPThruNAT", NULL },
	{ VID_NATT_IETF_00, NULL, 0, 0, NULL, "draft-ietf-ipsec-nat-t-ike-00",
		NULL },
	{ VID_NATT_IETF_02, NULL, 0, 0, NULL, "draft-ietf-ipsec-nat-t-ike-02",
		NULL },
	/* hash in draft-ietf-ipsec-nat-t-ike-02 contains '\n'... Accept both */
	{ VID_NATT_IETF_02, NULL, 0, 0, NULL, "draft-ietf-ipsec-nat-t-ike-02\n", 
		"draft-ietf-ipsec-nat-t-ike-02" },
	{ VID_NATT_IETF_03, NULL, 0, 0, NULL, "draft-ietf-ipsec-nat-t-ike-03",
		NULL },

	/* misc */
	
	{ VID_MISC_XAUTH, "\x09\x00\x26\x89\xdf\xd6\xb7\x12", 8, 0,
		NULL, NULL, "XAUTH" },
	{ VID_MISC_DPD,
		"\xaf\xca\xd7\x13\x68\xa1\xf1\xc9\x6b\x86\x96\xfc\x77\x57\x01\x00", 16,
		0, NULL, NULL, "Dead Peer Detection" },
	{ VID_MISC_HEARTBEAT_NOTIFY, NULL, 0, 1, "HeartBeat_Notify", NULL,
		"HeartBeat Notify" },

	/* -- */
	{ 0, NULL, 0, 0, NULL, NULL, NULL }

};

static int _vid_struct_init = 0;

void init_vendorid(void)
{
	struct vid_struct *vid;
	MD5_CTX ctx;

	for (vid = _vid_tab; vid->id; vid++) {
		if (vid->vid_string) {
			/** VendorID is a string **/
			vid->vid = strdup(vid->vid_string);
			vid->vid_len = strlen(vid->vid_string);
		}
		else if (vid->vid_string_to_hash) {
			/** VendorID is a string to hash with MD5 **/
			vid->vid = malloc(MD5_DIGEST_SIZE);
			if (vid->vid) {
				MD5Init(&ctx);
				MD5Update(&ctx, (unsigned char *)vid->vid_string_to_hash,
					strlen(vid->vid_string_to_hash));
				MD5Final(vid->vid, &ctx);
				vid->vid_len = MD5_DIGEST_SIZE;
			}
		}
		if (!vid->info_str) {
			/** Find something to display **/
			if (vid->vid_string)
				vid->info_str = vid->vid_string;
			else if (vid->vid_string_to_hash)
				vid->info_str = vid->vid_string_to_hash;
		}
#if 0
		DBG_log("vendorid_init: %d [%s]",
			vid->id,
			vid->info_str ? vid->info_str : ""
			);
		if (vid->vid) DBG_dump("VID:", vid->vid, vid->vid_len);
#endif
	}
	_vid_struct_init = 1;
}

static void handle_known_vendorid (struct msg_digest *md,
	const char *vidstr UNUSED, size_t len UNUSED, struct vid_struct *vid)
{
	int vid_usefull = 0;
	switch (vid->id) {
#ifdef NAT_TRAVERSAL
		/*
		 * Use first supported NAT-Traversal Method and ignored the other
		 * ones (implementations will send all supported methods but only
		 * one will be used)
		 */
		case VID_NATT_IETF_00:
			if ((nat_traversal_activated()) && (!md->nat_traversal_vid)) {
				md->nat_traversal_vid = vid->id;
				vid_usefull = 1;
			}
			break;
#ifdef NAT_T_SUPPORT_LAST_DRAFTS
		case VID_NATT_IETF_02:
		case VID_NATT_IETF_03:
			if ((nat_traversal_activated()) && (!md->nat_traversal_vid)) {
				md->nat_traversal_vid = vid->id;
				vid_usefull = 1;
			}
			break;
#endif
#endif
		case VID_MISC_DPD:
			md->dpd = 1;
			vid_usefull = 1;
			break;
		default:
			break;
	}
	loglog(RC_LOG_SERIOUS, "%s Vendor ID payload [%s]",
		vid_usefull ? "received" : "ignoring",
		vid->info_str ? vid->info_str : "", vid->id);
}

void handle_vendorid (struct msg_digest *md, const char *vid, size_t len)
{
	struct vid_struct *pvid;

	if (!_vid_struct_init) {
		init_vendorid();
	}

	/*
	 * Find known VendorID in _vid_tab
	 */
	for (pvid = _vid_tab; pvid->id; pvid++) {
		if (pvid->vid && vid && pvid->vid_len && len) {
			if (pvid->vid_len == len) {
				if (memcmp(pvid->vid, vid, len)==0) {
					handle_known_vendorid(md, vid, len, pvid);
					return;
				}
			}
			else if ((pvid->vid_len < len) && (pvid->substring)) {
				/**
				 * Guess who need a special handling...
				 */
				if (memcmp(pvid->vid, vid, pvid->vid_len)==0) {
					handle_known_vendorid(md, vid, len, pvid);
					return;
				}
			}
		}
	}

	/*
	 * Unknown VendorID. Log the beginning.
	 */
	{
		static const char hexdig[] = "0123456789abcdef";
		char log_vid[2*MAX_LOG_VID_LEN+1];
		size_t i;
		memset(log_vid, 0, sizeof(log_vid));
		for (i=0; (i<len) && (i<MAX_LOG_VID_LEN); i++) {
			log_vid[2*i] = hexdig[(vid[i] >> 4) & 0xF];
			log_vid[2*i+1] = hexdig[vid[i] & 0xF];
		}
		loglog(RC_LOG_SERIOUS, "ignoring Vendor ID payload [%s%s]",
			log_vid, (len>MAX_LOG_VID_LEN) ? "..." : "");
	}
}

/**
 * Add a vendor id payload to the msg
 */
bool out_vendorid (u_int8_t np, pb_stream *outs, unsigned int vid)
{
	struct vid_struct *pvid;

	if (!_vid_struct_init) {
		init_vendorid();
	}

	for (pvid = _vid_tab; (pvid->id) && (pvid->id!=vid); pvid++);

	if (pvid->id != vid) return STF_INTERNAL_ERROR; /* not found */
	if (!pvid->vid) return STF_INTERNAL_ERROR; /* not initialized */

	DBG(DBG_EMITTING,
		DBG_log("out_vendorid(): sending [%s]", pvid->info_str);
	);

	if (!out_modify_previous_np(ISAKMP_NEXT_VID, outs))
		return FALSE;

	return out_generic_raw(np, &isakmp_vendor_id_desc, outs,
		pvid->vid, pvid->vid_len, "V_ID");
}

