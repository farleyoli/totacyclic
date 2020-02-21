#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: add methods to delete stuff

struct Node {
	int v;
	struct Node *next;
	bool isOriented;
};

struct Orientation {
	int n;			/* number of vertices */
	int m;			/* number of edges */	
	struct Node **adjList;	/* list of vertices */
};

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
	int i;
	for (i = 0; i < noV; i++) {
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

void deleteNode (struct Orientation *orientation, int src, int dest) {
	// deletes Node dest from list of src
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
	deleteNode (orientation, dest, src);
	struct Node *node = orientation -> adjList[src];
	while(node -> v != dest)
		node = node -> next;
	node -> isOriented = true;
}

int main (void) {
	struct Orientation *orientation = createOrientation(6);

	for (int i = 0; i < 6; i++)
		for(int j = 0; j < 6; j++)
			if (i < j) 
				addEdge(orientation, i, j);

	printOrientation(orientation);
	printf("\n--------------------------------\n");

	for (int i = 0; i < 6; i++)
		for(int j = 0; j < 6; j++)
			if (i < j) 
				orientEdge(orientation, i, j);

	printOrientation(orientation);

	return 0;
}
