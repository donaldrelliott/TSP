PROG := FINAL
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

FINAL = main.o threads.o christofides.o

OBJS = main.o threads.o christofides.o

SRCS = main.cpp threads.cpp christofides.cpp

all: ${PROG}

FINAL: ${FINAL}
		${CXX} ${SRCS} -o tsp -lpthread

${OBJS}: ${SRCS}
		${CXX} ${CXXFLAGS} -c $(@:.o=.cpp)

clean:
		rm -f $(PROG) *.o *~ main
