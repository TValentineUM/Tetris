#include "server.hh"
#include <exception>
#include <iostream>
#include <netdb.h> // addrinfo
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
using namespace std;

void run_server() {

  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, new_fd;

  // Loading up address structs
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // USE IPV4 or IPV6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // Fill in my IP

  if (getaddrinfo(NULL, PORTNO, &hints, &res) != 0) {
    throw runtime_error(string(gai_strerror(errno)));
  }

  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <
      0) {
    throw runtime_error("ERROR opening socket");
  }

  // Bind the host address
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
    close(sockfd);
    throw runtime_error("Error on binding");
  }

  // Start listening for the clients
  listen(sockfd, QUEUE_SIZE);

  addr_size = sizeof their_addr;

  while (1) {

    // Accept connection form a client
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size)) <
        0) {
      close(sockfd);
      throw runtime_error("Error on accept");
    } else {
      thread t1(handle_connection, new_fd);
      threads.push_back(move(t1));
    }
  }
}

void handle_connection(int sockfd) {
  int n;
  char buffer[256];

  string message = "Enter your player name: ";
  if ((n = send(sockfd, message.c_str(), message.length(), 0)) < 0) {
    close(sockfd);
    throw runtime_error("Unable to write to socket");
  }

  memset(buffer, 0, MESSAGE_SIZE * sizeof(char));
  if ((n = recv(sockfd, buffer, MESSAGE_SIZE, 0)) < 0) {
    close(sockfd);
    throw runtime_error("Unable to read from socket");
  }

  string str(buffer);
  // Wait for access to the player list
  player_socket_mutex.lock();

  // Insert the new players name
  if (player_sockets.find(string(buffer)) != player_sockets.end()) {
    srand(time(NULL));
    while (player_sockets.find(str) != player_sockets.end()) {
      str = string(buffer);
      str.append(to_string(rand()));
    }
    player_sockets.insert({str, sockfd});
  } else {
    player_sockets.insert({str, sockfd});
  }
  player_socket_mutex.unlock();

  string player_name = str;
  memset(buffer, 0, MESSAGE_SIZE * sizeof(char));
  sprintf(buffer, "Player Name: %s", player_name.c_str());
  message = "Player Name: " + player_name;
  if ((n = send(sockfd, message.c_str(), message.length(), 0)) < 0) {
    close(sockfd);
    // Wait for access to the player list
    player_socket_mutex.lock();
    player_sockets.erase(player_name);
    player_socket_mutex.unlock();
    throw runtime_error("ERROR writing to socket");
  }
  print_players();

  while (1) {
  }

  close(sockfd);
}

void print_players() {

  cout << endl;
  cout << "Players: " << endl;
  for (const auto &[k, v] : player_sockets) {
    cout << "Player: " << k << ", Socket: " << to_string(v) << endl;
  }
}
