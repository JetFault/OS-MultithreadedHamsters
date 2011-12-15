#include <stdio.h>
#include <stdlib.h>

#include "hamster_pool.h"

h_pool createHamPool(FILE* file) {
	
	/* Create a hamster pool */
	h_pool hamPool = (h_pool)malloc(sizeof(struct hamster_pool));
	hamPool->parentPool = list_create();
	hamPool->babyPool = list_create();
	hamPool->numParents = 0;
	hamPool->numBabies = 0;

	char typeHamster;
	int sleepTime, doTime, index;

	/* Scan the file and add the hamsters to their respective pools */
	while(fscanf(file, "%d %c %d %d", &index, &typeHamster, &sleepTime, &doTime) != EOF) {

		/* Create a new hamster */
		hamster new_hamster = createHamster(index, typeHamster, sleepTime, doTime);

		/* Insert the hamster to the pool that it belongs in */
		if (IS_BABY(typeHamster)) {
			list_insert(hamPool->babyPool, (void*)new_hamster);
			hamPool->numBabies += 1;
		} else if (IS_PARENT(typeHamster)) {
			list_insert(hamPool->parentPool, (void*)new_hamster);
			hamPool->numParents += 1;
		} else {
			/* Not baby or parent, then do not add it to a pool */
			free(new_hamster);
		}
	}
	
	return hamPool;
}

void destroyHamPool(h_pool hp) {
	int i;
	for (i = 0; i < hp->numBabies; i++) {
		hamster hammy = (hamster) list_get(hp->babyPool, i);
		free(hammy);
	}
	for(i = 0; i < hp->numParents; i++) {
		hamster hammy = (hamster) list_get(hp->parentPool, i);
		free(hammy);
	}
	list_destroy(hp->parentPool);
	list_destroy(hp->babyPool);
	free(hp);
}

hamster createHamster(int index, char type, int sleep_time, int do_time) {

	hamster new_hamster = (hamster)malloc(sizeof(struct hamster_));
	/*printf("%d\n", index); */
	new_hamster->index = index;
	new_hamster->type = type;
	new_hamster->sleepTime = sleep_time;
	new_hamster->doTime = do_time;

	return new_hamster;
}

