all : server test_client

server : server.o signal_handlers.o SocketEndpoint.o ClientManager.o BlockingMessageQueue.o SessionManager.o Logger.o Colour.o
	g++ -o server server.o signal_handlers.o SocketEndpoint.o ClientManager.o BlockingMessageQueue.o SessionManager.o Logger.o Colour.o -pthread

server.o : src/server.cpp src/signal_handlers.h src/SocketEndpoint.h src/ClientManager.h src/SessionManager.h
	g++ -c src/server.cpp -std=c++14

signal_handlers.o : src/signal_handlers.cpp src/signal_handlers.h
	g++ -c src/signal_handlers.cpp

SocketEndpoint.o : src/SocketEndpoint.cpp src/SocketEndpoint.h
	g++ -c src/SocketEndpoint.cpp -std=c++11

SessionManager.o : src/SessionManager.cpp src/SessionManager.h
	g++ -c src/SessionManager.cpp -std=c++11

BlockingMessageQueue.o : src/BlockingMessageQueue.cpp src/BlockingMessageQueue.h
	g++ -c src/BlockingMessageQueue.cpp -std=c++11

ClientManager.o : src/ClientManager.cpp src/ClientManager.h src/SocketEndpoint.h src/BlockingMessageQueue.h src/SessionManager.h
	g++ -c src/ClientManager.cpp -std=c++14 -pthread

Logger.o : src/log/Logger.cpp src/log/Logger.h
	g++ -c src/log/Logger.cpp -std=c++14

Colour.o : src/log/Colour.cpp src/log/Colour.h
	g++ -c src/log/Colour.cpp -std=c++14


test_client : test_client.o signal_handlers.o
	g++ -o test_client test_client.o signal_handlers.o -pthread

test_client.o : src/test_client.cpp
	g++ -c src/test_client.cpp -std=c++11 -pthread


clean :
	rm -f server server.o
	rm -f test_client test_client.o
	rm -f signal_handlers.o SocketEndpoint.o BlockingMessageQueue.o ClientManager.o SessionManager.o Logger.o Colour.o