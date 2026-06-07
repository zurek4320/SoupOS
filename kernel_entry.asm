section .text
[bits 32]

global _start
extern main

_start:
    call main

hang:
    jmp hang
