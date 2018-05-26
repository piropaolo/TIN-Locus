#INC=-I/media/sf_UbuntuShared/TIN/crypto/include
INC=-Iinclude
LIB=lib/libcryptopp.a

all: Test

Test: testProgram.o Crypto.o
	g++ testProgram.o Crypto.o $(LIB) -o test

Crypto.o: Crypto.cpp Crypto.h
	g++ $(INC) -c Crypto.cpp

testProgram.o: testProgram.cpp Crypto.h
	g++ $(INC) -c testProgram.cpp

clean:
	rm -f test Crypto.o testProgram.o