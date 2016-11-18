CC = g++ 
DEBUG = -g -O0 -pedantic-errors 
CFLAGS = -Wall -std=c++11 -c $(DEBUG) 
LFLAGS = -Wall $(DEBUG) 

all: shell

shell: process.o shell.o 
	$(CC) $(LFLAGS) -o shell shell.o process.o 

process.o: process.cpp shell.h
	$(CC) $(CFLAGS) process.cpp

shell.o: shell.cpp shell.h
	$(CC) $(CFLAGS) shell.cpp 

.PHONY: 
	clean 

clean: 
	rm -rf *.o 
	rm -rf shell
