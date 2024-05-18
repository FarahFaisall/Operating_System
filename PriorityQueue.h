#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include "PCB.h"

typedef struct PriorityNode {
    PCB* data;
    int priority;
    struct PriorityNode *next;
} PriorityNode;

typedef struct {
    PriorityNode *front;
} PriorityQueue;

void initializePriorityQueue(PriorityQueue *pq);
int isEmptyPriority(PriorityQueue *pq);
void enqueuePriority(PriorityQueue *pq, PCB*  data, int priority);
PCB* dequeuePriority(PriorityQueue *pq);
void displayPriority(PriorityQueue *pq);

#endif
