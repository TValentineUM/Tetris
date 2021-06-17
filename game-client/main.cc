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

  initscr();
  cbreak();
  echo();
  curs_set(0);

  int xMax, yMax;
  getmaxyx(stdscr, yMax, xMax);
  WINDOW *chat_window = newwin(yMax - 10, xMax, 0, 0);
  WINDOW *text_window = newwin(5, xMax, yMax - 5, 0);

  box(chat_window, 0, 0);
  box(text_window, 0, 0);
  refresh();
  wmove(text_window, 1, 1);

  wrefresh(chat_window);
  wrefresh(text_window);

  thread t2(display_chat, chat_window);
  thread t1(send_messages, sockfd, text_window);
  thread t3(consume_chat, sockfd);
  endwin();

  t1.join();
  t2.join();
  t3.join();
  return 0;
}
