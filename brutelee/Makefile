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
	./brutelee imm/06-1m.jpg imm/09-1m.jpg -SL imm/02-1m.asf -DL imm/03-1m.asf -T 10
