#define ASSERT_POSTCONDITIONS 1

#include <sg_debug.h>
#include <sg_ns_utils.h>
#include "nvdb.h"

/* TODO
 * Deletion: - Can only take place if the node has no children.
 */

/*
 * The root of this structure - we only support 1 application wide structure 
 * for the time being.
 */
nvdb_node root = {NULL, NULL, NULL, NULL, ROOT_CONTEXT, ROOT_CONTEXT, NULL,};

/* BUGS
 * This is currently very heavy on mallocs!!
 */
int nvdb_set(gds_node *node, NameVal *vals)
{
	int empty = -1;
	int found = 0;
	NameValues *nvs;
	int res = SUCCESS;
	char *tmp;

	/*
	 * Find the node.
	 */
	for (j = 0;vals[j].name != NULL;j++) {
		empty = -1;
		found = 0;

		/*
		 * We use the count in the hope that this will reduce reallocations.
		 */
		for (i=0;i < node->nvcount;i++) {
			if (node.nameVals[i].name == NULL) {
				empty = i;
			} else {
				/*
				 * If we find the node - then fill it out appropriately.
				 */
				if (strcmp(node.nameVals[i].name, vals[j].name) == 0) {
					found = 1;
					if (vals[j].value) {
						tmp  = strdup(vals[j].value);
						if (!tmp) {
							res = NO_SYSTEM_RESOURCES;
						} else  {
							free(node.nameVals[j].value);
							node.nameVals[j].value = tmp;
						}
					} else { /*delete the node!!*/
						/* this should never happen - an empty value means
						 * delete the nameval pair!! */
						assert_post(node.nameVals[j].value, 
								"if name is NULL value is NULL");
						assert_post(node.nameVals[j].name, 
								"if name is NULL value is NULL");

						free(node.nameVals[j].value);
						free(node.nameVals[j].name);
						bzero(&node.nameVals[j], sizeof(struct NameVal));
					}
					break;
				}
			}
		}

		/*
		 * We couldn't match the name - need to allocate.
		 */
		if ((!found) && (vals[j].value != NULL)){
			if (empty > 0) { /*We found a place to put the nameval*/

				node.nameVals[empty].name = strdup(vals[j].name);
				if (!node.nameVals[empty].name) {
					res = NO_SYSTEM_RESOURCES;
					continue;
				}
				node.nameVals[empty].value = strdup(vals[j].name);
				if (!node.nameVals[empty].value) {
					res = NO_SYSTEM_RESOURCES;
					continue;
				}
			} else {
				/*
				 * Allocate some more memory which is a bit bigger.
				 */
				nvs = (NameValues *)realloc(node->nameVals, 
						sizeof(NameValues) * (node->nvcount + 1));
				if (nvs) {
					node->nameVals[node->nvcount].name = 
						strdup(NameValues[j].name);
					if (!node->nanmeVals[node->nvcount].name) {
						res = NO_SYSTEM_RESOURCES;
						continue;
					}
					node->nanmeVals[node->nvcount].value = 
						strdup(NameValues[j].value);
					if (!node->nanmeVals[node->nvcount].value) {
						res = NO_SYSTEM_RESOURCES;
						/*
						 * Failed to allocat - continue regardless!!
						 */
					}
					node->nvcount += 1;
				} else {
					res = NO_SYSTEM_RESOURCES;
				}
			}
		} 
	}

	/* TODO
	 * Support the call back mechanism.
	 */
#ifdef SUPPORT_SUBSCRIPTIONS
	/*
	 * Now launch callbacks.
	 */
	nvdb_callbacks(node);
#endif

	return res;
}

#ifdef SUPPORT_SUBSCRIPTIONS
/*
 * recursive callbacks!!
 */
void nvdb_callbacks(const nvdb_node *node) {
	int i;
	if (!node) {
		return;
	}

	for (i=0;i < MAX_CALLBACKS;i++) {
		if (cbs[i]) {
			cbs[i](cb_data, node, node->prim);
		}
	}
	if (node->parent) {
		nvdb_callbacks(node->parent);
	}
}
#endif

/*
 * Get all of the name value pairs associated with this node.
 */
const NameVal *nvdb_get_values(const nvdb_node *node)
{
	return node->namevals;
}


#ifdef SUPPORT_SUBSCRIPTIONS
/*
 * Subscribe to receive notifications of changes to this node via a callback.
 */
int nvdb_subscribe(nvdb_node *node, sub_callback *cb, void *data) 
{
	int i;

	for (i=0;i < MAX_CALLBACKS;i++) {
		if (node->cbs[i] == NULL) {
			node->cbs[i] = cb;
			node->cb_data[i] = data;
			return SUCCESS;
		}
	}
	return NO_SYSTEM_RESOURCES;
}


/*
 * Unsubscribe from notifications.
 */
int nvdb_unsubscribe(nvdb_node *node, sub_callback *cb, void *data)
{
	int i;

	for (i = 0;i < MAX_CALLBACKS; i++) {
		if ((node->cbs[i] == cb) && (node->cb_data[i] == data)) {
			node->cvs[i] = NULL;
			node->cb_data[i] = NULL;
			return SUCCESS;
		}
	}
	return NOT_FOUND;
}
#endif

/*
 * Return all of the node which matches the partial_primary.
 */
const nvdb_node *nvdb_search(PrimaryKey partial_primary, nvdb_node *start) 
{
	int i;
	nvdb_node *curr;

	assert_pre(start != NULL,
			"start is not NULL");
	assert_pre(partial_primary != NULL,
			"partial_primary is not NULL");
	assert_pre(!ns_is_equal(partial_primary, NOTHING),
			"Partial_primary is not NOTHING");

	/*
	 * remove the root context if it is present.
	 */
	partial_priamry = ns_remove_beginning_context(partial_primary, ROOT_CONTEXT);
	/*
	 * Now see if we can find a matching child.
	 */
	curr = nvdb_find_child(start, partial_primary);
	if (!curr) {
		return NULL;
	} else {
		/* TODO - add this function to the namespace functions.
		 * Pull off the front context.
		 */
		if ((i = ns_sameroot(partial_primary, curr->shortName)) == 0) {
			return NULL;
		}
		partial_primary += i;
		if (ns_is_equal(partial_primary, NOTHING)) { /*We have found our match*/
			return curr;
		} else { /* keep searching */
			return nvdb_search(partial_primary+i, curr);
		}
	}
	return NULL;
}


/*
 * desc:	Return the length of the initial same root or 0 if the root is not
 * 			the same.
 */
int ns_sameroot(char *ptr1, char *ptr2)
{
	int i;

	assert_pre(*ptr1 != '.', "No explicit ROOT - ptr1");
	assert_pre(*ptr2 != '.', "No explicit ROOT - ptr2");
	assert_pre(ptr1, "ptr1 is not NULL - ptr1");
	assert_pre(ptr2, "ptr1 is not NULL - ptr2");

	for (i = 0;(ptr1[i] == ptr2[i]);i++) {
		if (ptr1[i] == '.'){
			return i+1;
		} else if (ptr1[i] == '\0') {
			return i;
		}
	}
	return 0;
}


/*
 * Insert a child into this node.
 */
void nvdb_insert_child(nvdb_node *node, nvdb_node *child)
{
	if (node->kidlist == NULL) {
		node->kidlist = child;
	} else {
		insque(node->kidlist, child);
	}
}


/*
 * Find a child which matches.
 */
const nvdb_node *nvdb_find_child(nvdb_node *node, PrimaryKey prim)
{
	int i;
	if ((i = ns_sameroot(partial_primary)) == 0) {
		return NULL;
	}

	for (curr = start;curr;curr = curr->next) {
		if (strncmp(curr->partial, partial_primary, i) == 0) {
			return curr;
		}
	}
	return NULL;
}


/*
 * Delete a node.
 *
 * We can't delete nodes with children.  It screws up the name space and
 * doesn't make sense.
 */
int nvdb_delete(nvdb_node *node)
{
	assert_pre(node != &root, "node is not root node");

	/*
	 * We can't delete a node with kids - it make everything too complicated.
	 */
	if (node->kidList) {
		return NVDB_CANT_DELETE;
	}

	/*
	 * We need to remove ourselves from the list.
	 *
	 * Don't forget to update our parents kidlist!!
	 */
	if ((node->parent->kidList == node) && (node->next == NULL) && 
			(node->prev == NULL)) {
		node->parent->kidList = NULL;
	} else if (node->parent->kidList == node){
		node->parent->kidList = node->prev?:node->next;
		remque(node);
	} else {
		remque(node);
	}

	/*
	 * Now delete the individual bits.
	 */

	if (node->nameVals) {
		for (i=0;i < node->nvcount;i++) {
			if (node->nameVals[i].name) {
				free(node->nameVals[i].name);
			}

			if (node->nameVals[i].value) {
				free(node->nameVals[i].value);
			}
		}

		free (node->nameVals);
	}

	/*
	 * Longname and shortName are actually the same string.
	 */
	free (node->longName);
	return SUCCESS;
}


/*
 * Create a new node.
 */
nvdb_node *create_node(PartialPrimary prim, NameVal *nvs)
{
	nvdb_node *node;

	node = (nvdb_node *)calloc(sizeof(nvdb_node), NULL);


}
