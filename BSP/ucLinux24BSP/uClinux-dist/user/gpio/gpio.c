#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>

#define inpw(port)	(*((volatile unsigned int *)port));
#define outpw(port,val)  (*((volatile unsigned int *)port)=(val));

#define Base_Addr               0xFFF00000;
#define BASE_IN_DEC		(unsigned int)4293918720;
#define GPIO_CFG                (Base_Addr+0x83000);

/*
#define writeGPIO(reg, val) 	(*((volatile unsigned int *)(GPIO_CFG+reg))=(val));
#define readGPIO(reg)           (*((volatile unsigned int *)(GPIO_CFG+reg)));
outpw ( address , val)
inpw (address)

eg

myVal = inpw (0xfff83020);

printk ('ADDRESS value is:  %s', myVal); 
*/


void show_eth(){
	int i;
	printf ("We are in show eth0\n");
	i= inpw (0xfff83020);
	printf ("value at 0xfff83020 is: %d",i);
}

void usage() {
	printf ("\n -r ADDRESS			Reads a value from hex address specified\n");
	printf (" -w ADDRESS VALUE		Writes a value to the hex address specified\n\n\n");
	printf (" -e 					Reads the ethernet gpio value\n");
	printf ("Example:  getaddr   -r   0xFFF83020\n\n");
}

void read_value(long loc) {
	int i;long where;
	
	printf ("HEX Value: %x\nDec Value: %d\n",loc,loc);
y	where = BASE_IN_DEC  + loc;
	printf ("Offset to read: %X\n", where);
	i = inpw (where);
	printf ("Value at %x is %d\n",where, i);
}


void write_value (long loc, int val) {
        int i;long where;
	where = BASE_IN_DEC + loc;

        printf ("HEX Value: %x\nDec Value: %d\n",loc,loc);
        printf ("Offset to write: %X\n", where);
        i = outpw (where, val);
        printf ("Writing Value at %x with  %d\n",where, val);
	printf ("\n\nReading Value again:\n");
	read_value (loc);
}
int main (int argc, char *const *argv)
{
	int c;
	char *endptr;
	long loc, val;

	if (argc < 2) {
		usage();
		return(0);
	}

	while  ((c = getopt (argc, argv, "r:w:eh")) != -1) {
		switch (c) {
			case 'r':
				printf ("got r!\n");
				loc = strtol(optarg, &endptr, 16);
				read_value(loc);
				//do read value, and show
				return(0);
			break;
			
			case 'w':
				printf ("got w!\n");
				loc = strtol(optarg,&endptr,16);
				val = atol( argv[optind++] );
				//printf ("loc: %d val: %d\n",loc,val);
				write_value (loc, val);
				//do write value and show
				return(0);
			break;
			
			case 'e':
				printf ("got e!\n");
				show_eth();
				return (0);
				break;
		}		
	}

	return (0);	
}
