##
# Project Title
#
# @file
# @version 0.1

CXX=g++
CXXFLAGS=-I -Wall -std=c++17 -g
DEPS = server.hh tprotocol.hh
OBJ = server.o
MAINOBJ = main.o $(OBJ)

%.o: %.cc $(DEPS)
	$(CXX) -c $< -o $@  $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o server $^ $(CXXFLAGS) -lpthread

.PHONY: clean
clean:
	-rm -f server $(MAINOBJ)
# end
