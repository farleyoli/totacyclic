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

int DARemoveByIdx (int i, struct DynamicArray* da);

void DAPrint (struct DynamicArray* da);

void DATest ();

void DADelete (struct DynamicArray* da);

struct DynamicArray* DADifference (struct DynamicArray* da, struct DynamicArray* db);

struct DynamicArray* DACopy (struct DynamicArray* da);

struct DynamicArray* DAUnion (struct DynamicArray* da, struct DynamicArray* db, bool isSorted);

void DAAppend (int x, struct DynamicArray* da);

int DASize (struct DynamicArray* da);

int DAGetIdx (int x, struct DynamicArray* da);

int DARemoveElement(int x, struct DynamicArray* da);

void DAAddSorted (int x, struct DynamicArray* da);

bool DAAreElementsEqualInOrder(struct DynamicArray* da, struct DynamicArray* db);

bool DAIsSorted (struct DynamicArray* da);

bool DADoesContain (int x, struct DynamicArray* da, bool isSorted);

bool DAAreElementsEqualInOrderRestricted(struct DynamicArray* da, struct DynamicArray* db, struct DynamicArray* dc);

#endif
