#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "PriorityQueue.h" // Include the header file
#include "Queue.h"         // Include the header file
#include "KeyPointer.h"
#include "Utility.h"

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
    KeyPointer array[42];
    int currentLoc;
    KeyPointer PCBs[18];
    int pcbPointer;
} Memory;

// Defining a Mutex
typedef struct
{
    int value; // 1 available, 0 locked.(shows whether the resource protected by the mutex is currently in use.)
    PriorityQueue queue;
    int ownerID; // pid that has the resource
    int lock;    // simulate the lock with a simple flag
} Mutex;

// Global Initialization
Memory memory;
Queue readyQueue;
Queue blockedQueue;
Mutex inputMutex;
Mutex outputMutex;
Mutex fileMutex;
Queue queue1;
Queue queue2;
Queue queue3;
Queue queue4;

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
    if (input == NULL)
    {
        printf("Error opening file.\n");
        return;
    }
    int c = 0;
    while (fgets(instruction, 100, input))
    {
        KeyPointer kp;
        snprintf(kp.name, sizeof(kp.name), "instruction %d", c++);

        // Remove the newline character if present
        instruction[strcspn(instruction, "\n")] = '\0';

        // Copy the instruction to the value field
        strncpy(kp.value, instruction, sizeof(kp.value) - 1);
        kp.value[sizeof(kp.value) - 1] = '\0'; // Ensure null termination

        memory.array[memory.currentLoc++] = kp;
    }
    fclose(input);
}
void initializeMutex(Mutex *m)
{
    m->value = 1;                       // one means the mutex is available, zero means it's locked
    initializePriorityQueue(&m->queue); // Initialize the priority queue
    m->ownerID = -1;
    m->lock = 1; // lock is initially released (1 is released, 0 unreleased)
}

bool semWaitB(Mutex *m, KeyPointer *p)
{
    bool flag;
    m->lock = 0;
    if (m->value == 1)
    {
        m->ownerID = parseInt(p->value);
        m->value = 0;
        printf("Process %s acquired resource\n", p->value);
        flag = 0; // MAFEESH MOSHEKLA
    }
    else
    {
        enqueuePriority(&m->queue, p, parseInt((p + 2)->value));
        printf("Process %s is blocked waiting for resource\n", p->value);
        flag = 1; // FEEH MOSHKELA AKA Blocked
    }
    m->lock = 1;
    return flag;
}

KeyPointer *semSignalB(Mutex *m, KeyPointer *p)
{
    // ensures that only one thread at a time can execute the critical section;  no other thread can change these variables concurrently
    m->lock = 0;
    if (m->ownerID == parseInt(p->value))
    {
        if (isEmptyPriority(&m->queue))
        {
            m->value = 1;
            m->ownerID = -1;
            printf("Process %s released resource\n", p->value);
        }
        else
        {
            KeyPointer *nextProcess = dequeuePriority(&m->queue);
            m->ownerID = *(nextProcess->value);
            printf("Process %d is unblocked and now owns resource\n", m->ownerID);
            m->lock = 1;
            return nextProcess;
        }
    }
    // after modifying the Mutex state, the function releases the lock, allowing other threads to acquire it
    m->lock = 1;
    return NULL;
}

void initializeKeyPointer(KeyPointer *kp, char *name, char *value)
{
    strncpy(kp->name, name, sizeof(kp->name) - 1);
    kp->name[sizeof(kp->name) - 1] = '\0';
    strncpy(kp->value, value, sizeof(kp->value) - 1);
    kp->value[sizeof(kp->value) - 1] = '\0';
}
void initializeKeyPointerWithInt(KeyPointer *kp, char *name, int value)
{
    char valueStr[100];
    snprintf(valueStr, sizeof(valueStr), "%d", value); // Convert integer to string
    initializeKeyPointer(kp, name, valueStr);
}

bool execute(KeyPointer *PCB)
{
    int intPC=parseInt((PCB + 3)->value);
    int lowerBound = parseInt((PCB + 4)->value);
    char **lineSplitted = mySplit(memory.array[lowerBound + ((intPC)++)].value);
    sprintf(&((PCB + 3)->value),"%d",intPC);
    // what to do with first line
    if (strcmp(lineSplitted[0], "semWait") == 0)
    {
        bool isBlocked;
        if (strcmp(lineSplitted[1], "userInput"))
        {
            isBlocked = semWaitB(&inputMutex, PCB);
        }
        else if (strcmp(lineSplitted[1], "userOutput"))
        {
            isBlocked = semWaitB(&outputMutex, PCB);
        }
        else
        {
            isBlocked = semWaitB(&fileMutex, PCB);
        }
        if (isBlocked)
        {
            enqueue(&blockedQueue, PCB);
            return 1; // Current Processs is blocked
        }
    }
    else if (strcmp(lineSplitted[0], "semSignal") == 0)
    {
        if (strcmp(lineSplitted[1], "userInput"))
        {
            KeyPointer *unblocked = semSignalB(&inputMutex, PCB);
            if (unblocked != NULL)
            {
                dequeueSpecific(&blockedQueue, parseInt(unblocked->value));
                enqueue(&readyQueue, unblocked);
            }
        }
        else if (strcmp(lineSplitted[1], "userOutput"))
        {
            KeyPointer *unblocked = semSignalB(&outputMutex, PCB);
            if (unblocked != NULL)
            {
                dequeueSpecific(&blockedQueue, parseInt(unblocked->value));
                enqueue(&readyQueue, unblocked);
            }
        }
        else
        {
            KeyPointer *unblocked = semSignalB(&fileMutex, PCB);
            if (unblocked != NULL)
            {
                dequeueSpecific(&blockedQueue, unblocked);
                enqueue(&readyQueue, unblocked);
            }
        }
    }
    else if (strcmp(lineSplitted[0], "print") == 0)

    {
        bool found = false;
        int upperBound = (PCB + 5)->value;
        for (int i = upperBound - 2; i <= upperBound; i++)
        {
            if (strcmp(memory.array[i].name, lineSplitted[1]) == 0)
            {
                printf("%s \n", memory.array[i].value);
                found = true;
                break;
            }
        }
        if (!found)
        {
            printf("Variable %s not found in memory.\n", lineSplitted[1]);
        }
    }

    else if (strcmp(lineSplitted[0], "assign") == 0)

    {
        KeyPointer variable;
        //"assign x y" case when y is an input
        if (strcmp(lineSplitted[2], "input") == 0)
        {
            // handle mutex
            char input[100];
            scanf("Please enter a value %s", &input);
            initializeKeyPointer(&variable, lineSplitted[1], input);
        }
        //"assign x y" case when y is a value
        else if (strcmp(lineSplitted[2], "readFile") == 0)
        {
            FILE *file;
            char *buffer;
            long file_length;
            file = fopen(lineSplitted[1], "r");
            if (file == NULL)
            {
                fprintf(stderr, "Error opening file.\n");
                return 1;
            }
            // The third argument SEEK_END indicates that we want to move the file position indicator to the end of the file.
            // After this call, the file position indicator is at the end of the file.
            fseek(file, 0, SEEK_END);
            // ftell is a function that returns the current file position indicator for the given file stream.
            file_length = ftell(file);
            // fseek is called again to reset the file position indicator to the beginning of the file.
            fseek(file, 0, SEEK_SET);
            // allocate specified number of bytes with file length +1 for null terminate
            buffer = (char *)malloc(file_length + 1);

            if (buffer == NULL)
            {
                perror("Memory allocation error");
                fclose(file);
                return 1;
            }
            // second argument is the size of each element to be read, which is 1 byte in this case.
            fread(buffer, 1, file_length, file);
            buffer[file_length] = '\0'; // Null-terminate the string
            fclose(file);

            // add the value which is inside the buffer to the variable
            initializeKeyPointer(&variable, lineSplitted[1], buffer);
        }
        else
        {
            initializeKeyPointer(&variable, lineSplitted[1], lineSplitted[2]);
        }
        int upperBound = (PCB + 5)->value;
        bool full = true;
        for (int i = upperBound - 2; i <= upperBound; i++)
        {
            if (memory.array[i].name == '\0')
            {
                memory.array[i] = variable;
                full = false;
                break;
            }
        }
        if (full)
            printf("The memory space of process %d is full, there's no place to assign new variables\n", PCB->value);
    }
    else if (strcmp(lineSplitted[0], "writeFile") == 0)
    {
        FILE *file;
        // Open the file for writing
        file = fopen(lineSplitted[1], "w");
        if (file == NULL)
        {
            fprintf(stderr, "Error opening file.\n");
            return 1;
        }

        // Write content to the file
        fprintf(file, lineSplitted[2]);

        // Close the file
        fclose(file);
    }
    else if (strcmp(lineSplitted[0], "readFile") == 0)
    {
        // waiting for TA response
        FILE *file;
        file = fopen(lineSplitted[1], "r");
        if (file == NULL)
        {
            printf("Error oppening file");
        }
    }
    else if (strcmp(lineSplitted[0], "printFromTo") == 0)
    {
        // where x is the first number, and y is the second number.
        // variables to be edited

        int x;
        int y;
        bool varFound1 = false;
        bool varFound2 = false;
        int upperBound = (PCB + 5)->value;
        for (int i = upperBound - 2; i <= upperBound; i++)
        {

            if (strcmp(memory.array[i].name, lineSplitted[1]) == 0)
            {
                x = parseInt(lineSplitted[1]);
                varFound1 = true;
                break;
            }
        }
        for (int i = upperBound - 2; i <= upperBound; i++)
        {
            if (strcmp(memory.array[i].name, lineSplitted[2]) == 0)
            {
                y = parseInt(lineSplitted[1]);
                varFound2 = true;
                break;
            }
        }
        if (varFound2 == false || varFound1 == false)
        {
            printf("one of the variables is not found\n");
        }

        if (x <= y)
        {
            for (int i = x; i <= y; i++)
            {
                printf("%d", i);
            }
        }
        else
        {
            for (int i = x; i >= y; i--)
            {
                printf("%d", i);
            }
        }
    }

    else
    {
        printf("unidentified instruction\n");
    }
    return 0; // Current Process good
}



KeyPointer *allocateProcess(char process[], int id)
{
    PCB pcb = {id, "Ready", 1, 0, memory.currentLoc, 0};
    KeyPointer pID;
    KeyPointer PC;
    KeyPointer state = {"State", "Ready"};
    KeyPointer priority;
    KeyPointer lowerBound;
    KeyPointer upperBound;

    initializeKeyPointerWithInt(&pID, "Process ID: ", pcb.processID);
    initializeKeyPointerWithInt(&PC, "PC: ", pcb.PC);
    initializeKeyPointerWithInt(&priority, "Current Priority: ", pcb.currentPriority);
    initializeKeyPointerWithInt(&lowerBound, "Lower Bound", pcb.lowerBound);
    parseIntoLines(process);
    memory.currentLoc += 3;
    initializeKeyPointerWithInt(&upperBound, "Upper Bound: ", memory.currentLoc - 1);

    KeyPointer *pIDkp = &memory.PCBs[memory.pcbPointer];
    // // allocating the PCB in the PCB's memory space
    memory.PCBs[memory.pcbPointer++] = pID;
    memory.PCBs[memory.pcbPointer++] = PC;
    memory.PCBs[memory.pcbPointer++] = state;
    memory.PCBs[memory.pcbPointer++] = priority;
    memory.PCBs[memory.pcbPointer++] = lowerBound;
    memory.PCBs[memory.pcbPointer++] = upperBound;

    return pIDkp;
}

void initializeMemory(Memory *memory)
{
    memory->currentLoc = 0;
    memory->pcbPointer = 0;
    for (int i = 0; i < 42; i++)
    {
        memory->array[i].name[0] = '\0';  // Mark as empty
        memory->array[i].value[0] = '\0'; // Mark as empty
    }
    for (int i = 0; i < 18; i++)
    {
        memory->PCBs[i].name[0] = '\0';  // Mark as empty
        memory->PCBs[i].value[0] = '\0'; // Mark as empty
    }
}

int getRemainingExecTime(KeyPointer *kp)
{
    return ((kp + 5)->value) - 3 - ((kp + 3)->value);
}

int main()
{
    initializeMemory(&memory);
    initializeMemory(&readyQueue);
    initializeQueue(&blockedQueue);
    initializeMutex(&fileMutex);
    initializeMutex(&inputMutex);
    initializeMutex(&outputMutex);
    initializeQueue(&queue1);
    initializeQueue(&queue2);
    initializeQueue(&queue3);
    initializeQueue(&queue4);

    char proccesses[][20] = {"Program_1.txt", "Program_2.txt", "Program_3.text"};
    // sample input in arrivalTime is {3,2,10}
    int arrivalTime[3];
    for (int i = 0; i < 3; i++)
    {

        printf("Please input the arrivalTime time of program %d:\n", i + 1);
        scanf("%d", &arrivalTime[i]);

        // program 1 --> 1
        // program 2 --> 3
        // program 3 --> 10
    }

    int i = 0;
    int quantum = 0;
    KeyPointer *currPCB;
    // implementing the scheduler
    do
    {
        printf("Clock cycle %d --> %d\n", i, i + 1);

        // check if any process arrived
        for (int j = 0; j < 3; j++)
        {
            // checking the arrival of any new processes in the current clk cycle
            if (i == arrivalTime[j])
            {
                KeyPointer *PCBPointer = allocateProcess(proccesses[j], j + 1);
                // enqueuePriority(&readyQueue, PCBPointer, parseInt((PCBPointer + 2)->value));
                enqueue(&readyQueue, PCBPointer);
            }
        }
        while (!isEmpty(&readyQueue))
        {
            KeyPointer *tempPCB = dequeue(&readyQueue);
            if (parseInt((currPCB + 2)->value) == 1)
            {
                enqueue(&queue1, tempPCB);
            }
            else if (parseInt((currPCB + 2)->value) == 2)
            {
                enqueue(&queue2, tempPCB);
            }
            else if (parseInt((currPCB + 2)->value) == 3)
            {
                enqueue(&queue3, tempPCB);
            }
            else
            {
                enqueue(&queue4, tempPCB);
            }
        }
        // seeing which process will execute
        if (quantum == 0)
        {
            // INCREASE PRIORITY TO GO DOWN THE QUEUE LIST
            if (currPCB != NULL && getRemainingExecTime(currPCB) != 0)
            {
                char  *priority = &((currPCB + 2)->value);
                int x = parseInt(*priority);
                if (x < 4){
                    x++;
                    sprintf(*(priority),"%d",x);
                }
                if (parseInt((currPCB + 2)->value) == 2)
                {
                    enqueue(&queue2, currPCB);
                }
                else if ( parseInt((currPCB + 2)->value) == 3)
                {
                    enqueue(&queue3, currPCB);
                }
                else
                {
                    enqueue(&queue4, currPCB);
                }
            }
            if (!isEmpty(&queue1))
            {
                currPCB = dequeue(&queue1);
            }
            else if (!isEmpty(&queue2))
            {
                currPCB = dequeue(&queue2);
            }
            else if (!isEmpty(&queue3))
            {
                currPCB = dequeue(&queue3);
            }
            else if (!isEmpty(&queue4))
            {
                currPCB = dequeue(&queue4);
            }
            int priority = parseInt((currPCB + 2)->value);
            quantum = (int)pow(2, priority - 1);
        }
        bool processed = execute(currPCB);
        quantum--;
        if (processed)
        {
            currPCB = NULL;
            quantum = 0;
        }
        i++;
    } while (!isEmpty(&queue1) || !isEmpty(&queue2) || !isEmpty(&queue3) || !isEmpty(&queue4));

    for (int i = 0; i < 6; i++)
    {
        printf("Name : %s , Value: %s\n", memory.PCBs[i].name, memory.PCBs[i].value);
    }

    return 0;
}
