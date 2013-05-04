CFLAGS := $(CFLAGS) -Wall -O2 -mtune=native -std=c99 -g $(shell pkg-config --cflags ncurses menu)
MFLAGS := -shared -fPIC
INC    := -IcNBT $(INC)
LFLAGS := -LcNBT -lnbt -lz $(shell pkg-config --libs ncurses menu)
DEFINES:= $(DEFINES)
CC     := gcc
BINARY := nbtcurses
DEPS   := main.o print_nbt.o

.PHONY: all clean dev clang libnbt

all: libnbt $(BINARY)

dev: clean
	DEFINES="-DDEV" $(MAKE)

libnbt:
	$(MAKE) -C cNBT libnbt.a

main.o: main.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o main.o main.c

print_nbt.o: print_nbt.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o print_nbt.o print_nbt.c

$(BINARY): $(DEPS)
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -o $(BINARY) $(DEPS) $(LFLAGS)

clean:
	rm -rfv $(BINARY) $(DEPS)

install:
	cp $(BINARY) /usr/bin/$(BINARY)

clang:
	$(MAKE) CC=clang