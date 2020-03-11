#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "orientation.h"
#include "bdd.h"


struct BDDNode *newNullBDDNode (int v) {
	/* returns new BDDNode with index v pointing to NULLs*/
	struct BDDNode *retVal = malloc (sizeof (struct BDDNode));

	if (retVal == NULL)
		return NULL;

	retVal -> v = v;
	retVal -> hi = NULL;
	retVal -> lo = NULL;
	retVal -> isVisited = false;

	return retVal;
}

struct TOStack *createStack () {
	struct TOStack *s = malloc(sizeof(struct TOStack));
	s->n = 0;
	s->nodeArray = NULL;
	s->l = 0;
	return s;
}

void addTOStack (struct TOStack *s, struct TONode e) {
	// add e to s
	const int INITIAL_LENGTH = 30;
	if (s -> nodeArray == NULL) {
		printf("debug1\n");fflush(stdout);
		s -> n = 0;
		s -> nodeArray = malloc(sizeof(struct TONode) * INITIAL_LENGTH);
		s -> nodeArray[(s -> n)++] = e;
		s -> l = INITIAL_LENGTH;
		return;
	}

	if (s -> n == s -> l) {
		printf("debug2\n");fflush(stdout);
		// resize	
		free (s -> nodeArray);
		s -> l = 2 * (s -> l);
		s -> nodeArray = malloc(sizeof(struct TONode) * (s -> l));
		s -> nodeArray[(s -> n)++] = e;
		return;
	}

		
	printf("debug3\n");fflush(stdout);
	printf("s->n = %d\n", s->n); fflush(stdout);
	s -> nodeArray[(s -> n)++] = e;
}

struct TONode popTOStack(struct TOStack *s) {
	//TODO (not finished)
	// eliminates (and returns) right-most elemenet from stack
	struct TONode retVal;
	return retVal;
}

struct TONode createTONode (struct BDDNode *bdd) {
	// returns a TONode pointing to bdd
	// lo and hi values are set to -1
	struct TONode retVal; 
	retVal.bdd = bdd;
	retVal.lo = -1;
	retVal.hi = -1;
	return retVal;
}

void constructTOStackAux (struct BDDNode *bdd, struct TOStack *s) {
	bdd -> isVisited = true;
	struct TONode e = createTONode(bdd);
	addTOStack(s, e);
	if ((bdd->lo != NULL) && !(bdd -> lo -> isVisited)) {
		constructTOStackAux(bdd -> lo, s);
	}
	if ((bdd->hi != NULL) && !(bdd -> hi -> isVisited)) {
		constructTOStackAux(bdd -> hi, s);
	}
}

struct TOStack *constructTOStack (struct BDDNode *bdd) {
	// Receives a BDD and constructs stack with topological ordering
	// of its nodes using DFS.
	// Attention: the lo and hi fields are not filled here.
	
	struct TOStack *retVal = createStack();
	constructTOStackAux(bdd, retVal);	
	
	return retVal;
}

void delOneBDDNode (struct BDDNode *node) {
	if (node != NULL) {
		free (node -> lo);
		free (node -> hi);
		free (node);
	}
}

int sizeOfBDD (struct BDDNode *node) {
	// TODO: it is not working correctly now
	// This function returns the number of nodes of the BDD
	// headed by "node".
	if(node == NULL)
		return 0;
	int result = 1;
	result += sizeOfBDD(node -> hi);
	result += sizeOfBDD(node -> lo);
	return result;
}
