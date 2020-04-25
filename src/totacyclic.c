#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "bdd.h"
#include "orientation.h"
#include "dynamic-array.h"

struct OrientBuffer {
	/* Buffer containing the orientations for each level of the BDD*/
	struct OrientBuffer *next; 	/* next node in linked list */
	struct Orientation *orient; 	/* orientation of present node */
	struct BDDNode *node;		/* BDD Node to which current orientation corresponds*/
};

struct OrientBuffer *create_empty_buffer_node() {
	struct OrientBuffer *ret_val = malloc(sizeof(struct OrientBuffer));

	ret_val -> orient = NULL;
	ret_val -> next = NULL;
	ret_val -> node = NULL;
	return ret_val;
}

struct OrientBuffer *create_buffer_node(struct Orientation *orient) {
	struct OrientBuffer *ret_val = malloc(sizeof(struct OrientBuffer));

	ret_val -> orient = orient;
	ret_val -> next = NULL;
	ret_val -> node = NULL;
	return ret_val;
}

struct OrientBuffer *delete_initial_buffer_node (struct OrientBuffer *buf) {
	if (buf == NULL)
		return NULL;
	struct OrientBuffer *next = buf -> next;
	delete_orientation(buf -> orient);
	free(buf);
	return next;
}

void delete_buffer_list (struct OrientBuffer *initial) {
	while(initial != NULL)
		initial = delete_initial_buffer_node(initial);
}

struct BDDNode *add_to_buffer_list (struct OrientBuffer **buffer, struct Orientation *orient, int *ef, int sizeEF, struct BDDNode *bddNode, bool isLo, int V) {
	/*this function assumes that all elements have the same ef! */
	/* add orientation to the buffer list, as long as the orientation is not already present*/
	/* returns pointer to BDD node to which "node" should point in case it is already present and NULL otherwise*/
	if((*buffer) == NULL) {
		//printf("1\n");
		*buffer = create_buffer_node(orient);
		if (isLo) {
			bddNode -> lo = new_null_bdd_node(V);
			(*buffer) -> node = bddNode -> lo;
		} else {
			bddNode -> hi = new_null_bdd_node(V);
			(*buffer) -> node = bddNode -> hi;
		}
		return NULL;
	}

	struct OrientBuffer *temp = *buffer;

	do {
		//printf("2\n");
		if(are_reach_relations_equal(temp -> orient, orient, ef, sizeEF)) {
			/* no need to add, do some magic and return */
			return temp->node;
		}
		if(temp -> next != NULL) {
			temp = temp -> next; 
		}
	} while (temp -> next != NULL);
	if(are_reach_relations_equal(temp -> orient, orient, ef, sizeEF)) {
		return temp->node;
	}

	/* we add orientation to the BufferList */
	//printf("3\n");
	struct OrientBuffer *new = create_buffer_node(orient);
	temp -> next = new;
	if(isLo) {
		bddNode -> lo = new_null_bdd_node(V);
		(temp -> next) -> node = bddNode -> lo;
	} else {
		bddNode -> hi = new_null_bdd_node(V);
		(temp -> next) -> node = bddNode -> hi;
	}
	return NULL;
}

int size_buffer(struct OrientBuffer *b) {
	int i = 0;
	if(b == NULL) {
		return i;
	}

	while (b != NULL) {
		i++;
		b = b -> next;
	}

	return i;
}

void print_buffer(struct OrientBuffer *b) {
	if(b == NULL) {
		return;
	}

	while (b != NULL) {
		print_orientation(b -> orient);
		b = b -> next;
	}
}


struct BDDNode *create_bdd(struct Orientation *undir, bool is_acyclic) {
	/* the order of the vertices is the natural one */
	int i, j, m, size_buf;
	struct Orientation *temp_or_1 = NULL;
	struct Orientation *temp_or_2 = NULL;
	struct Orientation *temp = NULL;
	struct OrientBuffer *next_buffer = create_buffer_node(undir);
	struct OrientBuffer *prev_buffer = next_buffer;
	struct OrientBuffer *init_prev_buffer = NULL;
	m = undir -> m;
	//printf("m = %d\n", m);

	int *u = malloc(m * sizeof(int));
	int *v = malloc(m * sizeof(int));
	compute_lex_order(u, v, undir);

	int *ef = NULL;
	int sizeEF = 0;
	struct BDDNode *ret_val = new_null_bdd_node(1);
	struct BDDNode *trav = ret_val;
	struct BDDNode *temp_bdd_node = NULL;
	prev_buffer -> node = trav; 
	struct BDDNode *T = new_null_bdd_node(-1);
	struct BDDNode *F = new_null_bdd_node(-1);

	for (i = 0; i < m; i++) { 
		/* construct (i+1)-th level from i-th level */
		//printf("%d/%d\n", i, m);

		next_buffer = NULL;	/* buffer for (i+1)-th level */
		size_buf = size_buffer(prev_buffer);
		//printf("i------------------------------\n");
		//printf("Buffer (size = %d, i = %d):\n", size_buf, i);
		//print_buffer(prev_buffer);
		//printf("f------------------------------\n");

		init_prev_buffer = prev_buffer;

		if (i == m-1) {
			// The processing is different for the last
			// ((m-1)-th to m-th) level.
			if (is_acyclic) {
				for(j = 0; j < size_buf; j++) {
					temp_or_1 = copy_orientation(prev_buffer -> orient);
					trav = prev_buffer -> node;
					temp_or_2 = copy_orientation(temp_or_1);
					orient_edge(temp_or_1, u[i], v[i]);
					orient_edge(temp_or_2, v[i], u[i]);
					if (is_self_reachable(temp_or_1, 0)) {
						//printf("hue1\n");
						prev_buffer -> node -> lo = F;
					} else {
						//printf("hue2\n");
						prev_buffer -> node -> lo = T;
					}
					if (is_self_reachable(temp_or_2, 0)) {
						//printf("hue3\n");
						prev_buffer -> node -> hi = F;
					} else {
						//printf("hue4\n");
						prev_buffer -> node -> hi = T;
					}
					prev_buffer = prev_buffer -> next;
				}
			} else {
					temp_or_1 = copy_orientation(prev_buffer -> orient);
					trav = prev_buffer -> node;
					temp_or_2 = copy_orientation(temp_or_1);
					orient_edge(temp_or_1, u[i], v[i]);
					orient_edge(temp_or_2, v[i], u[i]);
					if (!is_self_reachable(temp_or_1, 0)) {
						//printf("hue1\n");
						prev_buffer -> node -> lo = F;
					} else {
						//printf("hue2\n");
						prev_buffer -> node -> lo = T;
					}
					if (!is_self_reachable(temp_or_2, 0)) {
						//printf("hue3\n");
						prev_buffer -> node -> hi = F;
					} else {
						//printf("hue4\n");
						prev_buffer -> node -> hi = T;
					}
					prev_buffer = prev_buffer -> next;
			}
			//size_buf = size_buffer(next_buffer);
			prev_buffer = next_buffer;
			delete_buffer_list(init_prev_buffer);
			break;
		}


		for(j = 0; j < size_buf; j++) {
			temp_or_1 = copy_orientation(prev_buffer -> orient);
			trav = prev_buffer -> node;
			temp_or_2 = copy_orientation(temp_or_1);


			/* lo */
			orient_edge(temp_or_1, u[i], v[i]);
			if (i > 1 && 
			((is_acyclic && is_cyclic(temp_or_1, u[i])) ||
			 (!is_acyclic && 
			  (is_source_or_sink(temp_or_1, u[i]) 
			   || is_source_or_sink(temp_or_1, v[i]))))) {
				trav->lo = F;	
				free(temp_or_1);
				goto label_hi;
			}

			if(j == 0) {
				ef = compute_elimination_front(temp_or_1, &sizeEF);	
			}

			temp_bdd_node = add_to_buffer_list(&next_buffer, temp_or_1, ef, sizeEF, trav, true, i+2);


			if (temp_bdd_node != NULL) {
				trav -> lo = temp_bdd_node; 
				free(temp_or_1);
			}
			
			label_hi:
			/* hi */
			orient_edge(temp_or_2, v[i], u[i]);
			if (i > 1 && 
			((is_acyclic && is_cyclic(temp_or_2, u[i])) ||
			 (!is_acyclic && 
			  (is_source_or_sink(temp_or_2, u[i]) 
			   || is_source_or_sink(temp_or_2, v[i]))))) {
				trav->hi = F;
				free(temp_or_2);
				prev_buffer = prev_buffer -> next;
				continue;
			}


			temp_bdd_node = add_to_buffer_list(&next_buffer, temp_or_2, ef, sizeEF, trav, false, i+2);

			if(temp_bdd_node != NULL) {
				trav -> hi = temp_bdd_node; 
				free(temp_or_2);
			}

			prev_buffer = prev_buffer -> next;
		}
		size_buf = size_buffer(next_buffer);
		/*
		if (size_buf == 1) {
			printf("Buffer for i = %d\n\n", i);
			print_buffer(next_buffer);
		}
		*/
		prev_buffer = next_buffer;
		free(ef);
		delete_buffer_list(init_prev_buffer);
	}
	//size_buf = size_buffer(prev_buffer);
	//printf("final------------------------------\n");
	//printf("Buffer (size = %d, i = %d):\n", size_buf, i);
	//print_buffer(prev_buffer);
	//printf("final------------------------------\n");
	free(u);
	free(v);
	return ret_val;
}

struct BDDNode *create_cycle_bdd(int n) {
	struct Orientation *undir = create_cycle(n);
	struct BDDNode *ret_val = create_bdd(undir, true);
	return ret_val;
}

struct BDDNode *create_complete_bdd(int n, bool is_acyclic) {
	struct Orientation *undir = create_complete_graph(n);
	struct BDDNode *ret_val = create_bdd(undir, is_acyclic);
	return ret_val;
}


struct BDDNode *create_erbdd(int n, double p) {
	struct Orientation *undir = create_erdos_renyi(n, p) ;
	struct BDDNode *ret_val = create_bdd(undir, true);
	return ret_val;
}


void test_buffer_list() {
	/* say test all the permutations of (1,2), (2,3) and (3,4) */
	/* there should be three elements, because there are only 2 elements in the elimination front */
	struct Orientation *orient = NULL;
	struct OrientBuffer *buffer = NULL;
	int *ef = NULL;
	int sizeEF = 0;


	orient = create_cycle(5);
	orient_edge(orient, 1, 2);
	orient_edge(orient, 2, 3);
	orient_edge(orient, 3, 4);
	buffer = create_buffer_node(orient);
	/* initialization */
	ef = compute_elimination_front(orient, &sizeEF);

	/*

	orient = create_cycle(5);
	orient_edge(orient, 1, 2);
	orient_edge(orient, 2, 3);
	orient_edge(orient, 4, 3);
	add_to_buffer_list(&buffer, orient, NULL, ef, sizeEF);

	orient = create_cycle(5);
	orient_edge(orient, 1, 2);
	orient_edge(orient, 3, 2);
	orient_edge(orient, 3, 4);
	add_to_buffer_list(&buffer, orient, NULL, ef, sizeEF);

	orient = create_cycle(5);
	orient_edge(orient, 1, 2);
	orient_edge(orient, 3, 2);
	orient_edge(orient, 4, 3);
	add_to_buffer_list(&buffer, orient, NULL, ef, sizeEF);

	orient = create_cycle(5);
	orient_edge(orient, 2, 1);
	orient_edge(orient, 2, 3);
	orient_edge(orient, 3, 4);
	add_to_buffer_list(&buffer, orient, NULL, ef, sizeEF);

	orient = create_cycle(5);
	orient_edge(orient, 2, 1);
	orient_edge(orient, 2, 3);
	orient_edge(orient, 4, 3);
	add_to_buffer_list(&buffer, orient, NULL, ef, sizeEF);

	orient = create_cycle(5);
	orient_edge(orient, 2, 1);
	orient_edge(orient, 3, 2);
	orient_edge(orient, 3, 4);
	add_to_buffer_list(&buffer, orient, NULL, ef, sizeEF);

	orient = create_cycle(5);
	orient_edge(orient, 2, 1);
	orient_edge(orient, 3, 2);
	orient_edge(orient, 4, 3);
	add_to_buffer_list(&buffer, orient, NULL, ef, sizeEF);

	*/

	free(ef);

	printf("size of buffer: %d\n", size_buffer(buffer));

	delete_buffer_list(buffer);
	
}




int main() {
	srand(time(0));

	//int n = 10;

	//struct Orientation *orient = create_complete_graph(n);
	//print_orientation(orient);

	//struct BDDNode *bdd = create_erbdd(12, 0.35);
	//struct BDDNode *bdd = create_cycle_bdd(1000);
	//testEdgeOrder();
	
	//int n;
	//scanf("%d", &n);

	
	struct Orientation *orient;
	char file_name_base[] = "../graphs/";
	char file_name_ending[] = ".txt";
	char file_name[256] = "";
	char num[32] = "";
	struct BDDNode *bdd;
	for(int i = 8; i < 10; i++) {
		/////printf("%d\n", i);
		sprintf(num, "%d", i+1);
		strcat(file_name, file_name_base);
		strcat(file_name, num);
		strcat(file_name, file_name_ending);
		orient = import_from_file(file_name);
		//print_orientation(orient);fflush(stdout);

		clock_t t; 
		t = clock(); 

		bdd = create_bdd(orient, true);
		//bdd = create_complete_bdd(i, false);

		t = clock() - t; 
		double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
		/////printf("It took %f seconds to create the BDD \n", time_taken);
		/////printf("%f\n", time_taken);
		test_stack(bdd);
		memset(file_name, 0, sizeof(file_name));
		delete_bdd(bdd);
	}
	return 0;
}
