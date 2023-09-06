bits 16

section _ENTRY class=CODE   ; tell the nasm to place the stuff below in CODE section

extern _cstart_             ; specify that _cstart_ is defined elsewhere
global entry                ; making the below routine global so to be used by other module

entry:
    cli
    ; setup stack
    mov ax, ds
    mov ss, ax
    mov sp, 0
    mov bp, sp
    sti

    ; expect boot drive in dl, send it as argument to cstart function
    xor dh, dh
    push dx
    call _cstart_

    cli
    hlt