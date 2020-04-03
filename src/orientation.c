#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "orientation.h"
#include "dynamic-array.h"
#include "general-utils.h"

struct Node *createNode(int v) {
	struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
	newNode -> v = v;
	newNode -> next = NULL;
	newNode -> orientation = 0;
	return newNode;
}


struct Orientation *createOrientation (int noV) {
	struct Orientation *orientation = malloc(sizeof(struct Orientation));
	orientation -> n = noV;
	orientation -> m = 0;
	orientation -> adjList = malloc(noV * sizeof(struct Node *));
	for (int i = 0; i < noV; i++) {
		orientation -> adjList[i] = NULL;
	}
	orientation -> sizeEF = -1;
	orientation -> reachTo  = NULL;
	orientation -> reachFrom = NULL;
	orientation -> EF = NULL;
	return orientation;
}


void appendNode (struct Node **list, struct Node *node, int v) {
	/* list: adjacency list of the orientation */
	/* node: node to be added*/
	/* v: index for the list*/

	if (list[v] == NULL) {
		list[v] = node;
		return;
	}
	
	struct Node *temp;
	temp = list[v];
	
	while(temp -> next != NULL) {
		temp = temp -> next;
	}

	if (temp -> next == NULL) {
		temp -> next = node;
	} else {
		printf("Something bad happened");
	}
}

void addEdge (struct Orientation *orientation, int src, int dest) {
	struct Node *newNode = createNode(dest);
	appendNode(orientation -> adjList, newNode, src);
	newNode = createNode(src);
	appendNode(orientation -> adjList, newNode, dest);
	(orientation -> m)++;
}

void printOrientation (struct Orientation *orientation) {
	int v;
	struct Node *temp;
	printf("------\n");
	for (v = 0; v < orientation -> n; v++) {
		temp = orientation -> adjList[v];
		printf("\n Adjacency list of vertex %d\n ", v);
		while (temp != NULL) {
			printf("%d ", temp -> v);
			printf("(%d)", temp -> orientation);
			printf(" -> ");
			temp = temp -> next;
		}
		printf("\n");
	}
	if(orientation->sizeEF > 0) printf("EF: ");
	DAPrint(orientation->EF);
	if(orientation->sizeEF > 0) printf("\n");
	for(int i = 0; i < orientation->sizeEF; i++) {
		printf("reachFrom");
		printf("[%d] (EF[%d] = %d): ", i, i, DAGet(i, orientation->EF));
		DAPrint(orientation->reachFrom[i]);
		printf("\n");
	}
	printf("\n");
	for(int i = 0; i < orientation->sizeEF; i++) {
	printf("reachTo  ");
		printf("[%d] (EF[%d] = %d): ", i, i, DAGet(i, orientation->EF));
		DAPrint(orientation->reachTo[i]);
		printf("\n");
	}

	printf("------\n");
}

void deleteEdgeAux (struct Orientation *orientation, int src, int dest) {
	/* deletes Node dest from list of src*/
	/* Attention: must run twice (both directions) for undirected*/
	struct Node *node, *next, *before;

	if (orientation -> adjList[src] == NULL) {
		printf("error, the node isn't in the list\n");
		return;
	}


	before = orientation -> adjList[src];
	if (before -> v == dest) {
		orientation -> adjList[src] = before -> next;
		free(before);
		return;
	}
	
	while ((before -> next) -> v != dest) {
		before = before -> next;
	}

	node = before -> next;
	next = node -> next;

	before -> next = next;
	free(node);
}

void deleteEdge (struct Orientation *orientation, int src, int dest) {
	/* deletes Node dest from list of src*/
	deleteEdgeAux(orientation, src, dest);
	deleteEdgeAux(orientation, dest, src);
}

bool isAdjacentToDirectedEdge(struct Orientation *orient, int v) {
	// Returns true iff v is adjacent to some oriented edge.
	struct Node *p = orient->adjList[v]; 
	while(p != NULL) {
		if(p->orientation == 1 || p->orientation == -1) {
			return true;
		}
		p = p->next;
	}
	return false;
}

bool isAdjacentToUndirectedEdge(struct Orientation *orient, int v) {
	// Returns true iff v is adjacent to some undirected edge.
	struct Node *p = orient->adjList[v]; 
	while(p != NULL) {
		if(p->orientation == 0) {
			return true;
		}
		p = p->next;
	}
	return false;
}

void orientEdge (struct Orientation *orientation, int src, int dest) { /* orient from src to dest*/ struct Node *node = orientation -> adjList[src];
	int sizeEFOriginal = orientation->sizeEF;
	bool isSrcInEFInOriginal;
	bool isDestInEFInOriginal;

	if(isAdjacentToDirectedEdge(orientation, src) &&
			isAdjacentToUndirectedEdge(orientation, src)) {
		isSrcInEFInOriginal = true;
	} else {
		isSrcInEFInOriginal = false;
	}

	if(isAdjacentToDirectedEdge(orientation, dest) &&
			isAdjacentToUndirectedEdge(orientation, dest)) {
		isDestInEFInOriginal = true;
	} else {
		isDestInEFInOriginal = false;
	}

	while(node -> v != dest) 
		node = node -> next;

	node -> orientation = 1;


	node = orientation -> adjList[dest];
	while(node -> v != src)
		node = node -> next;
	node -> orientation = -1;

	// From here on, we refresh information about EF.
	
	if(orientation->reachFrom == NULL) {
		// This is the first edge to be oriented.
		orientation -> sizeEF = 2;
		orientation -> EF = DAInitialize();
		if(src < dest) {
			DAAppend(src, orientation->EF);
			DAAppend(dest, orientation->EF);
		} else {
			DAAppend(dest, orientation->EF);
			DAAppend(src, orientation->EF);
		}

		assert(orientation->sizeEF == DASize(orientation->EF));
		orientation -> reachFrom = 
			malloc(orientation -> sizeEF * sizeof(struct DynamicArray *));
		orientation -> reachTo = 
			malloc(orientation -> sizeEF * sizeof(struct DynamicArray *));

		for(int i = 0; i < orientation->sizeEF; i++) {
			orientation -> reachFrom[i] = DAInitialize();
			orientation -> reachTo[i] = DAInitialize();
		}
		int EFIdxSrc = DAGetIdx(src, orientation->EF);
		int EFIdxDest = DAGetIdx(dest, orientation->EF);
		assert(EFIdxSrc != -1 && EFIdxDest != -1);

		DAAppend(src, orientation->reachFrom[EFIdxDest]);
		DAAppend(dest, orientation->reachTo[EFIdxSrc]);
		return;
	}

	// This is NOT the first edge to be oriented.
	// Remember: the only elements whose "is in" relation with respect
	// to the EF can change are src and dest;
	
	// Define the following variables (with respect to new EF)
	bool isSrcInEF;
	bool isDestInEF;

	if(isAdjacentToDirectedEdge(orientation, src) &&
			isAdjacentToUndirectedEdge(orientation, src)) {
		isSrcInEF = true;
	} else {
		isSrcInEF = false;
	}

	if(isAdjacentToDirectedEdge(orientation, dest) &&
			isAdjacentToUndirectedEdge(orientation, dest)) {
		isDestInEF = true;
	} else {
		isDestInEF = false;
	}

	// Update elimination front.
	bool isThereChange[4]; 	// Is there any change in EF?
	for(int i = 0; i < 4; i++) isThereChange[i] = false;
	if(isSrcInEF && !isSrcInEFInOriginal) {
		orientation->sizeEF++;
		isThereChange[0] = true;
	}
	if(!isSrcInEF && isSrcInEFInOriginal) {
		orientation->sizeEF--;
		isThereChange[1] = true;
	}
	if(isDestInEF && !isDestInEFInOriginal) {
		orientation->sizeEF++;
		isThereChange[2] = true;
	}
	if(!isDestInEF && isDestInEFInOriginal) {
		orientation->sizeEF--;
		isThereChange[3] = true;
	}
	bool wasThereChangeInEF = (isThereChange[0] || 
			isThereChange[1] || isThereChange[2] || isThereChange[3]);

	struct DynamicArray* prevEF = orientation->EF;

	if(!wasThereChangeInEF) {
		// There was no change in the EF, so we don't need
		// to update it.
		goto jump;
	}

	orientation->EF = DACopy(prevEF);

	// The only memberships of EF that can possibly change
	// are src and dest, and we can decide which of them have changed
	// it using the variables as defined above.
	
	if(isSrcInEF && !isSrcInEFInOriginal) {
		DAAddSorted(src, orientation->EF);
	}
	if(!isSrcInEF && isSrcInEFInOriginal) {
		DARemoveElement(src, orientation->EF);
	}
	if(isDestInEF && !isDestInEFInOriginal) {
		DAAddSorted(dest, orientation->EF);
	}
	if(!isDestInEF && isDestInEFInOriginal) {
		DARemoveElement(dest, orientation->EF);
	}


jump:;
	// Update reachFrom and reachTo for src and dest.
	//
	// First, we allocate the memory for the new ones.
	assert(orientation->sizeEF == DASize(orientation->EF));
	struct DynamicArray **originalReachFrom = orientation->reachFrom;
	struct DynamicArray **originalReachTo = orientation->reachTo;
	orientation->reachFrom = malloc(orientation->sizeEF * 
			sizeof(struct DynamicArray*));
	orientation->reachTo = malloc(orientation->sizeEF * 
			sizeof(struct DynamicArray*));


	for(int i = 0; i < orientation->sizeEF; i++) {
		orientation->reachFrom[i] = DAInitialize();
		orientation->reachTo[i] = DAInitialize();
	}

	// From here on, we update reachFrom and reachTo	
	int srcIdx, destIdx, pSrcIdx, pDestIdx;		// p = previous
	srcIdx = DAGetIdx(src, orientation->EF);	
	destIdx = DAGetIdx(dest, orientation->EF);	
	pSrcIdx = DAGetIdx(src, prevEF);	
	pDestIdx = DAGetIdx(dest, prevEF);
	
	if(isSrcInEF && isSrcInEFInOriginal) {
		orientation->reachFrom[srcIdx] = DACopy(originalReachFrom[pSrcIdx]);
	}


	if(!isSrcInEF) {
		;	
	} else if (isSrcInEFInOriginal && isDestInEFInOriginal) {
		orientation->reachTo[srcIdx] = DAUnion(originalReachTo[pSrcIdx], 
			originalReachTo[pDestIdx], true);
		DAAddSorted(dest,orientation->reachTo[srcIdx]);
	} else if(isSrcInEFInOriginal) {
		orientation->reachTo[srcIdx] = DACopy(originalReachTo[pSrcIdx]);
		DAAddSorted(dest,orientation->reachTo[srcIdx]);
	} else if(isDestInEFInOriginal) {
		orientation->reachTo[srcIdx] = DACopy(originalReachTo[pDestIdx]);
		DAAddSorted(dest,orientation->reachTo[srcIdx]);
	}

	if(!isDestInEF) {
		;	
	} else if (isSrcInEFInOriginal && isDestInEFInOriginal) {
		orientation->reachFrom[destIdx] = DAUnion(originalReachFrom[pSrcIdx], 
			originalReachFrom[pDestIdx], true);
		DAAddSorted(src,orientation->reachFrom[destIdx]);
	} else if(isSrcInEFInOriginal) {
		orientation->reachFrom[destIdx] = DACopy(originalReachFrom[pSrcIdx]);
		DAAddSorted(src,orientation->reachFrom[destIdx]);
	} else if(isDestInEFInOriginal) {
		orientation->reachFrom[destIdx] = DACopy(originalReachFrom[pDestIdx]);
		DAAddSorted(src,orientation->reachFrom[destIdx]);
	}


	if(isDestInEF && isDestInEFInOriginal) {
		orientation->reachTo[destIdx] = DACopy(originalReachTo[pDestIdx]);
	}

	assert(orientation->sizeEF == DASize(orientation->EF));
	
	for(int idx = 0; idx < orientation->sizeEF; idx++) {
		int v = DAGet(idx, orientation->EF);
		if (idx == srcIdx || idx == destIdx) {
			continue;
		}
		int pIdx = DAGetIdx(v, prevEF);
		assert(pIdx != -1);
		orientation->reachFrom[idx] = DACopy(originalReachFrom[pIdx]);
		if(DADoesContain(dest, orientation->reachFrom[idx], true)) {
			DAAddSorted(src, orientation->reachFrom[idx]);
		}
		orientation->reachTo[idx] = DACopy(originalReachTo[pIdx]);
		if(DADoesContain(src, orientation->reachTo[idx], true)) {
			DAAddSorted(dest, orientation->reachTo[idx]);
		}
	}


	// Free up memory of previous EF, reachFrom and reachTo.
	if(wasThereChangeInEF) {
		DADelete(prevEF);
	}
	for(int i = 0; i < sizeEFOriginal; i++) {
		if(originalReachFrom[i] != NULL)
			DADelete(originalReachFrom[i]);
		if(originalReachTo[i] != NULL)
			DADelete(originalReachTo[i]);
	}
	if(originalReachFrom != NULL) {
		free(originalReachFrom);
	}
	if(originalReachTo != NULL) {
		free(originalReachTo);
	}
}

bool deleteInitialNode (struct Orientation *orientation, int v) {
	/* Returns false iff adjList[v] is empty. */

	struct Node *initial = orientation -> adjList[v];
	if (initial == NULL)
		return false;
	orientation -> adjList[v] = initial -> next;
	free(initial);
	return true;
}

void deleteList (struct Orientation *orientation, int v) {
	// Deletes adjList[v].
	// (OBS: for now, use only when deleting the orientation!)
	
	while(deleteInitialNode(orientation, v))
		;
}

void deleteOrientation(struct Orientation *orientation) {
	// Frees memory used by orientation.
	
	int n = orientation -> n;
	for (int i = 0; i < n; i++) {
		deleteList(orientation, i);
	}
	free (orientation -> adjList);
	free(orientation->EF);
	free(orientation->reachTo);
	free(orientation->reachFrom);
	free(orientation);
}

bool isReachableAux(struct Orientation *orientation, int src, int dest, bool *visited) {
	// Auxiliary function for isReachable function.
	
	if (visited[src]) {
		/*printf("already visited\n"); */
		return false;
	}
	visited[src] = true;
	struct Node *initial = orientation -> adjList[src];
	if (initial == NULL) {
		/*printf("initial was null\n");*/
		return false;
	}
	bool result = false;
	while (initial != NULL) {
		/*printf("until here?\n");*/
		if ((initial -> v == dest) && (initial -> orientation == 1))
			return true;
		if ((initial -> orientation == 1) && isReachableAux(orientation, initial -> v, dest, visited))
			result = true;
		initial = initial -> next;
	}
	return result;
}

bool isReachable(struct Orientation *orientation, int src, int dest) {
	/* Returns true iff dest is reachable from src
	 * in partial orientation.
	 * Assumes that src is different from dest. */
	int n = orientation -> n;
	bool *visited;
	visited = (bool *) malloc(n * sizeof(bool));
	for (int i = 0; i < n; i++)
		visited[i] = false;
	bool result = isReachableAux(orientation, src, dest, visited);
	free(visited);
	return result;
}

struct Orientation *createCompleteGraph (int n) {
	// Allocates memory and returns pointer to a complete
	// undirected graph on n vertices.
	struct Orientation *graph = createOrientation(n);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (i < j)
				addEdge(graph, i, j);
	return graph;
}

struct Orientation *createCycle (int n) {
	// Allocates memory and returns pointer to a cycle
	// undirected graph on n vertices.
	struct Orientation *graph = createOrientation(n);
	for (int i = 0; i < n-1; i++)
		addEdge(graph, i, i+1);
	addEdge(graph, n-1, 0);
	return graph;
}

struct Orientation *createCompleteOrientation (int n) {
	// Allocates memory and returns pointer to a completely
	// oriented complete graph on n vertices.
	struct Orientation *orientation = createCompleteGraph(n);
	for (int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			if (i < j) 
				orientEdge(orientation, i, j);
	return orientation;

}

bool areReachRelationsEqual (struct Orientation *orient1, struct Orientation *orient2, int* setOfVertices, int length) {
	/* this function assume that the set of oriented edges are the same*/
	/* (and thus that the elimination front is the same)*/
	/* it checks if the reachability relation on the set of vertices given is the same*/
	for(int i = 0; i < length; i++) {
		assert(DAGet(i,orient1->EF) == DAGet(i, orient2->EF));
		assert(DAIsSorted(orient1->reachFrom[i]));
		assert(DAIsSorted(orient1->reachTo[i]));
		if(!DAAreElementsEqualInOrderRestricted(orient1->reachFrom[i], orient2->reachFrom[i], orient1->EF)) {
			return false;
		}
		if(!DAAreElementsEqualInOrderRestricted(orient1->reachTo[i], orient2->reachTo[i], orient1->EF)) {
			return false;
		}
	}

	return true;
}

bool isVertexInEF (struct Orientation *orient, int i) {
	/* i: vertex under cosideration */
	bool isThereUnd = false, isThereDir = false;
	struct Node *p = orient -> adjList[i];
	if (p == NULL) {
		return false;
	}
	while (p != NULL) {
		if((p -> orientation == 1) || (p -> orientation == -1)) {
			isThereDir = true;
		}
		else {
			isThereUnd = true;
		}
		p = p -> next;	
	}
	return isThereUnd && isThereDir;
}

int *computeEliminationFront(struct Orientation *orient, int *sizeEF) {
	int n = orient -> n;
	int *retArray;
	*sizeEF = 0;
	int i;
	for (i = 0; i < n; i++) {
		if(isVertexInEF(orient, i)) {
			(*sizeEF)++;
		}
	}

	if((*sizeEF) == 0) {
		printf("the size of the elimination front is 0\n");
		return NULL;
	}

	retArray = (int *) malloc(sizeof(int) * (*sizeEF));

	if(retArray == NULL) {
		printf("BAD\n");
		return NULL;
	}
	
	int j = 0;
	for (i = 0; i < n; i++) {
		if(isVertexInEF(orient, i)) {
			retArray[j++] = i;
		}
	}

	return retArray;
}

struct Node *copyNodeList(struct Node *original) {
	struct Node *retVal = NULL; 
	struct Node *trav = NULL;

	if (original == NULL) {
		return NULL;
	}

	retVal = createNode(original -> v);
	retVal -> orientation = original -> orientation;
	trav = retVal;

	while ( original -> next != NULL ) {
		original = original -> next;
		trav -> next = createNode(original -> v);
		trav = trav -> next;
		trav -> orientation = original -> orientation;
	}

	return retVal;
}

struct Orientation *copyOrientation (struct Orientation *original) {
	int i;
	struct Orientation *retVal = createOrientation(original -> n);
	retVal -> m = original -> m;
	for (i = 0; i < original -> n; i++) {
		retVal -> adjList[i] = copyNodeList(original -> adjList[i]);
	}

	// EF, reachFrom, reachTo
	
	retVal->sizeEF = original->sizeEF;
	if(original->sizeEF < 0) {
		retVal->EF = NULL;
		retVal->reachFrom = NULL;
		retVal->reachTo = NULL;
		return retVal;
	}

	retVal->EF = DACopy(original->EF);
	
	retVal->reachFrom = malloc(retVal->sizeEF * sizeof(struct DynamicArray*));
	retVal->reachTo = malloc(retVal->sizeEF * sizeof(struct DynamicArray*));
	for(int i = 0; i < retVal->sizeEF; i++) {
		retVal->reachFrom[i] = DACopy(original->reachFrom[i]);;	
		retVal->reachTo[i] = DACopy(original->reachTo[i]);;	
	}

	return retVal;
}


bool isSelfReachableAux (struct Orientation *orient, int src, int dest, int i, bool *visited) {
	if (visited[src] && src != i) {
		/*printf("already visited\n"); */
		return false;
	}
	visited[src] = true;
	struct Node *initial = orient -> adjList[src];
	if (initial == NULL) {
		/*printf("initial was null\n");*/
		return false;
	}
	bool result = false;
	while (initial != NULL) {
		/*printf("until here?\n");*/
		if ((initial -> v == dest) && (initial -> orientation == 1))
			return true;
		if ((initial -> orientation == 1) && isSelfReachableAux(orient, initial -> v, dest, i, visited))
			result = true;
		initial = initial -> next;
	}
	return result;
}

bool isSelfReachable (struct Orientation *orient, int i) {
	// Assumes that orient is a digraph.
	// Returns true if and only if vertex i can reach itself.
	int n = orient -> n;
	bool *visited;
	visited = (bool *) malloc(n * sizeof(bool));
	for (int i = 0; i < n; i++)
		visited[i] = false;
	bool result = isSelfReachableAux(orient, i, i, i, visited);
	free(visited);
	return result;
}

void computeLexOrder (int *u, int *v, struct Orientation *undir) {
	// This function receives an undirected (connected) graph, and
	// returns the lexicographical order of the edges in the form
	// of two int arrays u and v (i.e. #i-th edge = (u[i],v[i])).
	// It assumes memory has already been allocated to u and v.
	struct Orientation *o = copyOrientation(undir);
	int n = o -> n;
	int counter = 0;
	int next = -1;
	bool isFirst;
	struct Node *p;
	for(int i = 0; i < n; i++) {
		isFirst = true;
		if(o ->adjList[i] == NULL) {
			printf("Error! Graph is not connected!");
			return;
		}
		p = o -> adjList[i];
		while(p != NULL) {
			if(isFirst && p->orientation == 0) {
				isFirst = false;
				next = p->v;
			}
			if(p->orientation==0) {
				//printOrientation(o);
				orientEdge(o, i, p->v);
				u[counter] = i;
				v[counter++] = p->v;
			}
			p = p -> next;
		}
	}
	deleteOrientation(o);
}

bool isCyclic(struct Orientation *o, int v) {
	// This function is given a partial orientation o,
	// a vertex adjacent to the edge that has just been
	// oriented, and returns whether there is a cycle
	// containing v (i.e. whether the orientation is cyclic
	// if we use it throughout the whole BDD construction
	// process).
	if(o->adjList[v] == NULL)
		return false;
	return isSelfReachable(o, v);	
}

struct Orientation *createErdosRenyi (int n, double p) {
	struct Orientation *orientation = malloc(sizeof(struct Orientation));
	float r;

	orientation -> n = n;
	orientation -> adjList = malloc(n * sizeof(struct Node *));
	for (int i = 0; i < n; i++) {
		orientation -> adjList[i] = NULL;
	}
	
	for (int i = 0; i < n; i++) {
		for (int j = i+1; j < n; j++) {
			r = (rand()/(double)RAND_MAX);
			if( r < p ) {
				addEdge(orientation, i, j);
			}
		}
	}
	return orientation;
}

void testEdgeOrder() {
	struct Orientation *o = createCompleteGraph(6);
	int m = o->m;
	int *u = malloc(m * sizeof(int));
	int *v = malloc(m * sizeof(int));
	computeLexOrder(u, v, o);
	for(int i = 0; i < m; i++) {
		printf("(u[%d],v[%d]) = (%d, %d)\n", i, i, u[i], v[i]);
	}

	free(u);
	free(v);
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

void testReachability() {

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

struct Orientation *importFromFile (char *fileName) {
	FILE *p;
	char line[256];
	int n = -1; // number of vertices
	int i, j;
	p = fopen(fileName, "r");
	if(p == NULL) {
		printf("File does not exist.\n");
		exit(1);
	}
	// First traversal, get number of nodes.
	while (fgets(line, sizeof(line), p)) {
		sscanf(line, "%d %d {}", &i, &j);
		if (i + 1 > n) {
			n = i+1;
		}
		if (j + 1 > n) {
			n = j+1;
		}
	}
	rewind(p);
	struct Orientation *retVal = createOrientation(n);
	// Second traversal, add edges.
	while (fgets(line, sizeof(line), p)) {
		sscanf(line, "%d %d {}", &i, &j);
		addEdge(retVal, i, j);
	}
	fclose(p);
	return retVal;
}

void deleteOrientedEdges (struct Orientation *orientation, int v) {
	/* returns false iff list is empty */
	struct Node *p = orientation -> adjList[v]; //pointer
	struct Node *d = p; // to be deleted
	if (p == NULL) {
		return;
	}
	if(p -> next == NULL) {
		orientation -> adjList[v] = p -> next;
		free(p);
		return;
	}

	// Initial nodes.
	while(p -> orientation == -1 || p->orientation == 1) {
		d = p;
		orientation -> adjList[v] = p -> next;
		p = p -> next;
		free(d);
		if(p == NULL) {
			return;
		}
	}

	// Intermediate nodes.
	while(p -> next != NULL) {
		if(p -> next -> orientation == -1 || p->next->orientation == 1) {
			d = p->next;
			p->next = p->next->next;
			free(d);

		} else { // do not delete
			p = p -> next;
		}
	}
	
}

struct Orientation *getReachabilityOrientation (struct Orientation *original, int* EF, int sizeEF) {
	// Returns orientation where oriented edges have been substituted
	// for one oriented edge representing each related pair of vertices
	// in the elimination front
	int i, j;
	struct Orientation *retVal = copyOrientation(original);
	//return retVal;
	//printf("before (size EF) = %d\n", sizeEF);
	//printOrientation(retVal);
	for (i = 0; i < original -> n; i++) {
		deleteOrientedEdges(retVal, i);
	}

	for (i = 0; i < sizeEF; i++) {
		for(j = 0; j < sizeEF; j++) {
			if(i == j) {
				if(isSelfReachable(original, EF[i])) {
					printf("hey hey\n\n");
					addEdge(retVal, EF[i], EF[i]);
					orientEdge(retVal, EF[i], EF[i]);
				}
				continue;
			}
			if(isReachable(original, EF[i], EF[j])) {
				addEdge(retVal, EF[i], EF[j]);
				orientEdge(retVal, EF[i], EF[j]);
			}

		}
	}
	//printf("after\n");
	//printOrientation(retVal);
	
	if(!areReachRelationsEqual(original, retVal, EF, sizeEF)) {
		printf("Not equal!\n");
		printOrientation(original);
		printOrientation(retVal);
	}
	

	return retVal;
}
