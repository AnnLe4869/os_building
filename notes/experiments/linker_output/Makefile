GCC = gcc
GCC_FLAGS = -g

LD = gcc
LD_LINKFLAGS +=

SOURCES_C=$(wildcard *.c)
OBJECTS_C=$(patsubst %.c, %.o, $(SOURCES_C))


.PHONY: final

%o: %c
	$(GCC) $(GCC_FLAGS) -c -o $@ $<

final: $(OBJECTS_C)
	$(LD) $(LD_LINKFLAGS) -Wl,-Map=mapper.map -o final $^