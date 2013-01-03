#include "queue.h"

void queue_init(Queue *queue)
{
	queue->head = NULL;
	queue->tail = NULL;
	queue->counter = 0;
}

void queue_destroy(Queue *queue, void (*destroy)(void *))
{
	void *data;

	while(! queue_is_empty(queue)) {
		data = queue_dequeue(queue);
		if (destroy) {
			destroy(data);
		}
	}
	queue->counter = 0;
}
static struct node *make_node(void *value)
{
	struct node *n;

	n = malloc(sizeof(struct node));
	assert(n);
	n->data = value;
	n->next = NULL;
	return n;
}

void queue_enqueue(Queue *queue, void *value)
{
	struct node *n;

	n = make_node(value);

	if (queue_is_empty(queue)) {
		queue->head = n;
		queue->tail = n;
	} else {
		queue->tail->next = n;
		queue->tail = n;
	}

	queue->counter ++;
}

void *queue_dequeue(Queue *queue)
{
	void *data;
	struct node *n;

	n = queue->head;
	data = n->data;
	queue->head = n->next;
	free(n);
	queue->counter --;
	return data;
}

bool queue_is_empty(Queue *queue)
{
	return queue->head == NULL;
}
bool queue_is_full(Queue *queue)
{
	return false;
}
void *queue_search(Queue *queue, void *key,
	int (*compare)(const void *, const void *))
{
	struct node *n = queue->head;

	while(n) {
		if (! compare(n->data, key)) {
			return n->data;
		}
		n = n->next;
	}
	return NULL;
}
static struct node *find_min_node(Queue *queue,
	int (*compare)(const void *, const void *))
{
	struct node *n, *min;

	n = queue->head;
	min = queue->head;

	while(n) {
		if (compare(min->data, n->data) > 0) {
			min = n;
		}
		n = n->next;
	}
	return min;
}

static void queue_delete_node(Queue *queue, 
	struct node *key)
{
	if (key == queue->head) {
		queue->head = queue->head->next;
		return;
	}

	struct node *n = queue->head;

	while(n && n->next) {
		if (n->next == key) {
			if (key == queue->tail) {
				queue->tail = n;
			}
			n->next = key->next;
		}
		n = n->next;
	}
}

static void queue_insert_node(Queue *queue, 
						struct node *key) {
	if (queue_is_empty(queue)) {
		queue->head = key;
		queue->tail = key;
	} else {
		queue->tail->next = key;
		queue->tail = key;
	}
}

void queue_sort(Queue *queue,
	int (*compare)(const void *, const void *))
{
	struct node *n;
	Queue tmp;
	queue_init(&tmp);

	while(! queue_is_empty(queue)) {
		n = find_min_node(queue, compare);
		queue_delete_node(queue, n);
		queue_insert_node(&tmp, n);
	}
	queue->head = tmp.head;
	queue->tail = tmp.tail;
}

int queue_element_num(Queue *queue)
{
	return queue->counter;
}
