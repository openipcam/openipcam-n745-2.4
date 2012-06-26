/*
 * C interface to the GDS back-end - this is just a name value database.  With
 * the ability to receive notifications when a change takes place.
 */

/*
 * I don't like this, but I want to keep it simply for the time being.
 */
#define MAX_CALLBACKS	2


typedef struct {
	/*
	 * Keep here so this thing can be used with insque and remque.
	 */
	nvdb_node *next;			/*The next in the list of the peers*/
	nvdb_node *previous;		/*The previous in the list of peers*/

	/*
	 * Things need to keep track of kids and parents.
	 */
	nvdb_node *parent;			/*The parent structure*/
	nvdb_node *kidList;			/*The child nodes*/

	/*
	 * Interesting data regarding this node.
	 */
	PrimaryKey *longName;		/*The primary key this represents */
	PrimaryKey *shortName;		/*The last piece of the key this represents */

	int nvcount;				/*The number of nv values*/
	NameVal *nameVals;			/*The nv pairs*/

	sub_callback *cbs[MAX_CALLBACKS];	/*Null terminated list of callbacks*/
	void *cb_data[MAX_CALLBACKS];		/*Null terminated call back data*/
} nvdb_node;

extern nvdb_node root;

/*
 * Callback for indicating your subscription is fulfilled.
 *
 * node - the node that triggered the actual callback.
 * prim - the primary key which was asked for (may be a substring of the 
 * 			node that actually triggered the callback).
 * user_data - data that the user included in the initial subscription.
 */
typedef int( *sub_callback)(void *user_data, const nvdb_node *node, const PrimaryKey prim);

/*
 * Search for matching primary keys.
 *
 * partial_primary: The primary key to search for.
 * start - The node to start the search from.  This can be NULL to indicate the
 * 			to start from the root node.
 */
nvdb_node *nvdb_search(PrimaryKey partial_primary, nvdb_node *start);

/*
 * Get the values from a particular node.
 */
const NameVal *nvdb_get_values(nvdb_node *node);

/*
 * Return the primary key associated with this node.
 */
const PrimaryKey nvdb_get_primary(nvdb_node *node);

/*
 * Create a new subscription 
 */
int nvdb_subscribe(nvdb_node *node, sub_callback *cb, void *data);

/*
 * remove an existing subscriotion.
 */
int nvdb_unsubscribe(nvdb_node *node, sub_callback *cbm void *data);


/*
 * Set the values, making callbacks along the way
 */
int nvdb_set(nvdb_node *node, NameVal *vals);

/*
 * Create a new node (and return it).
 */
nvdb_node *nvdb_newnode(PrimaryKey prim);

#define NVDB_CANT_DELETE 100
