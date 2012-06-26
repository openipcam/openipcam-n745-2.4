
#include <linux/ledman.h>
#include <signal.h>
#include "main.h"
//#include <pthread.h>
#define NULL 0 //mcli add

int main (void) {
	
	//static sigset_t sig_mask;
	//int sig_recieved;
   
	struct sigaction sa;

   //sigemptyset(&sig_mask);
   //sigaddset(&sig_mask, SIGUSR1);

	memset(&sa,0,sizeof(struct sigaction));//mcli add
	sa.sa_handler = (void *)userHandler1;
	//mcli del sa.sa_mask = 0; //dont block any signals while this one is working
	sa.sa_flags = SA_RESTART; //restart the signal
	sigaction(SIGUSR1,&sa,NULL);

	ledman_cmd(LEDMAN_CMD_ON, LEDMAN_VPN);
	
	while(1) {
		printf("Waiting for signal\n");
		//sigwait(&sig_mask,&sig_recieved);
		pause();
		printf("signal Recieved\n");
	}
}

void userHandler1(void) {
	ledman_cmd(LEDMAN_CMD_OFF, LEDMAN_VPN);
	sleep(1);
	ledman_cmd(LEDMAN_CMD_ON, LEDMAN_VPN);
}
