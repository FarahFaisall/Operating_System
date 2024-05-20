#include <stdio.h>
#include <stdlib.h>
#include "PriorityQueue.h"
#include "Utility.h"


void initializePriorityQueue(PriorityQueue *pq) {
    pq->front = NULL;
}

int isEmptyPriority(PriorityQueue *pq) {
    return pq->front == NULL;
}

void enqueuePriority(PriorityQueue *pq, KeyPointer* data, int priority) {
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
    printf("Inserted %s with priority %d to the mutex blocked queue\n", data->value, priority);
}

KeyPointer * dequeuePriority(PriorityQueue *pq) {
    if (isEmptyPriority(pq)) {
        printf("Priority Queue is empty!\n");
        return NULL;
    } else {
        PriorityNode *temp = pq->front;
        KeyPointer * item = temp->data;
        pq->front = pq->front->next;
        free(temp);
        printf("Removed process with pID: %s from mutex blocked queue\n", item->value);
        return item;
    }
}

void displayPriority(PriorityQueue *pq) {
    PriorityNode *temp = pq->front;
    if (isEmptyPriority(pq)) {
        printf("Mutex Priority Queue is empty!\n");
    } else {
        printf("Mutex Priority Queue elements are:\n");
        while (temp) {
            KeyPointer * item=temp->data;
            printf("Data: %s, Priority: %d\n",item->value , temp->priority);
            temp = temp->next;
        }
    }
}