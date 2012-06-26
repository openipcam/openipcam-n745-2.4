How to use i2c driver

 

1. This program is a sample to access i2c bus on board, run on uClinux. The driver of i2c 

         is especially for W90N745 i2c bus interface. 

 

2. The access method is to open the device file "/dev/i2c...":

         "/dev/i2c0":    bus 0

         "/dev/i2c1":    bus 1

         

         This demo program will test i2c.

 

3. There are some command you can use by "ioctl", they are used to access i2c bus.

         a. I2C_IOC_SET_DEV_ADDRESS

                   set address of device on which the following operations act.

                   only 7-bits addresses are supported

		Ex:
         	     ioctl(int fd, I2C_IOC_SET_DEV_ADDRESS, 0x50);
                                      

         b. I2C_IOC_SET_SUB_ADDRESS

                   set sub-address, it depends on different devices.

                   struct sub_address{
				char sub_addr_len;			/* sub address length ( in byte ) */
				unsigned int sub_addr;		/* sub address */
			};

		Ex:
		     struct sub_address addr;

		     addr.sub_addr = 0x80;
		     addr.sub_addr_len = 2;
		     
         	     ioctl(int fd, I2C_IOC_SET_SUB_ADDRESS, &addr);

         	     NOTE: sub_addr will be increased by driver after read/write
                        

         c. I2C_IOC_SET_SPEED

                   set bus speed, only 100kbps and 400kbps are supported

		Ex:
         	     ioctl(int fd, I2C_IOC_SET_SPEED, 100);
                   

         d. I2C_IOC_GET_LAST_ERROR

  		     get last operation error code.
 
  		     error code:

  		     		I2C_ERR_NOERROR					No error
				I2C_ERR_LOSTARBITRATION			lost arbitration
				I2C_ERR_BUSBUSY					bus busy
				I2C_ERR_NACK						device response NACK
		Ex:
         	     ioctl(int fd, I2C_IOC_GET_LAST_ERROR, &last_error);
