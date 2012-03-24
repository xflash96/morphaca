CC=g++
CFLAGS=$(shell pkg-config opencv --cflags) -g -Wall -Wconversion 
CPPFLAGS=$(CFLAGS)
LDFLAGS=$(shell pkg-config opencv --libs) -lm -O3

all: parser.o morph_lib.o main.o
	${CC} parser.o morph_lib.o main.o ${CPPLAGS} ${LDFLAGS} -o morph
main.o: main.cpp
	${CC} main.cpp ${CPPLAGS} ${LDFLAGS} -c
parser.o: parser.h parser.cpp
	${CC} parser.cpp ${CPPLAGS} ${LDFLAGS} -c
morph_lib.o: morph_lib.h morph_lib.cpp
	${CC} morph_lib.cpp ${CPPLAGS} ${LDFLAGS} -c
clean:
	rm -f morph *.o
