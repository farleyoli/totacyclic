int compare_function (const void *a, const void *b) {
	// Compare function to be used with qsort to sort a list of
	// integers.
	int *x = (int *) a;
	int *y = (int *) b;
	return *x - *y;
}

int get_idx (int* array, int x, int length) {
	for(int i = 0; i < length; i++) {
		if(array[i] == x) {
			return i;
		}
	}
	return -1;
}
