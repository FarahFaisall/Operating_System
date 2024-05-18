#ifndef QUEUE_H
#define QUEUE_H

#include "KeyPointer.h"

typedef struct Node {
    KeyPointer* data;
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
} Queue;

void initializeQueue(Queue *q);
int isEmpty(Queue *q);
void enqueue(Queue *q, KeyPointer* data);
KeyPointer* dequeue(Queue *q);
void display(Queue *q);
KeyPointer* dequeueSpecificQueue(Queue* q, int processID);

#endif // QUEUE_H
