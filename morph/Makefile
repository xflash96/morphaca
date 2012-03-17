CC=g++
CFLAGS=$(shell pkg-config opencv --cflags) -g -Wall -Wconversion 
CPPFLAGS=$(CFLAGS)
LDFLAGS=$(shell pkg-config opencv --libs) -lm -O3

all: parser.o main.o
	${CC} parser.o main.o ${CPPLAGS} ${LDFLAGS} -o morph
main.o: main.cpp
	${CC} main.cpp ${CPPLAGS} ${LDFLAGS} -c
parser.o: parser.h parser.cpp
	${CC} parser.cpp ${CPPLAGS} ${LDFLAGS} -c
clean:
	rm -f morph *.o
