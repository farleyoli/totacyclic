#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "dynamic-array.h"
#include "general-utils.h"

struct DynamicArray* da_initialize () {
	struct DynamicArray* da = malloc(sizeof(struct DynamicArray));;
	da->n = 0;			
	da->l = 5;				// Initial value of 5.
	da->a = calloc(da->l,  sizeof(int));
	da->j = 0;
	return da;
}

int da_get (int i, struct DynamicArray* da) {
	return da->a[(da->j+i)%da->l];
}

int da_set (int i, int x, struct DynamicArray* da) {
	// Sets i-th value to x and return previous value stored there.
	int y = da->a[(da->j+i)%da->l];
	da->a[(da->j+i)%da->l] = x;
	return y;
}

void da_resize(struct DynamicArray* da) {
	int* b = calloc((da->n >= 1 ? 2 * da->n : 1), sizeof(int));
	for(int i = 0; i < da->n; i++) {
		b[i] = da->a[(da->j+i)%da->l];
	}
	da->l = (da->n >= 1 ? 2 * da->n : 1);
	da-> j = 0;
	free(da->a);
	da->a = b;
}

void da_sort(struct DynamicArray* da) {
	da_resize(da);
	qsort(da->a, da->n, sizeof(int), compare_function);
}

void da_add (int i, int x, struct DynamicArray* da) {
	if(da->n + 1 > da->l) da_resize(da);
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

void da_append (int x, struct DynamicArray* da) {
	da_add(da->n, x, da);
}
int da_remove_by_idx (int i, struct DynamicArray* da) {
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

	if (3*da->n < da->l) da_resize(da);

	return x;
}

void da_print (struct DynamicArray* da) {
	if(da == NULL) {
		//printf("DynamicArray is null.\n");
		return;
	}
	printf("n = %d, a.length = %d, j = %d\n", da->n, da->l, da->j);
	for(int i = 0; i < da->n; i++) {
		printf("%d ", da_get(i, da));
	}
	printf("\n");
}

void da_test () {
	struct DynamicArray* da = da_initialize();
	struct DynamicArray* db = da_initialize();
	da_append(0, da);
	da_append(7, da);
	da_append(0, da);
	da_append(7, da);
	da_append(1, da);
	da_append(9, da);
	da_append(9, da);
	da_append(1, da);
	da_append(9, da);
	da_append(1, da);
	da_append(5, da);
	da_sort(da);
	da_print(da);
	da_add_sorted(20, da);
	da_print(da);
	da_add_sorted(5, da);
	da_print(da);

	da_append(-1, db);
	da_append(1, db);
	da_append(-2, db);
	da_append(2, db);
	da_append(-2, db);
	da_append(3, db);
	da_append(-3, db);
	//da_sort(db);
	da_print(db);

	struct DynamicArray* dc = da_union(da, db, true);
	da_print(dc);
	da_print(da);
	da_print(db);
}

void da_delete (struct DynamicArray* da) {
	free(da->a);
	free(da);
}


int da_size (struct DynamicArray* da) {
	return da->n;
}

struct DynamicArray* da_copy (struct DynamicArray* da) {
	struct DynamicArray* db = da_initialize();
	for(int i = da->n-1; i >= 0; i--) {
		da_add(0, da_get(i, da), db);
	}
	return db;
}

struct DynamicArray* da_union (struct DynamicArray* das, struct DynamicArray* dbs, bool is_sorted) {
	// It returns a new (sorted) DynamicArray containing _a unique_ copy of each
	// element in da and db.
	// e.g.: [1 1 3] union [1 3 3 5] will return [1 3 5] 
	struct DynamicArray* da;	
	struct DynamicArray* db;	
	if(is_sorted) {
		da = das;
		db = dbs;
	} else {
		da = da_copy(das);
		da_sort(da);
		db = da_copy(dbs);
		da_sort(db);
	}
	struct DynamicArray *ret = da_initialize();

	da_append(INT_MAX, da);		// Sentinel.
	da_append(INT_MAX, db);		// Sentinel. 
	int ca = 0, cb = 0;
	int min;
	while(ca < da->n && cb < db->n) {
		if( da_get(ca, da) > da_get(cb,db) ) {
			min = da_get (cb++, db);
		} else {
			min = da_get (ca++, da);
		}
		//printf("min = %d, ca = %d, cb = %d na = %d, nb = %d\n", min, ca, cb, da->n, db->n);
		if ((!(ret->n == 0)) && min <= da_get(ret->n-1, ret)) {
			continue;
		}
		da_append(min, ret);
	}
	da_remove_by_idx(da->n, da);
	da_remove_by_idx(db->n, db);
	da_remove_by_idx(ret->n, ret);
	if(!is_sorted) {
		da_delete(da);
		da_delete(db);
	}
	return ret;
}

int da_get_idx (int x, struct DynamicArray* da) {
	for(int i = 0; i < da->n; i++) {
		if(da_get(i, da) == x) {
			return i;
		}
	}
	return -1;
}

int da_remove_element(int x, struct DynamicArray* da) {
	return da_remove_by_idx(da_get_idx(x, da), da);	
}

void da_add_sorted (int x, struct DynamicArray* da) {
	// Add element to a sorted DA and keep it sorted;
	// Uses binary search.
	
	int left = 0, right = da_size(da)-1;

	if(x <= da_get(left, da)) {
		da_add(left, x, da);
		return;
	}
	if(x >= da_get(right, da)) {
		da_add(right+1, x, da);
		return;
	}

	int mid = (left+right)/2;
	int valMid;
	while(left < right) {
		valMid = da_get(mid, da);
		if(x == valMid) {
			da_add(mid+1, x, da);
			return;
		} 
		
		if(x > valMid && x < da_get(mid+1, da)) {
			da_add(mid+1, x, da);
			return;
		} 

		if(x < valMid && x > da_get(mid+1, da)) {
			da_add(mid, x, da);
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
	printf("Something went wrong in da_add_sorted.\n");
	return;
}

bool da_are_elements_equal_in_order(struct DynamicArray* da, struct DynamicArray* db) {
	if(da == NULL && db == NULL) return true;
	if(da_size(da) != da_size(db)) return false;

	for(int i = 0; i < da_size(da); i++) {
		if(da_get(i, da) != da_get(i, db))
			return false;
	}

	return true;
}

bool da_is_sorted (struct DynamicArray* da) {
	if(da == NULL) {
		printf("da_is_sorted: Dynamic Array is null\n");
		return true;
	}
	for(int i = 0; i < da->n-1; i++) {
		if(da_get(i, da) > da_get(i+1, da)) return false;
	}
	return true;
}

bool da_does_contain (int x, struct DynamicArray* da, bool is_sorted) {
	// TODO: complete for non-sorted
	if(!is_sorted) {
		printf("da_does_contain for non-sorted array still not supported!\n");
		exit(0);
	}

	if(da == NULL) {
		printf("Array is null!\n");
		exit(0);
	}

	if(da->n == 0) {
		//printf("Array is empty!\n");
		return false;
	}

	int left = 0, right = da_size(da)-1;
	int mid = (left+right)/2;

	if(da_get(left, da) == x || da_get(right, da) == x) {
		return true;
	}

	while(mid != right && mid != left) {
		int mid_val = da_get(mid, da);
		if (mid_val == x) {
			return true;
		}
		if ( x < mid_val ) {
			right = mid;	
		} else {
			left = mid;
		}
		mid = (right+left)/2;
	}

	if(da_get(left, da) == x || da_get(right, da) == x) {
		return true;
	}

	return false;
}

bool da_are_elements_equal_in_order_restricted(struct DynamicArray* da, struct DynamicArray* db, struct DynamicArray* dc) {
	if(da == NULL && db == NULL) return true;
	
	for(int i = 0; i < da_size(dc); i++) {
		int elem = da_get(i, dc);
		if(da_does_contain(elem, da, true) && !da_does_contain(elem, db, true)) {
			return false;
		}
		if(!da_does_contain(elem, da, true) && da_does_contain(elem, db, true)) {
			return false;
		}
	}

	return true;
}
