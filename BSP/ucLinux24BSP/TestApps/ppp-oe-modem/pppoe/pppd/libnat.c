#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>

//#define TEST_NATA
#include "inter.h"
#include "w740nat.h"

/*  Add to net/ipv4/Af_inet.c line 821 
//winbond nata add  
	extern int nata_ioctl(int cmd,void *arg);
      		case 0x8968 :
      		case 0x8969 :
      			return (nata_ioctl(cmd,(void *)arg));
*/
#define NATA_CMD 0x8968
//#define NATA_CMD 0x891f
static int NATA_fd=0;

static int fd_init()
{
	if(NATA_fd!=0) 
		close(NATA_fd);
	NATA_fd=socket(PF_INET,SOCK_DGRAM,0);
	if(NATA_fd<0)
	{
		 printf("sokcet failed fd %d \n",NATA_fd);
		 return -1;
	 }	 
	return 0;
}	
static int fd_close()
{
	if(NATA_fd!=0)close(NATA_fd);
	NATA_fd=0;//mcli 2003-1-30 11:13
	return  0;
}	

int do_ioctl(struct   nata_req *req)
{
	int ret;
	fd_init();
	ret=ioctl(NATA_fd,NATA_CMD,req);
	if(ret<0)
		printf("ioctl failed fd %d ret %d %s\n",NATA_fd,ret,strerror(errno));
	fd_close();
	return ret;
}
	
int NATA_StartPPPoE(unsigned short session,unsigned long rmad )
{
	struct nata_req Req;
	int fd;
	struct  ppp_info info;
    
    Req.cmd=NATA_SET_PPPOE_HEADER;
    
    #if 1
    Req.type1.pppoe_header.sid=session;
    Req.type1.pppoe_header.PPPoE_rmad=rmad;
    #else
    #define PPPD_FILE  "/swap/.pppd_info"
    
    fd=open(PPPD_FILE,O_RDWR);
    if(fd<0){
    	perror("AA");
    	printf("failed open %s\n",PPPD_FILE);
    	//return -1;
    } 
    read(fd,&info,sizeof(info));
     Req.type1.pppoe_header.sid=session;
   // Req.type1.pppoe_header.sid=info.session_num;
    printf("info.session_num %d \n",info.session_num);
    Req.type1.pppoe_header.PPPoE_rmad=inet_addr(&info.loc_addr);
    memcpy(Req.type1.pppoe_header.remote_mac,info.ser_mac,8);
    {
    	int i;
    	 for(i=0;i<6;i++)
    	{
    		printf("%x ",info.ser_mac[i]);
    	}
    			
    }
    Req.type1.pppoe_header.start_pppoe=1;
   // Req.type1.pppoe_header.PPPoE_PPP_Protocol=0x0021;	
    printf("\nPPPOE:session %d gateway %x \n",Req.type1.pppoe_header.sid,\
    	Req.type1.pppoe_header.PPPoE_rmad);
    #endif
   return  do_ioctl(&Req);
}


int NATA_Stop()
{
	struct nata_req Req;
    Req.cmd=NATA_STOP;
   return  do_ioctl(&Req);
}
	
	
int NATA_Start()
{
	struct nata_req Req;
    Req.cmd=NATA_START;
   return  do_ioctl(&Req);
}
int NATA_Reset()
{
	struct nata_req Req;
    Req.cmd=NATA_RESET;
   return  do_ioctl(&Req);
}
int NATA_StopPPPoE()
{
	struct nata_req Req;
    Req.cmd=NATA_SET_PPPOE_HEADER;
  //  Req.type1.pppoe_header.sid=0;
     Req.type1.pppoe_header.start_pppoe=0;
   return  do_ioctl(&Req);
}
#if 0
int main(int argc,char ** argv)
{
	unsigned short  session;
	if(argc==1){
		printf("Please give opion:Stop,Start,Reset,PPPoe session\n");
		return 0;
	}
	if(!strcmp(argv[1],"stop"))
		 NATA_Stop();
	else if(!strcmp(argv[1],"start"))	 
		NATA_Start();
	else if(!strcmp(argv[1],"pppoe"))
		{
			
			if(argc==2)
				NATA_StopPPPoE();
			else
			{
				unsigned long rmad;
				session=strtoul(argv[2],NULL,10);

				rmad=inet_addr(argv[3]);
		//printf("PPPoE session %d,Rmad 0x%x\n",session,hton(rmad));
				NATA_StartPPPoE(session,rmad);
				
			}
		}
	else
	{
		printf("error option,please give option,start,stop,pppoe\n");
		return 0;				
	}
}	
#endif

#ifdef TEST_NATA
int main(int argc,char ** argv)
{
	unsigned short  session;
	
	if(argc>1)
		session=strtoul(argv[1],NULL,10);
	else
		session=10;
			
	printf("Begin To Test NATA API  CBHUNAG v0.1\n");	
	printf("NATA prepare to START\n");
	NATA_Start();	
	sleep(1);
	
	printf("NATA prepare to RESET\n");
	NATA_Reset();	
	sleep(1);

	printf("NATA prepare to START PPPOE SESSION %d\n",session);
	NATA_StartPPPoE(session);	
	sleep(1);
	printf("NATA prepare to STOP PPPoE \n");
	NATA_StopPPPoE();	
	sleep(1);
	printf("NATA prepare to STOP\n");
	NATA_Stop();
	printf("OK! Bye\n");
	return 0;
}		

#endif			 
