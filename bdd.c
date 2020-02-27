#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "orientation.h"

struct OrientBuffer {
	// Buffer containing the orientations for each level of the BDD
	struct OrientBuffer *next;
	struct Orientation *orient;
};

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

struct OrientBuffer *createBufferNode(struct Orientation *orient) {
	struct OrientBuffer *retVal = malloc(sizeof(struct OrientBuffer));

	retVal -> orient = orient;
	retVal -> next = NULL;
	return retVal;
}

struct OrientBuffer *deleteInitialBufferNode (struct OrientBuffer *buf) {
	if (buf == NULL)
		return NULL;
	struct OrientBuffer *next = buf -> next;
	deleteOrientation(buf -> orient);
	free(buf);
	return next;
}

void deleteBufferList (struct OrientBuffer *initial) {
	while(initial != NULL)
		initial = deleteInitialBufferNode(initial);
}

int main() {
	int n = 10;

	struct Orientation *orient = createCompleteGraph(n);
	struct OrientBuffer *initial = createBufferNode(orient);	
	orient = createCompleteGraph(n);
	deleteEdge(orient, 2, 3);
	initial -> next = createBufferNode(orient);	
	deleteBufferList(initial);

	//printOrientation(orient);

	return 0;
}
