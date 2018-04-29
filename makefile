all : server client

server : server.o
	g++ -o server server.o

server.o : server.cpp
	g++ -c server.cpp -std=c++11

client : client.o
	g++ -o client client.o

client.o : client.cpp
	g++ -c client.cpp

clean :
	rm server server.o
	rm client client.o