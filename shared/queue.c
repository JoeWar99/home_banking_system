#include "queue.h"

#include <stdlib.h>

queue *init_queue(){
	queue* new = (queue *) malloc (sizeof(queue));
	if (new == NULL) return NULL;
	new->front = NULL;
	return new;
}

queue_ele *new_ele(bank_account_t * val){
	queue_ele *new = (queue_ele *)malloc(sizeof(queue_ele));
	if (new == NULL) return NULL;

	bank_account_t  *content;
	content = (bank_account_t *)malloc(sizeof(bank_account_t));
	if (content == NULL) return NULL;

	content->account_id = val->account_id;
	content->balance = val->balance;
	strcpy(content->hash, val->hash);
	strcpy(content->salt, val->salt);

	new->next = NULL;
	new->val = content;

	return new;
}

int is_queue_empty(queue *q){
	if(q == NULL) return -1;
	return (q->front == NULL);
}

int queue_size(queue *q) {

	/* Check null pointer */
	if (q == NULL) return -1;

	int size = 0;
	queue_ele *iter = q->front;
	while(iter->next != NULL) {
		iter=iter->next;
		size++;
	}
	return size;
}

int queue_push(queue *q, bank_account_t * val){

	/* Check null pointer */
	if(q == NULL)
		return -1;

	/* Create new node */
	queue_ele *new = new_ele(val);
	if (new == NULL) return -1;

	/* Handle when q is empty */
	if(is_queue_empty(q)) {
		q->front = new;
		return 0;
	}

	/* Find the front of the queue */
	queue_ele *iter = q->front;
	while(iter->next != NULL)
		iter=iter->next;

	/* Assign correct pointers */
	iter->next = new;
	new->next = NULL;
	return 0;
}

int queue_pop(queue *q){

	/* Check null pointers */
	if (q == NULL || q->front == NULL)
		return -1;

	/* Remove the element at front */
	queue_ele *temp = q->front;
	q->front = q->front->next;

	/* Free allocated memory */
	free(temp->val);
	free(temp);
	return 0;
}

const bank_account_t * queue_front(queue *q){

	/* Check null pointers */
	if(q == NULL || q->front == NULL)
		return 0;

	/* Get contnet of element at front */
	return q->front->val;
}

int del_queue(queue *q){

	/* Check null pointer */
    if(q == NULL) return -1;

	/* Empty the queue */
	empty_queue(q);

	/* Free memory allocated for the queue */
	free(q);

	return 0;
}

int empty_queue(queue *q) {

	/* Check null pointer */
	if(q == NULL) return -1;

	/* Pop all elements */
	while(!is_queue_empty(q))
		queue_pop(q);

	return 0;
}
