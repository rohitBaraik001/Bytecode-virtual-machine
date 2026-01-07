#include "vm.h"
#include<stdio.h>
#include "loader.h"
#include<stdlib.h>

int main(int argc, char *argv[]){
    if(argc<2){
        printf("Usage: %s <bytecode_file>\n", argv[0]);
        return 1;
    }

    int code_size;
    int *bytecode = load_bytecode(argv[1],&code_size);
    if(!bytecode) return 1;

    VM vm;
    vm_init(&vm,bytecode);
    vm_run(&vm);
    if(vm.stack.sp>=0){
        int result = pop(&vm.stack);
        printf("Result: %d\n",result);
    }
    else{
        printf("Stack empty at the execution\n");
    }

    free(bytecode);
    return 0;
}