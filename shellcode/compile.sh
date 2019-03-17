#!/bin/bash
nasm -f elf64 $1.asm
ld -o $1 $1.o
