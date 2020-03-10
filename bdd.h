#ifndef BDDGEN
#define BDDGEN

struct BDDNode {
	int v;			/* index of variable */
	struct BDDNode *lo;	/* pointer to node when false */
	struct BDDNode *hi;	/* pointer to node when true */
};

struct BDDNode *newNullBDDNode (int v);

void delOneBDDNode (struct BDDNode *node);

int sizeOfBDD (struct BDDNode *node);

#endif
