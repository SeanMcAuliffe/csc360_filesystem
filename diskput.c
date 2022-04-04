/* CSC 360 - Programming Assignment 3
* Simple File System
* Sean McAuliffe, V00913346
* April 4, 2022
* PART III: Disk Get 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include "fat_types.h"
#include <string.h>
#include <ctype.h>

uint32_t FAT_ENTRIES[8056];
uint32_t NUM_ENTRIES = 0;
uint8_t NEW_ENTRY[4];


/* Function which marks a specified FAT entry with a given value. This is 
*   used when inserting new files into the .dmg, so that the chain of next
*   blocks can be added to the FAT.
*/
void markFAT(void* fatStart, uint32_t entry, uint16_t blockSize, uint32_t value) {
    void* p = fatStart + (entry*FAT_ENTRY_BYTES);
    *(uint32_t*)p = htonl(value);
}


/* Function which scans the FAT and returns the index of the next
*    unallocated entry.
*/
int getNextFreeBlock(void* fatStart, uint32_t fatBlockCount, uint16_t blockSize){
    void* p = fatStart;
    void* fatEnd = fatStart + (fatBlockCount*blockSize);
    uint32_t currentBlock = 0;
    while (p < fatEnd) {
        //printf("Inspecting FAT entry no. %d\n", currentBlock);
        uint32_t* status = (uint32_t*) p;
        //printf("Status is: %d\n", ntohl(*status));
        if (ntohl(*status) == 0) {
            markFAT(fatStart, currentBlock, blockSize, 0xEE);
            return currentBlock;
        }
        currentBlock++;
        p += FAT_ENTRY_BYTES;
    }
    return -1;
}


int main(int argc, char* argv[]) {
   
    struct stat imageInfo;
    struct stat fileInfo;
    uint32_t directory_entry = -1;
    uint32_t starting_block = -1;
    uint32_t index_count = 0;

    
    if (argc < 2 ) {
        printf("Must provide a disk image file.\n");
        return EXIT_FAILURE;
    } else if (argc < 3) {
        printf("Must specify a source file.\n");
        return EXIT_FAILURE;
    } else if (argc < 4) {
        printf("Must specify a copy file path.\n");
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDWR);
    int rc = fstat(fd, &imageInfo);

    if (rc == -1) {
        printf("fstat() error, unable to read supplied file.\n");
        return EXIT_FAILURE;
    }

    int input_filed = open(argv[2], O_RDONLY);
    rc = fstat(input_filed, &fileInfo);
    FILE* in = fdopen(input_filed, "rb");
    
    if (in == NULL) {
        printf("Error: Could not open file: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    // Get the size of the input file
    uint32_t inputFileSize = 0;
    fseek(in, 0, SEEK_END);
    inputFileSize = ftell(in);
    fseek(in, 0, SEEK_SET);

    void* address = mmap(NULL, imageInfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    superblock_t* sb = (struct superblock_t*) address;

    uint16_t blockSize = ntohs(sb->block_size);
    uint32_t startFAT = ntohl(sb->fat_start_block);
    uint32_t countFAT = ntohl(sb->fat_block_count);
    uint32_t startRoot = ntohl(sb->root_dir_start_block);
    uint32_t countRoot = ntohl(sb->root_dir_block_count);

    // Determine the required number of blocks
    int numberOfRequiredBlocks = inputFileSize / blockSize;
    if (inputFileSize % blockSize != 0) {
        numberOfRequiredBlocks++;
    }

    // Determine if we have enough free blocks
    uint16_t numberOfFreeBlocks = 0;
    void* ptrFAT = address + (startFAT*blockSize);

    while (ptrFAT < address + (startFAT*blockSize) + (countFAT*blockSize)) {
        uint32_t* status = (uint32_t*) ptrFAT;
        if (ntohl(*status) == 0) {
            numberOfFreeBlocks++;
        }
        ptrFAT += FAT_ENTRY_BYTES;
    }

    if (numberOfFreeBlocks < numberOfRequiredBlocks) {
        printf("Error: Not enough free space.\n");
        return EXIT_FAILURE;
    }

    // Helpful markers
    void* startDIR_ptr = address + (startRoot*blockSize);
    void* endDIR_ptr = startDIR_ptr + (countRoot*blockSize);
    void* startFAT_ptr = address + (startFAT*blockSize);

    // Is there space for a new directory entry?
    void* temp_ptr = startDIR_ptr;
    index_count = 0;
    while (temp_ptr < endDIR_ptr) {
        dir_entry_t* entry = (dir_entry_t*) temp_ptr;
        if (!(entry->status & 0b00000001)) {
            directory_entry = index_count;
            break;
        } 
        index_count++;
        temp_ptr += DIR_ENTRY_BYTES;
    }

    // If there is no space, exit
    if (directory_entry == -1){
        printf("Could not find space in the specified directory.\n");
        return EXIT_FAILURE;
    }

    // Otherwise, insert the new Directory Entry
    starting_block = getNextFreeBlock(startFAT_ptr, countFAT, blockSize);
    dir_entry_t newDirectoryEntry;
    dir_entry_timedate_t createTime;
    dir_entry_timedate_t modifyTime;
    createTime.year = htons(2022);
    createTime.month = 04;
    createTime.day = 05;
    createTime.hour = 12;
    createTime.minute = 30;
    createTime.second = 00;
    modifyTime.year = htons(2022);
    modifyTime.month = 04;
    modifyTime.day = 05;
    modifyTime.hour = 12;
    modifyTime.minute = 30;
    modifyTime.second = 00;
    newDirectoryEntry.status = 0b00000011;
    newDirectoryEntry.starting_block = htonl(starting_block);
    newDirectoryEntry.block_count = htonl(numberOfRequiredBlocks);
    newDirectoryEntry.size = htonl(fileInfo.st_size);
    newDirectoryEntry.create_time = createTime;
    newDirectoryEntry.modify_time = modifyTime;
    strcpy((char*)newDirectoryEntry.filename, argv[3]+1);
    for (int j = 0; j < 6; j++) {
        newDirectoryEntry.unused[j] = 0xFF;
    }

    temp_ptr = startDIR_ptr + (directory_entry*DIR_ENTRY_BYTES);
    memcpy(temp_ptr, &newDirectoryEntry, sizeof(newDirectoryEntry));

    // Copy the input file into the .dmg file block by block
    int loop = 0;
    int freeBlockNumber = -1;
    uint8_t fileBuffer[blockSize];
    void* ptrFDT = address + (startRoot*blockSize);
    ptrFAT = address + (startFAT*blockSize);

    for (int i = 0; i < numberOfRequiredBlocks; i++) {
        rc = fread(fileBuffer, blockSize, 1, in);
        if (loop == 0) {
            freeBlockNumber = starting_block;
            loop++;
        } else {
            freeBlockNumber = getNextFreeBlock(ptrFAT, countFAT, blockSize);
        }
        FAT_ENTRIES[NUM_ENTRIES] = freeBlockNumber;
        NUM_ENTRIES++;
        if (freeBlockNumber == -1) {
            printf("Something went wrong. Could not find free block in FAT.\n");
            return EXIT_FAILURE;
        }
        ptrFDT = address + (freeBlockNumber*blockSize);
        memcpy(ptrFDT, fileBuffer, blockSize);

        freeBlockNumber = -1;
    }

    // Mark the next block at each FAT entry
    for (int k = 0; k < NUM_ENTRIES - 1; k++) {
        markFAT(startFAT_ptr, FAT_ENTRIES[k], blockSize, FAT_ENTRIES[k+1]);
    }
    // Mark End of File in FAT
    markFAT(startFAT_ptr, FAT_ENTRIES[NUM_ENTRIES-1], blockSize, -1);

    return EXIT_SUCCESS;
}