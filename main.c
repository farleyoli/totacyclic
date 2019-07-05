#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct bdd { // to do
	int n; //number of variables
};

struct pd { //path decomposition
	int n; //number of bags
	int *size_bags; // array with the size of each bag
	int **bag; // "array of arrays" (variable length)
	int pw;
};

struct edge {
	// can be oriented or unoriented
	// for unoriented, v1 <= v2
	int v1;
	int v2;
};

void print_array2d(int **A, size_t height, size_t width) {
	for(size_t i = 0; i < height; ++i)
		for(size_t j = 0; j < width; ++j)
      			printf("A[%zu][%zu] = %d\n", i, j, A[i][j]);
}

void print_array1d(int *A, size_t length) {
	for(size_t i = 0; i < length; ++i)
		printf("A[%zu] = %d\n", i, A[i]);
}

void get_input_test (int ***graph, int *n, int *m, struct pd *decomp) {
	// this is a provisional version for test purposes
	// working with petersen graph, proper version
	// will be added later
	*n = 10;
	*m = 15;
	int graph_temp[10][10] = { 
		{0, 1, 0, 0, 1, 1, 0, 0, 0, 0},
		{1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
		{0, 1, 0, 1, 0, 0, 0, 1, 0 ,0},
		{0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
		{1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1, 1, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 1, 1},
		{0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
		{0, 0, 0, 1, 0, 1, 1, 0, 0, 0},
		{0, 0, 0, 0, 1, 0, 1, 1, 0, 0}
	};

	//*graph = graph_temp;
	(*graph) = (int **) malloc( (*n) * sizeof(int *));
	for (int i = 0; i < *n; i++)
		(*graph)[i] = (int *) malloc( (*n) * sizeof(int));

	for (int i = 0; i < *n; i++)
		for (int j = 0; j < *n; j++)
			(*graph)[i][j] = graph_temp[i][j];

	(*decomp).n = 5; 

	(*decomp).size_bags = (int *) malloc((*decomp).n * sizeof(int));
	(*decomp).bag = (int **) malloc((*decomp).n * sizeof(int *));

	(*decomp).size_bags[0] = 4; (*decomp).size_bags[1] = 5; (*decomp).size_bags[2] = 6; 
	(*decomp).size_bags[3] = 6; (*decomp).size_bags[4] = 6;

	(*decomp).pw = 5;

	for (int i = 0; i < (*decomp).n; i++) {
		(*decomp).bag[i] = (int *) malloc((*decomp).size_bags[i] * sizeof(int));
	}

	// i am so sorry
	(*decomp).bag[0][0] = 0; (*decomp).bag[0][1] = 1; (*decomp).bag[0][2] = 4; (*decomp).bag[0][3] = 5;
	(*decomp).bag[1][0] = 1; (*decomp).bag[1][1] = 2; (*decomp).bag[1][2] = 4; (*decomp).bag[1][3] = 5; (*decomp).bag[1][4] = 6;
	(*decomp).bag[2][0] = 2; (*decomp).bag[2][1] = 3; (*decomp).bag[2][2] = 4; (*decomp).bag[2][3] = 5; (*decomp).bag[2][4] = 6; (*decomp).bag[2][5] = 7;
	(*decomp).bag[3][0] = 3; (*decomp).bag[3][1] = 4; (*decomp).bag[3][2] = 5; (*decomp).bag[3][3] = 6; (*decomp).bag[3][4] = 7; (*decomp).bag[3][5] = 9;
	(*decomp).bag[4][0] = 3; (*decomp).bag[4][1] = 5; (*decomp).bag[4][2] = 6; (*decomp).bag[4][3] = 7; (*decomp).bag[4][4] = 8; (*decomp).bag[4][5] = 9;
}

void get_ordering(int **graph, int n, int m, struct pd decomp, struct edge **ordering) {
	*ordering = (struct edge *) malloc(m * sizeof(struct edge));

	int *vordering = (int *) malloc (n * sizeof(int)); // ordering of vertices
	int *vflag = (int *) malloc (n * sizeof(int)); // 1 iff vertex in vordering
	for (int i = 0; i < n; i++)
		vflag[i] = 0;
		
	int counter = 0;
	for (int i = 0; i < decomp.n; i++)
		for (int j = 0; j < decomp.size_bags[i]; j++)
			if (vflag[decomp.bag[i][j]] == 0) {
				vordering[counter++] = decomp.bag[i][j];
				vflag[decomp.bag[i][j]] = 1;
			}

	//print_array1d(vordering, n);
	free(vflag);

	counter = 0;
	for (int i = 0; i < n; i++) 
		for (int j = i+1; j < n; j++) {
			int v1 = vordering[i];
			int v2 = vordering[j];
			if (graph[v1][v2] != 0) {
				struct edge e;
				e.v1 = v1;
				e.v2 = v2;
				(*ordering)[counter++] = e;
			}
		}

	//for (int i = 0; i < m; i++)
	//	printf("%d %d\n", (*ordering)[i].v1, (*ordering)[i].v2);

	free(vordering);
}

bool is_int_in_array(int v, int *elimination_front, int size) {
	if (size == 0)
		return false;
	for (int i = 0; i < size; i++) {
		if(v == elimination_front[i])
			return true;
	}
	return false;
}

void update_elimination_front (int **graph, int n, int m, struct edge *ordering,
		int i, int *elimination_front, int* counter) {
	//TODO
	
}

void construct_original_bdd(int **graph, int n, int m, struct pd decomp,
		struct edge *ordering, struct bdd **original) {
	int elimination_front[decomp.pw]; //max size is pw
	int counter = 0;

	for (int i = 0; i < m; i++) {
		update_elimination_front(graph, n, m, ordering, i, 
				elimination_front, &counter);

	}
}

int main(void) {
	// get input
	int **graph, n, m; // adjacency matrix, number of vertices, number of edges
	struct pd decomp; 
	get_input_test(&graph, &n, &m, &decomp);

	// get ordering of edges
	struct edge *ordering;
	get_ordering(graph, n, m, decomp, &ordering);
	
	// construct original BDD 
	struct bdd *original;
	construct_original_bdd(graph, n, m, decomp, ordering, &original);
	// // create function which takes a graph, the elimination front and 
	// // returns the reachability relation on it (how to store it?)
	
	// reduce BDD
	return 0;	
}
