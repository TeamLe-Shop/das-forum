CC=clang
CFLAGS=-c -Wall -Wno-incompatible-pointer-types-discards-qualifiers
LDFLAGS=
SOURCES=src/*c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dasforum
INSTALL_PREFIX=/Users/user/install
LIBRARIES=lib

all: $(SOURCES) $(EXECUTABLE)

# Dependencies:
# - jansson
install-dependencies:
	# Jansson
	cd build && \
	git clone https://github.com/akheron/jansson.git && \
	cd jansson && \
	autoreconf -i && \
	./configure --prefix=$(INSTALL_PREFIX)&& \
	make && \
	make install

clean:
	sudo rm -r build/jansson

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -lncurses -ljansson \
	     -I$(INSTALL_PREFIX)/include/ -I$(LIBRARIES) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

