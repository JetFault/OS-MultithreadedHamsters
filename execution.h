#ifndef EXECUTION_H_
#define EXECUTION_H_

#include "hamster_pool.h"
#include "dish.h"

volatile int wake;

/* A structure to hold information for threads */
struct thread_arg_;
typedef struct thread_arg_* thread_arg;

/* Create and Start all the threads for each hamster */
void createThreads(h_pool HP, dish d);

/* What the baby thread should do */
void* runBaby(void* arg);

/* What the parent thread should do */
void* runParent(void* arg);

void printBabyStats();

void printParentStats();

#endif
