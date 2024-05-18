#include <stdio.h>
#include <stdlib.h>
#include "Queue.h"

void initializeQueue(Queue *q) {
    q->front = q->rear = NULL;
}

int isEmpty(Queue *q) {
    return q->front == NULL;
}

void enqueue(Queue *q, PCB* data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation error\n");
        return;
    }
    newNode->data = data;
    newNode->next = NULL;
    
    if (isEmpty(q)) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    printf("Inserted %d\n", data->processID);
}

PCB* dequeue(Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty!\n");
        return NULL;
    }
    Node *temp = q->front;
    PCB* item = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    printf("Deleted %d\n", item->processID);
    return item;
}

void display(Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty!\n");
        return;
    }
    Node *temp = q->front;
    printf("Queue elements are: \n");
    while (temp) {
        PCB* item=temp->data;
        printf("%d ", item->processID);
        temp = temp->next;
    }
    printf("\n");
}
