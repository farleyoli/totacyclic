#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "orientation.h"

struct BDDNode {
	int v;			/* index of variable */
	struct BDDNode *lo;	/* pointer to node when false */	
	struct BDDNode *hi;	/* pointer to node when true */
};

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

int main() {
	/*
	printf("hello world\n");
	struct BDDNode *test = newNullBDDNode(5);
	printf("%d\n", (test -> v));
	delOneBDDNode(test);
	*/

	struct Orientation *orient = createOrientation(10);
	addEdge(orient, 3, 5);
	addEdge(orient, 3, 9);
	printOrientation(orient);

	return 0;
}
