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

// Global Initialization
Memory memory;
Queue waitingQueue;
PriorityQueue readyQueue;
int a;
int b;
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
                //
            }
            else
            {
                initializeKeyPointer(&variable, lineSplitted[1], lineSplitted[2]);
            }
            int upperBound=(PCB+5)->value;

            for (int i= upperBound-2 ; i<=upperBound;i++){
    
            }
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
    initializeKeyPointerWithInt(&upperBound, "Upper Bound: ", memory.currentLoc - 1);
    memory.currentLoc += 3;

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

void initializeMemory(Memory *memory) {
    memory->currentLoc = 0;
    memory->pcbPointer = 0;
    for (int i = 0; i < 42; i++) {
        memory->array[i].name[0] = '\0';  // Mark as empty
        memory->array[i].value[0] = '\0'; // Mark as empty
    }
    for (int i = 0; i < 18; i++) {
        memory->PCBs[i].name[0] = '\0';  // Mark as empty
        memory->PCBs[i].value[0] = '\0'; // Mark as empty
    }
}

int main()
{
   initializeMemory(&memory);
    initializePriorityQueue(&readyQueue);
    initializeQueue(&waitingQueue);
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
                enqueuePriority(&readyQueue, PCBPointer, parseInt((PCBPointer)->value));
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
