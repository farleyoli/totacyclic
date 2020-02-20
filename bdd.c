#include <stdio.h>
#include <stdlib.h>

struct Node {
	int v;			/* index of variable */
	struct Node *lo;	/* pointer to node when false */	
	struct Node *hi;	/* pointer to node when true */
};

struct Node *newNullNode (int v) {
	/* returns new Node with index v pointing to NULLs*/
	struct Node *retVal = malloc (sizeof (struct Node));

	if (retVal == NULL)
		return NULL;

	retVal -> v = v;
	retVal -> hi = NULL;
	retVal -> lo = NULL;

	return retVal;
}

void delOneNode (struct Node *node) {
	if (node != NULL) {
		free (node -> lo);
		free (node -> hi);
		free (node);
	}
}

int main() {
	/*
	printf("hello world\n");
	struct Node *test = newNullNode(5);
	printf("%d\n", (test -> v));
	delOneNode(test);
	*/

	return 0;
}
