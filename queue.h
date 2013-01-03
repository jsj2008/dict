#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct node
{
	void *data;
	struct node *next;
} node;

typedef struct
{
	int counter;
	struct node *head;
	struct node *tail;
} Queue;

extern void queue_init(Queue *queue);
extern void queue_destroy(Queue *queue, void (*destroy)(void *));
extern void queue_enqueue(Queue *queue, void *value);
extern void *queue_dequeue(Queue *queue);
extern bool queue_is_empty(Queue *queue);
extern bool queue_is_full(Queue *queue);
extern void *queue_search(Queue *queue, void *key,
	int (*compare)(const void *, const void *));
extern void queue_sort(Queue *queue,
	int (*compare)(const void *, const void *));
extern int queue_element_num(Queue *queue);
#endif	//_QUEUE_H_
