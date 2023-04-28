CC = g++
CPPFLAGS = -g -Wall
CXXFLAGS = -std=c++11

main: main.o 
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -o winzigc main.o 

main.o: main.cpp
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c main.cpp

clean: 
	$(RM) winzigc *.o 