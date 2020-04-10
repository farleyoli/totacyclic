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

void deleteTOStack (struct TOStack *s) {
	free(s->nodeArray);	
	free(s);
}

void addTOStack (struct TOStack *s, struct TONode e) {
	// add e to s
	const int INITIAL_LENGTH = 4;
	if (s -> nodeArray == NULL) {
		//printf("debug1\n");fflush(stdout);
		s -> n = 0;
		s -> nodeArray = calloc(INITIAL_LENGTH, sizeof(struct TONode));
		s -> nodeArray[(s -> n)++] = e;
		s -> l = INITIAL_LENGTH;
		return;
	}

	if (s -> n == s -> l-1) {
		// Resize	
		// printf("debug2\n");fflush(stdout);
		struct TONode *temp = s -> nodeArray;
		s -> l = 2 * (s -> l);
		s -> nodeArray = malloc(sizeof(struct TONode) * (s -> l));

		for (int j = 0; j < s->n; j++) {
			s -> nodeArray[j] = temp[j];	
		}

		s -> nodeArray[(s -> n)++] = e;
		free (temp);
		return;
	}

		
	//printf("debug3\n");fflush(stdout);
	//printf("s->n = %d\n", s->n); fflush(stdout);
	s -> nodeArray[(s -> n)++] = e;
}

struct TONode createTONode (struct BDDNode *bdd) {
	// returns a TONode pointing to bdd
	// lo and hi values are set to -1
	struct TONode retVal; 
	retVal.v = -1;
	retVal.lo = -1;
	retVal.hi = -1;
	return retVal;
}

void constructTOStackAux (struct BDDNode *bdd, struct TOStack *s, int* k) {
	int kCurrent = *k;
	bdd -> isVisited = true;
	//printf("v = %d\n", bdd->v);
	struct TONode e = createTONode(bdd);
	addTOStack(s, e);
	s->nodeArray[kCurrent].v = bdd->v;
	if ((bdd->lo != NULL) && !(bdd -> lo -> isVisited)) {
		(*k)++;
		s->nodeArray[kCurrent].lo = (*k);
		constructTOStackAux(bdd -> lo, s, k);
	}
	if ((bdd->hi != NULL) && !(bdd -> hi -> isVisited)) {
		(*k)++;
		s->nodeArray[kCurrent].hi = (*k);
		constructTOStackAux(bdd -> hi, s, k);
	}
}

struct TOStack *constructTOStack (struct BDDNode *bdd) {
	// Receives a BDD and constructs stack with topological ordering
	// of its nodes using DFS.
	// Attention: the lo and hi fields are not filled here.
	int k = 0;	
	struct TOStack *retVal = createStack();
	constructTOStackAux(bdd, retVal, &k);	
	
	return retVal;
}

void deleteBDDAux (struct BDDNode *node, bool initialState) {
	if(node->isVisited != initialState) {
		return;
	}
	node->isVisited = !initialState;
	struct BDDNode *left = node->lo;
	struct BDDNode *right = node->hi;
	free(node);
	if(left != NULL && left->isVisited == initialState) {
		deleteBDDAux(left, initialState);
	}
	if(right != NULL && right->isVisited == initialState) {
		deleteBDDAux(right, initialState);
	}

}

void deleteBDD (struct BDDNode *node) {
	// This is the state of a node if it hasn't been visited yet.
	bool initialState = node->isVisited;

	deleteBDDAux(node, initialState);
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

int *computeProfile(struct TOStack *s, int *length) {
	//Receives a stack representing a certain BDD and
	//returns an array with the profile of this BDD.
	//The variable "length" represents the length of the
	//return value.


	// Find depth of BDD
	int max = -2;
	for(int i = 0; i < s->n; i++) {
		if((s->nodeArray[i].v) > max) {
			max = s->nodeArray[i].v;
		}
	}


	max += 1; //level of T and F BDDNodes

	// allocate memory for retVal
	int *retVal = (int *) calloc(max, sizeof(int));

	for(int i = 0; i < (s->n); i++) {
		if(s->nodeArray[i].v == -1) {
			retVal[max-1]++;
			continue;
		}
		//printf("%d, ", s->nodeArray[i].bdd->v);
		retVal[(s->nodeArray[i].v)-1]++;
	}	

	*length = max;

	
	return retVal;
}

void testStack(struct BDDNode *bdd) {
	struct TOStack *s = constructTOStack(bdd);	
	int n = s->n;
	printf("The TOStack is the following:\n");
	for(int i = 0; i < n; i++) {
		printf("Node #%d has v = %d, lo = %d, and hi = %d\n",
				i,
				s->nodeArray[i].v,
				s->nodeArray[i].lo,
				s->nodeArray[i].hi);
	}
	//printf("Total number of nodes is %d.\n", n);
	printf("%d\n", n);
	int length = 0;
	int *profile;
	profile = computeProfile(s, &length);
	printf("The profile of the BDD is (length = %d):\n", length);
	for(int i = 0; i < length; i++) {
		printf("%d ", profile[i]);
	}
	printf("\n");
	deleteTOStack(s);
}
