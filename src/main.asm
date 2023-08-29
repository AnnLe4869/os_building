; `org` tell assembler where we want the code to be loaded in memory
org 0x7C00
; `bits` tell assembler to emit 16/32/64 bits code - 16 bits required for backward compatibility
bits 16

main:
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

