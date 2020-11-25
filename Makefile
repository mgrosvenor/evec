CFLAGS= -Wall
LIBS=


.PHONY: all

all: debug

release: CFLAGS += -O3 -DNDEBUG
release: demo1 demo2 demo3

debug: CFLAGS += -Werror -g
debug: test demo1 demo2 demo3

test: test.c evec.h 
	$(CC) -o $@ test.c $(CFLAGS) $(LIBS)

demo1: demo1.c evec.h 
	$(CC) -o $@ demo1.c $(CFLAGS) $(LIBS)
	
demo2: demo2.c evec.h 
	$(CC) -o $@ demo2.c $(CFLAGS) $(LIBS)
	
demo3: demo3.c evec.h 
	$(CC) -o $@ demo3.c $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f test demo1 demo2 demo3
