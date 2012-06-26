/****************************************************************************
 *                                                                          *
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved. *
 *                                                                          *
 ****************************************************************************/ 

/****************************************************************************
 * 
 * FILENAME
 *     i2c_test.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file is a sample test program used to test I2c
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     08/25/2005            Ver 1.0 Created by PC34 QFu
 *
 * REMARK
 *     None
 **************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "w90n745_i2c.h"


int main()
{

	int fd, i, j;
	unsigned char sbuf[256],dbuf[256];
	struct sub_address addr;
	
	fd = open("/dev/i2c1", O_RDWR);
	if( fd < 0){
		printf("Open /dev/i2c1 error\n");
		return -1;
	}

	addr.sub_addr = 0x00;
	addr.sub_addr_len = 2;
	ioctl(fd, I2C_IOC_SET_DEV_ADDRESS, 0x50);
	ioctl(fd, I2C_IOC_SET_SPEED, 100);

	printf("byte write/read test ....\n");

	memset(sbuf, 0, 8);
	memset(dbuf, 0, 8);

	ioctl(fd, I2C_IOC_SET_SUB_ADDRESS, &addr);

	for(i = 0; i < 8; i ++){
		sbuf[i] = i;
		
		while(write(fd, sbuf + i, 1)<0);

	}

	ioctl(fd, I2C_IOC_SET_SUB_ADDRESS, &addr);

	for(i = 0; i < 8; i ++){

		while(read(fd, dbuf + i, 1)<0);

		printf("[%d] Read (%02x) ,  Write (%02x) ......", i, dbuf[i], sbuf[i]);

		if(dbuf[i] != sbuf[i])
			printf("Failed\n");
		else
			printf("OK\n");
	}

	memset(sbuf, 0, 8);
	memset(dbuf, 0, 8);


	printf("\n\nBlock write / read test ......\n");

	for(i = 0; i < 8; i ++){
		sbuf[i] = i + 8;
	}

	ioctl(fd, I2C_IOC_SET_SUB_ADDRESS, &addr);

	while(write(fd, sbuf, 8) < 0);

	ioctl(fd, I2C_IOC_SET_SUB_ADDRESS, &addr);

	while(read(fd, dbuf, 8) < 0);


	for(i = 0; i < 8; i ++){
		printf("[%d] Read (%02x) ,  Write (%02x) ......", i, dbuf[i], sbuf[i]);

		if(dbuf[i] != sbuf[i])
			printf("Failed\n");
		else
			printf("OK\n");
	}

	close(fd);

	return 0;

}
	
