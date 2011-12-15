#ifndef DISH_H_
#define DISH_H_

#include "hamster_pool.h"

struct dish_;
typedef struct dish_* dish;

/* Create a dish */
dish dish_create(int num_beans);

/*Remove a refiller from the dish */
void dish_remove_refiller(dish d);

/* Remove a consumer from the dish */
void dish_remove_consumer(dish d);

/* Refill the dish */
int dish_refill(dish d, int num_beans, hamster parent);

/* Remove a bean from the dish */
int dish_remove(dish d, hamster baby);

/* Wakes up starving children if parent dies */
void kill_all_hamsters(dish d);

/* Destroy a dish */
void dish_destroy(dish d);

#endif
