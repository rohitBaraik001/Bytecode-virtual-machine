#ifndef STACK_H
#define STACK_H

#define STACK_SIZE 1024

typedef struct {
	int data[STACK_SIZE];
	int sp;
}Stack;

void init_stack(Stack *s);
void push(Stack *s,int value);
int pop(Stack *s);
int peek(Stack *s);

#endif

