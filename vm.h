#ifndef VM_H
#define VM_H

#include "stack.h"

#define MEM_SIZE 1024
#define RET_STACK_SIZE 1024

typedef struct{
    Stack stack;
    int *bytecode;
    int pc;
    int running; // is vm running?
    int memory[MEM_SIZE];
    int valid[MEM_SIZE]; // is the current value stored is valid or not
    int ret_stack[RET_STACK_SIZE];
    int rsp;
}VM;

void vm_init(VM *vm,int *bytecode);
void vm_run(VM *vm);

#endif