INC=-Iinclude
LIB=lib/libcryptopp.a

all: Test

Test: testProgram.o Crypto.o
	g++ testProgram.o Crypto.o $(LIB) -o test

Crypto.o: Crypto.cpp Crypto.h
	g++ $(INC) -c Crypto.cpp -std=c++11

testProgram.o: testProgram.cpp Crypto.h
	g++ $(INC) -c testProgram.cpp -std=c++11

clean:
	rm -f test Crypto.o testProgram.o