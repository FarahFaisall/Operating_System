#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "KeyPointer.h"

typedef struct PriorityNode {
    KeyPointer* data;
    int priority;
    struct PriorityNode *next;
} PriorityNode;

typedef struct {
    PriorityNode *front;
} PriorityQueue;

void initializePriorityQueue(PriorityQueue *pq);
int isEmptyPriority(PriorityQueue *pq);
void enqueuePriority(PriorityQueue *pq, KeyPointer*  data, int priority);
KeyPointer* dequeuePriority(PriorityQueue *pq);
void displayPriority(PriorityQueue *pq);
KeyPointer* dequeueSpecific(PriorityQueue *pq, int processID);

#endif
