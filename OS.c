#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "PriorityQueue.h" // Include the header file
#include "Queue.h"         // Include the header file
#include "PCB.h"

// stores word for example: State: ”Ready”.
typedef struct
{
    char name[100];
    char value[100];
} KeyPointer;

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

void allocateProcess(char process[], int id)
{
    PCB pcb = {id, "Ready", 1, 0, memory.currentLoc, 0};
    KeyPointer pID = {"Process ID: ", pcb.processID};
    KeyPointer PC = {"PC: ", pcb.PC};
    KeyPointer priority = {"Current priotity: ", 1};

    strncpy(pcb.processState, "Running", 9);
    pcb.processState[9] = '\0'; // Ensure null-termination

    KeyPointer state;

    // Initialize the 'name' field
    strncpy(state.name, "State: ", sizeof(state.name) - 1);
    state.name[sizeof(state.name) - 1] = '\0'; // Ensure null-termination

    // Initialize the 'value' field
    strncpy(state.value, pcb.processState, sizeof(state.value) - 1);
    state.value[sizeof(state.value) - 1] = '\0'; // Ensure null-termination

    // KeyPointer priority = {"Current Priority: ", pcb.currentPriority};
    KeyPointer lowerBound = {"Lower Bound: ", pcb.lowerBound};
    parseIntoLines(process);
    memory.currentLoc += 3;
    KeyPointer upperBound = {"Upper Bound: ", pcb.upperBound = memory.currentLoc - 1};

    // // allocating the PCB in the PCB's memory space
    memory.PCBs[memory.pcbPointer++] = pID;
    memory.PCBs[memory.pcbPointer++] = PC;
    memory.PCBs[memory.pcbPointer++] = state;
    memory.PCBs[memory.pcbPointer++] = priority;
    memory.PCBs[memory.pcbPointer++] = lowerBound;
    memory.PCBs[memory.pcbPointer++] = upperBound;
}

int main()
{
    memory.currentLoc = 0;
    memory.pcbPointer = 0;
    char proccesses[][20] = {"Program_1.txt", "Program_2.txt", "Program_3.text"};
    int serviceTime[3];
    for (int i = 0; i < 3; i++)
    {
        printf("Please input the service time of program %d:\n", i + 1);
        scanf("%d", &serviceTime[i]);
    }
    int i = 0;

    do
    {
        printf("Clock cycle %d --> %d", i, i + 1);
        for (int j = 0; j < 3; j++)
        {
            if (i == serviceTime[j])
            {
                allocateProcess(proccesses[j], j);
            }
        }
        i++;
    } while (false);

    for (int k = 0; k < 7; k++)
    {
        printf("Name= %s\n", memory.array[k].name);
        printf("Value= %s\n", memory.array[k].value);
        printf("\n");
    }

    return 0;
}
