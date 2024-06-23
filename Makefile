CFLAGS=-Wall -O3 -pedantic -I./include/
LIBS=-L./lib/ -lraylib -lm

live: $(wildcard src/*.c) $(wildcard src/*.h)
	$(CC) $(CFLAGS) -o live src/main.c $(LIBS)
