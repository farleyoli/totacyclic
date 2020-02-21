#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "orientation.h"

/*
struct Node {
	int v;
	struct Node *next;
	bool isOriented;
};

struct Orientation {
	int n;			
	int m;		
	struct Node **adjList;
};

*/

struct Node *createNode(int v) {
	struct Node *newNode = malloc(sizeof(struct Node));
	newNode -> v = v;
	newNode -> next = NULL;
	newNode -> isOriented = false;
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
	// list: adjacency list of the orientation
	// node: node to be added
	// v: index for the list

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
}

void printOrientation (struct Orientation *orientation) {
	int v;
	struct Node *temp;
	for (v = 0; v < orientation -> n; v++) {
		temp = orientation -> adjList[v];
		printf("\n Adjacency list of vertex %d\n ", v);
		while (temp != NULL) {
			printf("%d ", temp -> v);
			printf(temp -> isOriented ? "true" : "false");
			printf(" -> ");
			temp = temp -> next;
		}
		printf("\n");
	}

}

void deleteEdge (struct Orientation *orientation, int src, int dest) {
	// deletes Node dest from list of src
	// Attention: must run twice (both directions) for undirected
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

void orientEdge (struct Orientation *orientation, int src, int dest) {
	// orient from src to dest
	deleteEdge (orientation, dest, src);
	struct Node *node = orientation -> adjList[src];
	while(node -> v != dest)
		node = node -> next;
	node -> isOriented = true;
}

bool deleteInitialNode (struct Orientation *orientation, int v) {
	// returns false iff list is empty
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
		//printf("already visited\n");
		return false;
	}
	visited[src] = true;
	struct Node *initial = orientation -> adjList[src];
	if (initial == NULL) {
		//printf("initial was null\n");
		return false;
	}
	bool result = false;
	while (initial != NULL) {
		//printf("until here?\n");
		if ((initial -> v == dest) && (initial -> isOriented))
			return true;
		if (initial -> isOriented && isReachableAux(orientation, initial -> v, dest, visited))
			result = true;
		initial = initial -> next;
	}
	return result;
}

bool isReachable(struct Orientation *orientation, int src, int dest) {
	// TODO: complete
	int n = orientation -> n;
	bool *visited;
	visited = (bool *) malloc(n * sizeof(bool));
	for (int i = 0; i < n; i++)
		visited[i] = false;
	return isReachableAux(orientation, src, dest, visited);
}
/*
int main (void) {
	struct Orientation *orientation = createOrientation(10);

	for (int i = 0; i < 6; i++)
		for(int j = 0; j < 6; j++)
			if (i < j) 
				addEdge(orientation, i, j);

	addEdge(orientation, 9, 0);

	printOrientation(orientation);
	printf("\n--------------------------------\n");

	for (int i = 0; i < 6; i++)
		for(int j = 0; j < 6; j++)
			if (i < j) 
				orientEdge(orientation, i, j);

	deleteList(orientation, 2);
	orientEdge(orientation,9,0);
	printOrientation(orientation);

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++) {
			if (i != j) {
				printf("Is %d reachable from %d?\n", j, i);
				printf(isReachable(orientation, i, j) ? "yes" : "no");
				printf("\n");
			}
		}
	
	deleteOrientation(orientation);

	return 0;
}
*/
