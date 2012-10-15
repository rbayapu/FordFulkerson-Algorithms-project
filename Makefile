run: FordFulkerson.o Graph.o
	g++ -o run FordFulkerson.o Graph.o

FordFulkerson.o:FordFulkerson.cpp
	g++ -c FordFulkerson.cpp

Graph.o:Graph.cpp
	g++ -c Graph.cpp
