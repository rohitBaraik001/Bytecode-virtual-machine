#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

void init_stack(Stack *s){
  s -> sp = -1;
}

void push(Stack *s, int value){
  if(s->sp >= STACK_SIZE - 1 ){
    printf("Stack overflow");
    exit(1);
  }
  // s ->data[++s ->sp] = value;
  s->sp = s->sp + 1;
  s->data[s->sp] = value;
}

int pop(){
  if(s -> sp < 0){
    printf("stack Underflow");
    exit(1);
  }
  return s->data[s->sp--];
}

int peek(Stack *s){
    if(s -> sp < 0){
        printf("Stack is empty\n");
        exit(1);
    }
    return s->data[s->sp];
}
