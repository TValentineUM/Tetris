##
# Project Title
#
# @file
# @version 0.1

CXX=clang++
CXXFLAGS=-I -Wall -std=c++17 -g -pthread -fsanitize=address
DEPS = server.hh tprotocol.hh
OBJ = server.cc
MAINOBJ = main.o $(OBJ)

%.o: %.cc $(DEPS)
	$(CXX) -c $< -o $@  $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm *.o

# end
