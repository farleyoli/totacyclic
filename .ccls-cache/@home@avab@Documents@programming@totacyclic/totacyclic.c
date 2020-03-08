#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bdd.h"
#include "orientation.h"

struct OrientBuffer {
	/* Buffer containing the orientations for each level of the BDD*/
	struct OrientBuffer *next; 	/* next node in linked list */
	struct Orientation *orient; 	/* orientation of present node */
	struct BDDNode *node;		/* BDD Node to which current orientation corresponds*/
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

struct BDDNode *addToBufferList (struct OrientBuffer *buffer, struct Orientation *orient, struct BDDNode *node, int *EF, int sizeEF) {
	/*this function assumes that all elements have the same EF! */
	/* add orientation to the buffer list, as long as the orientation is not already present*/
	/* returns pointer to BDD node to which "node" should point in case it is already present and NULL otherwise*/

	if(buffer == NULL) {
		buffer = createBufferNode(orient);
		return NULL;
	}

	struct OrientBuffer *temp = buffer;

	while (temp -> next != NULL) {
		if(areReachRelationsEqual(temp -> orient, orient, EF, sizeEF)) {
			/* no need to add, do some magic and return */
			return temp -> node;
		}
		temp = temp -> next;
	}

	/* we add orientation to the BufferList */
	struct OrientBuffer *new = createBufferNode(orient);
	new -> node = node;
	temp -> next = new;
	return NULL;
}

int sizeBuffer(struct OrientBuffer *b) {
	int i = 0;
	if(b == NULL) {
		return i;
	}

	if (b -> next == NULL) {
		i++;
		return i;
	}

	while (b -> next != NULL) {
		i++;
		b = b -> next;
	}

	return i;
}

void testReachability() {
	/* TODO: add better test with more complicated situation (use examples coming from sagemath) */

	/* simple test 1*/
	struct Orientation *orient1 = createCompleteGraph(4);
	struct Orientation *orient2 = createCompleteGraph(4);
	deleteEdge(orient1, 0, 3);
	deleteEdge(orient1, 1, 2);
	deleteEdge(orient2, 0, 3);
	deleteEdge(orient2, 1, 2);

	orientEdge(orient1, 0, 1);
	orientEdge(orient1, 0, 2);
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
	deleteEdge(orient1, 0, 3);
	deleteEdge(orient1, 1, 2);
	deleteEdge(orient2, 0, 3);
	deleteEdge(orient2, 1, 2);

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
	deleteEdge(orient1, 0, 3);
	deleteEdge(orient1, 1, 2);
	deleteEdge(orient2, 0, 3);
	deleteEdge(orient2, 1, 2);

	orientEdge(orient1, 0, 1);
	orientEdge(orient1, 2, 0);
	orientEdge(orient1, 3, 1);

	orientEdge(orient2, 2, 0);
	orientEdge(orient2, 0, 1);
	orientEdge(orient2, 1, 3);
	eliminationFront = computeEliminationFront(orient1, &sizeEF);
	if(areReachRelationsEqual(orient1, orient2, eliminationFront, sizeEF)){
		printf("bad\n");
	}
	else 
		printf("seems alrighty\n");
	deleteOrientation(orient1);
	deleteOrientation(orient2);
	free(eliminationFront);
}

void testBufferList() {
	/* say test all the permutations of (1,2), (2,3) and (3,4) */
	/* there should be three elements, because there are only 2 elements in the elimination front */
	struct Orientation *orient = NULL;
	struct OrientBuffer *buffer = NULL;
	int *EF = NULL;
	int sizeEF = 0;


	orient = createCycle(5);
	orientEdge(orient, 1, 2);
	orientEdge(orient, 2, 3);
	orientEdge(orient, 3, 4);
	buffer = createBufferNode(orient);
	/* initialization */
	EF = computeEliminationFront(orient, &sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 1, 2);
	orientEdge(orient, 2, 3);
	orientEdge(orient, 4, 3);
	addToBufferList(buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 1, 2);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 3, 4);
	addToBufferList(buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 1, 2);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 4, 3);
	addToBufferList(buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 2, 3);
	orientEdge(orient, 3, 4);
	addToBufferList(buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 2, 3);
	orientEdge(orient, 4, 3);
	addToBufferList(buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 3, 4);
	addToBufferList(buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 4, 3);
	addToBufferList(buffer, orient, NULL, EF, sizeEF);

	free(EF);

	printf("size of buffer: %d\n", sizeBuffer(buffer));

	deleteBufferList(buffer);
	
}

int main() {

	/*
	int n = 10;

	struct Orientation *orient = createCompleteOrientation(n);
	printOrientation(orient);
	*/

	/*testReachability();*/
	testBufferList();

	return 0;
}
