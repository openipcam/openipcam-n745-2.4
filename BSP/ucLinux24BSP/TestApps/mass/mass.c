/****************************************************************************
*                                                                           *
* Copyright (c) 2005 - 2007 Winbond Electronics Corp. All rights reserved.  *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   mass.c
*
* VERSION
*   1.2		modified by ns24 zswan,20061009. 
*
* DESCRIPTION
*   The mass driver by ns24 zswan@winbond.com.tw
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*
* REMARK
*   None
****************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>
#include <errno.h>


#include "mass.h"

#define MASS_BUFFER_SIZE		(SD_SECTOR_SIZE * 8)

//#define MASS_DEBUG
//#define MASS_ENTER_LEAVE

#ifdef MASS_DEBUG
#define PDEBUG	printf
#else
#define PDEBUG(fmt, arg...)	
#endif

#ifdef MASS_ENTER_LEAVE
#define ENTER()	PDEBUG("[%-10s] : Enter...\n", __FUNCTION__)
#define LEAVE()	PDEBUG("[%-10s] : Leave\n", __FUNCTION__)
#else
#define ENTER()
#define LEAVE()
#endif

int bLoop;
struct sd2ms_dev_struct dev;

 u16 inline get_be16(u8 *buf)
{
	return ((u16) buf[0] << 8) | ((u16) buf[1]);
}

 u32 inline get_be32(u8 *buf)
{
	return ((u32) buf[0] << 24) | ((u32) buf[1] << 16) |
			((u32) buf[2] << 8) | ((u32) buf[3]);
}

 void inline put_be16(u16 val, u8 *buf)
{
	buf[0] = val >> 8;
	buf[1] = val;
}

 void inline put_be32(u32 val, u8 *buf)
{
	buf[0] = val >> 24;
	buf[1] = val >> 16;
	buf[2] = val >> 8;
	buf[3] = val;
}

#define WBUSB_IOC_MAGIC 'u'
#define WBUSB_IOC_GETCBW _IOR(WBUSB_IOC_MAGIC, 0, char *)

int usb_fd;

int usb_get_cbw(char *buf)
{
	return ioctl(usb_fd, WBUSB_IOC_GETCBW, buf);
}

int usb_write(char *buf, int count)
{
	return write(usb_fd, buf, count);
}

int usb_read(char *buf, int count)
{
	return read(usb_fd, buf, count);
}

int ms_init(int fd)
{
	ENTER();

	printf("USB Reader by ns24 zswan,2006-10-23\n");

	memset(&dev, 0, sizeof(dev));

	dev.fd = fd;

	dev.nCapacityInByte = lseek(fd, 0, SEEK_END);
	if(dev.nCapacityInByte < 0){
		printf("Get media Size Error\n");
		return -1;
	}

	dev.nTotalSectors = dev.nCapacityInByte / SD_SECTOR_SIZE;
	dev.sense = 0;

	dev.buffer = (char *)malloc(MASS_BUFFER_SIZE);
	if(dev.buffer == NULL){
		printf("Out of memory\n");
		return -1;
	}

	printf("Media Size : %d(MB)\n", dev.nTotalSectors /2048);

	LEAVE();

	return 0;
}

void ms_exit(void)
{
	ENTER();

	free(dev.buffer);
	
	LEAVE();
}


int ms_check_cbw(struct ms_cbw_struct *cbw, int cmd_size, int dir)
{
	ENTER();
	
	if ( cbw->bCBWCBLength != 0 && cbw->bmCBWFlags != dir) {
		PDEBUG("Cmd dir error. ops[%02x] dir[%d]\n",
				cbw->CBWCB[0], dir);
		return -1;
	}

	LEAVE();

	return 0;
}

int ms_get_data(char *buf, int count)
{
	ENTER();
	
	if(count != 0)
		usb_read(buf, count);

	LEAVE();
	
	return 0;
}

int ms_put_data(char *buf, int count)
{
	int i, j;

	ENTER();
	
	if(count != 0)		
		usb_write(buf, count);

/*
	printf("Data ************************\n");
	for( i = 0 ; i < (count / 16); i ++){
		for( j = 0; j < 16; j ++)
			printf("%02x ", buf[i * 16 + j]);
		printf("\n");
	}

	for( j = 0; j < (count % 16); j ++)
			printf("%02x ", buf[i * 16 + j]);
	printf("\n");
*/

	LEAVE();

	return 0;
}


void ms_get_cbw(void)
{
	ENTER();

	while(1){

		memset(&dev.cbw, 0, sizeof(struct ms_cbw_struct));

		usb_get_cbw((char *)&dev.cbw);

		if ( dev.cbw.dCBWSignature != CBW_SIGNATURE ) {
			PDEBUG("CBW Signature error\n");
			continue;
		}

		break;
	}

	LEAVE();
}


void ms_put_csw(void)
{
	ENTER();

	dev.csw.dCSWSignature = CSW_SIGNATURE;
	dev.csw.dCSWTag = dev.cbw.dCBWTag;

	ms_put_data((char *)&dev.csw, 0x0d);

	LEAVE();
}


int ms_test_unit_ready(void)
{
	int retval = 0;

	ENTER();

	LEAVE();

	return 0;
}


int ms_inquiry(void)
{
	char *buf = dev.buffer;
	struct ms_cbw_struct *cbw = &dev.cbw;

	ENTER();

	static char vendor_id[] = "WINBOND";
	static char product_id[] = "USB Card Reader ";
	static char release_id[]="1.00";


	if (cbw->bCBWLUN != 0) {		/* unsupported lun */
		memset(buf, 0, 36);
		buf[0] = 0x7f;		
		return 36;
	}

	memset(buf, 0, 8);
	buf[1] = 0x80;	/* removable */
	buf[2] = 0;		// ANSI SCSI level 2
	buf[3] = 1;		// SCSI-2 INQUIRY data format
	buf[4] = 0x1f;		// Additional length
				// No special options
	sprintf(buf + 8, "%-8s%-16s%-4s", vendor_id, product_id,
			release_id);

	LEAVE();			

	return 36;
}

int ms_mode_select(void)
{
	ENTER();

	dev.sense = SS_INVALID_COMMAND;

	LEAVE();
	
	return -1;
}


int ms_mode_sense(void)
{
	int ret = 4, i;
	struct ms_cbw_struct *cbw =  &dev.cbw;
	char * buf = dev.buffer;

	ENTER();

	memset(buf, 0, 8);

	if ( cbw->CBWCB[0] == SC_MODE_SENSE_6 ) {
		buf[0] = 0x03;
		buf[1] = i;
	}
	else{
		buf[1] = 0x06;
		buf[2] = i;
		ret = 8;
	}

	LEAVE();

	return ret;
}


int ms_prevent_allow_medium_removal(void)
{
	int retval = 0;

	ENTER();

	if ( dev.cbw.CBWCB[4] & 0x01 ) {
		dev.sense = SS_INVALID_COMMAND;
		retval = -1;
	}
	else
		dev.sense = SS_NO_SENSE;

	LEAVE();

	return retval;
}


int ms_sd_read(void)
{
	unsigned int count, lba, i, bFailed, j;
	char *cmd = (char *)dev.cbw.CBWCB;

	ENTER();

	bFailed = 0;

	if ( ms_test_unit_ready() )
		bFailed = 1;

	if ( cmd[0] == SC_READ_6) {
		lba = ((cmd[1] & 0x1f) << 16) + get_be16(&cmd[2]);
		count = (cmd[4] & 0xff);
	}
	else {
		lba = get_be32(&cmd[2]);
		count = get_be16(&cmd[7]);
	}

	if ( lba >= dev.nTotalSectors || (lba + count) >= dev.nTotalSectors) {
		dev.sense = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
		bFailed = 1;
	}

	lseek(dev.fd, lba * SD_SECTOR_SIZE, SEEK_SET);

#if 0

	for(i = 0; i < count; i++){
		if( !bFailed)
			bFailed = read(dev.fd, dev.buffer, SD_SECTOR_SIZE) < 0 ? 1: 0;

		ms_put_data(dev.buffer, SD_SECTOR_SIZE);
	}

#else

	for(i = 0; i < (count / (MASS_BUFFER_SIZE/SD_SECTOR_SIZE)); i++){

		if( !bFailed)
			bFailed = read(dev.fd, dev.buffer, MASS_BUFFER_SIZE) < 0 ? 1: 0;

//		printf("SD Read : %08x , Size : %08x\n", dev.buffer, MASS_BUFFER_SIZE);

		for(j = 0; j < (MASS_BUFFER_SIZE / SD_SECTOR_SIZE); j++){
			ms_put_data(dev.buffer + j * SD_SECTOR_SIZE, SD_SECTOR_SIZE);
//			printf("MS Put : %08x , Size : %08x\n", dev.buffer + j * SD_SECTOR_SIZE, SD_SECTOR_SIZE);
		}
	}

	if(count % (MASS_BUFFER_SIZE/SD_SECTOR_SIZE)){

		if( !bFailed)
			bFailed = read(dev.fd, dev.buffer, (count % (MASS_BUFFER_SIZE/SD_SECTOR_SIZE)) * SD_SECTOR_SIZE) < 0 ? 1: 0;

//		printf("SD Read : %08x , Size : %08x\n", dev.buffer, count % (MASS_BUFFER_SIZE/SD_SECTOR_SIZE));

		for(j = 0; j < (count % (MASS_BUFFER_SIZE/SD_SECTOR_SIZE)); j++){
			ms_put_data(dev.buffer + j * SD_SECTOR_SIZE, SD_SECTOR_SIZE);
//			printf("MS Put : %08x , Size : %08x\n", dev.buffer + j * SD_SECTOR_SIZE, 
//							SD_SECTOR_SIZE);
		}
	}

#endif	
	
	if (bFailed)
		dev.csw.bCSWStatus = CMD_FAILED;

	LEAVE();

	return 0;
	
}


int ms_sd_write(void)
{
	unsigned int count, lba, i, bFailed, j;
	char *cmd = (char *)dev.cbw.CBWCB;

	ENTER();

	bFailed = 0;

	if ( ms_test_unit_ready()){
		bFailed = 1;
	}

	if ( cmd[0] == SC_WRITE_6) {
		lba = ((cmd[1] & 0x1f) << 16) + get_be16(&cmd[2]);
		count = (cmd[4] & 0xff);
	}
	else {
		lba = get_be32(&cmd[2]);
		count = get_be16(&cmd[7]);
	}

	if ( lba >= dev.nTotalSectors || (lba + count) >= dev.nTotalSectors) {
		dev.sense = SS_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
		bFailed = 1;
	}

	lseek(dev.fd, lba * SD_SECTOR_SIZE, SEEK_SET);


#if 0

	for(i = 0; i < count; i++){
		ms_get_data(dev.buffer, SD_SECTOR_SIZE);

		if( !bFailed)
			bFailed = write(dev.fd, dev.buffer, SD_SECTOR_SIZE) < 0 ? 1: 0;
	}

#else

	for(i = 0; i < count / (MASS_BUFFER_SIZE/SD_SECTOR_SIZE); i++){
		for(j = 0; j < MASS_BUFFER_SIZE / SD_SECTOR_SIZE; j++)
			ms_get_data(dev.buffer + j * SD_SECTOR_SIZE, SD_SECTOR_SIZE);
		
		if( !bFailed)
			bFailed = write(dev.fd, dev.buffer, MASS_BUFFER_SIZE) < 0 ? 1: 0;
	}

	if(count % (MASS_BUFFER_SIZE/SD_SECTOR_SIZE)){
		for(j = 0; j < (count % (MASS_BUFFER_SIZE/SD_SECTOR_SIZE)); j++)
			ms_get_data(dev.buffer + j * SD_SECTOR_SIZE, SD_SECTOR_SIZE);
		
		if( !bFailed)
			bFailed = write(dev.fd, dev.buffer, (count % (MASS_BUFFER_SIZE/SD_SECTOR_SIZE)) * SD_SECTOR_SIZE) < 0 ? 1: 0;
	}

#endif	

	if (bFailed)
		dev.csw.bCSWStatus = CMD_FAILED;

	LEAVE();

	return 0;

	
}

int ms_read_capacity(void)
{
	u8 *cmd = (char *)dev.cbw.CBWCB;
	u8 *buf = dev.buffer;
	int lba = get_be32(&cmd[2]);
	int pmi = cmd[8];

	ENTER();

	if ( ms_test_unit_ready() ){
		return -1;
	}

	/* Check the PMI and LBA fields */
	if (pmi > 1 || (pmi == 0 && lba != 0)) {
		dev.sense = SS_INVALID_FIELD_IN_CDB;
		return -1;
	}

	put_be32(dev.nTotalSectors - 1, &buf[0]);	// Max logical block
	put_be32(SD_SECTOR_SIZE, &buf[4]);				// Block length

	LEAVE();

	return 8;
}

static int ms_read_format_capacities(void)
{
	int i;
	u32 gTotalSectors=dev.nTotalSectors;
	char *buf = dev.buffer;

	ENTER();

	if (ms_test_unit_ready() ){
		return -1;
	}

	for (i = 0 ; i < 36 ; i++)
       	buf[i] = 0;

	buf[3] = 0x10;
	buf[4] = (gTotalSectors >> 24) & 0xff;
	buf[5] = (gTotalSectors >> 16) & 0xff;
	buf[6] = (gTotalSectors >> 8) & 0xff;
	buf[7] = (gTotalSectors) & 0xff;
	buf[8] = 0x02;
	buf[10] = 0x02;

	buf[12] =  (gTotalSectors >> 24) & 0xff;
	buf[13] = (gTotalSectors >> 16) & 0xff;
	buf[14] = (gTotalSectors >> 8) & 0xff;
	buf[15] = (gTotalSectors) & 0xff;

	buf[18] = 0x02;

	LEAVE();

	return 36;
}

static int 	ms_request_sense(void)
{
	char *buf = dev.buffer;
	int sd = dev.sense;

	ENTER();

	memset(buf, 0, 18);
	
	buf[0] = 0x70;			// Valid, current error
	buf[2] = SK(sd);
	buf[7] = 0x0a;			// Additional sense length
	buf[12] = ASC(sd);
	buf[13] = ASCQ(sd);

	LEAVE();

	return 18;
}

static int ms_start_stop_unit(void)
{
	int retval = 0;

	ENTER();

	if(dev.cbw.CBWCB[4] & 0x01){		/* start */
		if(ms_test_unit_ready())
			retval = -1;
	}

	LEAVE();

	return retval;
}



void ms_process_cbw(void)
{
	struct ms_cbw_struct *cbw = &dev.cbw;
	struct ms_csw_struct *csw = &dev.csw;
	char *cmd = (char *)cbw->CBWCB;
	int retval = -1, tmp, i;

	ENTER();

	csw->bCSWStatus = CMD_PASSED;
	csw->dCSWDataResidue = 0;

	switch(cmd[0]) {

		case SC_INQUIRY:
			PDEBUG("SC_INQUIRY\n");
			if ( ! ms_check_cbw(cbw, 6, DIR_IN) ) 
				retval = ms_inquiry();
			break;

		case SC_MODE_SELECT_6:
			PDEBUG("SC_MODE_SELECT_6\n");
			if ( ! ms_check_cbw(cbw, 6, DIR_OUT))
				retval = ms_mode_select();
			break;
			
		case SC_MODE_SELECT_10:
			PDEBUG("SC_MODE_SELECT_10\n");
			if ( ! ms_check_cbw(cbw, 6, DIR_OUT))
				retval = ms_mode_select();
			break;

		case SC_MODE_SENSE_6:
			PDEBUG("SC_MODE_SENSE_6\n");
			if ( ! ms_check_cbw(cbw, 6, DIR_IN) )
				retval = ms_mode_sense();
			break;
				
		case SC_MODE_SENSE_10:
			PDEBUG("\nSC_MODE_SENSE_10");
			if ( ! ms_check_cbw(cbw, 10, DIR_IN) )
				retval = ms_mode_sense();
			break;

		case SC_PREVENT_ALLOW_MEDIUM_REMOVAL:
			PDEBUG("\nSC_PREVENT_ALLOW_MEDIUM_REMOVAL");
			if ( ! ms_check_cbw(cbw, 6, DIR_OUT) ) 
				retval = ms_prevent_allow_medium_removal();
			break;

		case SC_READ_6:
			PDEBUG("R");
			if ( ! ms_check_cbw(cbw, 6, DIR_IN))
				retval = ms_sd_read();
			break;

		case SC_READ_10:
			PDEBUG("R");
			if ( ! ms_check_cbw(cbw, 10, DIR_IN))
				retval = ms_sd_read();
			break;

		case SC_WRITE_6:
			PDEBUG("W");
			if ( ! ms_check_cbw(cbw, 6, DIR_OUT))
				retval = ms_sd_write();
			break;

		case SC_WRITE_10:
			PDEBUG("W");
			if ( ! ms_check_cbw(cbw, 10, DIR_OUT))
				retval = ms_sd_write();
			break;
			
		case SC_READ_CAPACITY:
			PDEBUG("\nSC_READ_CAPACITY");
			if ( ! ms_check_cbw(cbw, 10, DIR_IN))
				retval = ms_read_capacity();
			break;

		case SC_READ_FORMAT_CAPACITIES:
			PDEBUG("\nSC_READ_FORMAT_CAPACITIES");
			if ( ! ms_check_cbw(cbw, 10, DIR_IN))
				retval = ms_read_format_capacities();
			break;

		case SC_REQUEST_SENSE:
			PDEBUG("\nSC_START_STOP_UNIT");
			if ( ! ms_check_cbw(cbw, 6, DIR_IN))
				retval = ms_request_sense();
			break;

		case SC_START_STOP_UNIT:
			PDEBUG("\nSC_START_STOP_UNIT");
			if ( ! ms_check_cbw(cbw, 6, DIR_OUT))
				retval = ms_start_stop_unit();
			break;

		case SC_TEST_UNIT_READY:
			PDEBUG("T");
			if ( ! ms_check_cbw(cbw, 6, DIR_OUT))
				retval = ms_test_unit_ready();
			
			break;

		case SC_FORMAT_UNIT:
		case SC_VERIFY:
		case SC_RELEASE:
		case SC_RESERVE:
		case SC_SEND_DIAGNOSTIC:

		default:
			PDEBUG("\nSD Command : <<%02x>>\n", cmd[0] & 0xff);
			retval = 0;
			break;
		
	}

	if(retval < 0) {
		csw->dCSWDataResidue = cbw->dCBWDataTransferLength;
		csw->bCSWStatus = CMD_FAILED;
		if(cbw->bmCBWFlags == DIR_IN)
			ms_put_data(dev.buffer, cbw->dCBWDataTransferLength);
	}
	else{
		if(cbw->bmCBWFlags == DIR_IN)
			ms_put_data(dev.buffer, retval);

		csw->dCSWDataResidue = cbw->dCBWDataTransferLength - retval;
	}
	
	LEAVE();	

}

int main(int argc, char *argv[])
{
	int fd;
	
	if(argc < 2){
		printf("Usage : mass <device_name>\n");
		return -1;
	}

	usb_fd = open("/dev/usbclient", O_RDWR);
	if ( usb_fd < 0 ) {
		printf("Can't open usbclient , perhaps, driver not loaded in\n");
		return -2;
	}

	fd = open(argv[1], O_RDWR);
	if(fd < 0){
		close(usb_fd);
		printf("Can't open %s, perhaps, card removed\n", argv[1]);
		return -3;
	}

	bLoop = 1;

	if(ms_init(fd)){
		close(fd);
		close(usb_fd);
		return -4;
	}

	while(bLoop){

		ms_get_cbw();

		ms_process_cbw();

		ms_put_csw();

	}

	ms_exit();

	close(fd);
	close(usb_fd);

}
