##
# Project Title
#
# @file
# @version 0.1

CXX=g++
CXXFLAGS=-I -Wall -std=c++17 -g

CLIENT_SERVER_DEPS = ../client-server/client-server.hh
CLIENT_SERVER_OBJ = ../client-server/client-server.o
CLIENT_SERVER_LIB = ../client-server/libcliser.so
CLISER_LINK = -L../client-server/ -Wl,-rpath=../client-server/

P2P_DEPS = ../p2p/p2p.hh
P2P_OBJ = ../p2p/p2p.o
P2P_LIB = ../p2p/libp2p.so
P2P_LINK = -L../p2p/ -Wl,-rpath=../p2p/


GAME_DEPS = ../tetris/tetris_game.hh ../tetris/pieces.hh ../tetris/boomer.hh ../tetris/fasttrack.hh ../tetris/risingtide.hh
GAME_OBJ = ../tetris/tetris_game.o ../tetris/boomer.o ../tetris/fasttrack.o ../tetris/risingtide.o

DEPS =  client.hh $(CLIENT_SERVER_DEPS) $(GAME_DEPS) $(P2P_DEPS) ../server/tprotocol.hh

MAINOBJ = main.o client.o $(GAME_OBJ)

ALL_OBJ = $(MAINOBJ) $(P2P_OBJ) $(CLIENT_SERVER_OBJ)

.PHONY: all
all: p2p cliser main


.PHONY: p2p
p2p: $(DEPS)
	$(CXX) -c $(P2P_OBJ:.o=.cc) -o $(P2P_OBJ) $(CXXFLAGS) -fPIC
	$(CXX) -shared -o $(P2P_LIB) $(P2P_OBJ)

.PHONY: cliser
cliser: $(DEPS)
	$(CXX) -c $(CLIENT_SERVER_OBJ:.o=.cc) -o $(CLIENT_SERVER_OBJ) $(CXXFLAGS) -fPIC
	$(CXX) -shared -o $(CLIENT_SERVER_LIB) $(CLIENT_SERVER_OBJ)


%.o: %.cc $(DEPS)
		$(CXX) -c $< -o $@  $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) $(CLISER_LINK) $(P2P_LINK) -o client $^ $(CXXFLAGS)  -lncurses -lpthread -lp2p -lcliser

.PHONY:
normal: $(ALL_OBJ)
		$(CXX) -o client $^ $(CXXFLAGS) -lncurses -lpthread

.PHONY: clean
clean:
	-rm -f client $(MAINOBJ) $(P2P_OBJ) $(CLIENT_SERVER_OBJ) $(P2P_LIB) $(CLIENT_SERVER_LIB)


# end
