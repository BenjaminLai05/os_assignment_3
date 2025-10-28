/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend) {	
	int i, j, k;

	int n1 = leftend - leftstart + 1;
	int n2 = rightend - rightstart + 1;

	int leftArr[n1], rightArr[n2];

	for (i = 0; i < n1; i++) {
		leftArr[i] = A[leftstart + i]; 
	}
	for (j = 0; j < n2; j++) {
		rightArr[j] = A[rightstart + j];
	}

	i = 0; 
	j = 0; 
	k = leftstart;

	while (i < n1 && j < n2) {
		if (leftArr[i] <= rightArr[j]) {
			A[k] = leftArr[i];
			i++;
		} else {
			A[k] = rightArr[j];
			j++;
		}
		k++;
	}

	while (i < n1) {
		A[k] = leftArr[i];
		i++;
		k++; 
	}

	while (j < n2) {
		A[k] = rightArr[j];
		j++;
		k++;
	}
}

/* this function will be called by parallel_mergesort() as its base case. this is known as mergesort in the assignment*/
void my_mergesort(int left, int right) {
	if (left < right) {
	int leftend = left + (right - left) / 2; 
	int rightstart = leftend + 1;
		
	my_mergesort(left, leftend);
	my_mergesort(rightstart, right);
	merge(left, leftend, rightstart, right);
	}
}


// assume our array is: t1 - {3,2,7,5,4} 

// {3,2,7,5,4} gets turned into:
// le: 2 
// rs: 3
// calls:
// 1st: ms(0, 2)

// ----

// ms(0,2) which is {3,2,7} calls:
// le: 1
// rs: 2
// 1st: ms(0, 1)

// ms(0,1) which is {3,2} calls:
// le: 0
// rs: 1
// 1st: ms(0, 0) Base case reached! returns nothing.

// Goes back up to ms(0,1) and calls 2nd function in ms(0,1).
// This gives
// le: 1
// rs: 1
// as the le in here is rs.
// 2nd: ms(1, 1) Base case reached! returns nothing

// merge(0,1) (This merges the first and second value)

// --- 

// Goes back to ms(0,2) and calls 1st ms function.
// le: 1 
// rs: 2
// ms(0,1) this returned the merged value of the merge of the first and second value

// 2nd ms called
// ms(2, 2)
// base case called! return nothing!

// call 3rd func, and merge
// ms(0,2)... continues


/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg) {
	struct argument *params;

	params = (struct argument *) arg;

	int left = params -> left;
	int right = params -> right;
	int level = params -> level;

	if (params -> level >= cutoff) {
		printf("[L%d] Serial: [%d..%d]\n", level, left, right);
		my_mergesort(left, right);
		return NULL;
	} else { 
		int leftend = left + (right - left) / 2;
		int rightstart = leftend + 1;

		printf("[L%d] Parallel: [%d..%d] -> LEFT[%d..%d] + RIGHT[%d..%d]\n", 
		       level, left, right, left, leftend, rightstart, right);

		pthread_t left_thread;
		pthread_t right_thread;

		struct argument* left_function = buildArgs(left, leftend, level + 1 ); 
		struct argument* right_function = buildArgs(rightstart, right, level + 1);

		pthread_create(&left_thread, NULL, parallel_mergesort, left_function);
		pthread_create(&right_thread, NULL, parallel_mergesort, right_function);

		pthread_join(left_thread, NULL);
		pthread_join(right_thread, NULL);

		free(left_function);
		free(right_function);
		
		
		printf("[L%d] Merged: [%d..%d]\n", level, left, right);

		merge(left, leftend, rightstart, right);
	}

	return NULL;
} 

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level)	{
	struct argument *p = malloc(sizeof *p);
	if (!p) { 
		perror("malloc");
		exit(1);
	} else {
		p -> left = left;
		p -> right = right; 
		p -> level = level;
		return p;
	}
		
}

