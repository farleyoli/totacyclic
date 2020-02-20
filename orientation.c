struct Node {
	int v;
	struct Node *next;
}

struct Orientation {
	int n;			/* number of vertices */
	int m;			/* number of edges */	
	struct Node ** adjList;	/* list of vertices */
}

struct Node *createNode(int v) {
	struct Node *newNode = malloc(sizeof(struct Node));
	newNode -> v = v;
	newNode -> next = NULL;
	return newNode;
}

struct Orientation *createOrientation (int noV) {
	struct Orientation *orientation = malloc(sizeof(struct Orientation));
	orientation -> n = noV;
	orientation -> adjList = malloc(noV * sizeof(struct *Node));
	int i;
	for (i = 0; i < noV; i++) {
		orientation -> adjList[i] = NULL;
	}
	return orientation;
}

void addEdge (struct Orientation *orientation, int src, int dest) {
	struct Node *newNode = createNode(dest);
	newNode -> next = orientation -> adjList[src];

	newNode = createNode(src);
	newNode -> next = orinetation -> adjList[dest];
	orientation -> adjList[dest] = newNode;
}
