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
   
    struct stat imageInfo;
    
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

    FILE* in = fopen(argv[2], "r");
    
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
    uint32_t startRoot = ntohl(sb->root_dir_start_block);
    uint32_t countRoot = ntohl(sb->root_dir_block_count);

    int numberOfBlocks = inputFileSize / blockSize;
   // if (inputFileSize % blocksize
   // Figure out if we need an extra block

    printf("Number of blocks: %d\n", numberOfBlocks);

    return EXIT_SUCCESS;

    
    // FILE* fp = fdopen(fd, "rb");
    // uint8_t fileBlock[blockSize];

    // void* ptrFAT = address + (startFAT*blockSize);
    // void* ptrFDT = address;
    // uint32_t nextBlock = -1;
    // uint32_t* entryFAT = (uint32_t*) ptrFAT;

    // fread(fileBlock, blockSize, 

    // do {
    //     if (count >= fileNumBlocks) {
    //         break;
    //     }
    //     // This should either be EOF or the next block
    //     entryFAT = (uint32_t*) ptrFAT;

    //     // Copy from file block to output file
    //     memcpy(fileBlock, ptrFDT, blockSize);
    //     fwrite(fileBlock, blockSize, 1, out);
        
    //     // If not EOF update pointers to point to next FAT, FDT entry
    //     if (ntohl(*entryFAT) != -1) {
    //         nextBlock = ntohl(*entryFAT);
    //         ptrFDT = address + (nextBlock*blockSize);
    //         ptrFAT = address + (startFAT*blockSize) + (nextBlock*FAT_ENTRY_BYTES);
    //     }


    // } while (*entryFAT != -1);

    // munmap(address, imageInfo.st_size);
    // fclose(fp);
    // fclose(in);
    // close(fd);

    // return EXIT_SUCCESS;
}