#ifndef __QUEUE_H__
#define __QUEUE_H__

#define UINT16 	short
#define INT16 	short
#define UINT8 	unsigned char
#define UINT32 	unsigned int

/*--------------fids queue-----------------*/
typedef struct prism_txfid_cell
{
	UINT8 flag;			//whether in use
	UINT16 fid;			//fid value
}txfid_cell;

typedef struct prism_txfid_queue
{
	txfid_cell cells[PRISM2_TX_FIDSTACKLEN_MAX];
	UINT8 capability;		//queue size
}Queue_txfid;
/*-------------fid queue operations--------------*/
#define GetAvailableCellNum(x) 		(x.capability)
#define ReduceAvailableCellNum(x) 	(--(x.capability))
#define AddAvailableCellNum(x) 		(++(x.capability))


/*-----------------function phototype----------------*/
/* private txfid queue function, for interval use */
INT16 Init_Queue();
/* Dump for debuging queue */
void Queue_dump();
/* public txfid queue function, for geting a tx fid */
INT16 Get_txfid();
/* public txfid queue function, for puting a tx fid */
INT16 Put_txfid(UINT16 val);

#endif
