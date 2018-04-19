#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vmtable.h"  /* Include the header (not strictly necessary here) */



/*
    Creates a Table that Contains array indexable values for Page
    Number and Frame Number for direct translation lookaside buffer (TLB)
    or page table entry (PTE).
    @Param length Represents the length of the constructed internal arrays
    Traverse with for loop using the following syntax:
}
*/
vmTable* createVMtable(int length)
{
    vmTable* new_table = malloc(sizeof(vmTable));
    new_table->length = length;
    new_table->pageNumArr = malloc(sizeof(int) * length);
    new_table->frameNumArr = malloc(sizeof(int) * length);
    new_table->pageFaultCount = 0;
    new_table->tlbHitCount = 0;
    new_table->tlbMissCount = 0;

    // If there is not enough memory to make call to malloc(), we must free memory
    if(new_table == NULL || new_table->pageNumArr == NULL || new_table->frameNumArr == NULL) {
        printf("Error - Could not allocate a new Virtual Memory Addressing Table!\n");
        free(new_table);
    }

    return new_table;
}

// Method to free dynamically allocated memory
void freeVMtable(vmTable** table)
{
    for (int i = 0; i < table->length; i++) {
        free(table->(*pageNumArr)[i]);
        free(table->(*frameNumArr)[i]);
    }
    free(table->pageNumArr);
    free(table->frameNumArr);
    free(table);
}

// This function prints contents of the vmTable
void displayTable(vmTable** tableToView)
{
    printf("\n********************* TABLE or BUFFER START ****************************\n ");
    for (int i = 0; i < tableToView->length; i++) {
        printf("Index(%d) := Page Number: %d\tFrame Number: %d\n", i, tableToView->pageNumArr[i], tableToView->frameNumArr[i]);
    }
    printf("\n********************* TABLE or BUFFER END ***************************\n ");
}

/* DECIDED NOT TO USE -- Too Complex to Implement, but keeping JIC I want to use/fix later

// Initializes dramMatrix
dramMatrix* createDRAMmatrix(int frameCount, int frameSize)
{
    dramMatrix* M = malloc(sizeof(dramMatrix));
    M->rowList = malloc(frameCount*sizeof(dramNode*));
    M->columnList = malloc(frameSize*sizeof(dramNode*));
    M->rows = frameCount;
    M->columns = frameSize;
    M->freeFrame = 0;

    // If there is not enough memory to make call to malloc(), we must free memory
    if(M == NULL || M->rowList == NULL || M->columnList == NULL) {
        printf("Error - Could not allocate a new Physical Memory Matrix!\n");
        free(M);
    }

    return M;
}

*/

// Accepts an int double pointer for creating simulated physical memory space
int** dramAllocate(int frameCount, int blockSize)
{
    int** temp;
    temp = malloc(frameCountSize * sizeof(int*));
    for(int i = 0; i< frameCountSize; i++) {
        temp[i] = (int*)malloc(sizeof(int) * blockSize);
        for(int j = 0; j < c; j++) {
            temp[i][j] = 0;
        }
    }
    // If there is not enough memory to make call to malloc(), we must free memory
    if(temp == NULL) {
        printf("Error - Could not allocate a new Physical Memory Matrix using dramAllocate() function!\n");
        free(temp);
    }
    return temp;
}

// Will free dram memory after usage
void free_dram(int*** dblPtrArr, int frameCount)
{
  for (int i = 0; i < frameCount; i++) {
      free((*dblPtrArr)[i]);
  }
  free(*dblPtrArr);
}

/*
    32-Bit masking function to extract page number
    This function assumes a high order page number and
    a low order page offset
*/
int getPageNumber(int mask, int value, int shift) {
   return ((value & mask)>>shift);
}

/*
    32-Bit masking function to extract physical memory offset
    This function assumes a high order page number and
    a low order page offset
*/
int getOffset(int mask, int value) {
   return value & mask;
}
