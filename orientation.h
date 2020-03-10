#ifndef ORIENT
#define ORIENT

struct Node {
	int v;			/* vertex identifier */
	struct Node *next;	/* pointer to next node in adjacency list*/
	int orientation;	/* -1, 0 or 1 depending on type of orientation */

};

struct Orientation {
	int n;			/* number of vertices */
	int m;			/* number of edges */	
	struct Node **adjList;	/* list of vertices */
};

struct Node *createNode(int v);

struct Orientation *createOrientation (int noV);

void appendNode (struct Node **list, struct Node *node, int v);

void addEdge (struct Orientation *orientation, int src, int dest);

void printOrientation (struct Orientation *orientation);

void deleteEdge (struct Orientation *orientation, int src, int dest);

void orientEdge (struct Orientation *orientation, int src, int dest);

bool deleteInitialNode (struct Orientation *orientation, int v);

void deleteList (struct Orientation *orientation, int v);

void deleteOrientation(struct Orientation *orientation);

bool isReachable(struct Orientation *orientation, int src, int dest);

struct Orientation *createCompleteGraph (int n);

struct Orientation *createCycle (int n);

struct Orientation *createCompleteOrientation (int n);

bool areReachRelationsEqual (struct Orientation *orient1, struct Orientation *orient2, int *setOfVertices, int length);

int *computeEliminationFront(struct Orientation *orient, int *sizeEF);

struct Orientation *copyOrientation (struct Orientation *original);

bool isStronglyConnected(struct Orientation *orient);

bool isSelfReachable (struct Orientation *orient, int i);
#endif
