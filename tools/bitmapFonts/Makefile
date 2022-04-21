fontgen: fontgen.o
	cc -Wall -o fontgen fontgen.c

fonts.h: fontgen
	./fontgen > fonts.h

install: fonts.h
	cp ./fonts.h ../../includes
