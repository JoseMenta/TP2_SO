

include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_OTHERS=$(wildcard **/*c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_OTHERS=$(SOURCES_OTHERS:.c=.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_OTHERS) _loader.o
	$(LD)  -T sampleCodeModule.ld _loader.o  $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_OTHERS) -o ../$(MODULE)
	$(LD)  -T sampleCodeModule.ld _loader.o $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_OTHERS) -o ../0000-sampleCodeModule.elf --oformat=elf64-x86-64

_loader.o:_loader.c
	$(GCC) $(GCCFLAGS) -c _loader.c

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c  $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o asm/*.o **/*.o

.PHONY: all clean print

