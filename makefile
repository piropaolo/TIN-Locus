all : server client

server : server.o signal_handlers.o SocketEndpoint.o ClientManager.o BlockingMessageQueue.o
	g++ -o server server.o signal_handlers.o SocketEndpoint.o ClientManager.o BlockingMessageQueue.o -pthread

server.o : server.cpp signal_handlers.h SocketEndpoint.h ClientManager.h
	g++ -c server.cpp -std=c++14

signal_handlers.o : signal_handlers.cpp signal_handlers.h
	g++ -c signal_handlers.cpp

SocketEndpoint.o : SocketEndpoint.cpp SocketEndpoint.h
	g++ -c SocketEndpoint.cpp -std=c++11

BlockingMessageQueue.o : BlockingMessageQueue.cpp BlockingMessageQueue.h
	g++ -c BlockingMessageQueue.cpp -std=c++11

ClientManager.o : ClientManager.cpp ClientManager.h SocketEndpoint.h BlockingMessageQueue.h
	g++ -c ClientManager.cpp -std=c++14 -pthread


client : client.o signal_handlers.o
	g++ -o client client.o signal_handlers.o -pthread

client.o : client.cpp
	g++ -c client.cpp -std=c++11 -pthread


clean :
	rm -f server server.o
	rm -f client client.o
	rm -f signal_handlers.o SocketEndpoint.o BlockingMessageQueue.o ClientManager.o