CC=gcc
CFLAGS=$(shell pkg-config opencv --cflags) -g -Wall -Wconversion -O1
CPPFLAGS=$(CFLAGS)
LDFLAGS=$(shell pkg-config opencv --libs) -lm -O1

all: morph

clean:
	rm -f morph *.o
