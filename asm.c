#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct{
    const char *mnemonic;
    int opcode;
    int has_operand;
}Instr;

Instr table[] = {
    {"PUSH", 1, 1}, {"POP", 2, 0}, {"DUP", 3, 0},
    {"ADD", 16, 0}, {"SUB", 17, 0}, {"MUL", 18, 0},
    {"DIV", 19, 0}, {"CMP", 20, 0},
    {"JMP", 32, 1}, {"JZ", 33, 1}, {"JNZ", 34, 1},
    {"STORE", 48, 1}, {"LOAD", 49, 1},
    {"CALL", 64, 1}, {"RET", 65, 0},
    {"HALT", 255, 0},
    {NULL, 0, 0}
};

int main(int argc, char *argv[]){
     if(argc<3){
        printf("Usage: %s input.asm output.bc\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1],"r");
    FILE *output_file = fopen(argv[2],"w");

    if(!input_file || !output_file){
        perror("file error");
        return 1;
    }

    char mnemonic[32];
    int operand;

    while(fscanf(input_file,"%s",mnemonic)==1){
        int found = 0;
        for(int i=0;table[i].mnemonic;i++){
            if(strcmp(mnemonic,table[i].mnemonic)==0){
                fprintf(output_file,"%d ",table[i].opcode);
                if(table[i].has_operand){
                    fscanf(input_file, "%d",&operand);
                    fprintf(output_file, "%d ", operand);
                }
                found = 1;
                break;
            }
        }

        if(!found){
            printf("Unknown Instruction: %s\n", mnemonic);
            return 1;
        }
    }

    fclose(input_file);
    fclose(output_file);
    return 0;
}