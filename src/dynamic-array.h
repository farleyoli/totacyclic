#ifndef DARRAY
#define DARRAY

struct DynamicArray {
	int* a;			// Arrray to store the elements.
	int n; 			// Current number of nodes in the array.
	int l;			// The length of the array a.
	int j;			// Index of the first element.
};

struct DynamicArray* da_initialize ();

int da_get (int i, struct DynamicArray* da);

int da_set (int i, int x, struct DynamicArray* da);

void da_resize(struct DynamicArray* da);

void da_add (int i, int x, struct DynamicArray* da); 

int da_remove_by_idx (int i, struct DynamicArray* da);

void da_print (struct DynamicArray* da);

void da_test ();

void da_delete (struct DynamicArray* da);

struct DynamicArray* da_difference (struct DynamicArray* da, struct DynamicArray* db);

struct DynamicArray* da_copy (struct DynamicArray* da);

struct DynamicArray* da_union (struct DynamicArray* da, struct DynamicArray* db, bool is_sorted);

void da_append (int x, struct DynamicArray* da);

int da_size (struct DynamicArray* da);

int da_get_idx (int x, struct DynamicArray* da);

int da_remove_element(int x, struct DynamicArray* da);

void da_add_sorted (int x, struct DynamicArray* da);

bool da_are_elements_equal_in_order(struct DynamicArray* da, struct DynamicArray* db);

bool da_is_sorted (struct DynamicArray* da);

bool da_does_contain (int x, struct DynamicArray* da, bool is_sorted);

bool da_are_elements_equal_in_order_restricted(struct DynamicArray* da, struct DynamicArray* db, struct DynamicArray* dc);

#endif
