#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "orientation.h"
#include "bdd.h"


struct BDDNode *new_null_bdd_node (int v) {
	/* returns new BDDNode with index v pointing to NULLs*/
	struct BDDNode *ret_val = malloc (sizeof (struct BDDNode));

	if (ret_val == NULL)
		return NULL;

	ret_val -> v = v;
	ret_val -> hi = NULL;
	ret_val -> lo = NULL;
	ret_val -> is_visited = false;

	return ret_val;
}

struct TOStack *create_stack () {
	struct TOStack *s = malloc(sizeof(struct TOStack));
	s->n = 0;
	s->node_array = NULL;
	s->l = 0;
	return s;
}

void delete_to_stack (struct TOStack *s) {
	free(s->node_array);	
	free(s);
}

void add_to_stack (struct TOStack *s, struct TONode e) {
	// add e to s
	const int INITIAL_LENGTH = 4;
	if (s -> node_array == NULL) {
		//printf("debug1\n");fflush(stdout);
		s -> n = 0;
		s -> node_array = calloc(INITIAL_LENGTH, sizeof(struct TONode));
		s -> node_array[(s -> n)++] = e;
		s -> l = INITIAL_LENGTH;
		return;
	}

	if (s -> n == s -> l-1) {
		// Resize	
		// printf("debug2\n");fflush(stdout);
		struct TONode *temp = s -> node_array;
		s -> l = 2 * (s -> l);
		s -> node_array = malloc(sizeof(struct TONode) * (s -> l));

		for (int j = 0; j < s->n; j++) {
			s -> node_array[j] = temp[j];	
		}

		s -> node_array[(s -> n)++] = e;
		free (temp);
		return;
	}

		
	//printf("debug3\n");fflush(stdout);
	//printf("s->n = %d\n", s->n); fflush(stdout);
	s -> node_array[(s -> n)++] = e;
}

struct TONode create_to_node (struct BDDNode *bdd) {
	// returns a TONode pointing to bdd
	// lo and hi values are set to -1
	struct TONode ret_val; 
	ret_val.v = -1;
	ret_val.lo = -1;
	ret_val.hi = -1;
	return ret_val;
}

void construct_to_stack_aux (struct BDDNode *bdd, struct TOStack *s, int* k) {
	int k_current = *k;
	bdd -> is_visited = true;
	//printf("v = %d\n", bdd->v);
	struct TONode e = create_to_node(bdd);
	add_to_stack(s, e);
	s->node_array[k_current].v = bdd->v;
	if ((bdd->lo != NULL) && !(bdd -> lo -> is_visited)) {
		(*k)++;
		s->node_array[k_current].lo = (*k);
		construct_to_stack_aux(bdd -> lo, s, k);
	}
	if ((bdd->hi != NULL) && !(bdd -> hi -> is_visited)) {
		(*k)++;
		s->node_array[k_current].hi = (*k);
		construct_to_stack_aux(bdd -> hi, s, k);
	}
}

struct TOStack *construct_to_stack (struct BDDNode *bdd) {
	// Receives a BDD and constructs stack with topological ordering
	// of its nodes using DFS.
	// Attention: the lo and hi fields are not filled here.
	int k = 0;	
	struct TOStack *ret_val = create_stack();
	construct_to_stack_aux(bdd, ret_val, &k);	
	
	return ret_val;
}

void delete_bdd_aux (struct BDDNode *node, bool initialState) {
	if(node->is_visited != initialState) {
		return;
	}
	node->is_visited = !initialState;
	struct BDDNode *left = node->lo;
	struct BDDNode *right = node->hi;
	free(node);
	if(left != NULL && left->is_visited == initialState) {
		delete_bdd_aux(left, initialState);
	}
	if(right != NULL && right->is_visited == initialState) {
		delete_bdd_aux(right, initialState);
	}

}

void delete_bdd (struct BDDNode *node) {
	// This is the state of a node if it hasn't been visited yet.
	bool initialState = node->is_visited;

	delete_bdd_aux(node, initialState);
}

int size_of_bdd (struct BDDNode *node) {
	// TODO: it is not working correctly now
	// This function returns the number of nodes of the BDD
	// headed by "node".
	if(node == NULL)
		return 0;
	int result = 1;
	result += size_of_bdd(node -> hi);
	result += size_of_bdd(node -> lo);
	return result;
}

int *compute_profile(struct TOStack *s, int *length) {
	//Receives a stack representing a certain BDD and
	//returns an array with the profile of this BDD.
	//The variable "length" represents the length of the
	//return value.


	// Find depth of BDD
	int max = -2;
	for(int i = 0; i < s->n; i++) {
		if((s->node_array[i].v) > max) {
			max = s->node_array[i].v;
		}
	}


	max += 1; //level of T and F BDDNodes

	// allocate memory for retVal
	int *ret_val = (int *) calloc(max, sizeof(int));

	for(int i = 0; i < (s->n); i++) {
		if(s->node_array[i].v == -1) {
			ret_val[max-1]++;
			continue;
		}
		//printf("%d, ", s->node_array[i].bdd->v);
		ret_val[(s->node_array[i].v)-1]++;
	}	

	*length = max;

	
	return ret_val;
}

void test_stack(struct BDDNode *bdd) {
	struct TOStack *s = construct_to_stack(bdd);	
	int n = s->n;
	/////printf("The TOStack is the following:\n");
	/////for(int i = 0; i < n; i++) {
	/////	printf("Node #%d has v = %d, lo = %d, and hi = %d\n",
	/////			i,
	/////			s->node_array[i].v,
	/////			s->node_array[i].lo,
	/////			s->node_array[i].hi);
	/////}
	//printf("Total number of nodes is %d.\n", n);
	//////printf("%d\n", n);
	int length = 0;
	int *profile;
	profile = compute_profile(s, &length);
	/////printf("The profile of the BDD is (length = %d):\n", length);
	for(int i = 0; i < length; i++) {
		printf("%d ", profile[i]);
	}
	printf("\n");
	delete_to_stack(s);
}
