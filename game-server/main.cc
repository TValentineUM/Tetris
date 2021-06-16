#include "server.hh"
#include <exception>
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    throw invalid_argument("No port provided");
  } else {
    run_server(atoi(argv[1]));
  }

  return 0;
}
