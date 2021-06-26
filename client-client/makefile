

CXX=clang++
CXXFLAGS=-I -Wall -std=c++17 -g -lncurses -lpthread
DEPS = client.cc
OBJ = client.cc
MAINOBJ = main.o $(OBJ)

%.o: %.cc $(DEPS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm *.o
