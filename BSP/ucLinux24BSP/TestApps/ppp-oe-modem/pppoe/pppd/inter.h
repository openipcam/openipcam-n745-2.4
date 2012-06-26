#ifndef INTER_H
#define INTER_H

#define CONNECTION_OFF 2   //-1 
#define CONNECTION_ON  1
#define CONNECTION_TRYING 0

#define DIAL_TYPE_MODEM  1
#define DIAL_TYPE_PPPOE  0
#define DIAL_TYPE_PPTP   2

#define PPPD_DEMAND     (0x1<<16)
#define PPPD_FLAG_RUN		(0x1<<8)
#define PPPD_START_RUN		1
#define PPPD_STOP_RUN		0


//#define CONFIG_FILE "/swap/.pppd_info"
#define CONFIG_FILE_PPPOE   "/swap/.pppd_info"
#define CONFIG_FILE_PPTP    "/swap/.pptp_info"
#define CONFIG_FILE_MODEM   "/swap/.modem_info"

typedef struct
{
    char pcUserName[64];    /* 用户名 */
    char pcUserPass[64];    /* 用户密码 */
    char pcSeviceName[64];    /* ISP服务名称 */
    int DisConnectTime;     /* 以秒计算的闲置挂断时间 */
} PPPoESTART;

typedef struct
{
    char pcUserName[64];    /* 用户名 */
    char pcUserPass[64];    /* 用户密码 */
    char pcSeviceName[64];    /* ISP服务名称 */
    int DisConnectTime;    /* 以秒计算的闲置挂断时间 */
} PPPMSTART;

typedef struct
{
	union 
	{
		PPPoESTART  e;
		PPPMSTART   m;
	}StartParam;
}PPPDSTART;

struct ppp_info{
 char   loc_addr[32]; //本地ip
 char   rem_addr[32]; //对方ip
 char   dnsaddr [3][32]; //dns 
 char   gateway[32];
 char   netmask[32];
 unsigned long   status;       // 2:未连上 ；1 连上 ； 0 正在连；
 long   live_sec ;    //连接时间
 long   session_num;  //session number
 unsigned int	bytes_in;
 unsigned int	bytes_out;
 PPPDSTART StartParam;
 int (*pWRSetRules)();
 pid_t thttpd_pid;
 unsigned char   ser_mac[8];//ETH_ALEN==6
 unsigned char   ppp_if[8];
};
unsigned char   ser_mac[8];

struct MAC_SID
{
	long session_num;
	unsigned char   ser_mac[8];
};

#define LIB_GET_STATUS(x) \
		    return ((x)&0xFFFF); 
#define LIB_GET_FLAG(x) \
			return ((x)>>16);
			
extern char * CONFIG_FILE;			
extern struct ppp_info pppd_info ;
extern int m_pppoe_id;
extern int old_pppoe_id;
struct ppp_info * get_ppp_info();

extern void ppp_set_out_ip(unsigned int);
 
int  read_config(char * file ); 
int  write_config(char * file); 

void  set_ppp_info_from_dial(long  loc_addr ,
		long rem_addr,long dns1addr,long dns2addr,long dns3addr,
		long gateway_addr,long netmask);
void clear_ppp_info(void);
void set_pppd_status(long status);
void set_ppp_dial_info(PPPDSTART *pStartParam);
extern void set_ppp_idle_time(void);
void get_user_passwd(PPPDSTART *pStartParam);
void printf_ppp_info(PPPDSTART *pStartParam);
extern int  set_host_info(char *filename,unsigned int local_ip, char * local_name);
//void ppp_set_out_ip(unsigned int add);
void ppp_do_other();

int pppd_set_run(int status);
void update_ppp_status(int sign);

void ppp_do_exit();

void GetMACSID(struct MAC_SID *rMSinfo);

int ppp_try_set_demand();

#endif
