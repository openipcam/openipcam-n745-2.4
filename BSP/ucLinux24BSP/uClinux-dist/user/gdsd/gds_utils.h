/*
 * Functions to get the various bits of the GDS structure.
 */


/*
 * desc:	Return whether the structure is valid or not.
 */
int IsValid(char *data);


/*
 * desc:	Convert the data into a NULL terminated array of values.
 *
 * PRE:		The data is a valid structure.
 */
const char **ToArray(char *data);

