#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#include "execution.h"

extern int run_time;
extern pthread_mutex_t file_lock;
extern FILE* output_file;

struct thread_arg_ {
	hamster hammy;
	dish d;
	int numBabies;
};

void* runBaby(void* arg) {
	thread_arg targ = (thread_arg)arg;
	hamster hammy = targ->hammy;
	dish d = targ->d;

	int numEats = 0;
	int timeWaited = 0;

	/* If wake is 1, we need to exit */
	while(wake != 1) {
		/*Think for a bit*/
		pthread_mutex_lock(&file_lock);
		fprintf(output_file, "child(id=%d), thinking, think_time=%d\n", hammy->index, hammy->sleepTime); 
		pthread_mutex_unlock(&file_lock);
		usleep(hammy->sleepTime * 1000);

		/*Do for a bit*/
		timeWaited += dish_remove(d, hammy);
		usleep(hammy->doTime * 1000);
		numEats++;
		dish_remove_consumer(d);
	}
	
	kill_all_hamsters(d);

	printf("TP=%f, WT=%dms (baby%d thread)\n", (float)numEats/run_time , timeWaited, hammy->index);
	
	free(targ);
	pthread_exit(0);
}

void* runParent(void* arg) {
	thread_arg targ = (thread_arg)arg;
	hamster hammy = targ->hammy;
	dish d = targ->d;

	int numRefills = 0;
	int timeWaited = 0;

	/* If wake is 1, we need to exit */
	while(wake != 1) {
		/*Sleep for a bit*/
		pthread_mutex_lock(&file_lock);
		fprintf(output_file, "parent(id=%d), thinking, think_time=%d\n", hammy->index, hammy->sleepTime); 
		pthread_mutex_unlock(&file_lock);
		usleep(hammy->sleepTime * 1000);

		/*Refill*/
		timeWaited += dish_refill(d, targ->numBabies, hammy);
		usleep(hammy->doTime * 1000);
		numRefills++;
		dish_remove_refiller(d);
	}
	
	kill_all_hamsters(d);

	printf("REF=%d (parent%d thread)\n", numRefills*(targ->numBabies), hammy->index);

	free(targ);
	pthread_exit(0);
}

void createThreads(h_pool HP, dish d) {
	int i;
	int numBabies = HP->numBabies;
	int numPar = HP->numParents;

	wake = 0;

	/* Create and run threads for babies */
	for(i = 0; i < numBabies; i++) {
		hamster babyHam = (hamster)list_get(HP->babyPool, i);
		thread_arg targ = (thread_arg)malloc(sizeof(struct thread_arg_));
		targ->hammy = babyHam;
		targ->d = d;
		targ->numBabies = numBabies;
		pthread_create(&(babyHam->thread), NULL, runBaby, (void *)targ);
	}
	
	/* Create and run threads for parents */
	for(i = 0; i < numPar; i++) {
		hamster parentHam = (hamster)list_get(HP->parentPool, i);
		thread_arg targ = (thread_arg)malloc(sizeof(struct thread_arg_));
		targ->hammy = parentHam;
		targ->d = d;
		targ->numBabies = numBabies;
		pthread_create(&(parentHam->thread), NULL, runParent, (void *)targ);
	}
}


void printBabyStats(int eats, int msWaits) {
	printf("Baby stopped\n");

}

void printParentStats(int refills, int msWaits) {
	printf("Parent stopped\n");
}
