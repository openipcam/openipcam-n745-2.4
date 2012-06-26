#if 1
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <sys/select.h>
#include <errno.h>
#include <sys/types.h>
#include <linux/route.h>
#include <sys/ioctl.h>
#include <string.h>
#else
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <utmp.h>
#include <mntent.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#endif

#include "inter.h"
extern unsigned char pptp_flag;
static struct rtentry rt;
static char name[8];
static unsigned int mgw;
void set_sockaddr(struct sockaddr_in *sin, unsigned long addr, unsigned short port)
{
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = addr;
	sin->sin_port = port;
}

int GetDefaultRoute()
{
	FILE *fp = fopen("/proc/net/route", "r");
	char buff[256];
		
	char *end;
	
	int sd;
	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd<0)
	    printf("Socket error!\n");
	    
	if(fp!=NULL)
	{
		
		char gate_addr[16];
		char dst_addr[16];
    	char mask_addr[16];
    	
    	unsigned int iflags, metric, refcnt, use, mtu, window, gateway,irtt,dst,mask;
    	memset(name,0,sizeof(name));
		memset(gate_addr,0,16);
		memset(dst_addr,0,16);
		memset(mask_addr,0,16);

    	fseek(fp,0,0);		
		fgets(buff, 255, fp);
		
		while (fgets(buff, 255, fp)){
			
				memset(&rt,0,sizeof(rt));
				sscanf(buff,"%8s%8s%8s%X%d%d%d%8s%d%d%h",
					&name[0],dst_addr,
					gate_addr,&iflags,&refcnt, &use, &metric,
					mask_addr,&mtu, &window, &irtt);
			
			dst = strtoul(dst_addr,&end,16);
			mask = strtoul(mask_addr,&end,16);
			gateway=strtoul(gate_addr,&end,16);
			mgw=gateway;
			if(!(iflags&RTF_GATEWAY))
			{
				continue;
			}
			//printf("mask:%x,dst:%x,name:%s\n",mask,dst,name);	
			if(read_config(CONFIG_FILE))
				AddHost(pppd_info.StartParam.StartParam.e.pcSeviceName,gateway,name);
				
			set_sockaddr((struct sockaddr_in *)&rt.rt_dst,0/*dst*/ , 0);
			set_sockaddr((struct sockaddr_in *) &rt.rt_gateway, strtoul(gate_addr,&end,16), 0);
			set_sockaddr((struct sockaddr_in *) &rt.rt_genmask, 0 /*mask*/, 0);
			//rt.rt_flags = iflags;
			rt.rt_metric = metric;
			rt.rt_mtu = mtu;
			rt.rt_window = window;
			rt.rt_irtt = irtt;			
			rt.rt_dev=&name[0];	

			rt.rt_flags = RTF_UP| RTF_GATEWAY;
			if ((ioctl(sd,SIOCDELRT, &rt)) < 0)
				printf("Cannot del default route:%d.\n",errno);

			break;
			
		}	
		fclose(fp);
		close(sd);
	}
}
void RestoreDefaultGateway()
{
	int sd;

	if(rt.rt_dev==NULL)
		return;

	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd<0)
	{
	    printf("Socket error!\n");
		return;
	}    

    if ((ioctl(sd,SIOCADDRT, &rt)) < 0)
	{ 
        printf("%s: Restore Cannot add route %x (%d).\n",rt.rt_dev,rt.rt_dst,errno);
		close(sd);
		return;
	}                    
    close(sd);

	if(mgw && read_config(CONFIG_FILE))
		DelHost(pppd_info.StartParam.StartParam.e.pcSeviceName,mgw,name);
}

void AddHost(char *host,unsigned int gw,char *ifname)
{
	if(!SetRoute(inet_addr(host),gw,0xFFFFFFFF,0,ifname,SIOCADDRT))
		printf("Add host OK!\n");
}
void DelHost(char *host,unsigned int gw,char *ifname)
{
	if(!SetRoute(inet_addr(host),gw,0xFFFFFFFF,0,ifname,SIOCDELRT))
		printf("Del host OK!\n");
}

static int SetRoute(u_int32_t IP,u_int32_t Gateway,u_int32_t Netmask,
    			short Metric, const char *pcInterface, int Action)
{
	 struct rtentry rt;
	 int fd;
	 if ((fd = socket(AF_INET,SOCK_DGRAM,0)) < 0) return 1;
	 
	 memset(&rt, 0, sizeof(rt));
	 set_sockaddr((struct sockaddr_in *) &rt.rt_dst, IP, 0);
	 set_sockaddr((struct sockaddr_in *) &rt.rt_gateway, Gateway, 0);
	 set_sockaddr((struct sockaddr_in *) &rt.rt_genmask, Netmask, 0);
	 rt.rt_metric = Metric + 1;
	 rt.rt_dev = (char *)pcInterface;
	 
	 rt.rt_flags = RTF_UP;
	    if (Gateway != 0L)
	     rt.rt_flags |= RTF_GATEWAY;
	    if ((IP & 0xFF000000) != 0L && Netmask == 0xFFFFFFFF)
	  rt.rt_flags |= RTF_HOST;
	 
	 if (ioctl(fd, Action, &rt) == -1)
	 {
	  fprintf(stderr, "ioctl errno = %d\n", errno);
	  close(fd);
	  return 1;
	 }
	 close(fd);
	 return 0;
}