#include <stdio.h>
#include <stdlib.h>
#include "PriorityQueue.h"


void initializePriorityQueue(PriorityQueue *pq) {
    pq->front = NULL;
}

int isEmptyPriority(PriorityQueue *pq) {
    return pq->front == NULL;
}

void enqueuePriority(PriorityQueue *pq, PCB* data, int priority) {
    PriorityNode *newNode = (PriorityNode *)malloc(sizeof(PriorityNode));
    newNode->data = data;
    newNode->priority = priority;
    newNode->next = NULL;

    if (isEmptyPriority(pq) || pq->front->priority > priority) {
        newNode->next = pq->front;
        pq->front = newNode;
    } else {
        PriorityNode *temp = pq->front;
        while (temp->next != NULL && temp->next->priority <= priority) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }
    printf("Inserted %d with priority %d\n", data->processID, priority);
}

PCB * dequeuePriority(PriorityQueue *pq) {
    if (isEmptyPriority(pq)) {
        printf("Priority Queue is empty!\n");
        return NULL;
    } else {
        PriorityNode *temp = pq->front;
        PCB * item = temp->data;
        pq->front = pq->front->next;
        free(temp);
        printf("Deleted %d\n", item->processID);
        return item;
    }
}

void displayPriority(PriorityQueue *pq) {
    PriorityNode *temp = pq->front;
    if (isEmptyPriority(pq)) {
        printf("Priority Queue is empty!\n");
    } else {
        printf("Priority Queue elements are:\n");
        while (temp) {
            PCB * item=temp->data;
            printf("Data: %d, Priority: %d\n",item->processID , temp->priority);
            temp = temp->next;
        }
    }
}
