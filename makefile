all : server test_client

server : server.o signal_handlers.o SocketEndpoint.o ClientManager.o BlockingMessageQueue.o SessionManager.o
	g++ -o server server.o signal_handlers.o SocketEndpoint.o ClientManager.o BlockingMessageQueue.o SessionManager.o -pthread

server.o : server.cpp signal_handlers.h SocketEndpoint.h ClientManager.h SessionManager.h
	g++ -c server.cpp -std=c++14

signal_handlers.o : signal_handlers.cpp signal_handlers.h
	g++ -c signal_handlers.cpp

SocketEndpoint.o : SocketEndpoint.cpp SocketEndpoint.h
	g++ -c SocketEndpoint.cpp -std=c++11

SessionManager.o : SessionManager.cpp SessionManager.h
	g++ -c SessionManager.cpp -std=c++11

BlockingMessageQueue.o : BlockingMessageQueue.cpp BlockingMessageQueue.h
	g++ -c BlockingMessageQueue.cpp -std=c++11

ClientManager.o : ClientManager.cpp ClientManager.h SocketEndpoint.h BlockingMessageQueue.h SessionManager.h
	g++ -c ClientManager.cpp -std=c++14 -pthread


test_client : test_client.o signal_handlers.o
	g++ -o test_client test_client.o signal_handlers.o -pthread

test_client.o : test_client.cpp
	g++ -c test_client.cpp -std=c++11 -pthread


clean :
	rm -f server server.o
	rm -f test_client test_client.o
	rm -f signal_handlers.o SocketEndpoint.o BlockingMessageQueue.o ClientManager.o SessionManager.o