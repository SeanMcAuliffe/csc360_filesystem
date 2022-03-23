#include <stdio.h>
#include <stdlib.h>
#include "fat_types.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Must specify a .dmg file location\n");
        return EXIT_FAILURE;
    }
    FILE* diskImageFile = fopen(argv[1], "r");

    printf("Super block information:\n");
    printf("Block size: %d\n", 0);
    printf("Block count: %d\n", 0);
    printf("FAT starts: %d\n", 0);
    printf("FAT blocks: %d\n", 0);
    printf("Root direction start: %d\n", 0);
    printf("Root directory blocks: %d\n", 0);
    printf("\nFAT information:\n");
    printf("Free Blocks: %d\n", 0);
    printf("Reserved Blocks: %d\n", 0);
    printf("Allocated Blocks: %d\n",0);

    fclose(diskImageFile);
    
    return EXIT_SUCCESS; 
}