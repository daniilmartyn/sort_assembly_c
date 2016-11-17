SYMFORMAT=dwarf
FORMAT=elf

all: sorttest

sorttest: asort.o csort.o main.o
	gcc -m32 -g -o sorttest asort.o csort.o main.o


asort.o: asort.asm
	nasm -f $(FORMAT) -g -F $(SYMFORMAT) asort.asm

csort.o: csort.c sort.h
	gcc -m32 -g -c -Wall -std=c99 -pedantic csort.c

main.o: main.c sort.h
	gcc -m32 -g -c -Wall -std=c99 -pedantic main.c

install: sorttest
	cp sorttest sorttest-mine sort.h main.c main.o asort.asm csort.c Makefile testfile.txt /cs_Share/class/cs261/sort

clean:
	rm sorttest *.o *~


