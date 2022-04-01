/* CSC 360 - Programming Assignment 3
* Simple File System
* Sean McAuliffe, V00913346
* April 4, 2022
* PART II: Disk List 
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

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("You must provide an input .dmg file.\n");
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);
    struct stat buffer;
    int status = fstat(fd, &buffer);
    if (status == -1) {
        printf("fstat() error, unable to read supplied file.\n");
        return EXIT_FAILURE;
    }

    // Map input .dmg file to virtual memory
    void* address = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
    superblock_t* sb = (struct superblock_t*) address;

    // Scan to beginning of root directory
    void* ptr = (address) + (ntohl(sb->root_dir_start_block) * ntohs(sb->block_size));
    void* endAddress = address + (ntohl(sb->root_dir_start_block) * ntohs(sb->block_size)) +
                                 (ntohl(sb->root_dir_block_count) * ntohs(sb->block_size));

    int available = 0;
    int file = 0;
    int directory = 0;

    while (ptr < endAddress) {
        dir_entry_t* entry = (dir_entry_t*) ptr;
        dir_entry_timedate_t* createTime = &entry->create_time;

        if (entry->status & 0b00000001) {
            available = 1;
        } else {
            available = 0;
        }
        if (entry->status & 0b00000010) {
            file = 1;
        } else {
            file = 0;
        }
        if (entry->status & 0b00000100) {
            directory = 1;
        } else {
            directory = 0;
        }

        if (available == 1) {
            if (file == 1) {
                printf("F ");
            } else if (directory == 1) {
                printf("D ");
            } else {
                printf("Something went wrong.\n");
                return EXIT_FAILURE;
            }

            int padding = 30 - strlen((char*)entry->filename);

            printf("%10d ", ntohl(entry->size));
            printf("%*s%s ", padding, "", entry->filename);
            printf("%04d/%02d/%02d %02d:%02d:%02d\n", ntohs(createTime->year), createTime->month, createTime->day,
                                        createTime->hour, createTime->minute, createTime->second);
        }

        ptr += DIR_ENTRY_BYTES;
    }

    munmap(address, buffer.st_size);
    close(fd);

    return EXIT_SUCCESS;
}