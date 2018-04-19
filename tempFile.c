// function to take the logical address and obtain the physical address and value stored at that address
void getPage(int logical_address){

    // obtain the page number and offset from the logical address
    int pageNumber = ((logical_address & ADDRESS_MASK)>>8);
    int offset = (logical_address & OFFSET_MASK);

    // first try to get page from TLB
    int frameNumber = -1; // initialized to -1 to tell if it's valid in the conditionals below

    int i;  // look through TLB for a match
    for(i = 0; i < TLB_SIZE; i++){
        if(TLBPageNumber[i] == pageNumber){   // if the TLB index is equal to the page number
            frameNumber = TLBFrameNumber[i];  // then the frame number is extracted
                TLBHits++;                // and the TLBHit counter is incremented
        } // NOTE: Checking for TLB Hit
    }
          // NOTE: TLB miss encountered
    // if the frameNumber was not found
    if(frameNumber == -1){
        int i;   // walk the contents of the page table
        for(i = 0; i < firstAvailablePageTableNumber; i++){
            if(pageTableNumbers[i] == pageNumber){         // if the page is found in those contents
                frameNumber = pageTableFrames[i];          // extract the frameNumber from its twin array
            }
        }
        // NOTE: Page Table Fault Encountered
        if(frameNumber == -1){                     // if the page is not found in those contents
            readFromStore(pageNumber);             // page fault, call to readFromStore to get the frame into physical memory and the page table
            pageFaults++;                          // increment the number of page faults
            frameNumber = firstAvailableFrame - 1;  // and set the frameNumber to the current firstAvailableFrame index
        }
    }

    insertIntoTLB(pageNumber, frameNumber);  // call to function to insert the page number and frame number into the TLB
    value = physicalMemory[frameNumber][offset];  // frame number and offset used to get the signed value stored at that address
    printf("frame number: %d\n", frameNumber);
    printf("offset: %d\n", offset);
    // output the virtual address, physical address and value of the signed char to the console
    printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, (frameNumber << 8) | offset, value);
}
