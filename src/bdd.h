#ifndef BDDGEN
#define BDDGEN

struct BDDNode {
	int v;			/* index of variable */
	struct BDDNode *lo;	/* pointer to node when false */
	struct BDDNode *hi;	/* pointer to node when true */
	bool isVisited;   	// get rid of this later
};

struct TONode {
	// Node for the array representing a topological order
	// of a BDD.
	struct BDDNode *bdd;
	int lo;	
	int hi;	
};

struct TOStack {
	struct TONode *nodeArray;
	int n;				// number of elements present
	int l;				// length of array
};

struct BDDNode *newNullBDDNode (int v);

void delOneBDDNode (struct BDDNode *node);

int sizeOfBDD (struct BDDNode *node);

void deleteBDD (struct BDDNode *node);

struct TOStack *constructTOStack (struct BDDNode *bdd);

int *computeProfile(struct TOStack *s, int *length);

void testStack(struct BDDNode *bdd);
#endif
