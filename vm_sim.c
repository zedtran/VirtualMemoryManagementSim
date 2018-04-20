#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>
#include "vmtypes.h"

/*
    Souces Cited:
    (1) How to use fgets() function: https://stackoverflow.com/a/19609987

    Compiled using clang compiler:
        <-> "$ clang -o vm_sim vm_sim.c vmtypes.c"
        <-> "$ ./vm_sim InputFile.txt"

    //TODO:
    (1) Compile message states that we are attempting to free memory that was not allocated at the end of program execution.
        Need to find out if it's coming from freeVMtable() or freeDRAM() function. 
*/

#define FRAME_SIZE 256          // Size of each frame
#define TOTAL_FRAME_COUNT 256   // Total number of frames in physical memory
#define PAGE_MASK  0xFF00       // Masks everything but the page number
#define OFFSET_MASK  0xFF       // Masks everything but the offset
#define SHIFT 8                 // Amount to shift when bitmasking
#define TLB_SIZE 16             // size of the TLB
#define PAGE_TABLE_SIZE 256     // size of the page table
#define MAX_ADDR_LEN 6          // The number of characters to read for each line from input file. strlen(value) + 1
#define PAGE_READ_SIZE 256      // Number of bytes to read

typedef enum { false = 0, true = !false } bool; // Simple true or false boolean -- unsure if I want to use yet

/*
    vmTable is a generic struct defined type that is used to implement any
    number of caches for logical address translations. It contains the following components:
        (1) int *pageNumArr; // page number array
        (2) int *frameNumArr; // frame number array for this
        (3) int length; // The table size
        (4) int pageFaultCount; // Represents number of page faults, if implemeneted as Page Table
        (5) int tlbHitCount; // Represents TLB hit count
        (6) int tlbMissCount; // Represents TLB misses
*/

vmTable_t* tlbTable; // The TLB Structure
vmTable_t* pageTable; // The Page Table
int** dram; // Physical Memory


int nextTLBentry = 0; // Tracks the next available index of entry into the TLB
int nextPage = 0;  // Tracks the next available page in the page table
int nextFrame = 0; // Tracks the next available frame TLB or Page Table

// input file and backing store
FILE* address_file;
FILE* backing_store;

// how we store reads from input file
char addressReadBuffer[MAX_ADDR_LEN];
int virtual_addr;
int page_number;
int offset_number;

// the buffer containing reads from backing store
signed char fileReadBuffer[PAGE_READ_SIZE];

// the translatedValue of the byte (signed char) in memory
signed char translatedValue;

// Function Prototypes
void translateAddress();
void readFromStore(int pageNumber);
void tlbFIFOinsert(int pageNumber, int frameNumber);
// void tlbLRUinsert(int pageNumber, int frameNumber);


// main opens necessary files and calls on translateAddress for every entry in the addresses file
int main(int argc, char *argv[])
{
    tlbTable = createVMtable(TLB_SIZE); // The TLB Structure
    pageTable = createVMtable(PAGE_TABLE_SIZE); // The Page Table
    dram = dramAllocate(TOTAL_FRAME_COUNT, FRAME_SIZE); // Physical Memory

    int translationCount = 0;

    // perform basic error checking
    if (argc != 2) {
        fprintf(stderr,"Usage: ./vm_sim [input file]\n");
        return -1;
    }

    // open the file containing the backing store
    backing_store = fopen("BACKING_STORE.bin", "rb");

    if (backing_store == NULL) {
        fprintf(stderr, "Error opening BACKING_STORE.bin %s\n","BACKING_STORE.bin");
        return -1;
    }

    // open the file containing the logical addresses
    address_file = fopen(argv[1], "r");

    if (address_file == NULL) {
        fprintf(stderr, "Error opening InputFile.txt %s\n",argv[1]);
        return -1;
    }

    // read through the input file and output each virtual address
    while (fgets(addressReadBuffer, MAX_ADDR_LEN, address_file) != NULL) {
        virtual_addr = atoi(addressReadBuffer); // converting from ascii to int

        // 32-bit masking function to extract page number
        page_number = getPageNumber(PAGE_MASK, virtual_addr, SHIFT);

        // 32-bit masking function to extract page offset
        offset_number = getOffset(OFFSET_MASK, virtual_addr);

        // get the physical address and translatedValue stored at that address
        translateAddress();
        translationCount++;  // increment the number of translated addresses
    }
    /*
    Welcome to Your Don's VM Simulator Version 1.0
    Number of logical pages: 256
    Page size: 256 bytes
    Page table size: 256
    TLB size: 16 entries
    Number of physical frames: 256
    Physical memory size: 65,536 bytes
    Display Physical Addresses? [yes or no] Yes
    Choose TLB Replacement Strategy [1: FIFO, 2: LRU] 1
    Virtual address: 28720; Physical address: 48; Value: 0
    Virtual address: 24976; Physical address: 400; Value: 0
    Virtual address: 24112; Physical address: 560; Value: 0
    Virtual address: 49454; Physical address: 814; Value: 48
    ……
    Page fault rate: 48%
    TLB hit rate: 7.5%
    Check the results in the outputfile: vm_sim_output.txt
    */

    // calculate and print out the stats
    printf("\n\nNumber of translated addresses = %d\n", translationCount);
    double pfRate = (double) pageTable->pageFaultCount / (double)translationCount;
    double TLBRate = (double) tlbTable->tlbHitCount / (double)translationCount;

    printf("Page Faults = %d\n", pageTable->pageFaultCount);
    printf("Page Fault Rate = %.3f\n",pfRate);
    printf("TLB Hits = %d\n", tlbTable->tlbHitCount);
    printf("TLB Hit Rate = %.3f\n\n", TLBRate);

    // close the input file and backing store
    fclose(address_file);
    fclose(backing_store);

    // NOTE: REMEMBER TO FREE DYNAMICALLY ALLOCATED MEMORY
    freeVMtable(&tlbTable);
    freeVMtable(&pageTable);
    freeDRAM(&dram, TOTAL_FRAME_COUNT);
    return 0;
}


/*
    This function takes the virtual address and translates
    into physical addressing, and retrieves the translatedValue stored
 */
void translateAddress()
{
    // First try to get page from TLB
    int frame_number = -1; // Assigning -1 to frame_number means invalid initially

    /*
        Look through the TLB to see if memory page exists.
        If found, extract frame number and increment hit count
    */
    for (int i = 0; i < tlbTable->length; i++) {
        if (tlbTable->pageNumArr[i] == page_number) {
            frame_number = tlbTable->frameNumArr[i];
            tlbTable->tlbHitCount++;
        }
    }

    /*
        We now need to see if there was a TLB miss.
        If so, increment the tlb miss count and go through
        the page table to see if the page number exists. If not on page table,
        read secondary storage (dram) and increment page table fault count.
    */
    if (frame_number == -1) {
        // walk the contents of the page table
        for(int i = 0; i < nextPage; i++){
            if(pageTable->pageNumArr[i] == page_number){  // if the page is found in those contents
                frame_number = pageTable->frameNumArr[i]; // extract the frame_number from its twin array
                // NOTE: See if we can break here legally so we don't continue iterating if we don't have to
            }
        }
        // NOTE: Page Table Fault Encountered
        if(frame_number == -1) {  // if the page is not found in those contents
            // page fault, call to readFromStore to get the frame into physical memory and the page table
            readFromStore(page_number);
            pageTable->pageFaultCount++;   // increment the number of page faults
            frame_number = nextFrame - 1;  // and set the frame_number to the current nextFrame index
        }
    }

    // Call function to insert the page number and frame number into the TLB
    tlbFIFOinsert(page_number, frame_number);

    // Frame number and offset used to get the signed translatedValue stored at that address
    translatedValue = dram[frame_number][offset_number];

    printf("\nFrame Number: %d; Offset: %d; ", frame_number, offset_number);

    // output the virtual address, physical address and translatedValue of the signed char to the console
    printf("Virtual address: %d Physical address: %d Value: %d", virtual_addr, (frame_number << SHIFT) | offset_number, translatedValue);
}

// Function to read from the backing store and bring the frame into physical memory and the page table
void readFromStore(int pageNumber)
{
    // first seek to byte PAGE_READ_SIZE in the backing store
    // SEEK_SET in fseek() seeks from the beginning of the file
    if (fseek(backing_store, pageNumber * PAGE_READ_SIZE, SEEK_SET) != 0) {
        fprintf(stderr, "Error seeking in backing store\n");
    }

    // now read PAGE_READ_SIZE bytes from the backing store to the fileReadBuffer
    if (fread(fileReadBuffer, sizeof(signed char), PAGE_READ_SIZE, backing_store) == 0) {
        fprintf(stderr, "Error reading from backing store\n");
    }

    // Load the bits into the first available frame in the physical memory 2D array
    for (int i = 0; i < PAGE_READ_SIZE; i++) {
        dram[nextFrame][i] = fileReadBuffer[i];
    }

    // Then we want to load the frame number into the page table in the next frame
    pageTable->pageNumArr[nextPage] = pageNumber;
    pageTable->frameNumArr[nextPage] = nextFrame;

    // increment the counters that track the next available frames
    nextFrame++;
    nextPage++;
}


// Function to insert a page number and frame number into the TLB with a FIFO replacement
void tlbFIFOinsert(int pageNumber, int frameNumber)
{

    int i;  // if it's already in the TLB, break
    for(i = 0; i < nextTLBentry; i++){
        if(tlbTable->pageNumArr[i] == pageNumber){
            break;
        }
    }

    // if the number of entries is equal to the index
    if(i == nextTLBentry){
        if(nextTLBentry < TLB_SIZE){  // IF TLB Buffer has open positions
            tlbTable->pageNumArr[nextTLBentry] = pageNumber;    // insert the page and frame on the end
            tlbTable->frameNumArr[nextTLBentry] = frameNumber;
        }
        else{ // No room in TLB Buffer

            // Replace the last TLB entry with our new entry
            tlbTable->pageNumArr[nextTLBentry - 1] = pageNumber;
            tlbTable->frameNumArr[nextTLBentry - 1] = frameNumber;

            // Then shift up all the TLB entries by 1 so we can maintain FIFO order
            for(i = 0; i < TLB_SIZE - 1; i++){
                tlbTable->pageNumArr[i] = tlbTable->pageNumArr[i + 1];
                tlbTable->frameNumArr[i] = tlbTable->frameNumArr[i + 1];
            }
        }
    }

    // if the index is not equal to the number of entries
    else{
        for(i = i; i < nextTLBentry - 1; i++){      // iterate through up to one less than the number of entries
            // Move everything over in the arrays
            tlbTable->pageNumArr[i] = tlbTable->pageNumArr[i + 1];
            tlbTable->frameNumArr[i] = tlbTable->frameNumArr[i + 1];
        }
        if(nextTLBentry < TLB_SIZE){                // if there is still room in the array, put the page and frame on the end
             // insert the page and frame on the end
            tlbTable->pageNumArr[nextTLBentry] = pageNumber;
            tlbTable->frameNumArr[nextTLBentry] = frameNumber;

        }
        else{  // otherwise put the page and frame on the number of entries - 1
            tlbTable->pageNumArr[nextTLBentry - 1] = pageNumber;
            tlbTable->frameNumArr[nextTLBentry - 1] = frameNumber;
        }
    }
    if(nextTLBentry < TLB_SIZE) { // if there is still room in the arrays, increment the number of entries
        nextTLBentry++;
    }
}
