
#include <sys/types.h>
#include <sys/socket.h>
/*
//#include <linux/kernel.h>
//#include <linux/wireless.h>
#include <linux/config.h>
#include <linux/types.h>
#include <linux/kernel.h>

#include <linux/sched.h> 
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h> 


//#include <linux/config.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
//#include <linux/mm.h>
#include <linux/socket.h>
#include <linux/in.h>
//#include <linux/inet.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
//#include <linux/etherdevice.h>
#include <linux/wireless.h>
*/


#include <stdio.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/sockios.h>



//#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>


int main (int argc, char *const *argv)
{
//	struct net_device *dev; 

	
	printf ("\nKicking eth0 into gear\n\n");


	int i, sockfd;
	static struct ifreq req;
	struct sockaddr_ll inside_address;

    	/* Low level socket */
	sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	if (sockfd < 0) {
		perror("Unable to create low level socket: ");
		exit(1);
    	}

memset(&inside_address, 0, sizeof(inside_address));

    	inside_address.sll_family = AF_PACKET;
    	inside_address.sll_protocol = htons(ETH_P_ALL);
    	inside_address.sll_hatype = ARPOP_REQUEST;
    	inside_address.sll_pkttype = PACKET_BROADCAST;
    	inside_address.sll_halen = 6;
    	strcpy(req.ifr_name, "eth0");
	ioctl(sockfd, SIOCGIFINDEX, &req);    
	

//	write_lock(&dev_base_lock); 
//	for (dev=dev_base; dev; dev = dev->next) {


		//netif_carrier_on(); 
//	}
//	write_unlock(&dev_base_lock); 

	
//	*(unsigned int volatile *)(0xfff83020) = 0x50000;
//	*(unsigned int volatile *)(0xfff83024) = 0; // Make sure all pins are in input mode.
//	ResetP(0);
//	ResetPhyChip(0);

	printf ("\nDid it reset?\n");
	//Main must return happily, or we kill the puppy.
	return (0);
}

