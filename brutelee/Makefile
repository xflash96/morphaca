CC=g++
CFLAGS=$(shell pkg-config opencv --cflags) -g -Wall -O3
CPPFLAGS=$(CFLAGS)
LDFLAGS=$(shell pkg-config opencv --libs) -lm -O3

all: main.o
	${CC} main.o ${CPPFLAGS} ${LDFLAGS} -o brutelee
main.o: main.cpp
	${CC} main.cpp ${CPPFLAGS} ${LDFLAGS} -c
clean:
	rm -f brutelee *.o
install:
	./brutelee imm/01-1m.jpg imm/02-1m.jpg
