/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/locks.h>
#include <linux/sched.h>

#include <linux/slab.h> /* kmalloc */
#include <asm/hardware.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/io.h>
#include <gpio/gpio_interface.h>

/*
 ===============================================================================
 ===============================================================================
 */

void init_EBI(void) /* lsshi 2003-12-29 16:40 */
{
#if 0
	DWORD_WRITE(EXT0CON, EXT0CON_DATA);
	DWORD_WRITE(EXT1CON, EXT1CON_DATA);
	DWORD_WRITE(EXT2CON, EXT2CON_DATA);
	DWORD_WRITE(EXT3CON, EXT3CON_DATA);
#endif
	return;
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel0_3(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_0_3) | (mode & 0x3)
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel4_9(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_9_4) | ((mode & 0x3) << 2)
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel10_11(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_11_10) | (mode & 0x3) << 4
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel12(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_12) | (mode & 0x3) << 6
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel13(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_13) | (mode & 0x3) << 8
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel14(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_14) | (mode & 0x3) << 10
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel15_16(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_16_15) | (mode & 0x3) << 12
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
    With IRQ capability
 ===============================================================================
 */
int GPIO_EnableChannel17(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_17) | (mode & 0x3) << 14
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel18(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_18) | (mode & 0x3) << 16
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
    mode=0x01 enable IRQ mode
 ===============================================================================
 */
int GPIO_EnableChannel19(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_19) | (mode & 0x3) << 18
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_EnableChannel20(int mode)
{
	if(mode > 3) return GPIO_ERROR;
	DWORD_WRITE
	(
		GPIO_CFG,
		(DWORD_READ(GPIO_CFG) & GPIO_CHANNEL_20) | (mode & 0x3) << 20
	);
	return(GPIO_OK);
}

/*
 ===============================================================================
    gpio inline functions
 ===============================================================================
 */
void GPIO_Enable_Channel(int channel)	/* normal I/O mode */
{
	DWORD_WRITE(GPIO_CFG, (DWORD_READ(GPIO_CFG) & channel));
	return;
}

/*
 ===============================================================================
 ===============================================================================
 */
int Set_Dir(int channel, int dir)
{
	if(dir) /* out */
	{
		DWORD_WRITE(GPIO_DIR, DWORD_READ(GPIO_DIR) | (1 << channel));
	}
	else	/* in */
	{
		DWORD_WRITE(GPIO_DIR, DWORD_READ(GPIO_DIR) &~(1 << channel));
	}

	return GPIO_OK;
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_Data_Out(int channel, int dataout)
{
	Set_Dir(channel, GPIO_OUT);

	if(dataout > 1) return GPIO_ERROR;
	if(dataout) /* write "1" */
	{
		DWORD_WRITE(GPIO_DATAOUT, DWORD_READ(GPIO_DATAOUT) | (1 << channel));
	}
	else		/* write "0" */
	{
		DWORD_WRITE(GPIO_DATAOUT, DWORD_READ(GPIO_DATAOUT) &~(1 << channel));
	}

	return GPIO_OK;
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_Data_In(int channel)
{
	Set_Dir(channel, GPIO_IN);
	return(DWORD_READ(GPIO_DATAIN) & (1 << channel));
}

/*
 ===============================================================================
 ===============================================================================
 */
void GPIO_SET(int line, int sig_status)
{
	GPIO_Data_Out(line, sig_status);

	return;
}

/*
 ===============================================================================
 ===============================================================================
 */
int GPIO_GET(int line)
{
	return GPIO_Data_In(line);
}

/*
 ===============================================================================
 ===============================================================================
 */
int check_irq_self(int main_type, int irq_type)
{
	/*--------------------------*/
	volatile unsigned int	value;
	volatile unsigned char	c;
	/*--------------------------*/
#if 0
	value = CSR_READ_OFFSET(SHARE_IRQ_ADDR, main_type);
	c = value & 0x0F;
	printk("main_type=%d,check_irq_self=%x\n",main_type,c);
	
	if(c==0x0f)
		return 0;

	if(irq_type & c)
		return 1;
	else
		return 0;
#endif
}
