#include "W90N745Prism.h"
#include "Queue.h"


extern Queue_txfid TxQueueObj;
/* private txfid queue function, for interval use */
INT16 Init_Queue()
{
	INT16 i;
	INT16 result = 0;
	INT16 retry = 0;
	
	memset(&TxQueueObj, 0, sizeof(Queue_txfid));
	for (i = 0; i < PRISM2_TX_FIDSTACKLEN_MAX; i++)
    {
retryalloc:
    	result = prism_cmd_allocate(PRISM2_TXBUF_MAX, &(TxQueueObj.cells[i].fid));
    	if (result != 0) {
            printk("Allocate(tx) command failed\n");
            result = -1;
            if(retry < 4) {
            	retry++;
            	goto retryalloc;
            }
            goto out;
        }
      	retry = 0;
        TxQueueObj.cells[i].flag |=0x1;
    }
out:
	TxQueueObj.capability = i;
	return i;
}

/* Dump for debuging queue */
void Queue_dump()
{
#ifdef WDEBUG
	int i;
	
	printk("Number of Cells in Queue: %d\n", TxQueueObj.capability);
	for(i = 0; i < TxQueueObj.capability; i++)
	{
		printk("%dth: txfid: %x, flag: %d\n", i, (TxQueueObj.cells[i].fid), TxQueueObj.cells[i].flag);
	}
#endif
}

/* public txfid queue function, for geting a tx fid */
INT16 Get_txfid()
{
	int i;
	
	if(GetAvailableCellNum(TxQueueObj) <= 0) {
		printk("No available cell for tx\n");
		return -1;
	}
	
	for(i = 0; (i < PRISM2_TX_FIDSTACKLEN_MAX)&&(!TxQueueObj.cells[i].flag); i++);
	if(i >= PRISM2_TX_FIDSTACKLEN_MAX) {
		printk("Erro:No available cell for tx\n");
		return -1;
	}
	TxQueueObj.cells[i].flag = 0;
	ReduceAvailableCellNum(TxQueueObj);
	return TxQueueObj.cells[i].fid;
}

/* public txfid queue function, for puting a tx fid */
INT16 Put_txfid(UINT16 val)
{
	INT16 result = 0;
	int i;
	
	if(GetAvailableCellNum(TxQueueObj) >= PRISM2_TX_FIDSTACKLEN_MAX) {
		//printk("queue have full\n");
		return -1;
	}
	
	for(i = 0; (i < PRISM2_TX_FIDSTACKLEN_MAX)&&(TxQueueObj.cells[i].fid != val); i++);
	if(i >= PRISM2_TX_FIDSTACKLEN_MAX) {
		printk("Illegal txfid for tx: %d \n", val);
		return -1;
	}
	if(!TxQueueObj.cells[i].flag) {
		TxQueueObj.cells[i].flag = 1;
		AddAvailableCellNum(TxQueueObj);
	}
	return result;
}
