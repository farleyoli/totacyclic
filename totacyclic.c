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

struct OrientBuffer *createEmptyBufferNode() {
	struct OrientBuffer *retVal = malloc(sizeof(struct OrientBuffer));

	retVal -> orient = NULL;
	retVal -> next = NULL;
	retVal -> node = NULL;
	return retVal;
}

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

struct BDDNode *addToBufferList (struct OrientBuffer **buffer, struct Orientation *orient, int *EF, int sizeEF, struct BDDNode *bddNode, bool isLo, int V) {
	/*this function assumes that all elements have the same EF! */
	/* add orientation to the buffer list, as long as the orientation is not already present*/
	/* returns pointer to BDD node to which "node" should point in case it is already present and NULL otherwise*/
	if((*buffer) == NULL) {
		printf("1\n");
		*buffer = createBufferNode(orient);
		if (isLo) {
			bddNode -> lo = newNullBDDNode(V);
			(*buffer) -> node = bddNode -> lo;
		} else {
			bddNode -> hi = newNullBDDNode(V);
			(*buffer) -> node = bddNode -> hi;
		}
		return NULL;
	}

	struct OrientBuffer *temp = *buffer;

	do {
		printf("2\n");
		if(areReachRelationsEqual(temp -> orient, orient, EF, sizeEF)) {
			/* no need to add, do some magic and return */
			return temp->node;
		}
		if(temp -> next != NULL) {
			temp = temp -> next; 
		}
	} while (temp -> next != NULL);
	if(areReachRelationsEqual(temp -> orient, orient, EF, sizeEF)) {
		return temp->node;
	}

	/* we add orientation to the BufferList */
	printf("3\n");
	struct OrientBuffer *new = createBufferNode(orient);
	temp -> next = new;
	if(isLo) {
		bddNode -> lo = newNullBDDNode(V);
		(temp -> next) -> node = bddNode -> lo;
	} else {
		bddNode -> hi = newNullBDDNode(V);
		(temp -> next) -> node = bddNode -> hi;
	}
	return NULL;
}

int sizeBuffer(struct OrientBuffer *b) {
	int i = 0;
	if(b == NULL) {
		return i;
	}

	while (b != NULL) {
		i++;
		b = b -> next;
	}

	return i;
}

void printBuffer(struct OrientBuffer *b) {
	if(b == NULL) {
		return;
	}

	while (b != NULL) {
		printOrientation(b -> orient);
		b = b -> next;
	}
}

struct BDDNode *createBDD(int n) {
	/* TODO: finish this */
	/* the order of the vertices is the natural one */
	int i, inext, j, m, sizeBuf;
	bool isLast = false;

	struct Orientation *undir = createCycle(n);

	struct Orientation *tempOr1 = NULL;
	struct Orientation *tempOr2 = NULL;

	struct OrientBuffer *nextBuffer = createBufferNode(undir);
	struct OrientBuffer *prevBuffer = nextBuffer;
	struct OrientBuffer *InitPrevBuffer = NULL;

	m = undir -> m;

	int *EF = NULL;
	int sizeEF = 0;

	struct BDDNode *retVal = newNullBDDNode(0);
	struct BDDNode *trav = retVal;
	struct BDDNode *tempBDDNode = NULL;
	prevBuffer -> node = trav; 

	for (i = 0; i < m; i++) { 
		/* construct (i+1)-th level from i-th level */

		if (i < m-1) {
			inext = i+1;
		} else {
			inext = 0;
			isLast = true;
		}

		nextBuffer = NULL;	/* buffer for (i+1)-th level */
		sizeBuf = sizeBuffer(prevBuffer);
		printf("i------------------------------\n");
		printf("Buffer (size = %d, i = %d):\n", sizeBuf, i);
		printBuffer(prevBuffer);
		printf("f------------------------------\n");

		InitPrevBuffer = prevBuffer;

		if (isLast) {
			// the processing is different for the last
			// ((m-1)-th to m-th) level
			for(j = 0; j < sizeBuf; j++) {
				tempOr1 = copyOrientation(prevBuffer -> orient);
				trav = prevBuffer -> node;
				tempOr2 = copyOrientation(tempOr1);
				orientEdge(tempOr1, i, inext);
				orientEdge(tempOr2, inext, i);
				struct BDDNode *T = newNullBDDNode(i+1);
				struct BDDNode *F = newNullBDDNode(i+1);

				if (isSelfReachable(tempOr1, 0)) {
					// (e.g.), link to 0-node		
					prevBuffer -> node -> lo = F;
				} else {
					prevBuffer -> node -> lo = T;
				}
				if (isSelfReachable(tempOr2, 0)) {
					prevBuffer -> node -> hi = F;
				} else {
					prevBuffer -> node -> hi = T;
				}
				prevBuffer = prevBuffer -> next;
			}
			//sizeBuf = sizeBuffer(nextBuffer);
			prevBuffer = nextBuffer;
			deleteBufferList(InitPrevBuffer);
			break;
		}


		for(j = 0; j < sizeBuf; j++) {
			tempOr1 = copyOrientation(prevBuffer -> orient);
			trav = prevBuffer -> node;
			tempOr2 = copyOrientation(tempOr1);


			/* lo */
			orientEdge(tempOr1, i, inext);
			if(j == 0) {
				EF = computeEliminationFront(tempOr1, &sizeEF);	
			}
			printf("tempOr1 (i = %d, j = %d)\n", i, j);
			printOrientation(tempOr1);
			printf("trav->v=%d\n",trav->v);

	
			tempBDDNode = addToBufferList(&nextBuffer, tempOr1, EF, sizeEF, trav, true, i+1);

			printf("!trav->v=%d\n",trav->v);

			if (tempBDDNode != NULL) {
				trav -> lo = tempBDDNode; 
				free(tempOr1);
			}
			

			/* hi */
			orientEdge(tempOr2, inext, i);

			printf("tempOr2 (i = %d, j = %d)\n", i, j);
			printOrientation(tempOr2);
			printf("trav->v=%d\n",trav->v);

			tempBDDNode = addToBufferList(&nextBuffer, tempOr2, EF, sizeEF, trav, false, i+1);
			printf("!trav->v=%d\n",trav->v);

			if(tempBDDNode != NULL) {
				trav -> hi = tempBDDNode; 
				free(tempOr2);
			}

			prevBuffer = prevBuffer -> next;
		}
		sizeBuf = sizeBuffer(nextBuffer);
		prevBuffer = nextBuffer;
		free(EF);
		deleteBufferList(InitPrevBuffer);
	}
	sizeBuf = sizeBuffer(prevBuffer);
	printf("final------------------------------\n");
	printf("Buffer (size = %d, i = %d):\n", sizeBuf, i);
	printBuffer(prevBuffer);
	printf("final------------------------------\n");
	return retVal;
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

	/*

	orient = createCycle(5);
	orientEdge(orient, 1, 2);
	orientEdge(orient, 2, 3);
	orientEdge(orient, 4, 3);
	addToBufferList(&buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 1, 2);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 3, 4);
	addToBufferList(&buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 1, 2);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 4, 3);
	addToBufferList(&buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 2, 3);
	orientEdge(orient, 3, 4);
	addToBufferList(&buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 2, 3);
	orientEdge(orient, 4, 3);
	addToBufferList(&buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 3, 4);
	addToBufferList(&buffer, orient, NULL, EF, sizeEF);

	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 4, 3);
	addToBufferList(&buffer, orient, NULL, EF, sizeEF);

	*/

	free(EF);

	printf("size of buffer: %d\n", sizeBuffer(buffer));

	deleteBufferList(buffer);
	
}

void testCopy() {
	struct Orientation *orient = NULL;
	struct Orientation *orientCopy = NULL;
	orient = createCycle(5);
	orientEdge(orient, 2, 1);
	orientEdge(orient, 3, 2);
	orientEdge(orient, 3, 4);
	orientCopy = copyOrientation(orient);
	printOrientation(orient);
	printOrientation(orientCopy);
}

int main() {

	/*
	int n = 10;

	struct Orientation *orient = createCompleteOrientation(n);
	printOrientation(orient);
	*/

	/*testReachability();*/
	/*testBufferList();*/
	/*testCopy();*/

	struct BDDNode *bdd = createBDD(7);

	printf("size of BDD: %d\n", sizeOfBDD(bdd));

	return 0;
}
