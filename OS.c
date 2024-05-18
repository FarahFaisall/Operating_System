#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

// Global Initialization
Memory memory;
Queue waitingQueue;
PriorityQueue readyQueue;



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

void initializeKeyPointer(KeyPointer* kp, const char* name, const char* value) {
    strncpy(kp->name, name, sizeof(kp->name) - 1);
    kp->name[sizeof(kp->name) - 1] = '\0';
    strncpy(kp->value, value, sizeof(kp->value) - 1);
    kp->value[sizeof(kp->value) - 1] = '\0';
}
void initializeKeyPointerWithInt(KeyPointer* kp, const char* name, int value) {
    char valueStr[100];
    snprintf(valueStr, sizeof(valueStr), "%d", value);  // Convert integer to string
    initializeKeyPointer(kp,name,valueStr);
}

KeyPointer* allocateProcess(char process[], int id)
{
    PCB pcb = {id, "Ready", 1, 0, memory.currentLoc, 0};
    KeyPointer pID;
    KeyPointer PC;
    KeyPointer state={"State","Ready"};
    KeyPointer priority;
    KeyPointer lowerBound;
    KeyPointer upperBound;

    initializeKeyPointerWithInt(&pID,"Process ID: ",pcb.processID);
    initializeKeyPointerWithInt(&PC,"PC: ",pcb.PC);
    initializeKeyPointerWithInt(&priority,"Current Priority: ",pcb.currentPriority);
    initializeKeyPointerWithInt(&lowerBound,"Lower Bound",pcb.lowerBound);
    parseIntoLines(process);
    memory.currentLoc += 3;
    initializeKeyPointerWithInt(&upperBound,"Upper Bound: ",memory.currentLoc-1);

    KeyPointer * pIDkp= & memory.PCBs[memory.pcbPointer];
    // // allocating the PCB in the PCB's memory space
    memory.PCBs[memory.pcbPointer++] = pID;
    memory.PCBs[memory.pcbPointer++] = PC;
    memory.PCBs[memory.pcbPointer++] = state;
    memory.PCBs[memory.pcbPointer++] = priority;
    memory.PCBs[memory.pcbPointer++] = lowerBound;
    memory.PCBs[memory.pcbPointer++] = upperBound;

    return pIDkp;
}


int main()
{
    memory.currentLoc = 0;
    memory.pcbPointer = 0;
    initializePriorityQueue(&readyQueue);
    initializeQueue(&waitingQueue);
    char proccesses[][20] = {"Program_1.txt", "Program_2.txt", "Program_3.text"};
    int serviceTime[3];
    for (int i = 0; i < 3; i++)
    {
        printf("Please input the service time of program %d:\n", i + 1);
        scanf("%d", &serviceTime[i]);
    }
    int i = 0;


    // implementing the scheduler
    do
    {
        printf("Clock cycle %d --> %d\n", i, i + 1);
        //check if any process arrived
        for (int j = 0; j < 3; j++)
        {
            if (i == serviceTime[j])
            {
                KeyPointer *PCBPointer= allocateProcess(proccesses[j], j+1);
                enqueuePriority(&readyQueue,PCBPointer,parseInt((PCBPointer+2)->value));
            }
        }

        i++;
    } while (i<0);

    for (int i=0;i<7;i++){
        printf("Name : %s , Pointer %s\n",memory.PCBs[i].name,memory.PCBs[i].value);
    }


    return 0;
}
