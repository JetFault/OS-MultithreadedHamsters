#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

struct l_list_node_;
typedef struct l_list_node_* l_list_node;

struct l_list_;
typedef struct l_list_* l_list;

/* Function pointer to use for comparisons in list_search */
typedef int (*comp_func)(void*, void*);

/* Create a linked list */
l_list list_create();

/* Insert into a linked list */
void list_insert(l_list list, void* value);

/* Search the linked list for a specified item/value */
/* Return NULL if not found, return a pointer to the item if found */
void* list_search(l_list list, void* value, comp_func compare);

/* Get the item at a specified index of the list*/
/* Return NULL if index is invalid or out of bounds */
void* list_get(l_list list, int index);

/* Destroy a linked list */
void list_destroy(l_list list);

#endif
