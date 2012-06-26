#define GPIO_OUT					1
#define GPIO_IN					0

#define GPIO_TIMEOUT				(-2)
#define GPIO_ERROR				(-1)
#define GPIO_OK					(0)

#define GPIO_LOW					0
#define GPIO_HIGH					1

#define GPIO_CHANNEL_0_3		0xFFFFFFFC
#define GPIO_CHANNEL_9_4		0xFFFFFFF3
#define GPIO_CHANNEL_11_10		0xFFFFFFCF
#define GPIO_CHANNEL_12			0xFFFFFF3F
#define GPIO_CHANNEL_13			0xFFFFFCFF
#define GPIO_CHANNEL_14			0xFFFFF3FF
#define GPIO_CHANNEL_16_15		0xFFFFCFFF
#define GPIO_CHANNEL_17			0xFFFFCFFF
#define GPIO_CHANNEL_18			0xFFFCFFFF
#define GPIO_CHANNEL_19			0xFFF3FFFF
#define GPIO_CHANNEL_20			0xFFCFFFFF


extern void GPIO_Enable_Channel(int channel);//normal I/O mode

extern int Set_Dir(int channel, int dir);
extern void GPIO_SET(int channel_num, int sig_status);
extern int GPIO_GET(int channel_num);//0-20

extern int GPIO_EnableChannel0_3(int mode);
extern int GPIO_EnableChannel4_9(int mode);
extern int GPIO_EnableChannel10_11(int mode);
extern int GPIO_EnableChannel12(int mode);
extern int GPIO_EnableChannel13(int mode);
extern int GPIO_EnableChannel14(int mode);
extern int GPIO_EnableChannel15_16(int mode);
extern int GPIO_EnableChannel17(int mode);
extern int GPIO_EnableChannel18(int mode);
extern int GPIO_EnableChannel19(int mode);
extern int GPIO_EnableChannel20(int mode);