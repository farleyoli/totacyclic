#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "orientation.h"
#include "bdd.h"


struct BDDNode *newNullBDDNode (int v) {
	/* returns new BDDNode with index v pointing to NULLs*/
	struct BDDNode *retVal = malloc (sizeof (struct BDDNode));

	if (retVal == NULL)
		return NULL;

	retVal -> v = v;
	retVal -> hi = NULL;
	retVal -> lo = NULL;

	return retVal;
}

void delOneBDDNode (struct BDDNode *node) {
	if (node != NULL) {
		free (node -> lo);
		free (node -> hi);
		free (node);
	}
}

int sizeOfBDD (struct BDDNode *node) {
	// This function returns the number of nodes of the BDD
	// headed by "node".
	if(node == NULL)
		return 0;
	int result = 1;
	result += sizeOfBDD(node -> hi);
	result += sizeOfBDD(node -> lo);
	return result;
}
