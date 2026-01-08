#include "vm.h"
#include<stdio.h>

#define OP_PUSH 0x01
#define OP_POP 0x02
#define OP_DUP 0x03
#define OP_ADD 0x10
#define OP_SUB 0x11
#define OP_MUL 0x12
#define OP_DIV 0x13
#define OP_CMP 0x14
#define OP_HALT 0xff
#define OP_JMP  0x20
#define OP_JZ   0x21
#define OP_JNZ  0x22
#define OP_STORE 0x30
#define OP_LOAD  0x31
#define OP_CALL 0x40
#define OP_RET 0x41


void vm_init(VM *vm,int *bytecode){
    init_stack(&vm->stack);
    vm->bytecode = bytecode;
    vm->pc = 0;
    vm->running = 1;
    vm->rsp = -1;
    vm->instruction_count = 0;
    for(int i=0;i<MEM_SIZE;i++){
        vm->memory[i] = 0; //clear the memory
        vm->valid[i] = 0; //nothing is valid initialy
    }
}

void vm_run(VM *vm){
    while(vm->running){
        int instruction = vm->bytecode[vm->pc++];
        vm->instruction_count++;
        switch(instruction){
            case OP_PUSH:{
                int value = vm->bytecode[vm->pc++];
                push(&vm->stack,value);
                break;
            }
            case OP_POP: {
                pop(&vm->stack);
                break;
            }
            case OP_DUP:{
                int value = peek(&vm->stack);
                push(&vm->stack,value);
                break;
            }
            case OP_ADD:{
                int b = pop(&vm->stack);
                int a = pop(&vm->stack);
                push(&vm->stack,a+b);
                break;
            }
            case OP_SUB:{
                int b = pop(&vm->stack);
                int a = pop(&vm->stack);
                push(&vm->stack,a-b);
                break;
            }
            case OP_MUL:{
                int b = pop(&vm->stack);
                int a = pop(&vm->stack);
                push(&vm->stack,a*b);
                break;
            }
            case OP_DIV:{
                int b = pop(&vm->stack);
                int a = pop(&vm->stack);
                if(b==0){
                    printf("Runtime error: division by zero\n");
                    vm->running = 0;
                    break;
                }
                push(&vm->stack,a/b);
                break;
            }
            case OP_CMP:{
                int b = pop(&vm->stack);
                int a = pop(&vm->stack);
                if(a<b) push(&vm->stack,1);
                else push(&vm->stack,0);
                break;
            }
            case OP_HALT:{
                vm->running = 0;
                break;
            }
            case OP_JMP:{
                int address = vm->bytecode[vm->pc++];
                vm->pc = address;
                break;
            }
            case OP_JZ:{
                int address = vm->bytecode[vm->pc++];
                int condition = pop(&vm->stack);
                if(condition==0) vm->pc = address;
                break;
            }
            case OP_JNZ:{
                int address = vm->bytecode[vm->pc++];
                int condition = pop(&vm->stack);
                if(condition!=0) vm->pc = address;
                break;
            }
            case OP_STORE:{
                int index = vm->bytecode[vm->pc++];
                int top = pop(&vm->stack);
                if(index>=MEM_SIZE){
                    printf("Memory Overflwo\n");
                    vm->running = 0;
                    break;
                }
                vm->memory[index] = top;
                vm->valid[index] = 1;
                break;
            }
            case OP_LOAD:{
                int index = vm->bytecode[vm->pc++];
                if(vm->valid[index]==0){
                    printf("your program trying to load the invalid data\n");
                    vm->running = 0;
                    break;
                }
                int value = vm->memory[index];
                push(&vm->stack,value);
                break;
            }
            case OP_CALL:{
                int address = vm->bytecode[vm->pc++];
                if(vm->rsp>=RET_STACK_SIZE){
                    printf("Return Stack Overflow\n");
                    vm->running = 0;
                    break;
                }
                vm->ret_stack[++vm->rsp] = vm->pc;
                vm->pc = address;
                break;
            }
            case OP_RET:{
                if(vm->rsp < 0){
                    printf("Return Stack Underflow\n");
                    vm->running = 0;
                    break;
                }
                vm->pc = vm->ret_stack[vm->rsp--];
                break;
            }
            default:
               printf("Unknown Instruction %d\n",instruction);
               vm->running = 0;
        }
    }
}