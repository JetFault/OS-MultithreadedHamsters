#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#define IS_PARENT(a) a == 'W' || a == 'w'
#define IS_BABY(a) a == 'R' || a == 'r'

#include <pthread.h>
#include <stdio.h>
#include "linked_list.h"

FILE* output_file;
pthread_mutex_t file_lock;
int run_time;

/* The thread pool of hamsters */
struct hamster_pool {
	l_list parentPool; /* List of parent hamsters */
	l_list babyPool; /* List of baby hamsters */
	int numParents, numBabies;
};
typedef struct hamster_pool* h_pool;

/* Hamster struct */
struct hamster_ {
	int index; /* The ID of the hamster */
	char type; /* The type of hamster (parent or baby) */
	int sleepTime; /* The amount of time that a hamster will sleep */
	int doTime; /* The amount of time that a hamster will take to perform an action */
	pthread_t thread; /* Thread corresponding to this hamster */
};
typedef struct hamster_* hamster;


/* Create the Hamster Pool and fill it in with hamsters from an input file */
h_pool createHamPool(FILE* file);

/* Destroy the Hamster Pool */
void destroyHamPool(h_pool hp);

/* Create a hamster struct */
hamster createHamster(int index, char type, int sleep_time, int do_time);

#endif
