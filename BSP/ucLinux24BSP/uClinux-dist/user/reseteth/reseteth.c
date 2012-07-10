#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <sys/ioctl.h> 
//#include <net/if.h>
#include <linux/netdevice.h>

//static inline void netif_carrier_on(struct net_device *dev);

void usage(char **argv) {
	printf  ("\n\nUsage is\n");
	printf  ("%s eth0\n",argv[0]);
}
 
int main (int argc, char**argv)  { 
  int sockfd; 
  int dev; 
  char buffer[1024]; 
  struct ifreq ifr;
	int c;


	if (argc < 2) {
                usage(argv);
                return(0);
        }

        while  ((c = getopt (argc, argv, "")) != -1) {
		printf  ("%s :", c); 
	}


  sprintf(ifr.ifr_name, "eth0");

  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
  if(sockfd < 0){ 
    printf ("Oh noes! didn't get a socket\n\n"); 
    return(1); 
  }

  dev = ioctl(sockfd, SIOCGIFHWADDR, (char *)&ifr); 
  if(dev < 0){ 
    printf ("ioctl"); 
    return(1); 
  } 

  printf("fetched HW address with ioctl on sockfd.\n"); 
  printf("HW address of interface is: %02X:%02X:%02X:%02X:%02X:%02X\n", 
    (unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[0],
    (unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[1],
    (unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[2],
    (unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[3],
    (unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[4],
    (unsigned char)ifr.ifr_ifru.ifru_hwaddr.sa_data[5]
  ); 
  

    printf ("Got a device:  %d\n",dev);

//    printf ("I shall try kicking it:\n");
//    netif_carrier_on(dev);
 
  return (0); 
}
