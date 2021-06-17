#include "server.hh"
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
using namespace std;

void run_server(int portno) {
  int sockfd, newsockfd, clilen, pid;
  struct sockaddr_in serv_addr, cli_addr;

  // Get port from command line
  // Create server socket (AF_INET, SOCK_STREAM)
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)), 0) {
    throw runtime_error("ERROR opening socket");
  }

  // Initialize socket structure
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  // Bind the host address
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    close(sockfd);
    throw runtime_error("Error on binding");
  }

  // Start listening for the clients
  listen(sockfd, QUEUE_SIZE);

  // Infinite loop
  clilen = sizeof(cli_addr);
  vector<thread> current_threads;
  vector<int> sockets_open;
  while (1) {

    // Accept connection form a client
    newsockfd =
        accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

    if (newsockfd < 0) {
      close(sockfd);
      cerr << "Error on accept" << endl;
    } else {
      // Create thread to serve the client
      thread t1(handle_connection, newsockfd);
      // t1.join();
      // close(newsockfd);
      current_threads.push_back(move(t1));
    }
  }
}

void handle_connection(int sock) {
  int n;
  char buffer[256];

  memset(buffer, 0, 256 * sizeof(char));

  if ((n = write(sock, "Enter your player name: ", 25)) < 0) {
    close(sock);
    throw runtime_error("Unable to write to socket");
  }
  if ((n = read(sock, buffer, 255)) < 0) {
    close(sock);
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
    player_sockets.insert({str, sock});
  } else {
    player_sockets.insert({str, sock});
  }
  player_socket_mutex.unlock();

  string player_name = str;
  memset(buffer, 0, MESSAGE_SIZE * sizeof(char));
  sprintf(buffer, "Player Name: %s", player_name.c_str());
  if ((n = write(sock, buffer, MESSAGE_SIZE)) < 0) {
    close(sock);
    throw runtime_error("ERROR writing to socket");
  }

  print_players();

  while (1) {
  }

  close(sock);
}

void print_players() {
  for (const auto &[k, v] : player_sockets) {
    cout << endl;
    cout << "Player: " << k << ", Socket: " << to_string(v) << endl;
  }
}
