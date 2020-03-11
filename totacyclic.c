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
		//printf("1\n");
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
		//printf("2\n");
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
	//printf("3\n");
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


struct BDDNode *createBDD(struct Orientation *undir) {
	/* TODO: finish this */
	/* the order of the vertices is the natural one */
	int i, inext, j, m, sizeBuf;
	bool isLast = false;
	struct Orientation *tempOr1 = NULL;
	struct Orientation *tempOr2 = NULL;
	struct OrientBuffer *nextBuffer = createBufferNode(undir);
	struct OrientBuffer *prevBuffer = nextBuffer;
	struct OrientBuffer *InitPrevBuffer = NULL;
	m = undir -> m;

	int *u = malloc(m * sizeof(int));
	int *v = malloc(m * sizeof(int));
	computeLexOrder(u, v, undir);

	int *EF = NULL;
	int sizeEF = 0;
	struct BDDNode *retVal = newNullBDDNode(1);
	struct BDDNode *trav = retVal;
	struct BDDNode *tempBDDNode = NULL;
	prevBuffer -> node = trav; 
	struct BDDNode *T = newNullBDDNode(-1);
	struct BDDNode *F = newNullBDDNode(-1);

	for (i = 0; i < m; i++) { 
		/* construct (i+1)-th level from i-th level */
		printf("%d/%d\n", i, m);

		nextBuffer = NULL;	/* buffer for (i+1)-th level */
		sizeBuf = sizeBuffer(prevBuffer);
		//printf("i------------------------------\n");
		//printf("Buffer (size = %d, i = %d):\n", sizeBuf, i);
		//printBuffer(prevBuffer);
		//printf("f------------------------------\n");

		InitPrevBuffer = prevBuffer;

		if (i == m-1) {
			// the processing is different for the last
			// ((m-1)-th to m-th) level
			for(j = 0; j < sizeBuf; j++) {
				tempOr1 = copyOrientation(prevBuffer -> orient);
				trav = prevBuffer -> node;
				tempOr2 = copyOrientation(tempOr1);
				orientEdge(tempOr1, u[i], v[i]);
				orientEdge(tempOr2, v[i], u[i]);
				if (isSelfReachable(tempOr1, 0)) {
					printf("hue1\n");
					prevBuffer -> node -> lo = F;
				} else {
					printf("hue2\n");
					prevBuffer -> node -> lo = T;
				}
				if (isSelfReachable(tempOr2, 0)) {
					printf("hue3\n");
					prevBuffer -> node -> hi = F;
				} else {
					printf("hue4\n");
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
			orientEdge(tempOr1, u[i], v[i]);
			if (isCyclic(tempOr1, u[i])) {
				trav->lo = F;	
				free(tempOr1);
				goto label_hi;
			}

			if(j == 0) {
				EF = computeEliminationFront(tempOr1, &sizeEF);	
			}

	
			tempBDDNode = addToBufferList(&nextBuffer, tempOr1, EF, sizeEF, trav, true, i+2);


			if (tempBDDNode != NULL) {
				trav -> lo = tempBDDNode; 
				free(tempOr1);
			}
			
			label_hi:
			/* hi */
			orientEdge(tempOr2, v[i], u[i]);

			if (isCyclic(tempOr2, u[i])) {
				trav->hi = F;
				free(tempOr2);
				prevBuffer = prevBuffer -> next;
				continue;
			}


			tempBDDNode = addToBufferList(&nextBuffer, tempOr2, EF, sizeEF, trav, false, i+2);

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
	//sizeBuf = sizeBuffer(prevBuffer);
	//printf("final------------------------------\n");
	//printf("Buffer (size = %d, i = %d):\n", sizeBuf, i);
	//printBuffer(prevBuffer);
	//printf("final------------------------------\n");
	free(u);
	free(v);
	return retVal;
}

struct BDDNode *createCycleBDD(int n) {
	struct Orientation *undir = createCycle(n);
	struct BDDNode *retVal = createBDD(undir);
	return retVal;
}

struct BDDNode *createCompleteBDD(int n) {
	struct Orientation *undir = createCompleteGraph(n);
	struct BDDNode *retVal = createBDD(undir);
	return retVal;
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




int main() {

	//int n = 10;

	//struct Orientation *orient = createCompleteGraph(n);
	//printOrientation(orient);

	struct BDDNode *bdd = createCompleteBDD(9);
	testStack(bdd);
	//testEdgeOrder();

	return 0;
}
