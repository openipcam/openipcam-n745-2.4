#include <stdio.h>

#include "compr_rubin.h"

#define __u32 int

/* _compress returns the compressed size, -1 if bigger */
int rtime_compress(unsigned char *data_in, unsigned char *cpage_out, 
		   __u32 *sourcelen, __u32 *dstlen);

void rtime_decompress(unsigned char *data_in, unsigned char *cpage_out,
		      __u32 srclen, __u32 destlen);

void main()
{
	unsigned char source[4096];
	unsigned char compr[40960];
	unsigned char dest[4096];

	int result;
	int i;
	int len,maxlen;
	FILE *file;
	
	memset(dest,0xA5,sizeof(dest));
	
	for (i=0;i<4096;i++)
		source[i] = i;
		
	file=fopen("asd","r");
	while (!feof(file)) {
		fread(source,1,4096,file);
	
		len = 4096;
		maxlen = 40960;
		result = rubin_compress(source,compr,&len, &maxlen);
		if (result) {
			printf("result is %d, exiting\n",result);
			exit(0);
		}
		printf("Compressed to %i bytes \n",maxlen);
		
		if (maxlen>4096)
			continue;
	
		rubin_decompress(compr,dest,4096,4096);
		for (i=0;i<4096;i++)
			if (source[i]!=dest[i]) {
				printf("Mismatch at position %i: %i -> %i \n",i,source[i],dest[i]);
			
			}
		
	}
	fclose(file);
}

