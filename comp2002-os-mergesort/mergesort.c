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

	// copy the entire range to be merged into temp array B
	// this uses the global array B instead of stack-based VLAs
	int size = rightend - leftstart + 1;
	memcpy(&B[leftstart], &A[leftstart], size * sizeof(int));

	i = leftstart;  // index for left subarray in B
	j = rightstart; // index for right subarray in B
	k = leftstart;  // index for merged array in A

	// merge the two sorted halves from B back into A
	while (i <= leftend && j <= rightend) {
		if (B[i] <= B[j]) {
			A[k] = B[i];
			i++; 
		} else {
			A[k] = B[j];
			j++;
		}
		k++;
	}

	// Copy any remaining elements from left half
	while (i <= leftend) {
		A[k] = B[i];
		i++;
		k++; 
	}
	
	// Copy any remaining elements from right half
	while (j <= rightend) {
		A[k] = B[j];
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

// if left is smaller than right, divide the array by half (with rightstart added), then recursively call it right and left = 0.


// Recursion Test Case: 
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
	struct argument *params; // create a pointer which is a type of struct argument

	params = (struct argument *) arg;  // params is created to tell arg it is type struct arugment in this case.

	int left = params -> left; 
	int right = params -> right;
	int level = params -> level;

	// get values inside of params which is a pointer to buildArgs

	if (params -> level >= cutoff) {
		printf("[L%d] Serial: [%d..%d]\n", level, left, right); // testing
		my_mergesort(left, right); // use my_mergesort on base case
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

