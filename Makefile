CC = g++
CCOPT = -g -c -Wall -O2

mazegen : mazegen.o Edge.o Graph.o Node.o
	${CC} -g -Wall -O2 -o mazegen mazegen.o Edge.o Graph.o Node.o

mazegen.o : mazegen.cpp
	${CC} ${CCOPT} mazegen.cpp

Edge.o : Edge.cpp Edge.h
	${CC} ${CCOPT} Edge.cpp

Graph.o : Graph.cpp Graph.h
	${CC} ${CCOPT} Graph.cpp

Node.o : Node.cpp Node.h
	${CC} ${CCOPT} Node.cpp

clean :
	-rm -f mazegen.o Edge.o Graph.o Node.o

spotless : clean
	-rm -f mazegen
