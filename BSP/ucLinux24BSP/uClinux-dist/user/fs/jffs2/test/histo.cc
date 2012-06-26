#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int histo[256];
int histo2[256];
int bits[8];
int total;
void main() 
{
	FILE *file;
	int i,i2;

	int max=0; 
	
/*	file = fopen("/lib/libc-2.2.so","r"); */
	file = fopen("bigsymb2","r");
	assert(file!=NULL);
	while (!feof(file)) {
		unsigned char cr;
		fread(&cr,1,1,file);
		histo[cr]++;		
		if (histo[cr]>max)
			max = histo[cr];
	}	
	fclose(file);

	i2 = 0;

#if 0
	printf("static int xlate[257] = { ");


	
	/* Ugh slow */
	while (max>=0) {	
		for (i=0;i<256;i++) {	
			if (histo[i]==max) {
				printf("\t%i,\n",i);
				histo2[i2++]=histo[i];
			}
		}
		max--;
	}	

	printf(" };\n");

	if (i2!=256)
		printf("/* Uh oh: %i != 256 */ \n");
#endif
	
	for (i=0;i<256;i++) {
		if (i&128)
			bits[7]+=histo[i];
		if (i&64)
			bits[6]+=histo[i];
		if (i&32)
			bits[5]+=histo[i];
		if (i&16)
			bits[4]+=histo[i];
		if (i&8)
			bits[3]+=histo[i];
		if (i&4)
			bits[2]+=histo[i];
		if (i&2)
			bits[1]+=histo[i];
		if (i&1)
			bits[0]+=histo[i];
		total+=histo[i];
	}
	
	printf("#define BIT_DIVIDER %i \n",total>>12);
	printf("static int bits[9] = { ");
	for (i=0;i<8;i++)
		if ((bits[i]>>12)>0)
			printf("%i,",bits[i]>>12);
		else 
			printf("1, ");
	printf("};\n");
}