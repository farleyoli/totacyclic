#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "orientation.h"

struct Node *createNode(int v) {
	struct Node *newNode = malloc(sizeof(struct Node));
	newNode -> v = v;
	newNode -> next = NULL;
	newNode -> orientation = 0;
	return newNode;
}


struct Orientation *createOrientation (int noV) {
	struct Orientation *orientation = malloc(sizeof(struct Orientation));
	orientation -> n = noV;
	orientation -> adjList = malloc(noV * sizeof(struct Node *));
	for (int i = 0; i < noV; i++) {
		orientation -> adjList[i] = NULL;
	}
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
void orientEdge (struct Orientation *orientation, int src, int dest) { /* orient from src to dest*/ struct Node *node = orientation -> adjList[src];
	while(node -> v != dest) node = node -> next;
	node -> orientation = 1;


	node = orientation -> adjList[dest];
	while(node -> v != src)
		node = node -> next;
	node -> orientation = -1;
}

bool deleteInitialNode (struct Orientation *orientation, int v) {
	/* returns false iff list is empty */
	struct Node *initial = orientation -> adjList[v];
	if (initial == NULL)
		return false;
	orientation -> adjList[v] = initial -> next;
	free(initial);
	return true;
}

void deleteList (struct Orientation *orientation, int v) {
	while(deleteInitialNode(orientation, v))
		;
}

void deleteOrientation(struct Orientation *orientation) {
	int n = orientation -> n;
	for (int i = 0; i < n; i++) {
		deleteList(orientation, i);
	}
	free (orientation -> adjList);
	free (orientation);
}

bool isReachableAux(struct Orientation *orientation, int src, int dest, bool *visited) {
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
	struct Orientation *graph = createOrientation(n);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (i < j)
				addEdge(graph, i, j);
	return graph;
}

struct Orientation *createCycle (int n) {
	struct Orientation *graph = createOrientation(n);
	for (int i = 0; i < n-1; i++)
		addEdge(graph, i, i+1);
	addEdge(graph, n-1, 0);
	return graph;
}

struct Orientation *createCompleteOrientation (int n) {
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
	int i, j;	
	for (i = 0; i < length; i++) {
		for (j = i+1; j < length; j++) {
			if (isReachable(orient1, setOfVertices[i], setOfVertices[j]) != isReachable(orient2, setOfVertices[i], setOfVertices[j])){
				printf("The culprit is (%d, %d)\n", i, j);
				return false;
			}
			if (isReachable(orient1, setOfVertices[j], setOfVertices[i]) != isReachable(orient2, setOfVertices[j], setOfVertices[i])){
				printf("The culprit is (%d, %d)\n", i, j);
				return false;
			}
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
	return retVal;
}

bool isStronglyConnected(struct Orientation *orient) {
	//TODO (probably use DFS?)
	// assumes that the orientation is a digraph
	// returns true if and only if the digraph is strongly connected
	
	return true;
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
	//TODO
	//assumes that orient is a digraph
	//returns true if and only if vertex i can reach itself
	int n = orient -> n;
	bool *visited;
	visited = (bool *) malloc(n * sizeof(bool));
	for (int i = 0; i < n; i++)
		visited[i] = false;
	bool result = isSelfReachableAux(orient, i, i, i, visited);
	free(visited);
	return result;
}
