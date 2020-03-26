#ifndef DARRAY
#define DARRAY

struct DynamicArray {
	int n; 			// Current number of nodes in the array.
	int* a;			// Arrray to store the elements.
	int l;			// The length of the array a.
	int j;			// Index of the first element.
};

struct DynamicArray* DAInitialize ();

int DAGet (int i, struct DynamicArray* da);

int DASet (int i, int x, struct DynamicArray* da);

void DAResize(struct DynamicArray* da);

void DAAdd (int i, int x, struct DynamicArray* da); 

int DARemove (int i, struct DynamicArray* da);

void DAPrint (struct DynamicArray* da);

void DATest ();

void DADelete (struct DynamicArray* da);

#endif
