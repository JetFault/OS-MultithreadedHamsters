#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "dish.h"
#include <sys/time.h>

extern FILE* output_file;
extern pthread_mutex_t file_lock;
extern volatile int wake;

struct dish_ {
	int num_beans; /* The number of beans currently in the dish */
	int num_consumers; /* The number of hamsters eating */
	int num_refillers; /* The number of hamsters waiting to refill */

	int endedProgram;
	
	/* Locks */
	pthread_mutex_t dish_lock; /* Lock the dish from being accessed from everyone! */
	pthread_mutex_t remove_lock; /* Lock the */
	pthread_mutex_t refiller_lock; /* Make changing number of refillers thread safe. */
	pthread_mutex_t consumer_lock; /* Make changing number of consumers thread safe. */

	/* Condition variables */
	pthread_cond_t refill_cond; /* Can we refill? Are babies eating? */
	pthread_cond_t remove_cond; /* Can we remove? Are parents refilling? */
};

dish dish_create(int num_beans) {
	dish new_dish = (dish)malloc(sizeof(struct dish_));
	new_dish->num_beans = num_beans;
	new_dish->num_consumers = 0;
	new_dish->num_refillers = 0;
	pthread_mutex_init(&(new_dish->dish_lock), NULL);
	pthread_mutex_init(&(new_dish->consumer_lock), NULL);
	pthread_mutex_init(&(new_dish->refiller_lock), NULL);
	pthread_mutex_init(&(new_dish->remove_lock), NULL);
	pthread_cond_init(&(new_dish->refill_cond), NULL);
	pthread_cond_init(&(new_dish->remove_cond), NULL);
	new_dish->endedProgram = 0;
	return new_dish;
}

/* Complete. I think? */
void dish_remove_refiller(dish d) {
	pthread_mutex_lock(&(d->refiller_lock));
	d->num_refillers -= 1;
	pthread_mutex_unlock(&(d->refiller_lock));
	pthread_cond_broadcast(&(d->refill_cond));
}

// Needs to be done
void dish_remove_consumer(dish d) {
	pthread_mutex_lock(&(d->consumer_lock));
	(d->num_consumers)--;
	pthread_mutex_unlock(&(d->consumer_lock));
	
	/* Tell parents to check if they can refill */
	pthread_cond_broadcast(&(d->refill_cond));
}


/* 	Parents refill the bowl. Make sure no one is eating. But also make sure hamsters
	don't start eating again if we are waiting to refill.

	Locks: 		dish_lock, refiller_lock
	Conditions: refill_cond
	Broadcasts: remove_cond */
int dish_refill(dish d, int num_beans, hamster parent) {
	int ms_wait = 0;
	
	/* No one can access dish if we are gonna refill/check how many babies are eating! 
	 Also prevents two parents from refilling at same time. */
	pthread_mutex_lock(&(d->dish_lock));

#ifdef PROBLEM2
	if (d->num_beans > num_beans/2 && parent->sleepTime < 256) {
		parent->sleepTime << 1;
		if (parent->sleepTime > 256) parent->sleepTime = 256;
	} else if (d->num_beans < num_beans/2 && parent->sleepTime > 1) {
		parent->sleepTime >> 1;
		if (parent->sleepTime < 1) parent->sleepTime = 1;
	}
#endif

	pthread_mutex_lock(&file_lock);
	fprintf(output_file, "parent(id=%d), aquire lock_%d\n", parent->index, parent->index);
	pthread_mutex_unlock(&file_lock);
	
	/* Wait for no one to be eating */
	while (d->num_consumers > 0) {
	
		/* We want to refill, Don't let other babies eat again until we do */
		pthread_mutex_lock(&(d->refiller_lock));
		d->num_refillers += 1;
		pthread_mutex_unlock(&(d->refiller_lock));

		/* Unlock dish so other parents can fall into cond wait or babies can eat */
		pthread_mutex_unlock(&(d->dish_lock));

		struct timeval start, end;
		gettimeofday(&start, NULL);
		/* Let's wait till babies wake parent hamsters up when they're done eating.
		 Prevent anyone else from changing bean count. */
		pthread_cond_wait(&(d->refill_cond), &(d->dish_lock));
		gettimeofday(&end, NULL);
		int ms_start = (start.tv_sec*1000) + (start.tv_usec/1000);
		int ms_end = (end.tv_sec*1000) + (end.tv_usec/1000);
		ms_wait += (ms_end - ms_start);
		
		/* When program exits, we signal all babies to wake up if they are
		   waiting to eat. Check if we should exit */
		if(d->endedProgram == 1) {
			pthread_mutex_unlock(&(d->dish_lock));
			return ms_wait;
		}
	}
	pthread_mutex_lock(&file_lock);
	fprintf(output_file, "parent(id=%d), refilling, process_time=%d, item=%d\n", parent->index, parent->doTime, d->num_beans);
	pthread_mutex_unlock(&file_lock);

	d->num_beans += num_beans;
	/*printf("P: Refilling, there are %d beans\n", d->num_beans);*/
	pthread_mutex_unlock(&(d->dish_lock));

	pthread_mutex_lock(&file_lock);
	fprintf(output_file, "parent(id=%d), releasing lock_%d\n", parent->index, parent->index);
	pthread_mutex_unlock(&file_lock);

	/* Wake up all babies waiting for beans, we refilled! */
	pthread_cond_broadcast(&(d->remove_cond));
	return ms_wait;
}

/* 	Locks: 		dish_lock, remove_lock
	Conditions: remove_cond */
int dish_remove(dish d, hamster hammy) {
	int ms_wait = 0;
	
	/* No one can access dish if we are gonna decrement/check how many beans exist */
	pthread_mutex_lock(&(d->dish_lock));
	pthread_mutex_lock(&file_lock);
	fprintf(output_file, "child(id=%d), aquire lock_%d\n", hammy->index, hammy->index);
	pthread_mutex_unlock(&file_lock);

	//Should we lock refill? This seems shady?
	pthread_mutex_lock(&(d->refiller_lock));

	/* Wait for dish to have beans or no parents waiting to refill */
	while(d->num_beans <= 0 || d->num_refillers > 0) {
	
		/* Make number of refillers thread safe */
		pthread_mutex_unlock(&(d->refiller_lock));
		
		/* Unlock dish so other babies can fall into wait or parents can refill */
		pthread_mutex_unlock(&(d->dish_lock));

		struct timeval start, end;
		gettimeofday(&start, NULL);
		/* Wait for beans to exist in dish! 
		 Make sure no one else can change bean count.*/
		pthread_cond_wait(&(d->remove_cond), &(d->dish_lock));
		gettimeofday(&end, NULL);
		int ms_start = (start.tv_sec*1000) + (start.tv_usec/1000);
		int ms_end = (end.tv_sec*1000) + (end.tv_usec/1000);
		ms_wait += (ms_end - ms_start);

		/* Make number of refillers thread safe */
		pthread_mutex_lock(&(d->refiller_lock));

		/* When program exits, we signal all babies to wake up if they are
		   waiting to eat. Check if we should exit */
		if(d->endedProgram == 1) {
			pthread_mutex_unlock(&(d->refiller_lock));
			pthread_mutex_unlock(&(d->dish_lock));
			return ms_wait;
		}
	}

	/* Make number of refillers thread safe */
	pthread_mutex_unlock(&(d->refiller_lock));
	
	/* Lets make changing number of consumers thread safe */
	pthread_mutex_lock(&(d->consumer_lock));
	d->num_consumers += 1;
	pthread_mutex_unlock(&(d->consumer_lock));

	pthread_mutex_lock(&file_lock);
	fprintf(output_file, "child(id=%d), eating, process_time=%d\n", hammy->index, hammy->doTime);
	pthread_mutex_unlock(&file_lock);

	d->num_beans -= 1;
	/*printf("B: Removed a bean. Total beans: %d\n", d->num_beans);
	printf("B: Num consumers: %d\n", d->num_consumers);*/

	pthread_mutex_lock(&file_lock);
	fprintf(output_file, "child(id=%d), releasing lock_%d\n", hammy->index, hammy->index);
	pthread_mutex_unlock(&file_lock);

	pthread_mutex_unlock(&(d->dish_lock));
	return ms_wait;
}

void kill_all_hamsters(dish d) {
	d->endedProgram = 1;
	pthread_cond_broadcast(&(d->remove_cond));
	pthread_cond_broadcast(&(d->refill_cond));
}

void dish_destroy(dish d) {
	free(d);
}
