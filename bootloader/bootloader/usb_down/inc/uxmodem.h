extern UINT32 volatile Bulk_Out_Transfer_Size;
#define NON_CACHE	0x80000000
#define PACKET_SIZE	1024
#define ALIGN	0x04

extern int usb_recv(UINT8* buf,UINT32 len);
extern int usb_send(UINT8* buf,UINT32 len);

