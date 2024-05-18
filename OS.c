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
Queue blockedQueue;
PriorityQueue readyQueue;
Mutex inputMutex;
Mutex outputMutex;
Mutex fileMutex;

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
    m->value = 1;  // one means the mutex is available, zero means it's locked
    initializePriorityQueue(&m->queue); // Initialize the priority queue
    m->ownerID = -1;
    m->lock = 1; // lock is initially released (1 is released, 0 unreleased)
}

void semWaitB(Mutex *m, KeyPointer *p)
{
    m->lock = 0;
    if (m->value == 1)
    {
        m->ownerID = p->value;
        m->value = 0;
        printf("Process %d acquired resource\n", p->value);
    }
    else
    {
        enqueuePriority(&m->queue, p, (p + 2)->value);
        printf("Process %d is blocked waiting for resource\n", p->value);
    }
    m->lock = 1;
}

void semSignalB(Mutex *m, KeyPointer *p)
{
    // ensures that only one thread at a time can execute the critical section;  no other thread can change these variables concurrently
    m->lock = 0;
    if (m->ownerID == p->value)
    {
        if (isEmptyPriority(&m->queue))
        {
            m->value = 1;
            m->ownerID = -1;
            printf("Process %d released resource\n", p->value);
        }
        else
        {
            KeyPointer *nextProcess = dequeuePriority(&m->queue);
            m->ownerID = nextProcess->value;
            printf("Process %d is unblocked and now owns resource\n", m->ownerID);
        }
    }
    // after modifying the Mutex state, the function releases the lock, allowing other threads to acquire it
    m->lock = 1;
}

void execute(KeyPointer *PCB, int quantum)
{
    int PC = (PCB + 3)->value;
    int lowerBound = (PCB + 4)->value;
    for (int i = quantum; i > 0; i--)
    {
        char **lineSplitted = mySplit(memory.array[lowerBound + (PC++)].value);
        // what to do with first line
        if (strcmp(lineSplitted[0], "semWait") == 0)
        {
        }
        else if (strcmp(lineSplitted[0], "semSignal") == 0)
        {
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
            int x = parseInt(lineSplitted[1]);
            int y = parseInt(lineSplitted[1]);
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
        }
    }
}

void initializeKeyPointer(KeyPointer *kp, const char *name, const char *value)
{
    strncpy(kp->name, name, sizeof(kp->name) - 1);
    kp->name[sizeof(kp->name) - 1] = '\0';
    strncpy(kp->value, value, sizeof(kp->value) - 1);
    kp->value[sizeof(kp->value) - 1] = '\0';
}
void initializeKeyPointerWithInt(KeyPointer *kp, const char *name, int value)
{
    char valueStr[100];
    snprintf(valueStr, sizeof(valueStr), "%d", value); // Convert integer to string
    initializeKeyPointer(kp, name, valueStr);
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

int main()
{
    initializeMemory(&memory);
    initializePriorityQueue(&readyQueue);
    initializeQueue(&blockedQueue);
    initializeMutex(&fileMutex);
    initializeMutex(&inputMutex);
    initializeMutex(&outputMutex);

    char proccesses[][20] = {"Program_1.txt", "Program_2.txt", "Program_3.text"};
    int arrivalTime[3];
    for (int i = 0; i < 3; i++)
    {
        printf("Please input the arrivalTime time of program %d:\n", i + 1);
        scanf("%d", &arrivalTime[i]);
    }

    int i = 0;
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
                enqueuePriority(&readyQueue, PCBPointer, parseInt((PCBPointer + 2)->value));
            }
        }
        // seeing which process will execute
        KeyPointer *PCBPointer = dequeuePriority(&readyQueue);
        int priority = parseInt((PCBPointer + 2)->value);
        int quantum = (int)pow(2, priority - 1);
        execute(PCBPointer, quantum);

        i++;
    } while (!isEmptyPriority(&readyQueue));

    for (int i = 0; i < 6; i++)
    {
        printf("Name : %s , Value: %s\n", memory.PCBs[i].name, memory.PCBs[i].value);
    }

    return 0;
}
