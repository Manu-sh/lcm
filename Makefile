#CC=cc
LDFLAGS=-lgmp
CFLAGS=-O3 -pipe -ffast-math
.PHONY: all clean

all: lcm.c
	$(CC) $(CFLAGS) -o lcm lcm.c $(LDFLAGS)

clean:
	rm -fv lcm
