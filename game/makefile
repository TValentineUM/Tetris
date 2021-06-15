##
# Project Title
#
# @file
# @version 0.1


CXX=clang++
CXXFLAGS=-I -Wall -std=c++17 -g
DEPS = board.hh pieces.hh
OBJ = board.o
MAINOBJ = main.o $(OBJ)

%.o: %.cc $(DEPS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS) -lncurses

clean:
	rm -rf **/*.o




# end
