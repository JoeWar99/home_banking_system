#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <string.h>
#include "types.h"

/**
 * @defgroup queue queue module
 * Contains the code for the queue data structure
 * @{
 */

/**
 * @brief Element of the queue, with a pointer to the next element and a pointer to the stored value
 */
typedef struct node {
	struct node *next;
	void *val;
} node_t;

/**
 * @brief Queue object
 */
typedef struct	{
	node_t *front;
} queue_t;

/**
 * @brief initializes a queue node
 *
 * @param val Value of the new node
 * @return New element of type queue_ele
 */
node_t *new_queue_node(void * val);

/**
 * @brief initializes the queue data structure
 *
 * @return Object of type queue
 */
queue_t * init_queue();

/**
 * @brief checks if the queue is empty
 *
 * @param q queue to check if empty
 * @return Returns true if queue is empty, false otherwise
 */
int is_queue_empty(queue_t *q);

/**
 * @brief puts a new element in the queue
 *
 * @param q queue to push element to
 * @param val value of the new element
 * @return Return 0 upon success and -1 otherwise
 */
int queue_push(queue_t *q, void * val);

/**
 * @brief removes the front of the queue
 *
 * @param q queue to pop element from
 * @return Return 0 upon success and -1 otherwise
 */
int queue_pop(queue_t *q);

/**
 * @brief returns the front of the queue
 *
 * @param q queue to get element from
 * @return void* Pointer to the value on the head of the list (NULL if it is empty) 
 */
const void * queue_front(queue_t *q);

/**
 * @brief deletes the queue data structure
 *
 * @param q queue to delete
 * @return Return 0 upon success and -1 otherwise
 */
int del_queue(queue_t *q);

/**
 * @brief empties a queue
 *
 * @param q queue to empty
 * @return Return 0 upon success and -1 otherwise
 */
int empty_queue(queue_t *q);

/** @} */

#endif
