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

int dkedlist_reverse(struct _dkedlist_ *list);

int dkedlist_insert(void *data, struct _dkedlist_ *list, struct _dkedlist_node_ **out_node);

int dkedlist_remove(struct _dkedlist_node_ *node);

int dkedlist_remove_clean(struct _dkedlist_node_ *node);

void dkedlist_destroy(struct _dkedlist_ **list);

void dkedlist_destroy_clean(struct _dkedlist_ **list);

#endif