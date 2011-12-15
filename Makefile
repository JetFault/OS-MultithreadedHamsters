CC = gcc
CCFLAGS = -Wall -g

all: problem1 problem2

problem1: problem.o dish.o hamster_pool.o linked_list.o execution.o
	$(CC) $(CCFLAGS) -o problem1 problem.o dish.o hamster_pool.o linked_list.o execution.o -lpthread

problem2: problem.o dish.o hamster_pool.o linked_list.o execution.o
	$(CC) $(CCFLAGS) -o problem2 problem.o dish.o hamster_pool.o linked_list.o execution.o -lpthread -DPROBLEM2

problem.o: hamster_pool.h execution.h dish.h problem.c
	$(CC) $(CCFLAGS) -c problem.c

dish.o: dish.h dish.c hamster_pool.h
	$(CC) $(CCFLAGS) -c dish.c

execution.o: execution.h hamster_pool.h dish.h execution.c
	$(CC) $(CCFLAGS) -c execution.c
 
hamster_pool.o: hamster_pool.h hamster_pool.c linked_list.h
	$(CC) $(CCFLAGS) -c hamster_pool.c

linked_list.o: linked_list.h linked_list.c
	$(CC) $(CCFLAGS) -c linked_list.c

clean:
	rm -f *.o
	rm -f problem1 problem2
