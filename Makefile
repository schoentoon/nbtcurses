CFLAGS := -Wall -O2 -mtune=native -std=c99 -g $(shell pkg-config --cflags ncurses menu)
MFLAGS := -shared -fPIC
INC    := -IcNBT -Iinclude $(INC)
LFLAGS := -LcNBT -lnbt -lz $(shell pkg-config --libs ncurses menu)
DEFINES:= $(DEFINES)
CC     := gcc
BINARY := nbtcurses
DEPS   := build/main.o build/print_nbt.o build/help_window.o build/nbt_editor.o build/colors.o build/find_window.o

.PHONY: all clean dev clang libnbt

all: build libnbt bin/$(BINARY)

dev: clean
	DEFINES="-DDEV" $(MAKE)

build:
	-mkdir build bin

libnbt:
	$(MAKE) -C cNBT libnbt.a

build/main.o: src/main.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/main.o src/main.c

build/print_nbt.o: src/print_nbt.c include/print_nbt.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/print_nbt.o src/print_nbt.c

build/help_window.o: src/help_window.c include/help_window.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/help_window.o src/help_window.c


build/nbt_editor.o: src/nbt_editor.c include/nbt_editor.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -Wno-implicit-function-declaration -Wno-unused-result -c -o build/nbt_editor.o src/nbt_editor.c

build/find_window.o: src/find_window.c include/find_window.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/find_window.o src/find_window.c

build/colors.o: src/colors.c include/colors.h
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o build/colors.o src/colors.c

bin/$(BINARY): $(DEPS)
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -o bin/$(BINARY) $(DEPS) $(LFLAGS)

clean:
	rm -rfv build bin

install:
	cp bin/$(BINARY) /usr/bin/$(BINARY)

clang:
	$(MAKE) CC=clang