; `org` tell assembler where we want the code to be loaded in memory
; i.e where the text segment starts
; https://stackoverflow.com/questions/3407023/what-does-org-assembly-instruction-do
; https://stackoverflow.com/questions/8140016/x86-nasm-org-directive-meaning
org 0x7C00
; `bits` tell assembler to emit 16/32/64 bits code - 16 bits required for backward compatibility
bits 16

; macros for end of line
%define ENDL 0x0d, 0x0a

; beginning execure from here
start:  
    jmp main


; print a string to screen
; Params:
;   - ds:si point to string
puts:
    ; save registers that we will modify
    push si
    push ax

.loop:
    lodsb   ; load a byte at address DS:(E)SI into AL
    or al, al   ; verify if the character is null
    jz .done    ; if null, jump to label .donef

    ; call BIOS interrupt code 0x10, AH=0E
    mov ah, 0x0e
    mov bh, 0x00
    int 0x10

    jmp .loop   ; else, continue the loop

.done:
    pop ax
    pop si
    ret     ; return to caller






main:
    ; the code segment is setup by the BIOS that point to physical mem location 0
    ; however, some BIOS jump using 0x7C00:0
    ; correct way is 0:0x7C00

    ; setup data and code segment
    mov ax, 0
    mov ds, ax
    mov es, ax
    ; setup stack segment
    mov ss, ax
    ; stack pointer
    ; we choose this location so that when stack grow, it won't hit the text segment
    mov sp, 0x7C00

    ; call function to print message
    mov si, msg_hello
    call puts

    ; `hlt` stop CPU from executing (i.e stop) - can be resumed by an interrupt
    hlt


msg_hello: db 'Hello World', ENDL, 0

; `db` write byte to assembled file
; `times` used to repeat an instruction/data a certain number of time
; in this case, used to filled up our program to 510 bytes
; why 510? B/c one section has 512 bytes, and OS requires a signature bytes sequence at the end
; $ refer to beginning of the current line
; $$ refer to the beginning of current section (in this case, our program)
times 510 - ($-$$) db 0

; `dw` write given word-size to assembled value file - word-size for this case is 2 bytes
dw 0AA55h

