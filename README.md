# Lab 4 – Virtual Machine

## Overview

This project implements a *Virtual Machine (VM)* along with:
- A *two-pass assembler* (with label support)
- A *bytecode loader*

The VM executes a custom instruction set supporting arithmetic, control flow, function calls, and stack manipulation.

---

## Directory Structure

. <br>
├── README.md <br>
├── main.c <br>
├── vm.c <br>
├── vm.h <br>
├── stack.c <br>
├── stack.h <br>
├── loader.c <br>
├── loader.h <br>
├── asm.c <br>
└── benchmarks/<br>



---

## Instruction Set (Summary)

| Opcode | Mnemonic | Description |
|------:|----------|-------------|
| 0x01 | PUSH | Push immediate value onto stack |
| 0x02 | POP | Pop top of stack |
| 0x03 | DUP | Duplicate top of stack |
| 0x10 | ADD | Add top two stack values |
| 0x11 | SUB | Subtract top two stack values |
| 0x12 | MUL | Multiply top two stack values |
| 0x13 | DIV | Divide top two stack values |
| 0x14 | CMP | Compare top two values |
| 0x20 | JMP | Unconditional jump |
| 0x21 | JZ | Jump if zero |
| 0x22 | JNZ | Jump if non-zero |
| 0x30 | STORE | Store value to memory |
| 0x31 | LOAD | Load value from memory |
| 0x40 | CALL | Function call |
| 0x41 | RET | Return from function |
| 0xFF | HALT | Stop execution |

---

## Assembler Design

The assembler uses *two passes*:

### Pass 1 – Label Resolution
- Scans the assembly file
- Records label → instruction address mappings
- Increments program counter (PC) for every instruction

### Pass 2 – Code Generation
- Converts mnemonics to opcodes
- Replaces labels with resolved addresses
- Emits final bytecode

---

## Virtual Machine Design

The VM consists of:
- Operand stack
- Return stack (for CALL/RET)
- Program Counter (PC)
- Bytecode memory
- Execution loop (fetch → decode → execute)

Execution continues until a HALT instruction is encountered.

---


## Build Instructions

### Compile Using gcc (Manual Build)

bash
gcc -Wall -Wextra -g main.c vm.c stack.c loader.c -o vm
gcc -Wall -Wextra -g asm.c -o asm

### Running the Program (Manual Build)
bash
./asm benchmarks/program.asm program.bc
./vm program.bc

### Build Instructions Using Makefile
bash
make clean
make all

### Running the Program (Makefile Build)
bash
./asm benchmarks/program.asm program.bc
./vm program.bc
