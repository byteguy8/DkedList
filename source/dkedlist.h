#ifndef _DKEDLIST_H_
#define _DKEDLIST_H_

/**
 * @brief Structure representing
 * every single node inside the list.
 *
 */
struct _dkedlist_node_
{
    struct _dkedlist_node_ *prev; // The previous node (if any).
    struct _dkedlist_node_ *next; // The next node (if any)
    struct _dkedlist_ *list;      // The list of which this node belongs to
    void *data;                   // The data inserted by the user. Could be NULL.
};

/**
 * @brief Structure representing the list.
 * This is a doubly linked list, meaning every
 * node has a pointer to the previous node (if any)
 * and another pointer to the next node (if any).
 *
 */
struct _dkedlist_
{
    struct _dkedlist_node_ *head;     // The first node in the list.
    struct _dkedlist_node_ *tail;     // The last node in the list.
    void (*destroy_data)(void *data); // Function used to help users deallocated allocated resources inserted in the list.
    unsigned long size;               // Numbers of nodes inside the list.
};

/**
 * @brief Iterator used to iterate over the lsit
 *
 */
struct _dkedlist_iter_
{
    char forward;                         // Specify if iterate forward or backward.
    char initialized;                     // Used to determinate if the iter have just been created.
    struct _dkedlist_ *list;              // The list over the iterator will iterate.
    unsigned long current_indx;           // The current index of the iteration.
    struct _dkedlist_node_ *current_node; // The current node of the iteration.
};

typedef struct _dkedlist_node_ DKedListNode;
typedef struct _dkedlist_ DkedList;
typedef struct _dkedlist_iter_ DkedListIter;

/**
 * @brief Initialize a _dkedlist_iter_ structure with the information
 * related to iterate the specified list.
 *
 * @param forward Specifiy if the iteration should be forward or backward.
 * @param iterator Pointer to the iter to initialize. Must not be NULL.
 * @param list Pointer to the list used by the iterator to iterate. Must not be NULL.
 */
void dkedlist_iter_create(char forward, struct _dkedlist_iter_ *iterator, struct _dkedlist_ *list);

/**
 * @brief Determinates if there is a next element remainig to iterate over.
 *
 * @param iterator _dkedlist_iter_ structure previously initialized with dkedlist_iter_create.
 * @return 0 if there are no more nodes to iterate ver, 1 otherwise.
 */
int dkedlist_iter_has_next(struct _dkedlist_iter_ iterator);

/**
 * @brief Gets the next node in the iteration.
 *
 * @param iterator Pointer to a _dkedlist_iter_ struture previously initialized
 * with dkedlist_iter_create. Must not be NULL.
 * @return NULL if there are no more nodes to iterate over. struct _dkedlist_node_* otherwise.
 */
struct _dkedlist_node_ *dkedlist_iter_next(struct _dkedlist_iter_ *iterator);

/**
 * @brief Creates a new list.
 *
 * @param destroy_data Pointer to a function used to help deallocate
 * memory allocated data inserted in the list (if any). Can be NULL.
 * @param out_list Pointer to a pointer where the created list will
 * be passed. Must not be NULL.
 * @return DKEDLIST_ERR_ALLOC if allocation error happens. DKEDLIST_OK otherwise.
 */
int dkedlist_create(void (*destroy_data)(void *data), struct _dkedlist_ **out_list);

/**
 * @brief Gets a specific node based in the submitted index
 *
 * @param index The index of the node.
 * @param list Pointer to the list structure. Must not be NULL.
 * @return NULL if index is out of bounds, meaning the index is greater or
 * equals to the list size. struct _dkedlist_node_* otherwise.
 */
struct _dkedlist_node_ *dkedlist_get_node(unsigned long index, struct _dkedlist_ *list);

/**
 * @brief Reverses the list.
 *
 * @param list Pointer to the list structure. Must not be NULL.
 */
void dkedlist_reverse(struct _dkedlist_ *list);

/**
 * @brief Joins two list into one. This functions does not clone the
 * data inserted inside the nodes, meaning the resulting list contains
 * the same pointers to the data inserted in the two input lists.
 *
 * If a list and b list are empty, this returns a empty list. If only
 * a list or b list contains elements, only those elements will be in
 * the resulting list.
 *
 * @param destroy_data Pointer to a function used to help deallocate
 * memory allocated data inserted in the list (if any). Can be NULL.
 * @param a_list Pointer to the first input list. Must not be NULL.
 * @param b_list Pointer to the second input list. Must not be NULL.
 * @param out_list Pointer to a pointer where the created list will
 * be passed. Must not be NULL.
 * @return DKEDLIST_ERR_ALLOC if allocation error happens. DKEDLIST_OK otherwise.
 */
int dkedlist_join(void (*destroy_data)(void *data), struct _dkedlist_ *a_list, struct _dkedlist_ *b_list, struct _dkedlist_ **out_list);

/**
 * @brief Creates new list from the given range (inclusive)
 *
 * @param from Start index (inclusive)
 * @param to End index (inclusive)
 * @param list Pointer to input list from which create the sub list. Must not be NULL.
 * @param out_list Pointer to a pointer where the created list will
 * be passed. Must not be NULL.
 * @return DKEDLIST_ILLEGAL_INDEX if 'from' greater to 'to' or 'to' is greater
 * or equals to list size. DKEDLIST_ERR_ALLOC if allocation error happens. DKEDLIST_OK otherwise.
 */
int dkedlist_sub_list(unsigned long from, unsigned long to, struct _dkedlist_ *list, struct _dkedlist_ **out_list);

/**
 * @brief Insert a data into the list.
 *
 * @param data Pointer to data to be inserted. Can be NULL.
 * @param list Pointer to list in which the data will be inserted. Must not be NULL.
 * @param out_node Pointer to a pointer in which the created node of the inserted
 * data will be saved. Can be NULL.
 * @return DKEDLIST_ERR_ALLOC if allocation error happens. DKEDLIST_OK otherwise.
 */
int dkedlist_insert(void *data, struct _dkedlist_ *list, struct _dkedlist_node_ **out_node);

/**
 * @brief Insert a new node with the specified data next to a specific node.
 *
 * @param data Pointer to data to be inserted. Can be NULL.
 * @param node Pointer to the node in wich the new one will be next inserted. Must not be NULL.
 * @param out_node Pointer to a pointer in which the created node of the inserted
 * data will be saved. Can be NULL.
 * @return DKEDLIST_ERR_ALLOC if allocation error happens. DKEDLIST_OK otherwise.
 */
int dkedlist_insert_next(void *data, struct _dkedlist_node_ *node, struct _dkedlist_node_ **out_node);

/**
 * @brief Insert a new node with the specified data previous to a specific node.
 *
 * @param data Pointer to data to be inserted. Can be NULL.
 * @param node Pointer to the node in wich the new one will be previous inserted. Must not be NULL.
 * @param out_node Pointer to a pointer in which the created node of the inserted
 * data will be saved. Can be NULL.
 * @return DKEDLIST_ERR_ALLOC if allocation error happens. DKEDLIST_OK otherwise.
 */
int dkedlist_insert_prev(void *data, struct _dkedlist_node_ *node, struct _dkedlist_node_ **out_node);

/**
 * @brief Removes a node from the list.
 *
 * @param node Pointer to pointer of the node to be removed. Must not be NULL.
 * @param data Pointer to pointer in which the data in the node will
 * be saved. Can be NULL.
 */
void dkedlist_remove(struct _dkedlist_node_ **node, void **data);

/**
 * @brief Removes a node from the list. This function calls the internal
 * destroy_data function, which is used to help users deallocated allocated
 * resources automatically when a node is remove from the list.
 *
 * @param node Pointer to pointer of the node to be removed. Must not be NULL.
 */
void dkedlist_remove_clean(struct _dkedlist_node_ **node);

/**
 * @brief Removes all nodes from the list.
 *
 * @param list Pointer to the list. Must not be NULL.
 */
void dkedlist_remove_all(struct _dkedlist_ *list);

/**
 * @brief Removes all nodes from the list. This function calls the internal
 * destroy_data function, which is used to help users deallocated allocated
 * resources automatically when a node is remove from the list.
 *
 * @param list
 */
void dkedlist_remove_all_clean(struct _dkedlist_ *list);

/**
 * @brief Destroys the list, deallocating every resource used for it.
 *
 * @param list Pointer to the list. Must not be NULL.
 */
void dkedlist_destroy(struct _dkedlist_ **list);

/**
 * @brief Destroys the list, deallocating every resource used for it.
 * This function calls the internal destroy_data function, which is used
 * to help users deallocated allocated
 * resources automatically when a node is remove from the list.
 * @param list Pointer to the list. Must not be NULL.
 */
void dkedlist_destroy_clean(struct _dkedlist_ **list);

/**
 * @brief Gets the first node from the list (if any).
 * Returns NULL in case of list size equals to 0.
 *
 */
#define dkedlist_first_node(list) (list->head)

/**
 * @brief Gets the last node from the list (if any).
 * Returns NULL in case of list size equals to 0.
 *
 */
#define dkedlist_last_node(list) (list->tail)

#endif