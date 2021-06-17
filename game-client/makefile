##
# Project Title
#
# @file
# @version 0.1

CXX=clang++
CXXFLAGS=-I -Wall -std=c++17 -g -lncurses -lpthread
DEPS = client.hh ../game-server/tprotocol.hh
OBJ = client.cc
MAINOBJ = main.o $(OBJ)

%.o: %.cc $(DEPS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o client $^ $(CXXFLAGS)

clean:
	rm *.o

# end
