; `org` tell assembler where we want the code to be loaded in memory
; i.e where the text segment starts
; https://stackoverflow.com/questions/3407023/what-does-org-assembly-instruction-do
; https://stackoverflow.com/questions/8140016/x86-nasm-org-directive-meaning
org 0x7C00
; `bits` tell assembler to emit 16/32/64 bits code - 16 bits required for backward compatibility
bits 16

; print a string to screen
; Params:
;   - ds:si point to string
puts:


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




    ; `hlt` stop CPU from executing (i.e stop) - can be resumed by an interrupt
    hlt

.halt:
    jmp .halt

; `db` write byte to assembled file
; `times` used to repeat an instruction/data a certain number of time
; in this case, used to filled up our program to 510 bytes
; why 510? B/c one section has 512 bytes, and OS requires a signature bytes sequence at the end
; $ refer to beginning of the current line
; $$ refer to the beginning of current section (in this case, our program)
times 510 - ($-$$) db 0

; `dw` write given word-size to assembled value file - word-size for this case is 2 bytes
dw 0AA55h

