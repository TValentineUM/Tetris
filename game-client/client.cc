#include "../game-server/tprotocol.hh"
#include "client.hh"
#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <exception>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h> // Linux fds
using namespace std;

int establish_connection(char *hostname, char *port) {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int rv;

  // Getting a list of sockets we can connect to
  if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      continue;
    }
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      continue;
    }
    break;
  }

  if (p == NULL) {
    throw std::runtime_error("Failed to connect to server");
  }

  freeaddrinfo(servinfo);
  return sockfd;
}

void consume_chat(int sockfd) {
  int rv;

  struct pollfd pfds[1];   // More if you want to monitor more
  pfds[0].fd = sockfd;     // Standard input
  pfds[0].events = POLLIN; // Tell me when ready to read

  for (;;) {
    if (poll(pfds, 2, -1) == -1) {
      perror("poll");
      exit(1);
    }
    if (pfds[0].revents & POLLIN) {

      char *buffer = (char *)malloc(sizeof(tmessage));
      if ((rv = recv(pfds[0].fd, (char *)buffer, sizeof(tmessage), 0)) < 0) {
        endwin();
        perror("ERROR reading from socket");
        close(sockfd);
        exit(1);
      }
      tmessage *msg = (tmessage *)buffer;

      cout << "GOT A MESSAGE DARNIT" << msg->buffer << endl;
      switch (msg->message_type) {
      case CHAT: {
        message_lock.lock();
        chat_messages.push_back(string(msg->buffer));
        message_lock.unlock();
      }
      default:
        break;
      }
      free(buffer);
    }
  }
}

void display_chat(WINDOW *win) {
  int current_size = chat_messages.size();
  bool messages_flag = false;
  while (1) {
    if (chat_messages.size() > 20) {
      message_lock.lock();
      chat_messages.erase(chat_messages.begin(),
                          chat_messages.begin() + (chat_messages.size() - 20));
      message_lock.unlock();
      messages_flag = true;
    }
    if (chat_messages.size() != current_size || messages_flag) {
      wclear(win);
      box(win, 0, 0);
      message_lock.lock();
      for (int i = 0; i < chat_messages.size(); i++) {
        mvwprintw(win, i + 1, 1, chat_messages[i].c_str());
      }
      message_lock.unlock();
      current_size = chat_messages.size();
      messages_flag = false;
    }
    wrefresh(win);
    this_thread::sleep_for(chrono::milliseconds(100));
  }
}

void send_messages(int sockfd, WINDOW *win) {
  int xMax, yMax;
  getmaxyx(stdscr, yMax, xMax);
  char buffer[255];
  int rv;
  while (1) {
    move(yMax - 4, 1);
    memset(buffer, 0, 255 * sizeof(char));
    getstr(buffer);
    wclear(win);
    box(win, 0, 0);
    tmessage msg;
    msg.message_type = CHAT;
    strcpy(msg.buffer, buffer);
    if (send(sockfd, (char *)&msg, sizeof(tmessage), 0) < 0) {
      perror("ERROR writing to socket");
      close(sockfd);
      endwin();
      exit(1);
    }
    wrefresh(win);
  }
}
