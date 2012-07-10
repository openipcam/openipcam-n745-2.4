#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>


//Our int's are actually int32 so unsigned int is 0xFFFFFFFF size

#define inpw(port)	(*((volatile unsigned int *)port));
#define outpw(port,val)  (*((volatile unsigned int *)port)=(val));

#define Base_Addr               0xFFF00000;
#define BASE_IN_DEC		4293918720;
#define GPIO_CFG                Base_Addr+0x83000;
#define GPIO_OFFSET		0xFFF80000;

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


int bit_value (int value,int pos) {
	if ( ((value) & (1<<(pos))) )
           { 
		return (1);
	    }
	else {
		return (0);
	}
}


void show_p(){
	int i;	

	printf ("\n");

	//SHOW GPIO Port 0 (directly)
	i= inpw (0xfff83000);
        printf ("\nGPIO_CFG0 0xFFF830000 Value: %d\n",i);
        printf ("\nPT0CFG0:  %d %d", bit_value(i,0),bit_value(i,1));
        printf ("\nPT0CFG1:  %d %d", bit_value(i,2),bit_value(i,3));
        printf ("\nPT0CFG2:  %d %d", bit_value(i,4),bit_value(i,5));
        printf ("\nPT0CFG3:  %d %d", bit_value(i,6),bit_value(i,7));
        printf ("\nPT0CFG4:  %d %d", bit_value(i,8),bit_value(i,9));
	printf ("\n");

	//SHOW GPIO_DIR0
	i= inpw (0xFFF83004);
	printf ("\nGPIO_DIR0 0xFFF83004 Value: %d\n",i);
	printf ("\nOMDEN0[4:0]:  %d %d %d %d %d\n", bit_value (i,0), bit_value(i,1), bit_value (i,2),bit_value (i,3), bit_value(i,4) );
	printf ("\nPUPEN0[3:0]:  %d %d %d %d\n", bit_value(i,16), bit_value(i,17), bit_value(i,18), bit_value (i,19));

	//SHOW GPIO_DATAOUT0
	i= inpw (0xFFF83008);
	printf ("\nDATAOUT0:  Value is: %d\n",i);
	i=i <<3;
	i=i >>3;
	printf ("\nDATAOUT0: Bitwise cleared value is: %d \n",i);



/*        //SHOW GPIO location VAL
        printf ("\nUSING GPIO + val: %d\n",val);
        j = (unsigned int)GPIO_OFFSET+val;
        printf ("\nUsing GPIO Value %X\n",j);
        i = inpw (j);
        printf ("GPIO 1 Port 0  LOC=%X vs 0xFFF83020? Value: %d\n",j,i);
        printf ("PT0CFG0:  %d %d", bit_value(i,0),bit_value(i,1));
        printf ("\nPT0CFG1:  %d %d", bit_value(i,2),bit_value(i,3));
        printf ("\nPT0CFG2:  %d %d", bit_value(i,4),bit_value(i,5));
        printf ("\nPT0CFG3:  %d %d", bit_value(i,6),bit_value(i,7));
        printf ("\nPT0CFG4:  %d %d", bit_value(i,8),bit_value(i,9));
	printf ("\n");

*/

        //SHOW GPIO PORT 1 CONFIG REGISTER
        i = inpw (0xfff83010);
        printf ("\nGPIO Port 1 0xFFF83010 CONFIG REGISTER Value: %d\n",i);
        printf ("\nPT1CFG0:  %d %d", bit_value(i,0),bit_value(i,1));
        printf ("\nPT1CFG1:  %d %d", bit_value(i,2),bit_value(i,3));
	printf ("\n");

	//SHOW GPIO_DIR1
	i= inpw (0xFFF83014);
        printf ("\nGPIO_DIR1 0xFFF83014 Value: %d\n",i);
        printf ("\nOMDEN2[7:0]:  %d %d %d %d %d %d %d\n", bit_value (i,0), bit_value(i,1), bit_value (i,2),bit_value (i,3), bit_value(i,4), 
                bit_value (i,5), bit_value(i,6), bit_value (i,7));

        printf ("\nOMDEN2[9:8]: %d %d\n", bit_value(i,8), bit_value (i,9));



	//SHOW GPIO_CFG2
	i = inpw (0xFFF83020);
	printf ("\nGPIO_CFG2  0xFFF83020 Value: %d\n",i);
        printf ("\nPT2CFG0:  %d %d", bit_value(i,0),bit_value(i,1));
        printf ("\nPT2CFG1:  %d %d", bit_value(i,2),bit_value(i,3));
        printf ("\nPT2CFG2:  %d %d", bit_value(i,4),bit_value(i,5));
        printf ("\nPT2CFG3:  %d %d", bit_value(i,6),bit_value(i,7));
        printf ("\nPT2CFG4:  %d %d", bit_value(i,8),bit_value(i,9));
        printf ("\nPT2CFG0:  %d %d", bit_value(i,10),bit_value(i,11));
        printf ("\nPT2CFG1:  %d %d", bit_value(i,12),bit_value(i,13));
        printf ("\nPT2CFG2:  %d %d", bit_value(i,14),bit_value(i,15));
        printf ("\nPT2CFG3:  %d %d", bit_value(i,16),bit_value(i,17));
        printf ("\nPT2CFG4:  %d %d", bit_value(i,18),bit_value(i,19));
	printf ("\n");

	//SHOW GPIO_DIR2
	i= inpw (0xFFF83024);
	printf ("\nGPIO_DIR2 0xFFF83024 Value: %d\n",i);
	printf ("\nOMDEN2[7:0]:  %d %d %d %d %d %d %d %d\n", bit_value (i,0), bit_value(i,1), bit_value (i,2),bit_value (i,3), bit_value(i,4), 
		bit_value (i,5), bit_value(i,6), bit_value (i,7));

	printf ("\nOMDEN2[9:8]: %d %d\n", bit_value(i,8), bit_value (i,9));


	printf ("\nPUPEN2: %d %d %d %d %d %d %d %d %d",bit_value(i,0),bit_value(i,1), bit_value (i,2),bit_value (i,3), bit_value(i,4),
		 bit_value (i,5), bit_value(i,6), bit_value (i,7), bit_value(i,8), bit_value (i,9));


	//SHOW GPIO_CFG4
        i = inpw (0xFFF83040);
        printf ("\nGPIO_CFG4  0xFFF83040 Value: %d\n",i);
        printf ("\nPT4CFG10:  %d %d", bit_value(i,20),bit_value(i,21));
	printf ("\n");

	//SHOW GPIO_DIR4
	i= inpw (0xFFF83044);
        printf ("\nGPIO_DIR4 0xFFF83044 Value: %d\n",i);
        printf ("\nOMDEN4[10]:  %d\n", bit_value (i,10));
        printf ("\nPUPEN4[10]:  %d\n", bit_value (i,26));

	//SHOW GPIO_CFG5
	i = inpw (0xFFF83050);
        printf ("\nGPIO_CFG5  0xFFF83050 Value: %d\n",i);
        printf ("\nPT5CFG0:   %d %d", bit_value(i,0), bit_value(i,1));
        printf ("\nPT5CFG1:   %d %d", bit_value(i,2), bit_value(i,3));
        printf ("\nPT5CFG2:   %d %d", bit_value(i,4), bit_value(i,5));
        printf ("\nPT5CFG3:   %d %d", bit_value(i,6), bit_value(i,7));
        printf ("\nPT5CFG4:   %d %d", bit_value(i,8), bit_value(i,9));
        printf ("\nPT5CFG5:   %d %d", bit_value(i,10),bit_value(i,11));
        printf ("\nPT5CFG6:   %d %d", bit_value(i,12),bit_value(i,13));
        printf ("\nPT5CFG7:   %d %d", bit_value(i,14),bit_value(i,15));
        printf ("\nPT5CFG8:   %d %d", bit_value(i,16),bit_value(i,17));
        printf ("\nPT5CFG9:   %d %d", bit_value(i,18),bit_value(i,19));
        printf ("\nPT5CFG10:  %d %d", bit_value(i,20),bit_value(i,21));
        printf ("\nPT5CFG11:  %d %d", bit_value(i,22),bit_value(i,23));
        printf ("\nPT5CFG12:  %d %d", bit_value(i,24),bit_value(i,25));
	printf ("\n");


	//SHOW GPIO_DIR5
	i= inpw (0xFFF83054);
        printf ("\nGPIO_DIR5 0xFFF83054 Value: %d\n",i);
        printf ("\nOMDEN5[7:0]:  %d %d %d %d %d %d %d %d\n", bit_value (i,0), bit_value(i,1), bit_value (i,2),bit_value (i,3), bit_value(i,4), 
                bit_value (i,5), bit_value(i,6), bit_value (i,7));
        printf ("\nOMDEN5[12:8]: %d %d %d %d %d\n", bit_value(i,8), bit_value (i,9), bit_value(i,10), bit_value(i,11), bit_value(i,12));

	printf ("\nPUPEN5[7:0]:  %d %d %d %d %d %d %d %d\n", bit_value (i,16), bit_value(i,17), bit_value (i,18),bit_value (i,19), bit_value(i,20), 
                bit_value (i,21), bit_value(i,22), bit_value (i,23));

        printf ("\nPUPEN5[12:8]: %d %d %d %d %d\n", bit_value(i,24), bit_value (i,25), bit_value(i,26), bit_value(i,27), bit_value(i,28));


	printf ("\n\n");//Putting it here, as we'll be adding lots of other shit above...
}


void usage() {
	printf ("\n -r ADDRESS at offset 0xFFF8000 + r value   Reads a value from hex address specified\n");
	printf (" -w ADDRESS VALUE		Writes a value to the hex address specified\n\n\n");
	printf (" -e 					Reads the ethernet gpio value\n");
	printf (" -p					Prints the gpio port value bits\n");
	printf ("Example:  getaddr   -r 3020\n\n");
}

void read_value(unsigned long int loc) {
	int i;unsigned long int where;
	
	printf ("HEX Value: %x\nDec Value: %d\n",loc,loc);
	where = (unsigned long int)GPIO_OFFSET+(unsigned long int)loc;
	printf ("Offset to read: %X\n", where);
	i = inpw (where);
	printf ("Value at %x is %d\n",where, i);
}


void write_value (unsigned int loc, int val) {
        int i;unsigned long  int where;

	where = (unsigned int)BASE_IN_DEC + (unsigned int)loc;

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
	unsigned long int loc; 
	int val;

	if (argc < 2) {
		usage();
		return(0);
	}

	while  ((c = getopt (argc, argv, "r:w:ep")) != -1) {
		switch (c) {
			case 'r':
				loc = strtol(optarg, &endptr, 16);
				read_value(loc);
				//do read value, and show
				return(0);
			break;
			
			case 'w':
				loc = strtol(optarg,&endptr,16);
				val = atol( argv[optind++] );
				//printf ("loc: %d val: %d\n",loc,val);
				write_value (loc, val);
				//do write value and show
				return(0);
			break;
			
			case 'e':
				show_eth();
				return (0);
				break;


			case 'p':
				show_p();
				return (0);
				break;
		}		
	}

	return (0);	
}
