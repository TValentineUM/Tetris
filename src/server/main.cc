#include "server.hh"
using namespace std;

#define PORTNO 42069

int main(int argc, char *argv[]) {
  run_server(PORTNO);
  return 0;
}
