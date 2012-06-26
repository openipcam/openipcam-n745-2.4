#ifndef __GPIO_I2C_H_
#define	__GPIO_I2C_H_

//I2C define
#define GPIO_I2C_SDA				0
#define GPIO_I2C_SCLK				1

/*schen add 2004-7-22 15:55 for smartcard module*/
#define GPIO_SC_IO1				2
#define GPIO_SC_IO2				3
#define GPIO_SC_CLK				4

#define GPIO_I2C_RD				0x01
#define GPIO_I2C_WR				0x02

/*schen add 2004-7-22 16:17 for smartcard module*/
#define GPIO_I2C_LOW				0
#define GPIO_I2C_HIGH				1

/*schen add 2004-7-22 15:55 for smartcard module*/
#define I2C_ACK    0  
#define I2C_NAK    1

#define GPIO_I2C_DELAY			200	//400khz (fast-mode) / 800 //100khz
#define GPIO_I2C_MAX_BYTES		8192 //max bytes to be transfer one time

void	GPIO_Enable_I2C_Channel(void);		/* choose gpio0 and gpio1 */


int		GPIO_I2C_Start(void);					/* start I2C communication */
void	GPIO_I2C_Stop(void);					/* stop I2C communication */
int		GPIO_I2C_Send_Byte(unsigned char c);	/* write a byte to slave * device */
int		GPIO_I2C_Read_Ack(void);				/* read acknowledge from * slave device */
int		GPIO_I2C_Read_Byte(void);				/* read a byte from slave * device */
int		GPIO_I2C_Read_Last_Byte(void);			/* read last byte from * slave device */
int		GPIO_I2C_Send_Ack(void);				/* master send acknowledge * when reading */
int		GPIO_I2C_Send_NoAck(void);				/* master send no * acknowledge before stop */
int 	output_data(unsigned char dat);
unsigned char input_data(unsigned char ack);

#if 1
#define I2C_DEBUG(x)
#else
#define I2C_DEBUG(x)	 x
#endif

#endif