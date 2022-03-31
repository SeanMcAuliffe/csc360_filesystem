/* CSC 360 - Programming Assignment 3
* Simple File System
* Sean McAuliffe, V00913346
* April 4, 2022
* PART I: Disk Info 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include "fat_types.h"


// struct stat { 
// dev_t     st_dev;     /* ID of device containing file */ 
// ino_t     st_ino;     /* inode number */ 
// mode_t    st_mode;    /* protection */ 
// nlink_t   st_nlink;   /* number of hard links */ 
// uid_t     st_uid;     /* user ID of owner */ 
// gid_t     st_gid;     /* group ID of owner */ 
// dev_t     st_rdev;    /* device ID (if special file) */ 
// off_t     st_size;    /* total size, in bytes */ 
// blksize_t st_blksize; /* blocksize for file system I/O */ 
// blkcnt_t  st_blocks;  /* number of 512B blocks allocated */ 
// time_t    st_atime;   /* time of last access */ 
// time_t    st_mtime;   /* time of last modification */ 
// time_t    st_ctime;   /* time of last status change */ 
// };

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Must specify a .dmg file location\n");
        return EXIT_FAILURE;
    }
    
    // Get file descriptor of .dmg file
    int fd = open(argv[1], O_RDONLY);
    struct stat buffer;
    int status = fstat(fd, &buffer);
    if (status == -1) {
        printf("fstat error\n");
        return EXIT_FAILURE;
    }

    // Map input .dmg file to virtual memory
    void* address = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
    struct superblock_t* sb = (struct superblock_t*) address;

    uint32_t blockCount = ntohl(sb->file_system_block_count);
    uint16_t blockSize = ntohs(sb->block_size);
    uint32_t fatStartBlock = ntohl(sb->fat_start_block);
    uint32_t fatBlockCount = ntohl(sb->fat_block_count);
    uint32_t rootStartBlock = ntohl(sb->root_dir_start_block);
    uint32_t rootBlockCount = ntohl(sb->root_dir_block_count);

    // Compute number of free, reserved, and allocated blocks
    void* startAddress = address + (fatStartBlock*blockSize);
    void* endAddress = startAddress + (fatBlockCount*blockSize);

    uint16_t available = 0;
    uint16_t reserved = 0;
    uint16_t allocated = 0;
    
    for (void* p = startAddress; p < endAddress; p+=4) {
        uint32_t* status = (uint32_t*) p;
        if (ntohl(*status) == 0) {
            available++;
        } else if (ntohl(*status) == 1) {
            reserved++;
        } else {
            allocated++;
        }
    }

    // Format output
    printf("Super block information:\n");
    printf("Block size: %d\n", blockSize);
    printf("Block count: %d\n", blockCount);
    printf("FAT starts: %d\n", fatStartBlock);
    printf("FAT blocks: %d\n", fatBlockCount);
    printf("Root directory start: %d\n", rootStartBlock);
    printf("Root directory blocks: %d\n", rootBlockCount);
    printf("\nFAT information:\n");
    printf("Free Blocks: %d\n", available);
    printf("Reserved Blocks: %d\n", reserved);
    printf("Allocated Blocks: %d\n", allocated);

    munmap(address,buffer.st_size);
    close(fd);

    return EXIT_SUCCESS; 
}