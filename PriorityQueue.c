#include <stdio.h>
#include <stdlib.h>
#include "PriorityQueue.h"

void initializeQueue(PriorityQueue *pq) {
    pq->front = NULL;
}

int isEmpty(PriorityQueue *pq) {
    return pq->front == NULL;
}

void enqueue(PriorityQueue *pq, int data, int priority) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->priority = priority;
    newNode->next = NULL;

    if (isEmpty(pq) || pq->front->priority > priority) {
        newNode->next = pq->front;
        pq->front = newNode;
    } else {
        Node* temp = pq->front;
        while (temp->next != NULL && temp->next->priority <= priority) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }
    printf("Inserted %d with priority %d\n", data, priority);
}

int dequeue(PriorityQueue *pq) {
    if (isEmpty(pq)) {
        printf("Priority Queue is empty!\n");
        return -1;
    } else {
        Node* temp = pq->front;
        int item = temp->data;
        pq->front = pq->front->next;
        free(temp);
        printf("Deleted %d\n", item);
        return item;
    }
}

void display(PriorityQueue *pq) {
    Node* temp = pq->front;
    if (isEmpty(pq)) {
        printf("Priority Queue is empty!\n");
    } else {
        printf("Priority Queue elements are: \n");
        while (temp) {
            printf("Data: %d, Priority: %d\n", temp->data, temp->priority);
            temp = temp->next;
        }
    }
}

