CFLAGS := -Wall -O2 -mtune=native -std=c99 -Wno-unknown-warning-option -g $(shell pkg-config --cflags ncurses menu)
MFLAGS := -shared -fPIC
INC    := -IcNBT $(INC)
LFLAGS := -LcNBT -lnbt -lz $(shell pkg-config --libs ncurses menu)
DEFINES:= $(DEFINES)
CC     := gcc
BINARY := nbtcurses
DEPS   := main.o print_nbt.o help_window.o nbt_editor.o colors.o

.PHONY: all clean dev clang libnbt

all: libnbt $(BINARY)

dev: clean
	DEFINES="-DDEV" $(MAKE)

libnbt:
	$(MAKE) -C cNBT libnbt.a

main.o: main.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o main.o main.c

print_nbt.o: print_nbt.c print_nbt.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o print_nbt.o print_nbt.c

help_window.o: help_window.c help_window.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o help_window.o help_window.c

nbt_editor.o: nbt_editor.c nbt_editor.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -Wno-implicit-function-declaration -Wno-unused-result -c -o nbt_editor.o nbt_editor.c

colors.o: colors.c colors.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o colors.o colors.c

$(BINARY): $(DEPS)
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -o $(BINARY) $(DEPS) $(LFLAGS)

clean:
	rm -rfv $(BINARY) $(DEPS)

install:
	cp $(BINARY) /usr/bin/$(BINARY)

clang:
	$(MAKE) CC=clang