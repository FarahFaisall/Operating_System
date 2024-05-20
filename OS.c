#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "PriorityQueue.h" // Include the header file
#include "Queue.h"         // Include the header file
#include "KeyPointer.h"
#include "Utility.h"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

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

// Defining renameFile.txt Mutex
typedef struct
{
    int value; // 1 available, 0 locked.(shows whether the resource protected by the mutex is currently in use.)
    PriorityQueue queue;
    int ownerID; // pid that has the resource
    int lock;    // simulate the lock with renameFile.txt simple flag
    char* name ;
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
    // renameFile.txt delimiter is found, then it moves to the next array index.
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] != ' ' && str[i] != '\r')
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
char *getLine(const char *str) {
    // Allocate memory for the result, considering the worst case where no '\r' is found
    char *storeSplit = (char *)malloc(strlen(str) + 1);
    if (storeSplit == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while (str[i] != '\r' && str[i] != '\0') {
        storeSplit[i] = str[i];
        i++;
    }
    storeSplit[i] = '\0';

    return storeSplit;
}

void parseIntoLines(char program[])
{
    FILE *input;
    // Open renameFile.txt file in read mode
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
        strncpy(kp.value, getLine(instruction), sizeof(kp.value) - 1);
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

bool semWaitB(Mutex *m, KeyPointer *p,int quantum)
{
    bool flag;
    m->lock = 0;
    if (m->value == 1)
    {
        m->ownerID = parseInt(p->value);
        m->value = 0;
        printf(GRN"Process %s acquired resource %s\n"RESET, p->value,m->name);
        flag = 0; // MAFEESH MOSHEKLA
    }
    else
    {
        printf(RED"Process %s is blocked waiting for resource %s\n"RESET, p->value,m->name);
        char* pcbState= (p + 2)->value;
        sprintf(pcbState,"%s","Blocked");
        int priority = parseInt((p + 3)->value);
        if (quantum==0) {
            sprintf((p+3)->value,"%d",++priority);
        }
        enqueuePriority(&m->queue, p, priority);
        flag = 1; // FEEH MOSHKELA AKA Blocked
    }
    m->lock = 1;
    return flag;
}

KeyPointer *semSignalB(Mutex *m, KeyPointer *p)
{
    // ensures that only one thread at renameFile.txt time can execute the critical section;  no other thread can change these variables concurrently
    m->lock = 0;
    if (m->ownerID == parseInt(p->value))
    {
        printf(GRN"Process %s released resource %s\n"RESET, p->value,m->name);
        if (isEmptyPriority(&m->queue))
        {
            m->value = 1;
            m->ownerID = -1;
        }
        else
        {
            KeyPointer *nextProcess = dequeuePriority(&m->queue);
            char* pcbState= (nextProcess + 2)->value;
            sprintf(pcbState,"%s","Ready");
            m->ownerID = parseInt(nextProcess->value);
            printf("Process %d is unblocked and now owns resource %s\n"RESET, m->ownerID,m->name);
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

bool execute(KeyPointer *PCB,int quantum)
{
    int intPC=parseInt((PCB + 1)->value);
    int lowerBound = parseInt((PCB + 4)->value);
    char **lineSplitted = mySplit(memory.array[lowerBound + ((intPC)++)].value);
    sprintf(((PCB + 1)->value),"%d",intPC);
    char* tempp = (PCB + 1)->value;
    // what to do with first line
    if (strcmp(lineSplitted[0], "semWait") == 0)
    {
        bool isBlocked;
        if (strcmp(lineSplitted[1], "userInput") == 0)
        {
            isBlocked = semWaitB(&inputMutex, PCB,quantum-1);
        }
        else if (strcmp(lineSplitted[1], "userOutput") == 0)
        {
            isBlocked = semWaitB(&outputMutex, PCB,quantum-1);
        }
        else
        {
            isBlocked = semWaitB(&fileMutex, PCB,quantum-1);
        }
        if (isBlocked)
        {
            enqueue(&blockedQueue, PCB);
            printf(" in blocked queue\n");
            return 1; // Current Processs is blocked
        }
    }
    else if (strcmp(lineSplitted[0], "semSignal") == 0)
    {
        if (strcmp(lineSplitted[1], "userInput") == 0)
        {
            KeyPointer *unblocked = semSignalB(&inputMutex, PCB);
            if (unblocked != NULL)
            {
                dequeueSpecific(&blockedQueue, parseInt(unblocked->value));
                printf("from blocked queue\n");
                enqueue(&readyQueue, unblocked);
                printf(" to ready queue\n");

            }
        }
        else if (strcmp(lineSplitted[1], "userOutput") == 0)
        {
            KeyPointer *unblocked = semSignalB(&outputMutex, PCB);
            if (unblocked != NULL)
            {
                dequeueSpecific(&blockedQueue, parseInt(unblocked->value));
                printf("from blocked queue\n");
                enqueue(&readyQueue, unblocked);
                printf(" to ready queue\n");
            }
        }
        else
        {
            KeyPointer *unblocked = semSignalB(&fileMutex, PCB);
            if (unblocked != NULL)
            {
                dequeueSpecific(&blockedQueue, parseInt(unblocked->value));
                printf("from blocked queue\n");
                enqueue(&readyQueue, unblocked);
                printf(" to ready queue\n");
            }
        }
    }
    else if (strcmp(lineSplitted[0], "print") == 0)

    {
        bool found = false;
        int upperBound = parseInt((PCB + 5)->value);
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
            printf("Please enter input value for %s: \n", lineSplitted[1]);
            scanf("%s", input);
            initializeKeyPointer(&variable, lineSplitted[1], input);
        }
            //"assign x y" case when y is renameFile.txt value
        else if (strcmp(lineSplitted[2], "readFile") == 0)
        {
            FILE *file;
            char *buffer;
            long file_length;

            //i want to search for file name inside the variable
            int upperBound = parseInt((PCB + 5)->value);
            char* x ;
            for (int i = upperBound - 2; i <= upperBound; i++)
            {
                if (strcmp(memory.array[i].name, lineSplitted[3]) == 0)
                {
                   x= memory.array[i].value;
                    break;
                }
            }

            file = fopen(x, "r");
            if (file == NULL)
            {
                fprintf(stderr, "Error opening file.\n");
                initializeKeyPointer(&variable, lineSplitted[1], "10");
                return 0;
            }
            // The third argument SEEK_END indicates that we want to move the file position indicator to the end of the file.
            // After this call, the file position indicator is at the end of the file.
            fseek(file, 0, SEEK_END);
            // ftell is renameFile.txt function that returns the current file position indicator for the given file stream.
            file_length = ftell(file);
            // fseek is called again to reset the file position indicator to the beginning of the file.
            fseek(file, 0, SEEK_SET);
            // allocate specified number of bytes with file length +1 for null terminate
            buffer = (char *)malloc(file_length + 1);

            if (buffer == NULL)
            {
                perror("Memory allocation error");
                fclose(file);
            }
            // second argument is the size of each element to be read, which is 1 byte in this case.
            fread(buffer, 1, file_length, file);
            buffer[file_length] = '\0'; // Null-terminate the string
            fclose(file);
            printf("Read %s from file: %s\n",buffer,x);
            // add the value which is inside the buffer to the variable
            initializeKeyPointer(&variable, lineSplitted[1], buffer);
        }
        else
        {
            initializeKeyPointer(&variable, lineSplitted[1], lineSplitted[2]);
        }
        int upperBound =parseInt((PCB + 5)->value);
        bool full = true;
        for (int i = upperBound - 2; i <= upperBound; i++)
        {
            if (memory.array[i].name[0] =='\0')
            {
                memory.array[i] = variable;
                full = false;
                break;
            }
        }
        if (full)
            printf("The memory space of process %s is full, there's no place to assign new variables\n", PCB->value);
    }
    else if (strcmp(lineSplitted[0], "writeFile") == 0)
    {

        //i want to search for file name inside the variable
        int upperBound = parseInt((PCB + 5)->value);
        char* fileName = NULL;
        char* data= NULL;
        for (int i = upperBound - 2; i <= upperBound; i++)
        {
            if (strcmp(memory.array[i].name, lineSplitted[1]) == 0)
                fileName= memory.array[i].value;
            if (strcmp(memory.array[i].name, lineSplitted[2]) == 0)
                data= memory.array[i].value;
        }
        if (fileName == NULL)
        {
            printf("File name not found in memory.\n");
            return 0;
        }
        FILE *file;
        // Open the file for writing
        file = fopen(fileName, "w");
        if (file == NULL)
        {
            fprintf(stderr, "Error opening file.\n");
            return 0;
        }
        printf("Writing into file: %s data: %s\n",fileName,data);

        // Write content to the file
        fprintf(file,"%s", data);

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
            return 0;
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
        int upperBound = parseInt((PCB + 5)->value);
        for (int i = upperBound - 2; i <= upperBound; i++)
        {
            if (strcmp(memory.array[i].name, lineSplitted[1]) == 0)
            {
                x = parseInt(memory.array[i].value);
                varFound1 = true;
                break;
            }
        }
        for (int i = upperBound - 2; i <= upperBound; i++)
        {
            if (strcmp(memory.array[i].name, lineSplitted[2]) == 0)
            {
                y = parseInt(memory.array[i].value);
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
                printf("   %d\n", i);
            }
        }
        else
        {
            for (int i = x; i >= y; i--)
            {
                printf("    %d\n", i);
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

    initializeKeyPointerWithInt(&pID, "Process ID ", pcb.processID);
    initializeKeyPointerWithInt(&PC, "PC ", pcb.PC);
    initializeKeyPointerWithInt(&priority, "Current Priority ", pcb.currentPriority);
    initializeKeyPointerWithInt(&lowerBound, "Lower Bound", pcb.lowerBound);
    parseIntoLines(process);
    memory.currentLoc += 3;
    initializeKeyPointerWithInt(&upperBound, "Upper Bound ", memory.currentLoc - 1);

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
    int lowerBound = parseInt(((kp + 4)->value));
    int upperBound = parseInt(((kp + 5)->value));
    int pc = parseInt(((kp + 1)->value));
    int exec = upperBound - lowerBound - pc - 3;
    return exec;
}

int main()
{
    fileMutex.name = "file";
    inputMutex.name = "userInput";
    outputMutex.name = "userOutput";
    initializeMemory(&memory);
    initializeQueue(&readyQueue);
    initializeQueue(&blockedQueue);
    initializeMutex(&fileMutex);
    initializeMutex(&inputMutex);
    initializeMutex(&outputMutex);
    initializeQueue(&queue1);
    initializeQueue(&queue2);
    initializeQueue(&queue3);
    initializeQueue(&queue4);

    char proccesses[][20] = {"Program_1.txt", "Program_2.txt", "Program_3.txt"};
    // sample input in arrivalTime is {3,2,10}
    int arrivalTime[3];
    for (int i = 0; i < 3; i++)
    {

        printf("Please input the arrivalTime time of program %d:\n", i + 1);
        scanf("%d", &arrivalTime[i]);

    }

    int i = 0;
    int quantum = 0;
    KeyPointer *currPCB = NULL;
    int programCount=3;

    // implementing the scheduler
    do
    {
        if(currPCB != NULL && getRemainingExecTime(currPCB)<0)
        {
            quantum=0;
            char* pcbState= (currPCB + 2)->value;
            printf(YEL"Process %d terminated\n"RESET,parseInt((currPCB)->value));
            sprintf(pcbState,"%s","Terminated");
            currPCB=NULL;
            programCount--;
        }

        if (programCount>0)
            printf(BLU"Clock cycle %d --> %d\n"RESET, i, i + 1);

        // check if any process arrived
        for (int j = 0; j < 3; j++)
        {
            // checking the arrival of any new processes in the current clk cycle
            if (i == arrivalTime[j])
            {
                KeyPointer *PCBPointer = allocateProcess(proccesses[j], j + 1);
                printf("Process %d arrived\n", j + 1);
                enqueue(&readyQueue, PCBPointer);
                printf(" in ready queue\n");
            }
        }
        while (!isEmpty(&readyQueue))
        {
            KeyPointer *tempPCB = dequeue(&readyQueue);
            printf(" from ready queue to insert it in multilevel feedback queue: ");
            if (parseInt((tempPCB + 3)->value) == 1)
            {
                enqueue(&queue1, tempPCB);
                printf(" in queue 1\n");
            }
            else if (parseInt((tempPCB + 3)->value) == 2)
            {
                enqueue(&queue2, tempPCB);
                printf(" in queue 2\n");
            }
            else if (parseInt((tempPCB + 3)->value) == 3)
            {
                enqueue(&queue3, tempPCB);
                printf(" in queue 3\n");
            }
            else
            {
                enqueue(&queue4, tempPCB);
                printf(" in queue 4\n");
            }
        }

        // seeing which process will execute
        if (quantum == 0)
        {

            // INCREASE PRIORITY TO GO DOWN THE QUEUE LIST
            // if (currPCB != NULL && processExecuting)
            if (currPCB != NULL)
            {
                char *priority = (currPCB + 3)->value;
                int x = parseInt(priority);
                if (x < 4)
                {
                    x++;
                    sprintf(priority, "%d", x);
                }
                if (parseInt(priority) == 2)
                {
                    enqueue(&queue2, currPCB);
                    printf(" in queue 2\n");
                }
                else if (parseInt(priority) == 3)
                {
                    enqueue(&queue3, currPCB);
                    printf(" in queue 3\n");
                }
                else
                {
                    enqueue(&queue4, currPCB);
                    printf(" in queue 4\n");
                }
            }
            if (!isEmpty(&queue1))
            {
                currPCB = dequeue(&queue1);
                printf(" from queue 1 to execute\n");
                quantum = 1;
                //processExecuting = true;
            }
            else if (!isEmpty(&queue2))
            {
                currPCB = dequeue(&queue2);
                printf(" from queue 2 to execute\n");
                quantum = 2;
                //processExecuting = true;
            }
            else if (!isEmpty(&queue3))
            {
                currPCB = dequeue(&queue3);
                printf(" from queue 3 to execute\n");
                quantum = 4;
                //processExecuting = true;
            }
            else if (!isEmpty(&queue4))
            {
                currPCB = dequeue(&queue4);
                printf(" from queue 4 to execute\n");
                quantum=8;
                //processExecuting = true;
            }
        }
        bool processed=NULL;
        if(currPCB!=NULL) {
            printf(CYN"Execution Phase: \n"RESET);
            processed = execute(currPCB, quantum);
            quantum--;
            if (quantum == 0)
            {
                printf(MAG"Quantum finished\n"RESET);
            }
        }
        if (processed)
        {
            currPCB = NULL;
            quantum = 0;
        }
        i++;
    } while (!isEmpty(&queue1) || !isEmpty(&queue2) || !isEmpty(&queue3) || !isEmpty(&queue4) || !isEmpty(&readyQueue) || !isEmpty(&blockedQueue) || currPCB!=NULL || programCount>0);

    printf(GRN"The main memory\n"RESET);
    for (int i = 0; i < 18; i++)
    {
        printf("Name : %s , Value: %s\n", memory.PCBs[i].name, memory.PCBs[i].value);
    }
    for (int i =0 ; i<memory.currentLoc;i++) {
        printf("Name : %s , Value: %s\n", memory.array[i].name, memory.array[i].value);
    }
    return 0;
}


