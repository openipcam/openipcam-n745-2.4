typedef struct FirmWare{
	int flag;
	int length_bin;
	int length_img;
}FirmWare;

#define BINZIP	0x00000001
#define IMGZIP	0x00000002

#define ISBINZIP(flag)	 ((flag)&0x00000001)
#define ISBINUNZIP(flag) !((flag)&0x00000001)
#define ISIMGZIP(flag)	 ((flag)&0x00000002)
#define ISIMGUNZIP(flag) !((flag)&0x00000002)
