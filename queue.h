#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Queue *Queue;

Queue createQueue(size_t elemSize, size_t elemMaxNum);
bool isFull(Queue queue);
bool isEmpty(Queue queue);
void pop(Queue queue, void *elem);
void push(Queue queue, void *elem);
void freeQueue(Queue queue);
void emptyQueue(Queue queue);
size_t getElemNum(Queue queue);
#endif