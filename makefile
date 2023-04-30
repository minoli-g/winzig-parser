CC = g++
CPPFLAGS = -g -Wall
CXXFLAGS = -std=c++17

main: main.o lex.o token.o
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -o winzigc main.o lex.o token.o

main.o: main.cpp
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c main.cpp

lex.o: lex.hpp lex.cpp
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c lex.cpp
	
token.o: token.hpp token.cpp
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c token.cpp

clean: 
	$(RM) winzigc *.o 