#include "loader.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_CODE_SIZE 1024

int* load_bytecode(const char *filename, int *out_size){
    FILE *fp = fopen(filename,"r");
    if(!fp){
        perror("failed to open bytecode file");
        return NULL;
    }

    int *bytecode = malloc(sizeof(int)*MAX_CODE_SIZE);
    if(!bytecode){
        perror("Memory Allocation Failed");
        fclose(fp);
        return NULL;
    }

    int value;
    int count = 0;
    while(fscanf(fp,"%d",&value)==1){
        if(count>=MAX_CODE_SIZE){
            printf("Bytecode too large\n");
            break;
        }
        bytecode[count++] = value;
    }

    fclose(fp);
    *out_size = count;
    return bytecode;
}