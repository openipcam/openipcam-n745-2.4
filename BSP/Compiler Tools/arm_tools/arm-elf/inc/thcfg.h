struct thread_cfg{
	unsigned long manager_stack_size;
	unsigned long each_thread_stack_size;
	unsigned long main_thread;	
};
extern struct thread_cfg thcfg;
