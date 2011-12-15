#include <stdlib.h>
#include "linked_list.h"

struct l_list_node_ {
	void* item;
	l_list_node next;
};

struct l_list_ {
	l_list_node head;
	l_list_node tail;
};

l_list list_create() {
	l_list list = (l_list)malloc(sizeof(struct l_list_));
	list->head = NULL;
	list->tail = NULL;
	return list;
}

void list_insert(l_list list, void* value) {
	/* Create the node */
	l_list_node node = (l_list_node)malloc(sizeof(struct l_list_node_));
	node->item = value;
	node->next = NULL;

	if (list->head == NULL) {
		/* Adding to an empty list */
		list->head = node;
	} else if (list->head == list->tail) {
		/* Adding to a list with only one node */
		list->head->next = node;
	} else {
		/* Adding to the end of a list */
		list->tail->next = node;
	}
	list->tail = node;

}

void* list_search(l_list list, void* value, comp_func compare) {

	l_list_node ptr = list->head;
	void* item = NULL;

	while(ptr != NULL && item == NULL) {
		if (compare(ptr, value) == 0) {
			item = ptr->item;
		} else {
			ptr = ptr->next;
		}
	}

	return item;
}

void* list_get(l_list list, int index) {

	l_list_node ptr = list->head;

	int i;
	for(i = 0; i < index && ptr != NULL; i++) {
		ptr = ptr->next;
	}

	return ptr->item;
}

void list_destroy(l_list list) {
	l_list_node ptr = list->head;

	while (ptr != NULL) {
		l_list_node temp = ptr;
		ptr = ptr->next;
		free(temp);
	}

	free(list);
}
