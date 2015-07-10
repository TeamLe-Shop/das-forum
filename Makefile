CC=clang
CFLAGS=-c -Wall -Wno-incompatible-pointer-types-discards-qualifiers
LDFLAGS=
SOURCES=src/*c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dasforum

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -lncurses -ljansson -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

