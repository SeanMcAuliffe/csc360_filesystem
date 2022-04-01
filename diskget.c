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


int main(int argc, char* argv[]) {

    uint8_t directoryEntry[64] = {0};
    
    uint32_t fileStartingBlock = -1;
    uint32_t fileNumBlocks = -1;
    struct stat imageInfo;
    
    if (argc < 2 ) {
        printf("Must provide a disk image file.\n");
        return EXIT_FAILURE;
    } else if (argc < 3) {
        printf("Must specify a file to be copied.\n");
        return EXIT_FAILURE;
    } else if (argc < 4) {
        printf("Must specify a copy destination.\n");
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDWR);
    int rc = fstat(fd, &imageInfo);

    if (rc == -1) {
        printf("fstat() error, unable to read supplied file.\n");
        return EXIT_FAILURE;
    }

    void* address = mmap(NULL, imageInfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    superblock_t* sb = (struct superblock_t*) address;

    uint16_t blockSize = ntohs(sb->block_size);
    uint32_t startFAT = ntohl(sb->fat_start_block);
    uint32_t startRoot = ntohl(sb->root_dir_start_block);
    uint32_t countRoot = ntohl(sb->root_dir_block_count);

    FILE* out = fopen(argv[3], "wb");
    FILE* fp = fdopen(fd, "rb");
    uint8_t fileBlock[blockSize];

    // Scan to start of root directory
    fseek(fp, (startRoot*blockSize), SEEK_SET);

    // Read through the root directory until we find the specified file
    int numDirEntries = (countRoot * blockSize) / DIR_ENTRY_BYTES;
    int index = 0;

    // Find the starting block of the file
    while (index < numDirEntries) {
        fread(directoryEntry, sizeof(dir_entry_t), 1, fp);
        dir_entry_t* entry = (dir_entry_t*) directoryEntry;
        if (entry->status %2 != 0) {
            if (!strcmp(argv[2]+1, (char*) entry->filename)) {
                fileStartingBlock = ntohl(entry->starting_block);
                fileNumBlocks = ntohl(entry->block_count);
                break;
            }
        }
        index++;
    }

    if (fileStartingBlock == -1) {
        printf("Error: no file could be found at: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    // Pointer to the start of the FAT, pointer to current file block
    void* ptrFAT = address + (startFAT*blockSize) + (fileStartingBlock*FAT_ENTRY_BYTES);
    void* ptrFDT = address + (fileStartingBlock*blockSize);
    uint32_t nextBlock = -1;
    uint32_t* entryFAT = (uint32_t*) ptrFAT;
    int count = 0;

    do {
        if (count >= fileNumBlocks) {
            break;
        }
        // This should either be EOF or the next block
        entryFAT = (uint32_t*) ptrFAT;

        // Copy from file block to output file
        memcpy(fileBlock, ptrFDT, blockSize);
        fwrite(fileBlock, blockSize, 1, out);
        
        // If not EOF update pointers to point to next FAT, FDT entry
        if (ntohl(*entryFAT) != -1) {
            nextBlock = ntohl(*entryFAT);
            ptrFDT = address + (nextBlock*blockSize);
            ptrFAT = address + (startFAT*blockSize) + (nextBlock*FAT_ENTRY_BYTES);
        }

        count++;

    } while (*entryFAT != -1);

    // This is just a sanity check, to make sure that we looped through
    // the correct number of blocks
    if (count != fileNumBlocks) {
        printf("Sync error occurred. Output file may be corrupted.\n");
        printf("Number of blocks in file: %d\n", fileNumBlocks);
        printf("Number of blocks found: %d\n", count);
    }

    munmap(address, imageInfo.st_size);
    fclose(fp);
    fclose(out);
    close(fd);

    return EXIT_SUCCESS;
}