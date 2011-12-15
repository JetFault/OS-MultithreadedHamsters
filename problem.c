#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "hamster_pool.h"
#include "execution.h"
#include "dish.h"

extern volatile int wake;
extern FILE* output_file;
extern pthread_mutex_t file_lock;
extern int run_time;

int main(int argc, char **argv)
{
	if(argc < 4) {
		printf("Not enough arguments, program exits.\n");
		return 1;
	}

	FILE* input_file = fopen(argv[1], "r");
	output_file = fopen(argv[2], "w");
	pthread_mutex_init(&file_lock, NULL);

	if (!input_file) {
		printf("Input file doesn't exist.\n");
		return 1;
	}

	run_time = atoi(argv[3]);

	h_pool hp = createHamPool(input_file);

	dish d = dish_create(hp->numBabies);

	createThreads(hp, d);
		
	/* Execute for this long */
	sleep(atoi(argv[3]));

	/* Tells all threads they should stop executing */
	wake = 1;
	kill_all_hamsters(d);

	/* Want to join the threads so main thread doesn't exit before
	   all threads are finished executing */
	int i = 0;
	for (i = 0; i < hp->numBabies; i++) {
		hamster hammy = (hamster) list_get(hp->babyPool, i);
		pthread_join(hammy->thread, NULL);
	}
	for(i = 0; i < hp->numParents; i++) {
		hamster hammy = (hamster) list_get(hp->parentPool, i);
		pthread_join(hammy->thread, NULL);
	}

	dish_destroy(d);
	destroyHamPool(hp);

	fclose(input_file);
	fclose(output_file);

	return 0;
}

