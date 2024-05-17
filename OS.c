#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// we are implementing priority queue structure
typedef struct Node
{
    int data;
    int priority;
    struct Node *next;
} Node;

typedef struct
{
    Node *front;
} PriorityQueue;

void initializeQueue(PriorityQueue *pq)
{
    pq->front = NULL;
}

int isEmpty(PriorityQueue *pq)
{
    return pq->front == NULL;
}

void enqueue(PriorityQueue *pq, int data, int priority)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->priority = priority;
    newNode->next = NULL;

    if (isEmpty(pq) || pq->front->priority > priority)
    {
        newNode->next = pq->front;
        pq->front = newNode;
    }
    else
    {
        Node *temp = pq->front;
        while (temp->next != NULL && temp->next->priority <= priority)
        {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }
    printf("Inserted %d with priority %d\n", data, priority);
}

int dequeue(PriorityQueue *pq)
{
    if (isEmpty(pq))
    {
        printf("Priority Queue is empty!\n");
        return -1;
    }
    else
    {
        Node *temp = pq->front;
        int item = temp->data;
        pq->front = pq->front->next;
        free(temp);
        printf("Deleted %d\n", item);
        return item;
    }
}

void display(PriorityQueue *pq)
{
    Node *temp = pq->front;
    if (isEmpty(pq))
    {
        printf("Priority Queue is empty!\n");
    }
    else
    {
        printf("Priority Queue elements are: \n");
        while (temp)
        {
            printf("Data: %d, Priority: %d\n", temp->data, temp->priority);
            temp = temp->next;
        }
    }
}
//----------------------------------------------------------------------
// BEGGINING OF PROJECT

// stores word for example: State: ”Ready”.
typedef struct
{
    char name[100];
    char value[100];
} KeyPointer;

// PCB stores info about a process
typedef struct
{
    int processID;
    char processState[10];
    int currentPriority;
    int PC;
    int lowerBound;
    int upperBound;
} PCB;

// Stores 60 key pointers
typedef struct
{
    KeyPointer array[60];
    int currentLoc;

} Memory;

// Global Initialization
Memory memory;

void PCBinit()
{
    
}

char **mySplit(char *str)
{

    char **storeSplit = (char **)malloc(sizeof(char *) * strlen(str));
    int counter = 0;

    // Initializes all the string[] values to "" so when the string
    // and char concatenates, 'null' doesn't appear.
    for (int i = 0; i < strlen(str); i++)
    {
        storeSplit[i] = (char *)malloc(sizeof(char) * 2);
        storeSplit[i][0] = '\0';
    }

    // Puts the str values into the split array and concatenates until
    // a delimiter is found, then it moves to the next array index.
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] != ' ')
        {
            char temp[2];
            temp[0] = str[i];
            temp[1] = '\0';
            strcat(storeSplit[counter], temp);
        }
        else
        {
            ++counter;
            // Move to the next index
            storeSplit[counter] = (char *)malloc(sizeof(char) * 2);
            storeSplit[counter][0] = '\0';
        }
    }

    return storeSplit;
}

void parseIntoLines(char program[])
{
    FILE *input;
    // Open a file in read mode
    char instruction[100];

    input = fopen(program, "r");
    int c = 0;
    while (fgets(instruction, 100, input))
    {
        KeyPointer kp;
        snprintf(kp.name, sizeof(kp.name), "instruction %d", c++);
        snprintf(kp.value, sizeof(kp.value), "%s", instruction);
        memory.array[memory.currentLoc++] = kp;
    }
    fclose(input);
}

void execute(char line[])
{
    char **lineSplitted = mySplit(line);
    // what to do with first line
    if (strcmp(lineSplitted[0], "semWait") == 0)
    {
    }
    else if (strcmp(lineSplitted[0], "semSignal") == 0)
    {
    }
    else if (strcmp(lineSplitted[0], "print") == 0)
    {
    }
    else if (strcmp(lineSplitted[0], "assign") == 0)
    {
    }
    else if (strcmp(lineSplitted[0], "writeFile") == 0)
    {
    }
    else if (strcmp(lineSplitted[0], "readFile") == 0)
    {
    }
    else if (strcmp(lineSplitted[0], "printFromTo") == 0)
    {
    }
    else
    {
    }
}

int main()
{
    memory.currentLoc = 0;
    char proccesses[][20] = {"Program_1.txt", "Program_2.txt", "Program_3.text"};
    int serviceTime[] = {6, 2, 0};
    int i = 0;
    while (true){
        for (int j=0; j<3;j++){
            if (i==serviceTime[j])
            {
                PCB pcb;
                KeyPointer PC={"PC: ",pcb.PC=0};
                // pcb.currentPriority=1;
                // pcb.lowerBound=memory.currentLoc;
                // pcb.processState="Ready";
                // memory.array[memory.currentLoc++]=pcb;
                // proccesses[j];
            }
        }
    }

    PriorityQueue pq;
    initializeQueue(&pq);

    return 0;
}
