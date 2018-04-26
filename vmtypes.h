#ifndef VMTYPES_H_   /* Include guard */
#define VMTYPES_H_

/*
    Defines a Virtual Memory Addressing table that can be
    represented as either a TLB cache or Page Table
*/
typedef struct vmTable_t {
    int *pageNumArr; // page number array
    int *frameNumArr; // frame number array for this
    int *entryAgeArr; // Age of each index
    int length;
    int pageFaultCount;
    int tlbHitCount;
    int tlbMissCount;
} vmTable_t;


/*
    The dramNode is used to form nodes in a linked list
    where each linked list represents phyical memory offset values.
    As with the other structs in this file, it is self-referential, but this
    struct is intended to be used as a component of the dramMatrix below to
    create a sparse matrix.
*/
/* DECIDED NOT TO USE -- Too Complex to Implement, but keeping JIC I want to use/fix later
typedef struct dramNode {
    int row, column,
    int value;
    struct dramNode* rowPtr;
    struct dramNode* colPtr;
} dramNode;
*/
/*
    The matrix component of the physical memory data structure is a struct that contains
    two arrays of dramNode pointers, each pointing to the first element in a row
    or column. The overall matrix is stored in a structure as follows and is
    implemented as an array of linked lists:

    *************               *************************
    * rowList   * ---------->   *           m           *
    *************               *************************
    * columnList*                           |
    *************                           |
          |                                 |
          v                                 v
         *****                       *****************
         *   *                       *               *
         *   *---------------------> *  dramNode     *
         * n *                       *               *
         *   *                       *****************
         *****
*/

/* DECIDED NOT TO USE -- Too Complex to Implement, but keeping JIC I want to use/fix later
typedef struct dramMatrix {
    dramNode** rowList; // rowList is a pointer to the array of rows
    dramNode** columnList; // column list is a pointer to the array of columns.
    int rows, columns; // store the number of rows and columns of the matrix
    int freeFrame; // The first usable frame in physical memory--initially 0
} dramMatrix;
*/

// This function creates a new Virtual Memory Table for
// Logical address referencing -- Can represent either the TLB or Page Table Cache
vmTable_t* createVMtable(int length);

// This function prints contents of the vmTable
void displayTable(vmTable_t** tableToView);

/* DECIDED NOT TO USE -- Too Complex to Implement, but keeping JIC I want to use/fix later
// This function creates a physical memory sparse matrix
dramMatrix* createDRAMmatrix(int frameCount, int frameSize);
*/
// This function frees dynamically allocated memory
void freeVMtable(vmTable_t** table);

// Accepts an int double pointer for creating simulated physical memory space
int** dramAllocate(int frameCount, int blockSize);

// Will free dram memory after usage
void freeDRAM(int*** dblPtrArr, int frameCount);


// 32-Bit masking function to extract page number
int getPageNumber(int mask, int value, int shift);

// 32-Bit masking function to extract page offset
int getOffset(int mask, int value);


#endif // VMTYPES_H_
