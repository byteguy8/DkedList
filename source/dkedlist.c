#include "dkedlist.h"
#include "dkedlist_codes.h"
#include <stdlib.h>
#include <assert.h>

void _custom_dealloc_(unsigned long size, void *ptr)
{
    free(ptr);
}

static void *(*dkedlist_allocate)(unsigned long size) = malloc;
static void (*dkedlist_deallocte)(unsigned long size, void *ptr) = _custom_dealloc_;

int _create_list_(void (*destroy_data)(void *data), struct _dkedlist_ **out_list)
{
    assert((out_list || *out_list) && "out_list can't be NULL");

    struct _dkedlist_ *list = (struct _dkedlist_ *)dkedlist_allocate(sizeof(struct _dkedlist_));

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

int _create_node_(void *data, struct _dkedlist_ *list, struct _dkedlist_node_ **out_node)
{
    assert(list && "list can't be NULL");
    assert(out_node || *out_node && "out_node can't be NULL");

    struct _dkedlist_node_ *node = (struct _dkedlist_node_ *)dkedlist_allocate(sizeof(struct _dkedlist_node_));

    if (!node)
    {
        return DKEDLIST_ERR_ALLOC;
    }

    node->prev = NULL;
    node->next = NULL;
    node->list = list;
    node->data = data;

    *out_node = node;

    return DKEDLIST_OK;
}

int _remove_node_(char clean_up, struct _dkedlist_node_ *node)
{
    assert(node && "node can't be NULL");

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

    dkedlist_deallocte(sizeof(struct _dkedlist_node_), node);

    list->size = list->size - 1;

    if (list->size == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return DKEDLIST_OK;
}

int _remove_all_nodes_(char clean_up, struct _dkedlist_ *list)
{
    struct _dkedlist_node_ *current = list->head;

    while (current)
    {
        struct _dkedlist_node_ *next = current->next;

        _remove_node_(clean_up, current);

        current = next;
    }
}

void _destroy_list_(int clean_up, struct _dkedlist_ **list)
{
    if (!list || !(*list))
    {
        return;
    }

    _remove_all_nodes_(clean_up, *list);

    (*list)->destroy_data = NULL;
    (*list)->head = NULL;
    (*list)->tail = NULL;
    (*list)->size = 0;

    dkedlist_deallocte(sizeof(struct _dkedlist_), *list);

    *list = NULL;
}

int _validate_iter_(struct _dkedlist_iter_ iterator)
{
    struct _dkedlist_ *list = iterator.list;

    if (list->size == 0)
    {
        return 0;
    }

    if (iterator.initialized)
    {
        return 1;
    }

    if (iterator.forward && iterator.current_indx >= list->size - 1)
    {
        return 0;
    }

    if (!iterator.forward && iterator.current_indx == 0)
    {
        return 0;
    }

    return 1;
}

void dkedlist_set_malloc(void *(*dkedlist_malloc)(unsigned long size))
{
    if (dkedlist_malloc)
    {
        dkedlist_allocate = dkedlist_malloc;
        return;
    }

    dkedlist_allocate = malloc;
}

void dkedlist_set_free(void (*dkedlist_free)(unsigned long size, void *ptr))
{
    if (dkedlist_free)
    {
        dkedlist_deallocte = dkedlist_free;
        return;
    }

    dkedlist_deallocte = _custom_dealloc_;
}

void dkedlist_iter_create(char forward, struct _dkedlist_iter_ *iterator, struct _dkedlist_ *list)
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
}

int dkedlist_iter_has_next(struct _dkedlist_iter_ iterator)
{
    return _validate_iter_(iterator);
}

struct _dkedlist_node_ *dkedlist_iter_next(struct _dkedlist_iter_ *iterator)
{
    struct _dkedlist_ *list = iterator->list;

    int has_next = _validate_iter_(*iterator);

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
    struct _dkedlist_ *list = NULL;

    if (_create_list_(destroy_data, &list))
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

struct _dkedlist_node_ *dkedlist_get_node(unsigned long index, struct _dkedlist_ *list)
{
    if (index >= list->size)
    {
        return NULL;
    }

    int end_diff = (list->size - 1) - index;
    int start_diff = index;

    int forward = 1;

    if (end_diff < start_diff)
    {
        forward = 0;
    }

    struct _dkedlist_iter_ iter;
    struct _dkedlist_node_ *node = NULL;

    dkedlist_iter_create(forward, &iter, list);

    while (dkedlist_iter_has_next(iter))
    {
        struct _dkedlist_node_ *current = dkedlist_iter_next(&iter);

        if (iter.current_indx == index)
        {
            node = current;
            break;
        }
    }

    return node;
}

void dkedlist_reverse(struct _dkedlist_ *list)
{
    if (list->size < 2)
    {
        return;
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
}

int dkedlist_join(void (*destroy_data)(void *data), struct _dkedlist_ *a_list, struct _dkedlist_ *b_list, struct _dkedlist_ **out_list)
{
    struct _dkedlist_iter_ iter;
    struct _dkedlist_ *list = NULL;

    if (_create_list_(destroy_data, &list))
    {
        return DKEDLIST_ERR_ALLOC;
    }

    dkedlist_iter_create(1, &iter, a_list);

    while (dkedlist_iter_has_next(iter))
    {
        struct _dkedlist_node_ *node = dkedlist_iter_next(&iter);
        void *raw_data = node->data;

        if (dkedlist_insert(raw_data, list, NULL))
        {
            goto CLEAN_UP;
        }
    }

    dkedlist_iter_create(1, &iter, b_list);

    while (dkedlist_iter_has_next(iter))
    {
        struct _dkedlist_node_ *node = dkedlist_iter_next(&iter);
        void *raw_data = node->data;

        if (dkedlist_insert(raw_data, list, NULL))
        {
            goto CLEAN_UP;
        }
    }

    *out_list = list;

    goto FINISH;

CLEAN_UP:
    dkedlist_destroy(&list);
    return DKEDLIST_ERR_ALLOC;

FINISH:
    return DKEDLIST_OK;
}

int dkedlist_sub_list(unsigned long from, unsigned long to, struct _dkedlist_ *list, struct _dkedlist_ **out_list)
{
    if (from > to)
    {
        return DKEDLIST_ILLEGAL_INDEX;
    }

    if (to >= list->size)
    {
        return DKEDLIST_ILLEGAL_INDEX;
    }

    struct _dkedlist_iter_ iter;
    struct _dkedlist_ *new_list = NULL;

    if (_create_list_(list->destroy_data, &new_list))
    {
        return DKEDLIST_ERR_ALLOC;
    }

    dkedlist_iter_create(1, &iter, list);

    while (dkedlist_iter_has_next(iter))
    {
        struct _dkedlist_node_ *node = dkedlist_iter_next(&iter);
        void *raw_data = node->data;

        if (iter.current_indx >= from && iter.current_indx <= to)
        {
            if (dkedlist_insert(raw_data, new_list, NULL))
            {
                goto CLEAN_UP;
            }
        }
    }

    *out_list = new_list;

    goto FINISH;

CLEAN_UP:
    dkedlist_destroy(&new_list);
    return DKEDLIST_ERR_ALLOC;

FINISH:
    return DKEDLIST_OK;
}

int dkedlist_insert(void *data, struct _dkedlist_ *list, struct _dkedlist_node_ **out_node)
{
    struct _dkedlist_node_ *node = NULL;

    if (_create_node_(data, list, &node))
    {
        return DKEDLIST_ERR_ALLOC;
    }

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

int dkedlist_insert_next(void *data, struct _dkedlist_node_ *node, struct _dkedlist_node_ **out_node)
{
    struct _dkedlist_ *list = node->list;
    struct _dkedlist_node_ *new_node = NULL;

    if (_create_node_(data, list, &new_node))
    {
        return DKEDLIST_ERR_ALLOC;
    }

    if (node->next)
    {
        node->next->prev = new_node;
        new_node->next = node->next;
    }

    node->next = new_node;
    new_node->prev = node;

    if (list->tail == node)
    {
        list->tail = new_node;
    }

    list->size++;

    if (out_node)
    {
        *out_node = new_node;
    }

    return DKEDLIST_OK;
}

int dkedlist_insert_prev(void *data, struct _dkedlist_node_ *node, struct _dkedlist_node_ **out_node)
{
    struct _dkedlist_ *list = node->list;
    struct _dkedlist_node_ *new_node = NULL;

    if (_create_node_(data, list, &new_node))
    {
        return DKEDLIST_ERR_ALLOC;
    }

    if (node->prev)
    {
        node->prev->next = new_node;
        new_node->prev = node->prev;
    }

    node->prev = new_node;
    new_node->next = node;

    if (list->head == node)
    {
        list->head = new_node;
    }

    list->size++;

    if (out_node)
    {
        *out_node = new_node;
    }

    return DKEDLIST_OK;
}

void dkedlist_remove(struct _dkedlist_node_ **node, void **data)
{
    if (data)
    {
        *data = (*node)->data;
    }

    _remove_node_(0, *node);

    (*node) = NULL;
}

void dkedlist_remove_clean(struct _dkedlist_node_ **node)
{
    _remove_node_(1, *node);

    (*node) = NULL;
}

void dkedlist_remove_all(struct _dkedlist_ *list)
{
    _remove_all_nodes_(0, list);
}

void dkedlist_remove_all_clean(struct _dkedlist_ *list)
{
    _remove_all_nodes_(0, list);
}

void dkedlist_destroy(struct _dkedlist_ **list)
{
    _destroy_list_(0, list);
}

void dkedlist_destroy_clean(struct _dkedlist_ **list)
{
    _destroy_list_(1, list);
}