#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <signal.h>
#include "pppd.h"
#include "pathnames.h"
#include "inter.h"
#include "system.h"

#include <errno.h>

extern unsigned char pptp_flag;
char * _PATH_SYSOPTIONS=NULL;
char *CONFIG_FILE =NULL;
extern char gl_username[64];
extern char gl_password[64];

static inline _syscall1(void,set_out_ip,unsigned long *,args);
static inline _syscall1(void,set_pptp,unsigned char*,args);
struct ppp_info pppd_info ={"","",{"","",""},"","",0,-1,0,0,"",""};

struct ppp_info * get_ppp_info()
{
	return &pppd_info;
}

void get_user_passwd(PPPDSTART *pStartParam)
{
//	printf("In  get_user_passwd\n");
	read_config(CONFIG_FILE);
	strlcpy(user,  pStartParam->StartParam.e.pcUserName, sizeof(user));     
	strlcpy(passwd, pStartParam->StartParam.e.pcUserPass, sizeof(passwd));		
	
	if((*gl_username)&&(*gl_password))
    {
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserName,gl_username);
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserPass,gl_password);
    }
    
	write_config(CONFIG_FILE);
}

int read_config(char * config_file)
{	
  static  pppd_flag=0;
  FILE *fp;
  PPPD_DEBUG(printf("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__));
  if((fp=fopen(config_file,"rb"))==NULL)
  {
  	printf("Open file %s failed\n",config_file);
  	return 0;
  }
  
  if(fread(&pppd_info,1,sizeof(struct ppp_info),fp))
  	{
		PPPD_DEBUG(printf("pppd_info.StartParam.StartParam.e.pcSeviceName=%s\n",pppd_info.StartParam.StartParam.e.pcSeviceName));
  		fclose(fp);  		
  		return 1;
  	}
  	
  	printf("read file failed\n");
  	fclose(fp);
  	return 0;  
}
  
int write_config(char * config_file)
{	
  static  pppd_flag=0;
  FILE *fp;
  int i;
  
  if((fp=fopen(config_file,"wb"))==NULL)
  {
  	printf("Open write file %s failed\n",config_file);
  	return 0;
  }
  #if 0
	printf("pppd_info.status===%x\n",pppd_info.status);
	printf("pppd_info.live_sec===%x\n",pppd_info.live_sec);
	printf("pppd_info.session_num===%x\n",pppd_info.session_num);
	printf("pppd_info.ser_mac: ");
	for(i=0;i<6;i++)
 		printf("%x ",pppd_info.ser_mac[i]);
 	printf("\n");
	#endif
 // printf("pppd_info.StartParam.StartParam.e.DisConnectTime 3===%d\n",pppd_info.StartParam.StartParam.e.DisConnectTime);
  if(fwrite(&pppd_info,1,sizeof(struct ppp_info),fp)>0)
  	{
  		fclose(fp);  		
  		return 1;
  	}  	
  	printf("	@@write file failed\n");
  	fclose(fp);
  	return 0;  
}

////////////////////////////////////////////////////
void  set_ppp_info_from_dial(long  loc_addr ,
		long rem_addr,long dns1addr,long dns2addr,long dns3addr,
		long gateway_addr,long netmask) 
{
	 
     int  ip;
	 char num[32];    
	 
	 read_config(CONFIG_FILE);
	 ip=loc_addr;	
	 slprintf(num, sizeof(num), "%d.%d.%d.%d", (ip  ) & 0xff,
		     (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip>>24) & 0xff);
	 strcpy(pppd_info.loc_addr,num );
	    /***************/ 
     ip=rem_addr;     
	 slprintf(num, sizeof(num), "%d.%d.%d.%d", (ip) & 0xff,
		     (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip>>24) & 0xff);
	 strcpy(pppd_info.rem_addr,num );
	    /*********************/
	 ip=dns1addr; 	
	 slprintf(num, sizeof(num), "%d.%d.%d.%d", (ip ) & 0xff,
		     (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip>>24) & 0xff);
	 strcpy(pppd_info.dnsaddr[0],num );  
	 
	 ip=dns2addr; 	
	 slprintf(num, sizeof(num), "%d.%d.%d.%d", (ip ) & 0xff,
		     (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip>>24) & 0xff);
	 strcpy(pppd_info.dnsaddr[1],num );      
	 
	 ip=dns3addr; 	
	 slprintf(num, sizeof(num), "%d.%d.%d.%d", (ip ) & 0xff,
		     (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip>>24) & 0xff);
	 strcpy(pppd_info.dnsaddr[2],num );    
	 /********************/
   	
   	//printf("live_sec====%d\n",live_sec);
   	//printf("session_num====%d\n",session_num);
    //pppd_info.live_sec=live_sec;
   // pppd_info.session_num=session_num;

    ip=gateway_addr; 	
	slprintf(num, sizeof(num), "%d.%d.%d.%d", (ip ) & 0xff,
		     (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip>>24) & 0xff);
	strcpy(pppd_info.gateway,num ); 
	/********************/
	ip=netmask; 	
	slprintf(num, sizeof(num), "%d.%d.%d.%d", (ip ) & 0xff,
		     (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip>>24) & 0xff);
	strcpy(pppd_info.netmask,num ); 
   /********************/
    //pppd_info.status=status;
    if((*gl_username)&&(*gl_password))
    {
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserName,gl_username);
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserPass,gl_password);
    }
    
    write_config(CONFIG_FILE);
}

void clear_ppp_info(void)
{
	pid_t pid;
	PPPDSTART  param;
	read_config(CONFIG_FILE);
	pid=pppd_info.thttpd_pid;
	param=pppd_info.StartParam;
	memset((char *)(&pppd_info),0, sizeof( struct ppp_info));
	pppd_info.StartParam=param;
	
	pppd_info.thttpd_pid=pid;
	
	pppd_info.status = pppd_info.status&0xFFFFFF00;
	pppd_info.status = CONNECTION_OFF|pppd_info.status;
	
	if((*gl_username)&&(*gl_password))
    {
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserName,gl_username);
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserPass,gl_password);
    }
    
	write_config(CONFIG_FILE);
}


void set_pppd_status(long status)
{
	read_config(CONFIG_FILE);
	pppd_info.status = pppd_info.status & 0xFFFFFF00; //mcli
	pppd_info.status = status|pppd_info.status;
	
	if((*gl_username)&&(*gl_password))
    {
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserName,gl_username);
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserPass,gl_password);
    }
    
	write_config(CONFIG_FILE);	
}


void set_ppp_dial_info(PPPDSTART *pStartParam)
{
	//ok;
	pStartParam=pStartParam;
	return;
	
}

void set_ppp_idle_time(void) 
{
	read_config(CONFIG_FILE);
	idle_time_limit=pppd_info.StartParam.StartParam.e.DisConnectTime;
//	printf("set_ppp_idle_time idle_time_limit===%d\n",idle_time_limit);
	
	if((*gl_username)&&(*gl_password))
    {
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserName,gl_username);
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserPass,gl_password);
    }
    
	write_config(CONFIG_FILE);
	//printf("pppd_info.StartParam.StartParam.e.DisConnectTime===%d\n",pppd_info.StartParam.StartParam.e.DisConnectTime);
}

//  start for checking parameter
void printf_ppp_info(PPPDSTART *pStartParam)
{
	printf("pcUserName %s\n",pStartParam->StartParam.e.pcUserName);    /* 用户名 */
    printf("pcUserPass %s\n",pStartParam->StartParam.e.pcUserPass );    /* 用户密码 */
    printf("pcSeviceName %s\n",pStartParam->StartParam.e.pcSeviceName);    /* ISP服务名称 */
    printf("session_num %d\n",pppd_info.session_num);
    
}
// wirte /etc/hosts for gethostbyname 
int  set_host_info(char *filename,unsigned int local_ip, char * local_name)
{
 
	FILE *fp;
	unsigned int ip=local_ip;
	char num[32];
	 
	fp = fopen(filename, "w+");
    if (fp == NULL) {
	      printf("   open   file %s:  failed\n", filename);
		  return 1;	
     } 
  	
  	 slprintf(num, sizeof(num), "%d.%d.%d.%d", (ip ) & 0xff,
		     (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip>>24) & 0xff);
     strcat(num,"	");
     strcat(num,"winbond");
   //  printf("IN set_host_info \n");
     
     if(fputs(num,fp)<0)
     	{
     		fclose(fp);
     	  	return 1;
     	  }
     	
     fclose(fp); 
     return 0;	
}
// add for lsshi kernel syscall
void ppp_set_out_ip(unsigned int add)
{
	set_out_ip(&add);
}

void ppp_do_other()
{ 
    pid_t pid;
    int signal_num;
    read_config(CONFIG_FILE);
    
 //   printf("In ppp_do_other \n");
    
	if(pppd_info.pWRSetRules)
	{
		if(pppd_info.pWRSetRules())
		{
			printf("ppp_do_other failed\n");
		}
		return;
	}
//	printf(" pppd no call back\n");
	
	pid=pppd_info.thttpd_pid;
	

//	printf("pid===%x,sig:%d\n",pid,SIGWINCH);
 	if(pptp_flag==0)
		signal_num=kill(pid,SIGWINCH); //notify http server we have done ok!
	else
	{	
		printf("pptp_flag=%d\n",pptp_flag);
		set_pptp(&pptp_flag);
		signal_num=kill(pid,29);
	}

	if(signal_num==0){
		printf("Signal Sucess!\n");			
//	printf("      PPPoE  kill  %u signal SIGWINCH %u  \n",pid,SIGWINCH);
    }
   else if(signal_num==-1)
   {
     	printf( "Signal errno %d\n",errno);
   }
   
  // printf("Final ppp_do_other\n");
}
///////////////////////////////////////////////

int pppd_set_run(status)
{
	unsigned long flag_status;
	unsigned long flag;
	unsigned long mask;
	
	read_config(CONFIG_FILE);
	flag_status = pppd_info.status;
	
	if(status)
	{
		
	  flag =  flag_status | PPPD_FLAG_RUN ;
	   
//	   printf("		PPPD start RUN   @@\n");
	  
	} 
	else
    {
	  mask = ~PPPD_FLAG_RUN;
	  
	  flag =  flag_status & mask;    	
//	  printf("    PPPD stop RUN\n");
	}	
	pppd_info.status = flag;
	
	if((*gl_username)&&(*gl_password))
    {
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserName,gl_username);
    	strcpy(pppd_info.StartParam.StartParam.e.pcUserPass,gl_password);
    }
    
	write_config(CONFIG_FILE);
	return 0;
}
void update_ppp_status(int sign)
{
   
   // printf("In update_ppp_status 2003-1-8 11:01 \n");
    update_link_stats(0);
   
    write_config(CONFIG_FILE);
    memcpy(pppd_info.ser_mac, ser_mac, 6);//mcli add
    pppd_info.session_num=m_pppoe_id;//mcli add
    
    #if 0
    printf("#######PPP STATUS\n");
    
    printf("***live sec   :%d\n",pppd_info.live_sec);
    printf("***session id :%d\n",pppd_info.session_num);
    printf("***in bytes   :%d\n",pppd_info.bytes_in);
    printf("***out bytes  :%d\n",pppd_info.bytes_out);
    #endif 
  	return;
}
///////////////////////////////////////////////////////
void ppp_do_exit()
{
	pid_t pid;
    int signal_num;
    read_config(CONFIG_FILE); 
	pid=pppd_info.thttpd_pid;
	pppd_set_run(PPPD_STOP_RUN);
   #if 0 //2003-1-9 11:02
	signal_num=kill(pid,32); //notify http server we have done ok!
	if(signal_num==0){
		printf("Signal Sucess! 111");			
	//printf("      PPPoE  kill  %u signal 32 \n",pid );
    }
   else if(signal_num==-1)
   {
     	printf( "Signal errno %d\n",errno);
   }
   #endif
	
}

void GetMACSID(struct MAC_SID *rMSinfo)
{
	int i;
	
	read_config(CONFIG_FILE);
	rMSinfo->session_num=pppd_info.session_num;
	//printf("pppd_info.ser_mac:");
	for(i=0;i<6;i++)
		printf("pppd_info.ser_mac[%d]=%x\n",i,pppd_info.ser_mac[i]);
		
	memcpy(rMSinfo->ser_mac, pppd_info.ser_mac, 6);
	
	return;
}
//////////////////////////////////////////////////////////////////
int ppp_try_set_demand()  
{
	unsigned long flag_status;
	unsigned long flag;
	read_config(CONFIG_FILE);
	flag_status = pppd_info.status;
	flag =  flag_status & PPPD_DEMAND;
//	printf("Ppp_Try_Set_Demand flag=%x  persist=%x\n",flag,persist);
	//return 0;
	persist = 1;
	if(flag)
	{
		demand  = 1;
		//persist = 1;
		printf("		DIAL ON DEMAND   @@\n");
		return 1;
	}
	return 0;
}