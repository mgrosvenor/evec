CFLAGS= -Wall
LIBS=


.PHONY: all

all: debug

release: CFLAGS += -O3 -DNDEBUG
release: test

debug: CFLAGS += -pedantic -Werror -Wno-language-extension-token -std=c99
debug: test

test: test.c evec.h
	$(CC) -o $@ test.c $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f test
