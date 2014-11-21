.PHONY: prog

CFLAGS=-ansi -Wall -std=c++11 -c -g

all: prog

prog: parser.o defines.o macroTable.o
	g++ -g main.cpp parser.o MacroTable.o defines.o -o tradutor.out

defines.o: defines.h defines.cpp
	g++ $(CFLAGS) defines.cpp

macroTable.o: MacroTable.h MacroTable.cpp
	g++ $(CFLAGS) MacroTable.cpp 

parser.o: parser.h parser.cpp
	g++ $(CFLAGS) parser.cpp 

clean:
	rm -f *.pre
	rm -f *.mcr
	rm -f *.o
	rm -f *.out
