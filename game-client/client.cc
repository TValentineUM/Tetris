#include "../game-server/tprotocol.hh"
#include "client.hh"
#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <exception>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <regex>
#include <sstream>
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

tmessage parse_message(string str) {
  // Determining if its a command
  if (str.at(0) == '!') {
    istringstream ss(str);
    string command;
    ss >> command; // Getting the first word
    tmessage msg;
    strcpy(msg.buffer, str.c_str());
    if (regex_match(command, CMD_LEADERBOARD)) {
      msg.message_type = LEADERBOARD;
    } else if (regex_match(command, CMD_PLAYERS)) {
      msg.message_type = PLAYER;
    } else if (regex_match(command, CMD_PLAYERSTATS)) {
      msg.message_type = PLAYERSTATS;
    } else if (regex_match(command, CMD_NICKNAME)) {
      msg.message_type = NICKNAME;
    } else if (regex_match(command, CMD_BATTLE)) {
      msg.message_type = BATTLE;
    } else if (regex_match(command, CMD_QUICK)) {
      msg.message_type = QUICKPLAY;
    } else if (regex_match(command, CMD_CHILL)) {
      msg.message_type = CHILL;
    } else if (regex_match(command, CMD_GO)) {
      msg.message_type = GO;
    } else {
      throw invalid_argument("Unable to parse command");
    }
    return msg;
  } else {
    tmessage msg;
    msg.message_type = CHAT;
    strcpy(msg.buffer, str.c_str());
    return msg;
  }
}
