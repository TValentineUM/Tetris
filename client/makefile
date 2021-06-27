##
# Project Title
#
# @file
# @version 0.1

CXX=clang++
CXXFLAGS=-I -Wall -std=c++17 -g

CLIENT_SERVER_DEPS = ../client-server/client-server.hh
CLIENT_SERVER_OBJ = ../client-server/client-server.o

P2P_DEPS = ../p2p/p2p.hh
P2P_OBJ = ../p2p/p2p.o

GAME_DEPS = ../tetris/tetris_game.hh ../tetris/pieces.hh ../tetris/boomer.hh ../tetris/fasttrack.hh ../tetris/risingtide.hh
GAME_OBJ = ../tetris/tetris_game.o ../tetris/boomer.o ../tetris/fasttrack.o ../tetris/risingtide.o

DEPS =  client.hh $(CLIENT_SERVER_DEPS) $(GAME_DEPS) $(P2P_DEPS) ../server/tprotocol.hh

MAINOBJ = main.o client.o $(CLIENT_SERVER_OBJ) $(GAME_OBJ) $(P2P_OBJ)

%.o: %.cc $(DEPS)
		$(CXX) -c $< -o $@  $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS) -lncurses -lpthread

.PHONY: clean
clean:
	-rm -f main $(MAINOBJ)


# end
