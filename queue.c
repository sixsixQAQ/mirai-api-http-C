#include "queue.h"
#include "std.h"
#include "string.h"
#include <stdlib.h>
struct Queue {
    void *elemArray;
    size_t head, tail;
    size_t maxNum;
    size_t elemSize;
};
Queue createQueue(size_t elemSize, size_t elemMaxNum)
{
    Queue queue = (Queue)malloc2(sizeof(struct Queue));
    queue->elemArray = malloc2(elemSize * elemMaxNum);
    queue->elemSize = elemSize;
    queue->maxNum = elemMaxNum;
    queue->head = queue->tail = 0;
    return queue;
}
bool isFull(Queue queue)
{
    if ((queue->tail + 1) % queue->maxNum == queue->head)
        return true;
    return false;
}
bool isEmpty(Queue queue)
{
    if (queue->head == queue->tail)
        return true;
    return false;
}
void pop(Queue queue, void *elem)
{
    memcpy(elem,
           (char *)queue->elemArray + queue->elemSize * queue->head,
           queue->elemSize);
    queue->head = (queue->head + 1) % queue->maxNum;
}
void push(Queue queue, void *elem)
{
    memcpy((char *)queue->elemArray + queue->elemSize * queue->tail,
           elem,
           queue->elemSize);
    queue->tail = (queue->tail + 1) % queue->maxNum;
}
void freeQueue(Queue queue)
{
    free2(queue->elemArray);
    free2(queue);
}
void emptyQueue(Queue queue)
{
    queue->head = queue->tail;
}
size_t getElemNum(Queue queue)
{
    return abs(queue->tail - queue->head);
}