##
# Project Title
#
# @file
# @version 0.1


CXX=g++
CXXFLAGS=-I -Wall -std=c++17 -g

P2P_DEPS = ../p2p/p2p.hh
P2P_OBJ = ../p2p/p2p.o

GAME_DEPS = tetris_game.hh pieces.hh boomer.hh fasttrack.hh risingtide.hh
GAME_OBJ = tetris_game.o boomer.o fasttrack.o risingtide.o

DEPS = $(GAME_DEPS) $(P2P_DEPS)

MAINOBJ = main.o $(GAME_OBJ) $(P2P_OBJ)


%.o: %.cc $(DEPS)
	$(CXX) -c $< -o $@  $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS) -lncurses -lpthread

clean:
	rm *.o




# end
