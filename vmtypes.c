#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vmtypes.h"  /* Include the header (not strictly necessary here) */

/*
    Creates a Table that Contains array indexable values for Page
    Number and Frame Number for direct translation lookaside buffer (TLB)
    or page table entry (PTE).

    @Param  {length}       Represents the int length of the constructed internal arrays
    @Return {vmTable_t*}   A pointer to the the created table which will represent either the TLB or Page Table
*/
vmTable_t* createVMtable(int length)
{
    vmTable_t* new_table = malloc(sizeof(vmTable_t));
    new_table->length = length;
    new_table->pageNumArr = malloc(sizeof(int) * length);
    new_table->frameNumArr = malloc(sizeof(int) * length);
    new_table->pageFaultCount = 0;
    new_table->tlbHitCount = 0;
    new_table->tlbMissCount = 0;

    // If there is not enough memory to make call to malloc(), we must free memory
    if(new_table == NULL || new_table->pageNumArr == NULL || new_table->frameNumArr == NULL) {
        printf("Error - Could not allocate a new Virtual Memory Addressing Table!\r\n");
        exit(-1);
    }
    return new_table;
}
/*
    Method to free dynamically allocated memory

    @Param {table} The TLB or Page Table we want to clear from memory
*/
void freeVMtable(vmTable_t** table)
{
    free((*table)->pageNumArr);
    free((*table)->frameNumArr);
    free(table);
}
/*
    This function prints contents of the vmTable

    @Param {tableToView} The TLB or Page Table who's contents we want to view in console
*/
void displayTable(vmTable_t** tableToView)
{
    printf("\n********************* TABLE or BUFFER START ****************************\n ");
    for (int i = 0; i < (*tableToView)->length; i++) {
        printf("Index(%d) := Page Number: %d\tFrame Number: %d\n", i, (*tableToView)->pageNumArr[i], (*tableToView)->frameNumArr[i]);
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
/*
    Creating simulated physical memory space

    @Param  {frameCount} The number of frames we want to represent in physical memory
    @Param  {blockSize}  The number of offsets we want per physical memory frame
    @Return {int**}      The dynamically allocated physical memory space
*/
int** dramAllocate(int frameCount, int blockSize)
{
    int** temp;
    temp = malloc(frameCount * sizeof(int*));
    for(int i = 0; i< frameCount; i++) {
        temp[i] = (int*)malloc(sizeof(int) * blockSize);
        for(int j = 0; j < blockSize; j++) {
            temp[i][j] = 0;
        }
    }
    // If there is not enough memory to make call to malloc(), we must free memory
    if(temp == NULL) {
        printf("Error - Could not allocate a new Physical Memory Matrix using dramAllocate() function!\r\n");
        exit(-1);
    }
    return temp;
}
/*
    Will free dram memory after usage

    @Param {dblPtrArr}  The physical memory we want to clear
    @Param {frameCount} The number of frames in the specified physical memory
*/
void freeDRAM(int*** dblPtrArr, int frameCount)
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

    @Param {mask}   The int masking value we will use to perform AND operation w.r.t. value
    @Param {value}  The int value we wish to mask
    @Param {shift}  The relative number of bits we want to shift right after the bitwise operation
    @Return {int}   The int representation for Page Number
*/
int getPageNumber(int mask, int value, int shift) {
   return ((value & mask)>>shift);
}

/*
    32-Bit masking function to extract physical memory offset
    This function assumes a high order page number and
    a low order page offset

    @Param {mask}   The int masking value we will use to perform AND operation w.r.t. value
    @Param {value}  The int value we wish to mask
    @Return {int}   The int representation for physical memory offset
*/
int getOffset(int mask, int value) {
   return value & mask;
}
