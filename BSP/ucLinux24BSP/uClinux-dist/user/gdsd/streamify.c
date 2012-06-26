#include "gds_client.h"
#include "gds_interface.h"

#define MAX_PACKET 65536

#define CHECK_DATA_LENGTH 	if (here - packet > MAX_PACKET) { \
			*data = NULL; \
			return GDS_TOO_BIG; \
		} \


int streamify(int fd, PrimaryKey primary, NameVal *values, int result, char cmd,
		char **data)
{
	int i;
	static char packet[MAX_PACKET];
	struct gdsx_v1_header *hdr = &packet;
	int data_length = sizeof (gdsx_v1_header);
	char *here;
	int completed = 0;

	/*
	 * Set up some header information.
	 */
	hdr->version=1;
	hdr->type = cmd;
	hdr->result = htonl(result);
	hdr->reserved = 0;

	here = packet + sizeof(gdsx_v1_header);

	/*
	 * Copy the primary key (if any).  Check for too much data.
	 */
	if (primary) {
		here += strlen(primary) + 1;
		CHECK_DATA_LENGTH
		strcpy(here - strlen(primary) - 1, primary);
	}

	if (values) {
		for (i=0;values[i].name;i++) {
			here += strlen(values[i].name) +1;
			CHECK_DATA_LENGTH
			strcpy(here - strlen(values[i].name) - 1, values[i].name);
			if (values[i].value == NULL) { /*This indicates deletion */
				here += 1;
				CHECK_DATA_LENGTH
				*(here - 1) = '\0';
			} else if (values[i].value[0] == '\0') { /*make the node MT */
				here += 2;
				CHECK_DATA_LENGTH
				*(here - 1) = '\0';
				*(here - 2) = '\0';
			} else {					/* just set the value appropriately */
				here += strlen(values[i].name) + 1;
				CHECK_DATA_LENGTH
				strcpy(here - strlen(values[i].name) - 1, values[i].name);
			}
		}
	}
	*data = packet;
	return SUCCESS;
}


/*
 * Warning this may block (but in reality won't)!!
 */
int unstreamify(int fd, PrimaryKey primary, NameVal *values, int *result)
{
	int version;
	int data_length;
	char packet[MAX_PACKET];
	struct gdsx_v1_header *hdr = &packet;

	version = hdr->version;
}
