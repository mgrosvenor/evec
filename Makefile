CFLAGS= -Wall
LIBS=


.PHONY: all

all: debug

release: CFLAGS += -O3 -DNDEBUG
release: test

debug: CFLAGS += -pedantic -Werror -Wno-language-extension-token -std=gnu11
debug: test

test: test.c ev.h
	$(CC) -o $@ test.c $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f test