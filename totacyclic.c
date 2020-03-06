#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bdd.h"
#include "orientation.h"

struct OrientBuffer {
	/* Buffer containing the orientations for each level of the BDD*/
	struct OrientBuffer *next;
	struct Orientation *orient;
	struct BDDNode *node;
};

struct OrientBuffer *createBufferNode(struct Orientation *orient) {
	struct OrientBuffer *retVal = malloc(sizeof(struct OrientBuffer));

	retVal -> orient = orient;
	retVal -> next = NULL;
	retVal -> node = NULL;
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

void addToBufferList (struct OrientBuffer *buffer, struct Orientation *orient, struct BDDNode *node) {
	/* TODO: finish this */
	/* add orientation to the buffer list, as long as the orientation is not already present*/
	/* returns pointer to BDD node to which "node" should point in case it is already present and NULL otherwise*/
	while (buffer -> next != NULL) {
		/*if(areReachRelationsEqual(*/
	}
}

void testReachability() {
	/* TODO: add better test with more complicated situation (use examples coming from sagemath) */

	/* simple test*/
	struct Orientation *orient1 = createCompleteGraph(4);
	orientEdge(orient1, 0, 1);
	orientEdge(orient1, 0, 2);
	struct Orientation *orient2 = createCompleteGraph(4);
	orientEdge(orient2, 1, 0);
	orientEdge(orient2, 2, 0);
	int sizeEF;
	int *eliminationFront = computeEliminationFront(orient1, &sizeEF);
	if(areReachRelationsEqual(orient1, orient2, eliminationFront, sizeEF)){
		printf("seems alrighty\n");
	}
	else 
		printf("bad\n");
	deleteOrientation(orient1);
	deleteOrientation(orient2);
	free(eliminationFront);

	/* simple test2*/
	orient1 = createCompleteGraph(4);
	orient2 = createCompleteGraph(4);

	orientEdge(orient1, 0, 1);
	orientEdge(orient1, 2, 0);
	orientEdge(orient1, 3, 1);

	orientEdge(orient2, 0, 2);
	orientEdge(orient2, 1, 0);
	orientEdge(orient2, 1, 3);
	eliminationFront = computeEliminationFront(orient1, &sizeEF);
	if(areReachRelationsEqual(orient1, orient2, eliminationFront, sizeEF)){
		printf("seems alrighty\n");
	}
	else 
		printf("bad\n");
	deleteOrientation(orient1);
	deleteOrientation(orient2);
	free(eliminationFront);
	
	/* simple test3: this time the ReachRelations are different*/
	orient1 = createCompleteGraph(4);
	orient2 = createCompleteGraph(4);

	orientEdge(orient1, 0, 1);
	orientEdge(orient1, 2, 0);
	orientEdge(orient1, 3, 1);

	orientEdge(orient2, 2, 0);
	orientEdge(orient2, 0, 1);
	orientEdge(orient2, 1, 3);
	eliminationFront = computeEliminationFront(orient1, &sizeEF);
	printOrientation(orient1);
	printOrientation(orient2);
	for (int i = 0; i < sizeEF; i++) {
		printf("%d\n",eliminationFront[i]);
	}

	if(areReachRelationsEqual(orient1, orient2, eliminationFront, sizeEF)){
		printf("bad\n");
	}
	else 
		printf("seems alrighty\n");
	deleteOrientation(orient1);
	deleteOrientation(orient2);
	free(eliminationFront);
}

int main() {

	/*
	int n = 10;

	struct Orientation *orient = createCompleteOrientation(n);
	printOrientation(orient);
	*/

	testReachability();

	return 0;
}
