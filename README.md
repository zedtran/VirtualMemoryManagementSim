# VirtualMemoryManagementSim
A C-Program that simulates Virtual Memory Management based on a text file input of logical addresses which represents sequential instructions with address range 0 thru 2<sup>16</sup> - 1. See the [Project Report](https://github.com/zedtran/VirtualMemoryManagementSim/blob/master/ProjectReport.txt) for more details regarding usage. 

## Overview
This project is the design and implementation of a standalone virtual memory manager, where there is a software-managed TLB. The program is responsible to (1) load a [file](https://github.com/zedtran/VirtualMemoryManagementSim/blob/master/InputFile.txt) containing a list of logical addresses, (2) translate logical addresses into physical addresses for a virtual address space of size 2<sup>16</sup> = 65,536 bytes, and (3) output the value of the byte stored at the translated physical address.

## Information about the Simulated Virtual Memory
#### 16-bit Logical Addresses
The program reads a file containing a list of 32-bit integer numbers, which represent 32-bit logical addresses. It should be noted that the program only deals with 16-bit addresses. Thus, this simulation implements masking for the rightmost 16 bits of each logical address loaded from the file.


|           Page Number             |             Offset                |
|:---------------------------------:|:---------------------------------:|
|           Bits 15 - 8             |           Bits 7 - 0              |


## System Parameters of the Virtual Memory
The page table size is 2<sup>8</sup> bytes; the TLB contains 16 entries. The page size is 2^8^ bytes, which is the same as the frame size. There are a total of 256 frames in the physical memory, meaning that the total physical memory capability is 65,536 bytes (i.e., 256 frames * 256 bytes/frame). The system parameters of the simulated virtual memory is summarized below.

* Page table size: _2<sup>8</sup>_
* Number of TLB entries: _16_
* Page size: _2<sup>8</sup> bytes_
* Frame size: _2<sup>8</sup> bytes_
* Number of frames: _256_
* Physical memory size: _65,536 bytes_

## How Page Faults are Handled
This virtual memory system implements demand paging. The backing store is simulated by a file called “BACKING_STORE.bin”. BACKING_STORE is a binary file of 65,536 bytes. When a page fault occurs, the virtual memory system will perform the following four steps:

* Step 1: read a 256-byte page from the file BACKING_STORE and
* Step 2: store the loaded page frame in the physical memory.
* Step 3: Update the page table
* Step 4: Update the TLB

For example, if a logical address with page number 15 triggers a page fault, your virtual memory system will read in page 15 from the file BACKING_STORE. Then, the loaded page frame is placed in the physical memory. After the page frame is fetched from the disk, the page table and the TLB will be updated accordingly. Subsequent access of page 15 will be referenced by accessing either the TLB or the page table.

## Compilation and Program Execution

- [ ] Compiled and tested using clang compiler: `$ clang -o vm_sim vm_sim.c vmtypes.c`
- [ ] ALSO Compiled and tested using gcc: `$ gcc -o vm_sim vm_sim.c vmtypes.c`
- [ ] Created Makefile with gcc as Constant
- [ ] SIMPLY download this project and execute `make` in command line
- [ ] Execute program with `$ ./vm_sim InputFile.txt` or use an appropriate `[input].txt` file

## Relevant Sources Cited

   * [For how to create smart arrays](https://www.youtube.com/watch?v=QhwFwWpq4dQ)
   * [Dynamic Allocation of 2D Array in struct](https://www.geeksforgeeks.org/dynamically-allocate-2d-array-c/)
   * [For creating physical memory struct](https://www.cs.cmu.edu/~ab/15-123S09/lectures/Lecture%2011%20%20-%20%20Array%20of%20Linked%20Lists.pdf)
   * [More dynamic allocation](https://www.youtube.com/watch?v=t72BzxMAQKs)
   * [Double Pointer Return Type](https://stackoverflow.com/a/4339353)
   * [How to Perform Bit Masking](https://stackoverflow.com/a/10493604)
   * [Proper malloc() error output and exiting](https://stackoverflow.com/a/2574771)
   * [How to use fgets() function](https://stackoverflow.com/a/19609987)
   * [How to measure clock time](https://stackoverflow.com/a/12743207)
   *  <b>NOTE</b>: Other sources were also used to gain understanding of Virtual Memory Management concepts

## Sample Input/Output

#### Upon Running the Program the user will be prompted with the following

```
Welcome to Don's VM Simulator Version 1.0
Number of logical pages: 256
Page size: 256 bytes
Page Table Size: 256
TLB Size: 16 entries
Number of Physical Frames: 256
Physical Memory Size: 65536 bytes
Display All Physical Addresses? [y/n]: n
Choose TLB Replacement Strategy [1: FIFO, 2: LRU]: 1
```
#### User chooses Display Option and TLB Replacement Strategy 

If FIFO is selected, program displays to console:

```
Results Using FIFO Algorithm: 
Number of translated addresses = 1000
Page Faults = 244
Page Fault Rate = 24.400 %
TLB Hits = 51
TLB Hit Rate = 5.100 %
Average time spent retrieving data from backing store: 5.537 millisec

```
If LRU is selected, program displays to console:

```
Welcome to Don's VM Simulator Version 1.0
Number of logical pages: 256
Page size: 256 bytes
Page Table Size: 256
TLB Size: 16 entries
Number of Physical Frames: 256
Physical Memory Size: 65536 bytes
Display All Physical Addresses? [y/n]: n
Choose TLB Replacement Strategy [1: FIFO, 2: LRU]: 2

-----------------------------------------------------------------------------------

Results Using LRU Algorithm: 
Number of translated addresses = 1000
Page Faults = 244
Page Fault Rate = 24.400 %
TLB Hits = 56
TLB Hit Rate = 5.600 %
Average time spent retrieving data from backing store: 4.598 millisec

-----------------------------------------------------------------------------------
```

### Displayed Physical Addresses

If user chooses to display physical addresses, a similar output will immediately precede the results:

```
-----------------------------------------------------------------------------------
Virtual address: 16916		Physical address: 20		Value: 0
Virtual address: 62493		Physical address: 285		Value: 0
Virtual address: 30198		Physical address: 758		Value: 29
Virtual address: 53683		Physical address: 947		Value: 108
Virtual address: 40185		Physical address: 1273		Value: 0
Virtual address: 28781		Physical address: 1389		Value: 0
Virtual address: 24462		Physical address: 1678		Value: 23
Virtual address: 48399		Physical address: 1807		Value: 67
Virtual address: 64815		Physical address: 2095		Value: 75
Virtual address: 18295		Physical address: 2423		Value: -35
Virtual address: 12218		Physical address: 2746		Value: 11
Virtual address: 22760		Physical address: 3048		Value: 0
Virtual address: 57982		Physical address: 3198		Value: 56
Virtual address: 27966		Physical address: 3390		Value: 27
...
-----------------------------------------------------------------------------------
```

