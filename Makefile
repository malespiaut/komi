# Makefile for Komi
# Allan Crossman

# Where the graphics and sound files go...
DATAPATH=/usr/local/share/komidata/

# Where to put the binary...
BINPATH=/usr/local/bin/

# Where to put the manpage...
MANPATH=/usr/local/man/man6/

MIXER=-lSDL_mixer
SDL_LIB=$(shell sdl-config --static-libs) $(MIXER)

CFLAGS=-Wall -Wno-long-long -pedantic -std=c99 -O2 \
	$(shell sdl-config --cflags) \
	-DDATAPATH=\"$(DATAPATH)\"

all:	komi

clean:
	-rm komi
	-rm *.o

install:
	install -d $(DATAPATH)
	cp -R komidata/* $(DATAPATH)
	chmod -R 644 $(DATAPATH)
	chmod 777 $(DATAPATH)
	cp komi $(BINPATH)
	chmod 755 $(BINPATH)komi
	install -d $(MANPATH)
	cp komi.6 $(MANPATH)komi.6

komi:	komi.o
	$(CC) $(CFLAGS) komi.o -o komi $(SDL_LIB)

komi.o:	komi.c
