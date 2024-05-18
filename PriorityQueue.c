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
    printf("Inserted %s with priority %d\n", data->value, priority);
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
        printf("Deleted %s\n", item->value);
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
            KeyPointer * item=temp->data;
            printf("Data: %s, Priority: %d\n",item->value , temp->priority);
            temp = temp->next;
        }
    }
}
KeyPointer* dequeueSpecific(PriorityQueue* pq, int processID) {
    if (isEmptyPriority(pq)) {
        printf("Priority Queue is empty!\n");
        return NULL;
    }

    PriorityNode* temp = pq->front;
    PriorityNode* prev = NULL;
    while (temp != NULL && parseInt(temp->data-> value) != processID) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Process ID %d not found in the queue!\n", processID);
        return NULL;
    }

    if (prev == NULL) {
        pq->front = temp->next;
    } else {
        prev->next = temp->next;
    }

    KeyPointer* item = temp->data;
    free(temp);
    printf("Deleted process ID %s\n", item->value);
    return item;
}
