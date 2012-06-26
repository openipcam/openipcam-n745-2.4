

#pragma packed

/*
 * All values from packets are in network byte order.
 */

#define GDSX_PORT (htons(0x1200))

/*
 * Define the GDS exchange header format.
 */
typedef struct {
	int version:8;			/*version of protocol*/
	int type:8;				/*type of this exchange*/
	int reserved:16;		/* must be 0's*/
	int data_length:32;		/*length of the data*/
	int result:32;			/*The error result value.  0 for non result cmd 
							  values*/
} gdsx_v1_header;

#define CMD_RESULT (0x80)
/*
 * define the commands
 */
enum {
	GDSX_CMD_SEARCH=0,				/*perform a search*/
	GDSX_CMD_GET_NV,				/*perform a get on name value pairs*/
	GDSX_CMD_SET_NV,				/*set a name value pair*/
	GDSX_CMD_SUB,					/*subscribe*/

	GDSX_CMD_SEARCHR=CMD_RESULT,	/*result from a search*/
	GDSX_CMD_GET_NVR,				/*result from a get*/
	GDSX_CMD_SET_NVR,				/*Indicate the result o the transaction*/
	GDSX_CMD_SUBR,					/*result from a subscribe*/

	GDSX_CMD_MAX=0xff
};

#define GDSX_VERSION (1)					/* command set version */

/*
 * Data formats:
 *
 * the GDS_CMD_SEARCH has a single NULL terminated string directly after the
 * packet header which indicates the string to search for.
 *
 * the GDS_CMD_SEARCHR has multiple NULL terminated strings directly after
 * the header.  Each of the strings represents a primary key which was found
 * to match the original search string.
 *
 * the GDS_CMD_GET_NV has a single NULL terminated string directly after the
 * packet header.
 *
 * the GDS_CMD_GET_NVR has multiple NULL terminated strings directly after
 * the header.
 *
 * the GDS_CMD_SET_NV has a multiple NULL terminated strings after the header.
 * The first NULL terminated string is the primary key that you want to set
 * values on.  After this NULL terminated string there are several NULL
 * terminated strings consisting of"
 *
 * name\0Cvalue\0
 * name - is the value to be changed.
 * C - is the command, either S for set, or D for delete.
 * 		value is the option value.  If the command is D, then tha value is
 * 		ignored. If the command is set, and the value is empty, then the node is
 * 		set to be empty (although not deleted).
 * value - the value to set the name to.  If the command is D then this is
 * 		ignored.
 *
 * the GDS_CMD_SET_NVR has no data associated with it, it simply indicates
 * the success or failure in the result field.
 *
 * the GDS_CMD_SUB has a single NULL terminated string after the the header.
 *
 * the GDS_CMD_SUBR has a single NULL terminated string after the header
 * which is the primary key of the changed interface.
 */

