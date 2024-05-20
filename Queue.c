#include <stdio.h>
#include <stdlib.h>
#include "Queue.h"
#include "Utility.h"

void initializeQueue(Queue *q) {
    q->front = q->rear = NULL;
}

int isEmpty(Queue *q) {
    return q->front == NULL;
}

void enqueue(Queue *q, KeyPointer* data) {
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
    printf("Inserted process with pID: %s", data->value);
}

KeyPointer* dequeue(Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty!\n");
        return NULL;
    }
    Node *temp = q->front;
    KeyPointer* item = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    printf("Removed process with pID: %s", item->value);
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
        KeyPointer* item=temp->data;
        printf("PID: %s     ", item->value);
        temp = temp->next;
    }
    printf("\n");
}

KeyPointer* dequeueSpecific(Queue* q, int processID) {
    if (isEmpty(q)) {
        printf("Queue is empty!\n");
        return NULL;
    }

    Node* temp = q->front;
    Node* prev = NULL;

    while (temp != NULL && parseInt(temp->data->value) != processID) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Process ID %d not found in the queue!\n", processID);
        return NULL;
    }

    if (prev == NULL) {
        q->front = temp->next;
        if (q->front == NULL) {
            q->rear = NULL;
        }
    } else {
        prev->next = temp->next;
        if (prev->next == NULL) {
            q->rear = prev;
        }
    }

    KeyPointer* item = temp->data;
    free(temp);
    printf("process with pID: %s successfully removed", item->value);
    return item;
}

