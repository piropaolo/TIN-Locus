all : server client

server : server.o signal_handlers.o
	g++ -o server server.o signal_handlers.o -pthread

server.o : server.cpp signal_handlers.h
	g++ -c server.cpp -std=c++11 -pthread

client : client.o signal_handlers.o
	g++ -o client client.o signal_handlers.o

client.o : client.cpp
	g++ -c client.cpp -std=c++11

signal_handlers.o : signal_handlers.cpp signal_handlers.h
	g++ -c signal_handlers.cpp

clean :
	rm server server.o
	rm client client.o
	rm signal_handlers.o