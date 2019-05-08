#include "queue.h"

#include <stdlib.h>

/**
 *  +------------------------+
 *  |                        |
 *  |      NODE FUNCT        |
 *  |                        |
 *  +------------------------+
 */
node_t *new_queue_node(void * val){
    node_t * new = (node_t *) malloc(sizeof(node_t));

    if( new == NULL ) return new;
    new->next = NULL;
    new->val = val;

    return new;
}

/**
 *  +------------------------+
 *  |                        |
 *  |      LIST FUNCT        |
 *  |                        |
 *  +------------------------+
 */
queue_t *init_queue(){
	queue_t * new = (queue_t *) malloc (sizeof(queue_t));
	if (new == NULL) return NULL;
	new->front = NULL;
	return new;
}

int is_queue_empty(queue_t *q){
	if(q == NULL) return -1;
	return (q->front == NULL);
}

int queue_push(queue_t *q, void * val){

	/* Check null pointer */
	if(q == NULL)
		return -1;

	/* Create new node */
	node_t *new_node = new_queue_node(val);
	if (new_node == NULL) return -1;

	/* Handle when q is empty */
	if(is_queue_empty(q)) {
		q->front = new_node;
		return 0;
	}

	/* Find the front of the queue */
	node_t *iter = q->front;
	while(iter->next != NULL)
		iter=iter->next;

	/* Assign correct pointers */
	iter->next = new_node;
	//new_node->next = NULL;
	return 0;
}

int queue_pop(queue_t *q){

	/* Check null pointers */
	if (q == NULL || q->front == NULL)
		return -1;

	/* Remove the element at front */
	node_t *temp = q->front;
	q->front = q->front->next;

	/* Free allocated memory */
	free(temp->val);
	free(temp);
	return 0;
}

const void * queue_front(queue_t *q){

	/* Check null pointers */
	if(q == NULL || q->front == NULL)
		return NULL;

	/* Get contnet of element at front */
	return q->front->val;
}

int del_queue(queue_t *q){

	/* Check null pointer */
    if(q == NULL) return -1;

	/* Empty the queue */
	empty_queue(q);

	/* Free memory allocated for the queue */
	free(q);

	return 0;
}

int empty_queue(queue_t *q) {

	/* Check null pointer */
	if(q == NULL) return -1;

	/* Pop all elements */
	while(!is_queue_empty(q))
		queue_pop(q);

	return 0;
}
