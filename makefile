CLASSES=
CC = gcc
CXXFLAGS= -g -Wall -pthread

all: master bin_adder

master: $(CLASSES)
	$(CC) -o $@  $^ $(CXXFLAGS) $@.c -lm

bin_adder: $(CLASSES)
	$(CC) -o $@ $^ $(CXXFLAGS) $@.c -lm

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM *.tar.gz master bin_adder adder_log
