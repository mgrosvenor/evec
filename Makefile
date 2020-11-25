CFLAGS= -Wall
LIBS=


.PHONY: all

all: debug

release: CFLAGS += -O3 -DNDEBUG
release: test

debug: CFLAGS += -Werror -g
debug: test

test: test.c evec.h
	$(CC) -o $@ test.c $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f test
