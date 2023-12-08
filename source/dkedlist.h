#ifndef _DKEDLIST_H_
#define _DKEDLIST_H_

struct _dkedlist_node_
{
    struct _dkedlist_node_ *prev;
    struct _dkedlist_node_ *next;
    struct _dkedlist_ *list;
    void *data;
};

struct _dkedlist_
{
    struct _dkedlist_node_ *head;
    struct _dkedlist_node_ *tail;
    void (*destroy_data)(void *data);
    unsigned long size;
};

struct _dkedlist_iter_
{
    char forward;
    char initialized;
    struct _dkedlist_ *list;
    unsigned long current_indx;
    struct _dkedlist_node_ *current_node;
};

typedef struct _dkedlist_node_ DKedListNode;
typedef struct _dkedlist_ DkedList;
typedef struct _dkedlist_iter_ DkedListIter;

int dkedlist_iter_create(char forward, struct _dkedlist_iter_ *iterator, struct _dkedlist_ *list);

int dkedlist_iter_has_next(struct _dkedlist_iter_ *iterator);

struct _dkedlist_node_ *dkedlist_iter_next(struct _dkedlist_iter_ *iterator);

int dkedlist_create(void (*destroy_data)(void *data), struct _dkedlist_ **out_list);

struct _dkedlist_node_ *dkedlist_get_node(unsigned long index, struct _dkedlist_ *list);

int dkedlist_reverse(struct _dkedlist_ *list);

int dkedlist_join(void (*destroy_data)(void *data), struct _dkedlist_ *a_list, struct _dkedlist_ *b_list, struct _dkedlist_ **out_list);

int dkedlist_sub_list(unsigned long from, unsigned long to, struct _dkedlist_ *list, struct _dkedlist_ **out_list);

int dkedlist_insert(void *data, struct _dkedlist_ *list, struct _dkedlist_node_ **out_node);

int dkedlist_insert_next(void *data, struct _dkedlist_node_ *node, struct _dkedlist_node_ **out_node);

int dkedlist_insert_prev(void *data, struct _dkedlist_node_ *node, struct _dkedlist_node_ **out_node);

void dkedlist_remove(struct _dkedlist_node_ **node, void **data);

void dkedlist_remove_clean(struct _dkedlist_node_ **node);

void dkedlist_remove_all(struct _dkedlist_ *list);

void dkedlist_remove_all_clean(struct _dkedlist_ *list);

void dkedlist_destroy(struct _dkedlist_ **list);

void dkedlist_destroy_clean(struct _dkedlist_ **list);

#define dkedlist_first_node(list) (list->head)
#define dkedlist_last_node(list) (list->tail)

#endif