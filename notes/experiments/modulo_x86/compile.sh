# g++ -S -masm=intel -fverbose-asm -g -O0 main.c -o main.s
/usr/bin/watcom/binl/wcc -4 -d3 -s -wx -ms -zq -fo main.c
/usr/bin/watcom/binl/wdis -l main.o