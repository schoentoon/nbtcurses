CFLAGS := $(CFLAGS) -Wall -O2 -mtune=native -g
MFLAGS := -shared -fPIC
INC    := -IcNBT $(INC)
LFLAGS := -LcNBT -lnbt -lz
DEFINES:= $(DEFINES)
CC     := gcc
BINARY := nbtcurses
DEPS   := main.o

.PHONY: all clean dev clang libnbt

all: libnbt $(BINARY)

dev: clean
	DEFINES="-DDEV" $(MAKE)

libnbt:
	$(MAKE) -C cNBT libnbt.a

main.o: main.c
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -c -o main.o main.c

$(BINARY): $(DEPS)
	$(CC) $(CFLAGS) $(DEFINES) $(INC) -o $(BINARY) $(DEPS) $(LFLAGS)

clean:
	rm -rfv $(BINARY) $(DEPS)

install:
	cp $(BINARY) /usr/bin/$(BINARY)

clang:
	$(MAKE) CC=clang