#include "../client-server/client-server.hh"
#include "../p2p/p2p.hh"
#include "../tetris/tetris_game.hh"
#include "client.hh"

#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <exception>
#include <iostream>
#include <netdb.h> // addrinfo types
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h> // Linux fds
using namespace std;

int main(int argc, char *argv[]) {

  char *hostname = argv[1];
  char *port = argv[2];
  int sockfd = establish_connection(hostname, port);

  start_irc();

  struct pollfd pfds[2];
  pfds[0].fd = 0; // STDIN
  pfds[0].events = POLLIN;

  pfds[1].fd = sockfd;
  pfds[1].events = POLLIN;

  int xMax, yMax;
  getmaxyx(stdscr, yMax, xMax);

  for (;;) {
    move(yMax - 4, 1);
    int poll_count = poll(pfds, 2, -1); // Wait till we have input

    if (poll_count == -1) {
      exit(1);
    } else {
      for (int i = 0; i < 2; i++) {
        if (pfds[i].revents & POLLIN) {
          if (pfds[i].fd == 0) {
            // Standard Input is ready
            send_message(sockfd);
          } else if (pfds[i].fd == sockfd) {
            recieve_message(sockfd);
          }
        }
      }
    }
  }
}
