#ifndef QUEUE_H
#define QUEUE_H

#include "PCB.h"

typedef struct Node {
    PCB* data;
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
} Queue;

void initializeQueue(Queue *q);
int isEmpty(Queue *q);
void enqueue(Queue *q, PCB* data);
PCB* dequeue(Queue *q);
void display(Queue *q);

#endif // QUEUE_H
