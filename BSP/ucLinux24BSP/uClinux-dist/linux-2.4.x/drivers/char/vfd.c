#include <stdio.h>
#include "gpio_ads.h" 
#include "i2c_ads.h"

#define I2C_DELAY {volatile int ii; for(ii = 0; ii < 10; ii++);}
#define I2C_STARTDELAY {volatile int ii; for(ii = 0; ii < 100; ii++);}

static int Set_I2C_Dir(int channel, int dir)
{
	if(dir) /* out */
		DWORD_WRITE(GPIO_DIR, DWORD_READ(GPIO_DIR) | (1 << channel));
	else	/* in */
		DWORD_WRITE(GPIO_DIR, DWORD_READ(GPIO_DIR) &~(1 << channel));

	return GPIO_OK;
}

void set_GPIO_Dir(int channel, int dir)
{
	Set_I2C_Dir(channel,dir);
}

void change_GPIO_Dir(int channel, int dir)
{
	Set_I2C_Dir(channel, dir);

}

static void I2C_SET(int line, int sig_status)
{ 

	Set_I2C_Dir(line, GPIO_OUT);
	if(sig_status) /* write "1" */
		DWORD_WRITE(GPIO_DATAOUT, DWORD_READ(GPIO_DATAOUT) | (1 << line));
	else		/* write "0" */
		DWORD_WRITE(GPIO_DATAOUT, DWORD_READ(GPIO_DATAOUT) &~(1 << line));


	return;
}

static int output_data(unsigned char dat)
{
	int i;
	for(i=7; i>=0; i--)
	{
	
		I2C_SET(GPIO_I2C_SDA, 0x01 & (dat>>i));		
		I2C_SET(GPIO_I2C_SCLK, GPIO_I2C_LOW);
		I2C_DELAY;
		I2C_SET(GPIO_I2C_SCLK, GPIO_I2C_HIGH);
	}

}

void Write_command(unsigned char com)
{
	I2C_SET(GPIO_I2C_SCLK, GPIO_I2C_HIGH);
	I2C_SET(GPIO_I2C_STB,GPIO_I2C_LOW);
 	I2C_STARTDELAY;
	I2C_SET(GPIO_I2C_SCLK, GPIO_I2C_LOW);
  	output_data(0xf8);
  	output_data(com);
  	I2C_SET(GPIO_I2C_STB,GPIO_I2C_HIGH);
}

void Write_data(unsigned char data)
{
	I2C_SET(GPIO_I2C_SCLK, GPIO_I2C_HIGH);
	I2C_SET(GPIO_I2C_STB,GPIO_I2C_LOW);
 	I2C_STARTDELAY;
	I2C_SET(GPIO_I2C_SCLK, GPIO_I2C_LOW);
  	output_data(0xfa);
  	output_data(data);
  	I2C_SET(GPIO_I2C_STB,GPIO_I2C_HIGH);
}

void Read_data(unsigned char data)
{
	I2C_SET(GPIO_I2C_SCLK, GPIO_I2C_HIGH);
	I2C_SET(GPIO_I2C_STB,GPIO_I2C_LOW);
 	I2C_STARTDELAY;
	I2C_SET(GPIO_I2C_SCLK, GPIO_I2C_LOW);
  	output_data(0xfe);
  	output_data(data);
  	I2C_SET(GPIO_I2C_STB,GPIO_I2C_HIGH);
}



int main()
{
	int i,j=0;
	int ii;
	Write_command(0x0f);//Display ON
	Write_command(0x3c);//function set 
	Write_command(0x01);//clear display
	Write_command(0x06);
	Write_command(0x0e);
	for(i=0x41;i<0x70;i++)
	{
	j++;
	if(j==21)Write_command(0xc0);
		Write_data(i);
		for(ii=0;ii<100000;ii++)
	 	I2C_STARTDELAY;
	}
	return 0;
}  	  	
