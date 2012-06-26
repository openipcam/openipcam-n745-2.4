#define __u32 int

/* _compress returns the compressed size, -1 if bigger */
int rtime_compress(unsigned char *data_in, unsigned char *cpage_out, 
		   __u32 *sourcelen, __u32 *dstlen);

void rtime_decompress(unsigned char *data_in, unsigned char *cpage_out,
		      __u32 srclen, __u32 destlen);
