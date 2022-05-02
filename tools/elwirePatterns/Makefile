seqgen: seqgen.o seqdefs.h
	cc -Wall -o seqgen seqgen.c

sequences.h: seqgen
	./seqgen > sequences.h

install: sequences.h
	cp ./sequences.h ../../include/
