##
# Project Title
#
# @file
# @version 0.1


CXX=clang++
CXXFLAGS=-I -Wall -std=c++17 -g -lpthread
DEPS = tetris_game.hh pieces.hh ../client-client/client.hh
OBJ = tetris_game.o ../client-client/client.o
MAINOBJ = main.o $(OBJ)

%.o: %.cc $(DEPS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS) -lncurses

clean:
	rm -rf **/*.o




# end
