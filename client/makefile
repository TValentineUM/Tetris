##
# Project Title
#
# @file
# @version 0.1

CXX=clang++
CXXFLAGS=-I -Wall -std=c++17 -g

CLIENT_SERVER_DEPS = ../client-server/client-server.hh
CLIENT_SERVER_OBJ = ../client-server/client-server.o

DEPS =  $(CLIENT_SERVER_DEPS) ../game-server/tprotocol.hh

MAINOBJ = main.o $(CLIENT_SERVER_OBJ)

%.o: %.cc $(DEPS)
	$(CXX) -c $< -o $@  $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS) -lncurses -lpthread

clean:
	rm *.o

# end
