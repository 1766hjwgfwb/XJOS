#ifndef XJOS_LIST_H
#define XJOS_LIST_H

#include <xjos/types.h>
#include <libc/assert.h>

// ===================================
//      Core Utility Macros
// ===================================

/**
 * @brief Calculates the byte offset of a member within a struct.
 */
#define element_offset(type, member) ((u32)(&((type *)0)->member))

/**
 * @brief Calculates the starting address of the containing struct
 * from a pointer to one of its members.
 */
#define element_entry(type, member, ptr) ((type *)((u32)(ptr) - element_offset(type, member)))


// ===================================
//      Core Data Structures
// ===================================

typedef struct list_node_t {
    struct list_node_t *prev;
    struct list_node_t *next;
} list_node_t;

typedef struct {
    list_node_t head;
} list_t;


// ===================================
//      List-Specific Macros
// ===================================

/**
 * @brief Gets the pointer to the containing struct from a pointer to a list node member.
 */
#define list_entry(ptr, type, member) \
    element_entry(type, member, ptr)

/**
 * @brief Iterates over a list.
 * @param pos  The list_node_t* to use as a loop cursor.
 * @param list The list_t pointer to iterate over.
 */
#define list_for_each(pos, list) \
    for (list_node_t *pos = (list)->head.next; pos != &(list)->head; pos = pos->next)

/**
 * @brief Iterates over a list of structs.
 * @param pos    A pointer to your struct, used as a loop cursor.
 * @param list   The list_t pointer to iterate over.
 * @param member The name of the list_node_t member in your struct.
 */
#define list_for_each_entry(pos, list, member)                                  \
    for (pos = list_entry((list)->head.next, typeof(*(pos)), member);           \
         &pos->member != &(list)->head;                                         \
         pos = list_entry(pos->member.next, typeof(*(pos)), member))


// func declaration
static inline void list_init(list_t *list);
static inline void list_push(list_t *list, list_node_t *node);
static inline void list_pushback(list_t *list, list_node_t *node);
static inline list_node_t *list_pop(list_t *list);
static inline list_node_t *list_popback(list_t *list);
static inline void list_remove(list_node_t *node);
static inline bool list_empty(list_t *list);
static inline bool list_search(list_t *list, list_node_t *node_to_find);
static inline u32 list_len(list_t *list);


// ===================================
//      API Function Implementations
// ===================================

// Internal helper: inserts new_node between prev and next.
static _inline void __list_add(list_node_t *new_node, list_node_t *prev, list_node_t *next) {
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

// Internal helper: removes the node between prev and next.
static _inline void __list_del(list_node_t *prev, list_node_t *next) {
    next->prev = prev;
    prev->next = next;
}

// Initializes a list.
static _inline void list_init(list_t *list) {
    list->head.next = &list->head;
    list->head.prev = &list->head;
}

// Inserts 'node' after 'anchor'.
static _inline void list_insert_after(list_node_t *anchor, list_node_t *node) {
    __list_add(node, anchor, anchor->next);
}

// Inserts 'node' before 'anchor'.
static _inline void list_insert_before(list_node_t *anchor, list_node_t *node) {
    __list_add(node, anchor->prev, anchor);
}

// Adds a node to the front of the list (head).
static _inline void list_push(list_t *list, list_node_t *node) {
    list_insert_after(&list->head, node);
}

// Adds a node to the back of the list (tail).
static _inline void list_pushback(list_t *list, list_node_t *node) {
    list_insert_before(&list->head, node);
}

// Removes a node from the list.
static _inline void list_remove(list_node_t *node) {
    __list_del(node->prev, node->next);
    // Pointer Poisoning: Invalidate pointers to make use-after-free bugs fail fast.
    node->prev = NULL;
    node->next = NULL;
}

// Removes and returns the node from the front of the list.
static _inline list_node_t *list_pop(list_t *list) {
    assert(!list_empty(list));
    list_node_t *node = list->head.next;
    list_remove(node);
    return node;
}

// Removes and returns the node from the back of the list.
static _inline list_node_t *list_popback(list_t *list) {
    assert(!list_empty(list));
    list_node_t *node = list->head.prev;
    list_remove(node);
    return node;
}

// Checks if the list is empty.
static _inline bool list_empty(list_t *list) {
    return list->head.next == &list->head;
}

// Searches for a node in the list.
static _inline bool list_search(list_t *list, list_node_t *node_to_find) {
    list_node_t *pos;
    list_for_each(pos, list) {
        if (pos == node_to_find) {
            return true;
        }
    }
    return false;
}

// Gets the length of the list (O(n) operation, use with caution).
static _inline u32 list_len(list_t *list) {
    u32 len = 0;
    list_node_t *pos;
    list_for_each(pos, list) {
        len++;
    }
    return len;
}

#endif /* XJOS_LIST_H */