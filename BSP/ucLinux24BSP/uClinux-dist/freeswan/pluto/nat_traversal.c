/* FreeS/WAN NAT-Traversal
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
 * RCSID $Id: nat_traversal.c,v 1.1.1.1 2006-07-11 09:28:11 andy Exp $
 */

#ifdef NAT_TRAVERSAL

#include <stdlib.h>
#include <errno.h>

#include <freeswan.h>
#include <pfkeyv2.h>
#include <pfkey.h>

#include "constants.h"
#include "defs.h"
#include "log.h"
#include "id.h"
#include "x509.h"
#include "connections.h"
#include "packet.h"
#include "demux.h"
#include "whack.h"
#include "state.h"
#include "server.h"
#include "timer.h"
#include "sha1.h"
#include "md5.h"
#include "crypto.h"
#include "vendor.h"
#include "cookie.h"

#include "nat_traversal.h"

#include "ike_alg.h"

/* #define FORCE_NAT_TRAVERSAL */
/* #define NAT_D_DEBUG */

#ifndef SOL_UDP
#define SOL_UDP 17
#endif

#ifndef UDP_ESPINUDP
#define UDP_ESPINUDP    100
#endif

#define DEFAULT_KEEP_ALIVE_PERIOD  20

#ifdef _IKE_ALG_H
/* Alg patch: hash_digest_len -> hash_digest_size */
#define hash_digest_len hash_digest_size
#endif

static bool _nat_t_activated = 0;
static unsigned int _kap = 0;
static unsigned int _ka_evt = 0;

static const char *natt_version = "0.3";

static const char *natt_methods[] = {
	"draft-ietf-ipsec-nat-t-ike-00",
	"draft-ietf-ipsec-nat-t-ike-03"
};

void init_nat_traversal (bool activate, unsigned int keep_alive_period)
{
	_nat_t_activated = activate;
	_kap = keep_alive_period ? keep_alive_period : DEFAULT_KEEP_ALIVE_PERIOD;
	log("  including NAT-Traversal patch (Version %s)%s",
		natt_version, activate ? "" : " [disabled]");
}

bool nat_traversal_activated(void)
{
	return _nat_t_activated;
}

static void _natd_hash(const struct hash_desc *hasher, char *hash,
	u_int8_t *icookie, u_int8_t *rcookie,
	const ip_address *ip, u_int16_t port)
{
	union hash_ctx ctx;

	if (is_zero_cookie(icookie))
		DBG_log("_natd_hash: Warning, icookie is zero !!");
	if (is_zero_cookie(rcookie))
		DBG_log("_natd_hash: Warning, rcookie is zero !!");

	/**
	 * draft-ietf-ipsec-nat-t-ike-01.txt
	 *
	 *   HASH = HASH(CKY-I | CKY-R | IP | Port)
	 *
	 * All values in network order
	 */
	hasher->hash_init(&ctx);
	hasher->hash_update(&ctx, icookie, COOKIE_SIZE);
	hasher->hash_update(&ctx, rcookie, COOKIE_SIZE);
	switch (addrtypeof(ip)) {
		case AF_INET:
			hasher->hash_update(&ctx,
				(const u_char *)&ip->u.v4.sin_addr.s_addr,
				sizeof(ip->u.v4.sin_addr.s_addr));
			break;
		case AF_INET6:
			hasher->hash_update(&ctx,
				(const u_char *)&ip->u.v6.sin6_addr.s6_addr,
				sizeof(ip->u.v6.sin6_addr.s6_addr));
			break;
	}
	hasher->hash_update(&ctx, (const u_char *)&port, sizeof(u_int16_t));
	hasher->hash_final(hash, &ctx);
#ifdef NAT_D_DEBUG
	DBG_log("_natd_hash: hasher=%p(%d)", hasher, hasher->hash_digest_len);
	DBG_dump("_natd_hash: icookie=", icookie, COOKIE_SIZE);
	DBG_dump("_natd_hash: rcookie=", rcookie, COOKIE_SIZE);
	switch (addrtypeof(ip)) {
		case AF_INET:
			DBG_dump("_natd_hash: ip=", &ip->u.v4.sin_addr.s_addr,
				sizeof(ip->u.v4.sin_addr.s_addr));
			break;
	}
	DBG_log("_natd_hash: port=%d", port);
	DBG_dump("_natd_hash: hash=", hash, hasher->hash_digest_len);
#endif
}

/**
 * Add NAT-Traversal VIDs (supported ones)
 *
 * Used when we're Initiator
 */
bool nat_traversal_add_vid(u_int8_t np, pb_stream *outs)
{
	bool r = TRUE;
#ifdef NAT_T_SUPPORT_LAST_DRAFTS
	if (r) r = out_vendorid(np, outs, VID_NATT_IETF_03);
	if (r) r = out_vendorid(np, outs, VID_NATT_IETF_02);
#endif
	if (r) r = out_vendorid(np, outs, VID_NATT_IETF_00);
	return r;
}

void nat_traversal_natd_lookup(struct msg_digest *md)
{
	char hash[MAX_DIGEST_LEN];
	struct payload_digest *p;
	struct state *st = md->st;
	int i;

	if (!st || !md->iface || !st->st_oakley.hasher) {
		loglog(RC_LOG_SERIOUS, "NAT-Traversal: assert failed %s:%d",
			__FILE__, __LINE__);
		return;
	}

	/** Count NAT-D **/
	for (p = md->chain[ISAKMP_NEXT_NATD_R], i=0; p != NULL; p = p->next, i++);

	/**
	 * We need at least 2 NAT-D (1 for us, many for peer)
	 */
	if (i < 2) {
		loglog(RC_LOG_SERIOUS,
		"NAT-Traversal: Only %d NAT-D - Aborting NAT-Traversal negotiation", i);
		st->nat_traversal = 0;
		return;
	} 
#ifdef NAT_D_DEBUG
	else
		loglog(RC_LOG_SERIOUS,
		"NAT-Traversal: %d NAT-D - Continuing NAT-Traversal negotiation", i);
#endif


	/**
	 * First one with my IP & port
	 */
	p = md->chain[ISAKMP_NEXT_NATD_R];
	_natd_hash(st->st_oakley.hasher, hash, st->st_icookie, st->st_rcookie,
		&(md->iface->addr), ntohs(st->st_connection->this.host_port));
	if (!( (pbs_left(&p->pbs) == st->st_oakley.hasher->hash_digest_len) &&
		(memcmp(p->pbs.cur, hash, st->st_oakley.hasher->hash_digest_len)==0)
		)) {
#ifdef NAT_D_DEBUG
		DBG_log("NAT_TRAVERSAL_NAT_BHND_ME");
		DBG_dump("expected NAT-D:", hash, st->st_oakley.hasher->hash_digest_len);
		DBG_dump("received NAT-D:", p->pbs.cur, pbs_left(&p->pbs));
#endif
		st->nat_traversal |= LELEM(NAT_TRAVERSAL_NAT_BHND_ME);
	}

	/**
	 * The others with sender IP & port
	 */
	_natd_hash(st->st_oakley.hasher, hash, st->st_icookie, st->st_rcookie,
		&(md->sender), ntohs(md->sender_port));
	for (p = p->next, i=0 ; p != NULL; p = p->next) {
		if ( (pbs_left(&p->pbs) == st->st_oakley.hasher->hash_digest_len) &&
			(memcmp(p->pbs.cur, hash, st->st_oakley.hasher->hash_digest_len)==0)
			) {
			i++;
		}
	}
	if (!i) {
#ifdef NAT_D_DEBUG
		DBG_log("NAT_TRAVERSAL_NAT_BHND_PEER");
		DBG_dump("expected NAT-D:", hash, st->st_oakley.hasher->hash_digest_len);
		p = md->chain[ISAKMP_NEXT_NATD_R];
		for (p = p->next, i=0 ; p != NULL; p = p->next) {
			DBG_dump("received NAT-D:", p->pbs.cur, pbs_left(&p->pbs));
		}
#endif
		st->nat_traversal |= LELEM(NAT_TRAVERSAL_NAT_BHND_PEER);
	}
#ifdef FORCE_NAT_TRAVERSAL
	st->nat_traversal |= LELEM(NAT_TRAVERSAL_NAT_BHND_PEER);
	st->nat_traversal |= LELEM(NAT_TRAVERSAL_NAT_BHND_ME);
#endif
}

bool nat_traversal_add_natd(u_int8_t np, pb_stream *outs,
	struct msg_digest *md)
{
	char hash[MAX_DIGEST_LEN];
	struct state *st = md->st;

	if (!out_modify_previous_np(ISAKMP_NEXT_NATD, outs))
		return FALSE;

	if (!st || !st->st_oakley.hasher) {
		loglog(RC_LOG_SERIOUS, "NAT-Traversal: assert failed %s:%d",
			__FILE__, __LINE__);
		return FALSE;
	}

	/**
	 * First one with sender IP & port
	 */
	_natd_hash(st->st_oakley.hasher, hash, st->st_icookie,
		is_zero_cookie(st->st_rcookie) ? md->hdr.isa_rcookie : st->st_rcookie,
		&(md->sender),
#ifdef FORCE_NAT_TRAVERSAL
		0
#else
		ntohs(md->sender_port)
#endif
	);
	if (!out_generic_raw(ISAKMP_NEXT_NATD, &isakmp_nat_d, outs,
		hash, st->st_oakley.hasher->hash_digest_len, "NAT-D"))
		return FALSE;

	/**
	 * Second one with my IP & port
	 */
	_natd_hash(st->st_oakley.hasher, hash, st->st_icookie,
		is_zero_cookie(st->st_rcookie) ? md->hdr.isa_rcookie : st->st_rcookie,
		&(md->iface->addr),
#ifdef FORCE_NAT_TRAVERSAL
		0
#else
		ntohs(st->st_connection->this.host_port)
#endif
	);
	return (out_generic_raw(np, &isakmp_nat_d, outs,
		hash, st->st_oakley.hasher->hash_digest_len, "NAT-D"));
}

/**
 * nat_traversal_natoa_lookup()
 * 
 * Look for NAT-OA in message
 */
void nat_traversal_natoa_lookup(struct msg_digest *md)
{
	struct payload_digest *p;
	struct state *st = md->st;
	int i;
	ip_address ip;

	if (!st || !md->iface) {
		loglog(RC_LOG_SERIOUS, "NAT-Traversal: assert failed %s:%d",
			__FILE__, __LINE__);
		return;
	}

	/** Count NAT-OA **/
	for (p = md->chain[ISAKMP_NEXT_NATOA_R], i=0; p != NULL; p = p->next, i++);

#if 0
	DBG_log("NAT-Traversal: received %d NAT-OA.", i);
#endif

	if (i==0) {
		return;
	}
	else if (!(st->nat_traversal & LELEM(NAT_TRAVERSAL_NAT_BHND_PEER))) {
		loglog(RC_LOG_SERIOUS, "NAT-Traversal: received %d NAT-OA. "
			"ignored because peer is not NATed", i);
		return;
	}
	else if (i>1) {
		loglog(RC_LOG_SERIOUS, "NAT-Traversal: received %d NAT-OA. "
			"using first, ignoring others", i);
	}

	/** Take first **/
	p = md->chain[ISAKMP_NEXT_NATOA_R];

	DBG(DBG_PARSING,
		DBG_dump("NAT-OA:", p->pbs.start, pbs_room(&p->pbs));
	);

	switch (p->payload.nat_oa.isanoa_idtype) {
		case ID_IPV4_ADDR:
			if (pbs_left(&p->pbs) == sizeof(struct in_addr)) {
				initaddr(p->pbs.cur, pbs_left(&p->pbs), AF_INET, &ip);
			}
			else {
				loglog(RC_LOG_SERIOUS, "NAT-Traversal: received IPv4 NAT-OA "
					"with invalid IP size (%d)", pbs_left(&p->pbs));
				return;
			}
			break;
		case ID_IPV6_ADDR:
			if (pbs_left(&p->pbs) == sizeof(struct in6_addr)) {
				initaddr(p->pbs.cur, pbs_left(&p->pbs), AF_INET6, &ip);
			}
			else {
				loglog(RC_LOG_SERIOUS, "NAT-Traversal: received IPv6 NAT-OA "
					"with invalid IP size (%d)", pbs_left(&p->pbs));
				return;
			}
			break;
		default:
			loglog(RC_LOG_SERIOUS, "NAT-Traversal: "
				"invalid ID Type (%d) in NAT-OA - ignored",
				p->payload.nat_oa.isanoa_idtype);
			return;
			break;
	}

	DBG(DBG_PARSING,
		{
			char ip_t[ADDRTOT_BUF];
			addrtot(&ip, 0, ip_t, sizeof(ip_t));
			DBG_log("received NAT-OA: %s", ip_t);
		}
	);

	if (isanyaddr(&ip)) {
		loglog(RC_LOG_SERIOUS, "NAT-Traversal: received %%any NAT-OA...");
	}
	else {
		st->nat_oa = ip;
	}
}

bool nat_traversal_add_natoa(u_int8_t np, pb_stream *outs,
	struct state *st)
{
	struct isakmp_nat_oa natoa;
	pb_stream pbs;
	unsigned char ip_val[sizeof(struct in6_addr)];
	size_t ip_len = 0;
	ip_address *ip;

	if ((!st) || (!st->st_connection)) {
		loglog(RC_LOG_SERIOUS, "NAT-Traversal: assert failed %s:%d",
			__FILE__, __LINE__);
		return FALSE;
	}
	ip = &(st->st_connection->this.host_addr);

	if (!out_modify_previous_np(ISAKMP_NEXT_NATOA, outs))
		return FALSE;

	memset(&natoa, 0, sizeof(natoa));
	natoa.isanoa_np = np;

	switch (addrtypeof(ip)) {
		case AF_INET:
			ip_len = sizeof(ip->u.v4.sin_addr.s_addr);
			memcpy(ip_val, &ip->u.v4.sin_addr.s_addr, ip_len);
			natoa.isanoa_idtype = ID_IPV4_ADDR;
			break;
		case AF_INET6:
			ip_len = sizeof(ip->u.v6.sin6_addr.s6_addr);
			memcpy(ip_val, &ip->u.v6.sin6_addr.s6_addr, ip_len);
			natoa.isanoa_idtype = ID_IPV6_ADDR;
			break;
		default:
			loglog(RC_LOG_SERIOUS, "NAT-Traversal: "
				"invalid addrtypeof()=%d", addrtypeof(ip));
			return FALSE;
	}

	if (!out_struct(&natoa, &isakmp_nat_oa, outs, &pbs))
		return FALSE;

	if (!out_raw(ip_val, ip_len, &pbs, "NAT-OA"))
		return FALSE;

#if 0
	DBG_dump("NAT-OA (S):", ip_val, ip_len);
#endif

	close_output_pbs(&pbs);
	return TRUE;
}

void nat_traversal_show_result (u_int32_t nt)
{
	const char *mth = NULL, *rslt = NULL;
	switch (nt & NAT_TRAVERSAL_METHOD) {
		case LELEM(NAT_TRAVERSAL_IETF_00_01):
			mth = natt_methods[0];
			break;
		case LELEM(NAT_TRAVERSAL_IETF_02_03):
			mth = natt_methods[1];
			break;
	}
	switch (nt & NAT_T_DETECTED) {
		case 0:
			rslt = "no NAT detected";
			break;
		case LELEM(NAT_TRAVERSAL_NAT_BHND_ME):
			rslt = "i am NATed";
			break;
		case LELEM(NAT_TRAVERSAL_NAT_BHND_PEER):
			rslt = "peer is NATed";
			break;
		case LELEM(NAT_TRAVERSAL_NAT_BHND_ME) | LELEM(NAT_TRAVERSAL_NAT_BHND_PEER):
			rslt = "both are NATed";
			break;
	}
	loglog(RC_LOG_SERIOUS,
		"NAT-Traversal: Result using %s: %s",
		mth ? mth : "unknown method",
		rslt ? rslt : "unknown result"
		);
}

int nat_traversal_espinudp_socket (int sk, u_int32_t type)
{
	int r;
	r = setsockopt(sk, SOL_UDP, UDP_ESPINUDP, &type, sizeof(type));
	if ((r<0) && (errno == ENOPROTOOPT)) {
		loglog(RC_LOG_SERIOUS,
			"NAT-Traversal: ESPINUDP(%d) not supported by kernel -- "
			"NAT-T disabled", type);
		_nat_t_activated = FALSE;
	}
	return r;
}

void nat_traversal_new_ka_event (void)
{
	if (_ka_evt) return;  /* Event already schedule */
	event_schedule(EVENT_NAT_T_KEEPALIVE, _kap, NULL);
	_ka_evt = 1;
}

static void nat_traversal_send_ka (struct state *st)
{
	static unsigned char ka_payload = 0xff;
	chunk_t sav;

#if 0
	DBG_log("ka_event: send NAT-KA");
#endif

	/** save state chunk */
	setchunk(sav, st->st_tpacket.ptr, st->st_tpacket.len);

	/** send keep alive */
	setchunk(st->st_tpacket, &ka_payload, 1);
	_send_packet(st, "NAT-T Keep Alive", FALSE);

	/** restore state chunk */
	setchunk(st->st_tpacket, sav.ptr, sav.len);
}

void nat_traversal_ka_event (void)
{
	unsigned int _kap_st = 0;

	/**
	 * Find ISAKMP States with NAT-T and send keep-alive
	 */
	static void nat_traversal_ka_event_state (struct state *st)
	{
		const struct connection *c = st->st_connection;
		if (!c) return;
		if ( ((st->st_state == STATE_MAIN_R3) ||
				(st->st_state == STATE_MAIN_I4)) &&
			(st->nat_traversal & NAT_T_DETECTED) &&
			(st->nat_traversal & LELEM(NAT_TRAVERSAL_NAT_BHND_ME)) ) {
			/**
			 * - ISAKMP established
			 * - NAT-Traversal detected
			 * - NAT-KeepAlive needed (we are NATed)
			 */
			if (c->newest_isakmp_sa != st->st_serialno) {
				/** 
				 * if newest is also valid, ignore this one, we will only use
				 * newest. 
				 */
				struct state *st_newest;
				st_newest = state_with_serialno(c->newest_isakmp_sa);
				if ((st_newest) && ((st_newest->st_state==STATE_MAIN_R3) ||
					(st_newest->st_state==STATE_MAIN_I4)) &&
					(st_newest->nat_traversal & NAT_T_DETECTED) &&
					(st_newest->nat_traversal & LELEM(NAT_TRAVERSAL_NAT_BHND_ME))) {
					return;
				}
			}
			set_cur_state(st);
			nat_traversal_send_ka(st);
			reset_cur_state();
			_kap_st++;
		}
	}

	_ka_evt = 0;  /* ready to be reschedule */

	for_each_state((void *)nat_traversal_ka_event_state);

	if (_kap_st) {
		/**
		 * If there are still states who needs Keep-Alive, schedule new event
		 */
		nat_traversal_new_ka_event();
	}
}

void process_pfkey_nat_t_new_mapping(
	struct sadb_msg *msg __attribute__ ((unused)),
	struct sadb_ext *extensions[SADB_EXT_MAX + 1])
{
	struct sadb_sa *sa = (void *) extensions[SADB_EXT_SA];
	struct sadb_address *srcx = (void *) extensions[SADB_EXT_ADDRESS_SRC];
	struct sadb_address *dstx = (void *) extensions[SADB_EXT_ADDRESS_DST];
	struct sockaddr *srca, *dsta;
	ip_address src, dst;
	u_int16_t sport, dport;
	err_t ugh = NULL;

	static void nat_traversal_find_new_mapp_state (struct state *st)
	{
		struct connection *c = st->st_connection;

		if ((c) && (st->st_esp.present) &&
			sameaddr(&c->that.host_addr, &src) &&
			(c->that.host_port == sport) &&
			(st->st_esp.our_spi == sa->sadb_sa_spi)) {
			update_host_pair("NAT-T (KLIPS new mapping)", c,
				&c->this.host_addr, c->this.host_port,
				&dst, dport);
		}
	}

	if ((!sa) || (!srcx) || (!dstx)) {
		log("SADB_X_NAT_T_NEW_MAPPING message from KLIPS malformed: "
			"got NULL params");
		return;
	}

	srca = ((struct sockaddr *)(void *)&srcx[1]);
	dsta = ((struct sockaddr *)(void *)&dstx[1]);

	if ((srca->sa_family != AF_INET) || (dsta->sa_family != AF_INET)) {
		ugh = "only AF_INET supported";
	}
	else {
		initaddr((const void *) &((const struct sockaddr_in *)srca)->sin_addr,
			sizeof(((const struct sockaddr_in *)srca)->sin_addr),
			srca->sa_family, &src);
		sport = ntohs(((const struct sockaddr_in *)srca)->sin_port);
		initaddr((const void *) &((const struct sockaddr_in *)dsta)->sin_addr,
			sizeof(((const struct sockaddr_in *)dsta)->sin_addr),
			dsta->sa_family, &dst);
		dport = ntohs(((const struct sockaddr_in *)dsta)->sin_port);
		for_each_state((void *)nat_traversal_find_new_mapp_state);
	}

	if (ugh != NULL)
		log("SADB_X_NAT_T_NEW_MAPPING message from KLIPS malformed: %s", ugh);
}

bool
nat_traversal_port_float(struct state *st, struct msg_digest *md, bool in)
{
	struct connection *c = st->st_connection;
	struct iface *i = NULL;

	if ((in) && (md->iface) && (md->iface->ike_float == TRUE) &&
		(c->interface->ike_float == FALSE) &&
		(sameaddr(&md->iface->addr, &c->interface->addr))) {
		c->interface = md->iface;
		update_host_pair("NAT-T (Port floating)", c,
			NULL, NAT_T_IKE_FLOAT_PORT,
			&md->sender, md->sender_port);
		return TRUE;
	}
	else if ((!in) && (md->iface) && (md->iface->ike_float == FALSE) &&
		(c->interface->ike_float == FALSE)) {
		for (i = interfaces; i !=  NULL; i = i->next) {
			if ((sameaddr(&md->iface->addr, &i->addr)) &&
				(i->ike_float == TRUE)) {
				md->iface = i;
				c->interface = i;
				update_host_pair("NAT-T (Port floating)", c,
					NULL, NAT_T_IKE_FLOAT_PORT,
					&md->sender, NAT_T_IKE_FLOAT_PORT);
			}
		}
	}
	return FALSE;
}

#endif

