CC=clang
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=src/*c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dasforum

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -lncurses -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

