#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "dynamic-array.h"
#include "general-utils.h"

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

void DASort(struct DynamicArray* da) {
	DAResize(da);
	qsort(da->a, da->n, sizeof(int), compareFunction);
}

void DAAdd (int i, int x, struct DynamicArray* da) {
	if(da->n + 1 > da->l) DAResize(da);
	if(i < (da->n)/2) { 
		// Shift a[0], ..., a[i-1] left.
		da->j = (da->j == 0) ? da->l - 1: da->j - 1;
		for(int k = 0; k < i; k++) {
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
int DARemoveByIdx (int i, struct DynamicArray* da) {
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
	if(da == NULL) {
		printf("DynamicArray is null.\n");
		return;
	}
	printf("n = %d, a.length = %d, j = %d\n", da->n, da->l, da->j);
	for(int i = 0; i < da->n; i++) {
		printf("%d ", DAGet(i, da));
	}
	printf("\n");
}

void DATest () {
	struct DynamicArray* da = DAInitialize();
	struct DynamicArray* db = DAInitialize();
	DAAppend(0, da);
	DAAppend(7, da);
	DAAppend(0, da);
	DAAppend(7, da);
	DAAppend(1, da);
	DAAppend(9, da);
	DAAppend(9, da);
	DAAppend(1, da);
	DAAppend(9, da);
	DAAppend(1, da);
	DAAppend(5, da);
	DASort(da);
	DAPrint(da);
	DAAppendSorted(20, da);
	DAPrint(da);
	DAAppendSorted(5, da);
	DAPrint(da);

	DAAppend(-1, db);
	DAAppend(1, db);
	DAAppend(-2, db);
	DAAppend(2, db);
	DAAppend(-2, db);
	DAAppend(3, db);
	DAAppend(-3, db);
	//DASort(db);
	DAPrint(db);

	struct DynamicArray* dc = DAUnion(da, db, true);
	DAPrint(dc);
	DAPrint(da);
	DAPrint(db);
}

void DADelete (struct DynamicArray* da) {
	free(da->a);
	free(da);
}


int DASize (struct DynamicArray* da) {
	return da->n;
}

struct DynamicArray* DACopy (struct DynamicArray* da) {
	struct DynamicArray* db = DAInitialize();
	for(int i = da->n-1; i >= 0; i--) {
		DAAdd(0, DAGet(i, da), db);
	}
	return db;
}

struct DynamicArray* DAUnion (struct DynamicArray* das, struct DynamicArray* dbs, bool isSorted) {
	// It returns a new (sorted) DynamicArray containing _a unique_ copy of each
	// element in da and db.
	// e.g.: [1 1 3] union [1 3 3 5] will return [1 3 5] 
	struct DynamicArray* da;	
	struct DynamicArray* db;	
	if(isSorted) {
		da = das;
		db = dbs;
	} else {
		da = DACopy(das);
		DASort(da);
		db = DACopy(dbs);
		DASort(db);
	}
	struct DynamicArray *ret = DAInitialize();

	DAAppend(INT_MAX, da);		// Sentinel.
	DAAppend(INT_MAX, db);		// Sentinel. 
	int ca = 0, cb = 0;
	int min;
	while(ca < da->n && cb < db->n) {
		if( DAGet(ca, da) > DAGet(cb,db) ) {
			min = DAGet (cb++, db);
		} else {
			min = DAGet (ca++, da);
		}
		//printf("min = %d, ca = %d, cb = %d na = %d, nb = %d\n", min, ca, cb, da->n, db->n);
		if ((!(ret->n == 0)) && min <= DAGet(ret->n-1, ret)) {
			continue;
		}
		DAAppend(min, ret);
	}
	DARemoveByIdx(da->n, da);
	DARemoveByIdx(db->n, db);
	DARemoveByIdx(ret->n, ret);
	if(!isSorted) {
		DADelete(da);
		DADelete(db);
	}
	return ret;
}

int DAGetIdx (int x, struct DynamicArray* da) {
	for(int i = 0; i < da->n; i++) {
		if(DAGet(i, da) == x) {
			return i;
		}
	}
	return -1;
}

int DARemoveElement(int x, struct DynamicArray* da) {
	return DARemoveByIdx(DAGetIdx(x, da), da);	
}

void DAAppendSorted (int x, struct DynamicArray* da) {
	// Append element to a sorted DA and keep it sorted;
	// Uses binary search.
	
	int left = 0, right = DASize(da)-1;

	if(x <= DAGet(left, da)) {
		DAAdd(left, x, da);
		return;
	}
	if(x >= DAGet(right, da)) {
		DAAdd(right+1, x, da);
		return;
	}

	int mid = (left+right)/2;
	int valMid;
	while(left < right) {
		valMid = DAGet(mid, da);
		if(x == valMid) {
			DAAdd(mid+1, x, da);
			return;
		} 
		
		if(x > valMid && x < DAGet(mid+1, da)) {
			DAAdd(mid+1, x, da);
			return;
		} 

		if(x < valMid && x > DAGet(mid+1, da)) {
			DAAdd(mid, x, da);
			return;
		} 

		if(x > valMid) {
			left = mid;
			mid = (right+left)/2;
			continue;
		}
		if(x < valMid) {
			right = mid;
			mid = (right+left)/2;
		}
	}
	printf("Something went wrong in DAAppendSorted.\n");
	return;
}

