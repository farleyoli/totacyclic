#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "dynamic-array.h"

struct DynamicArray* DAInitialize () {
	struct DynamicArray* da = malloc(sizeof(struct DynamicArray*));;
	da->n = 0;			
	da->l = 5;				// Initial value of 5.
	da->a = calloc(da->l,  sizeof(int));
	da->j = 0;
	return da;
}

int DAGet (int i, struct DynamicArray* da) {
	return da->a[(da->j+i)%da->l];
}

int DASet (int i, int x, struct DynamicArray* da) {
	// Sets i-th value to x and return previous value stored there.
	int y = da->a[(da->j+i)%da->l];
	da->a[(da->j+i)%da->l] = x;
	return y;
}

void DAResize(struct DynamicArray* da) {
	int* b = calloc((da->n >= 1 ? 2 * da->n : 1), sizeof(int));
	for(int i = 0; i < da->n; i++) {
		b[i] = da->a[(da->j+i)%da->l];
	}
	da->l = (da->n >= 1 ? 2 * da->n : 1);
	da-> j = 0;
	free(da->a);
	da->a = b;
}

void DAAdd (int i, int x, struct DynamicArray* da) {
	if(da->n + 1 > da->l) DAResize(da);
	if(i < (da->n)/2) { 
		// Shift a[0], ..., a[i-1] left.
		da->j = (da->j == 0) ? da->l - 1: da->j - 1;
		for(int k = 0; k < i-1; k++) {
			da->a[(da->j+k)%da->l] = da->a[(da->j+k+1)%da->l];
		}
	} else {
		// Shift a[i], ..., a[n-1] right.
		for(int k = da->n; k > i; k--) {
			da->a[(da->j+k)%da->l] = da->a[(da->j+k-1)%da->l];
		}
	}
	da->a[(da->j+i)%da->l] = x;
	da->n++;
}

void DAAppend (int x, struct DynamicArray* da) {
	DAAdd(da->n, x, da);
}
int DARemove (int i, struct DynamicArray* da) {
	int x = da->a[(da->j+1)%da->l];
	if(i < da->n/2) { 
		// Shift a[0], ..., a[i-1] right.
		for(int k = i; k > 0; k--) {
			da->a[(da->j+k)%da->l] = da->a[(da->j+k-1)%da->l];
		}
		da->j = (da->j+1)%da->l;
	} else {
		// Shift a[i], ..., a[n-1] left.
		for(int k = i; k < da->n - 1; k++) {
			da->a[(da->j+k)%da->l] = da->a[(da->j+k+1)%da->l];
		}
	}
	da->n--;

	if (3*da->n < da->l) DAResize(da);

	return x;
}

void DAPrint (struct DynamicArray* da) {
	printf("n = %d, a.length = %d, j = %d\n", da->n, da->l, da->j);
	for(int i = 0; i < da->n; i++) {
		printf("%d ", DAGet(i, da));
	}
	printf("\n");
}

void DATest () {
	struct DynamicArray* da = DAInitialize();
	DAAppend(0, da);
	DAPrint(da);
	DAAppend(7, da);
	DAPrint(da);
	DAAppend(0, da);
	DAPrint(da);
	DAAppend(7, da);
	DAPrint(da);
	DAAppend(1, da);
	DAPrint(da);
	DAAppend(9, da);
	DAPrint(da);
	DAAppend(9, da);
	DAPrint(da);
	DAAppend(1, da);
	DAPrint(da);
	DAAppend(9, da);
	DAPrint(da);
	DAAppend(1, da);
	DAPrint(da);
	DAAdd(2, 7, da);
	DAPrint(da);
	DARemove(2, da);
	DAPrint(da);
	DARemove(0, da);
	DAPrint(da);
	DARemove(0, da);
	DAPrint(da);
	DARemove(0, da);
	DAPrint(da);
	DARemove(0, da);
	DAPrint(da);
	DARemove(0, da);
	DAPrint(da);
	DARemove(0, da);
	DAPrint(da);
	DARemove(0, da);
	DAPrint(da);
	//DADelete(da);
}

void DADelete (struct DynamicArray* da) {
	free(da->a);
	free(da);
}
