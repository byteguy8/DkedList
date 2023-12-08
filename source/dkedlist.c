#include "dkedlist.h"
#include "dkedlist_codes.h"
#include <stdlib.h>

int _remove_node_(char clean_up, struct _dkedlist_node_ *node)
{
    struct _dkedlist_ *list = node->list;

    if (node == list->head)
    {
        list->head = node->next;
    }
    else if (node == list->tail)
    {
        list->tail = node->prev;
    }
    else
    {
        if (node->prev)
        {
            node->prev->next = node->next;
        }

        if (node->next)
        {
            node->next->prev = node->prev;
        }
    }

    if (clean_up)
    {
        if (list->destroy_data)
        {
            list->destroy_data(node->data);
        }
    }

    node->data = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->list = NULL;

    free(node);

    list->size = list->size - 1;

    if (list->size == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return DKEDLIST_OK;
}

int _validate_iter_(struct _dkedlist_iter_ *iterator)
{
    struct _dkedlist_ *list = iterator->list;

    if (list->size == 0)
    {
        return 0;
    }

    if (iterator->forward && iterator->current_indx >= list->size - 1)
    {
        return 0;
    }

    if (!iterator->forward && iterator->current_indx == 0)
    {
        return 0;
    }

    return 1;
}

int dkedlist_iter_create(char forward, struct _dkedlist_iter_ *iterator, struct _dkedlist_ *list)
{
    if (forward)
    {
        iterator->current_indx = 0;
    }
    else
    {
        iterator->current_indx = list->size - 1;
    }

    iterator->initialized = 1;
    iterator->current_node = NULL;
    iterator->forward = forward;
    iterator->list = list;

    return DKEDLIST_OK;
}

int dkedlist_iter_has_next(struct _dkedlist_iter_ *iterator)
{
    return _validate_iter_(iterator);
}

struct _dkedlist_node_ *dkedlist_iter_next(struct _dkedlist_iter_ *iterator)
{
    struct _dkedlist_ *list = iterator->list;

    int has_next = _validate_iter_(iterator);

    if (!has_next)
    {
        return NULL;
    }

    if (iterator->forward)
    {
        if (iterator->initialized)
        {
            iterator->initialized = 0;
            iterator->current_node = list->head;

            return iterator->current_node;
        }

        iterator->current_indx++;
        iterator->current_node = iterator->current_node->next;
    }
    else
    {
        if (iterator->initialized)
        {
            iterator->initialized = 0;
            iterator->current_node = list->tail;

            return iterator->current_node;
        }

        iterator->current_indx--;
        iterator->current_node = iterator->current_node->prev;
    }

    return iterator->current_node;
}

int dkedlist_create(void (*destroy_data)(void *data), struct _dkedlist_ **out_list)
{
    struct _dkedlist_ *list = (struct _dkedlist_ *)malloc(sizeof(struct _dkedlist_));

    if (!list)
    {
        return DKEDLIST_ERR_ALLOC;
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->destroy_data = destroy_data;

    *out_list = list;

    return DKEDLIST_OK;
}

int dkedlist_reverse(struct _dkedlist_ *list)
{
    if (list->size < 2)
    {
        return DKEDLIST_OK;
    }

    struct _dkedlist_node_ *head = list->head;
    struct _dkedlist_node_ *tail = list->tail;

    struct _dkedlist_node_ *last = tail;
    struct _dkedlist_node_ *current = tail->prev;

    while (current)
    {
        struct _dkedlist_node_ *prev = current->prev;

        last->next = current;
        current->prev = last;

        last = current;
        current = prev;
    }

    tail->prev = NULL;
    head->next = NULL;

    list->head = tail;
    list->tail = head;

    return DKEDLIST_OK;
}

int dkedlist_insert(void *data, struct _dkedlist_ *list, struct _dkedlist_node_ **out_node)
{
    struct _dkedlist_node_ *node = (struct _dkedlist_node_ *)malloc(sizeof(struct _dkedlist_node_));

    if (!node)
    {
        return DKEDLIST_ERR_ALLOC;
    }

    node->prev = NULL;
    node->next = NULL;
    node->list = list;
    node->data = data;

    if (list->size == 0)
    {
        list->head = node;
    }
    else
    {
        node->prev = list->tail;
        list->tail->next = node;
    }

    list->tail = node;
    list->size = list->size + 1;

    if (out_node)
    {
        *out_node = node;
    }

    return DKEDLIST_OK;
}

int dkedlist_remove(struct _dkedlist_node_ *node)
{
    _remove_node_(0, node);

    return DKEDLIST_OK;
}

int dkedlist_remove_clean(struct _dkedlist_node_ *node)
{
    _remove_node_(1, node);

    return DKEDLIST_OK;
}

void dkedlist_destroy(struct _dkedlist_ **list)
{
    if (!list || !(*list))
    {
        return;
    }

    struct _dkedlist_node_ *current = (*list)->head;

    while (current)
    {
        struct _dkedlist_node_ *next = current->next;

        _remove_node_(0, current);

        current = next;
    }

    (*list)->destroy_data = NULL;
    (*list)->head = NULL;
    (*list)->tail = NULL;
    (*list)->size = 0;

    free(*list);

    *list = NULL;
}

void dkedlist_destroy_clean(struct _dkedlist_ **list)
{
    if (!list || !(*list))
    {
        return;
    }

    struct _dkedlist_node_ *current = (*list)->head;

    while (current)
    {
        struct _dkedlist_node_ *next = current->next;

        _remove_node_(1, current);

        current = next;
    }

    (*list)->destroy_data = NULL;
    (*list)->head = NULL;
    (*list)->tail = NULL;
    (*list)->size = 0;

    free(*list);

    *list = NULL;
}