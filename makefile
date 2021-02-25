CLASSES=
CC = gcc
CXXFLAGS= -g -Wall -pthread

all: master bin_adder

master: $(CLASSES)
	$(CC) -o $@  $^ $(CXXFLAGS) $@.c

bin_adder: $(CLASSES)
	$(CC) -o $@ $^ $(CXXFLAGS) $@.c

clean:
	rm -rf *.o *~ *.gch *.swp *.dSYM *.tar.gz master bin_adder