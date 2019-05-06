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
typedef struct _queue_ele {
	struct _queue_ele *next;
	struct bank_account *val;
} queue_ele;

/**
 * @brief Queue object
 */
typedef struct	{
	queue_ele *front;
} queue;

/**
 * @brief initializes the queue data structure
 *
 * @return Object of type queue
 */
queue* init_queue();

/**
 * @brief initializes a queue node
 *
 * @param val Value of the new node
 * @return New element of type queue_ele
 */
queue_ele *new_ele(struct bank_account * val);

/**
 * @brief checks if the queue is empty
 *
 * @param q queue to check if empty
 * @return Returns true if queue is empty, false otherwise
 */
int is_queue_empty(queue *q);

/**
 * @brief determines queue size
 *
 * @param q queue to check size
 * @return Returns queue size upon success and -1 otherwise
 */
int queue_size(queue *q);

/**
 * @brief puts a new element in the queue
 *
 * @param q queue to push element to
 * @param val value of the new element
 * @return Return 0 upon success and -1 otherwise
 */
int queue_push(queue *q, struct bank_account * val);

/**
 * @brief removes the front of the queue
 *
 * @param q queue to pop element from
 * @return Return 0 upon success and -1 otherwise
 */
int queue_pop(queue *q);

/**
 * @brief returns the front of the queue
 *
 * @param q queue to get element from
 * @return Char on top of queue
 */
const struct bank_account * queue_front(queue *q);

/**
 * @brief deletes the queue data structure
 *
 * @param q queue to delete
 * @return Return 0 upon success and -1 otherwise
 */
int del_queue(queue *q);

/**
 * @brief empties a queue
 *
 * @param q queue to empty
 * @return Return 0 upon success and -1 otherwise
 */
int empty_queue(queue *q);

/** @} */

#endif
