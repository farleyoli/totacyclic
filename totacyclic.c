#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bdd.h"
#include "orientation.h"

struct OrientBuffer {
	// Buffer containing the orientations for each level of the BDD
	struct OrientBuffer *next;
	struct Orientation *orient;
};

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

void testReachability() {
	// TODO: add better test with more complicated situation (maybe see paper) 

	// simple test
	struct Orientation *orient1 = createCompleteGraph(4);
	orientEdge(orient1, 0, 1);
	orientEdge(orient1, 0, 2);
	struct Orientation *orient2 = createCompleteGraph(4);
	orientEdge(orient2, 1, 0);
	orientEdge(orient2, 2, 0);
	int eliminationFront[2] = {1, 2};
	if(areReachRelationsEqual(orient1, orient2, eliminationFront, 2)){
		printf("seems alrighty\n");
	}
	else 
		printf("bad\n");
	deleteOrientation(orient1);
	deleteOrientation(orient2);

	// simple test2
	orient1 = createCompleteGraph(4);
	orient2 = createCompleteGraph(4);

	orientEdge(orient1, 0, 1);
	orientEdge(orient1, 2, 0);
	orientEdge(orient1, 3, 1);

	orientEdge(orient2, 0, 2);
	orientEdge(orient2, 1, 0);
	orientEdge(orient2, 1, 3);
	int eliminationFront2[2] = {2, 3};
	if(areReachRelationsEqual(orient1, orient2, eliminationFront2, 2)){
		printf("seems alrighty\n");
	}
	else 
		printf("bad\n");
	deleteOrientation(orient1);
	deleteOrientation(orient2);
	
	// simple test3: this time the ReachRelations are different
	orient1 = createCompleteGraph(4);
	orient2 = createCompleteGraph(4);

	orientEdge(orient1, 0, 1);
	orientEdge(orient1, 2, 0);
	orientEdge(orient1, 3, 1);

	orientEdge(orient2, 2, 0);
	orientEdge(orient2, 0, 1);
	orientEdge(orient2, 1, 3);
	int eliminationFront3[2] = {2, 3};
	if(areReachRelationsEqual(orient1, orient2, eliminationFront3, 2)){
		printf("bad\n");
	}
	else 
		printf("seems alrighty\n");
	deleteOrientation(orient1);
	deleteOrientation(orient2);
}

int main() {

	/*
	int n = 10;

	struct Orientation *orient = createCompleteGraph(n);
	struct OrientBuffer *initial = createBufferNode(orient);
	struct Orientation *orient2 = createCompleteGraph(n);
	deleteEdge(orient2, 2, 3);
	initial -> next = createBufferNode(orient2);
	//deleteBufferList(initial);

	printOrientation(orient2);

	*/

	testReachability();

	return 0;
}
