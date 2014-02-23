
CC = g++
LD = g++
CCargs = -g -c -Wall
LDargs = -Wall

bin/longitude-townextract: bin/main.o
	$(LD) $(LDargs) -o bin/longitude-townextract bin/main.o

bin/main.o: src/main.cpp
	$(CC) $(CCargs) -o bin/main.o src/main.cpp