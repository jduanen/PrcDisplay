fontgen: fontgen.c
	cc -Wall -o fontgen fontgen.c

fonts.h: fontgen
	./fontgen > fonts.h

install: fonts.h
	cp ./fonts.h ../../include/

clean:
	rm -f fontgen.o fontgen fonts.h
