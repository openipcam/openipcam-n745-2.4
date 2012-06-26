/*
 * NETBIOS protocol formats
 *
 * @(#) $Header: /usr/local/cvsroot/W90N745/uClinux-dist/user/tcpdump/netbios.h,v 1.1.1.1 2006-07-11 09:33:19 andy Exp $
 */

struct p8022Hdr {
    u_char	dsap;
    u_char	ssap;
    u_char	flags;
};

#define	p8022Size	3		/* min 802.2 header size */

#define UI		0x03		/* 802.2 flags */

