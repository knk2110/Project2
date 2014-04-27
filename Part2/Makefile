CC = gcc
CFLAGS = -g -Wall

probe: probe.o
	gcc -o probe probe.o	

probe.o: probe.c probe.h
	gcc -msse4 -c probe.c

.PHONY: clean
clean:
	rm -f *.o *~ a.out core probe

