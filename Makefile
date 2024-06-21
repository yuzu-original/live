PKGS=sdl2
CFLAGS=-Wall -O3 -pedantic `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)` -lm

live: $(wildcard src/*.c) $(wildcard src/*.h)
	$(CC) $(CFLAGS) -o live src/main.c $(LIBS)
