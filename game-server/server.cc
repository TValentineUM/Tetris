#include "server.hh"
#include "tprotocol.hh"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

void relay(std::string message) {
  tmessage *msg = (tmessage *)malloc(sizeof(tmessage));
  msg->message_type = CHAT;
  strcpy(msg->buffer, message.c_str());
  for (auto &[k, v] : socket_player) {
    if (send(k, msg, sizeof(tmessage), 0) < 0) {
      perror("send");
    };
  }
}

std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] = "0123456789"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

void handle_connection(int sock) {

  // Generating and Regestering Player
  string name = random_string(8);
  player_mutex.lock();
  socket_player.insert({sock, name});
  player_mutex.unlock();

  int nfds, epollfd;
  struct pollfd pfds[1];   // More if you want to monitor more
  pfds[0].fd = sock;       // Socket
  pfds[0].events = POLLIN; // Tell me when ready to read

  for (;;) {
    if (poll(pfds, 2, -1) == -1) {
      perror("poll");
      exit(1);
    }
    if (pfds[0].revents & POLLIN) {
      char *buffer = (char *)malloc(
          sizeof(tmessage)); // Creating an incomming buffer for messages
      int nbytes = recv(pfds[0].fd, buffer, sizeof(tmessage), 0);
      if (nbytes < 0) {
        perror("ERROR reading from socket");
        close(sock);
        return;
      } else if (nbytes == 0) {
        cout << "user on socket " << sock << " disconnected" << endl;
        player_mutex.lock();
        socket_player.erase(sock);
        player_mutex.unlock();
        close(sock);
        return;
      } else {
        cout << "We got some data here " << endl;
        // We have valid data
        tmessage *msg = (tmessage *)buffer;
        switch (msg->message_type) {
        case CHAT: {
          string str(socket_player.at(sock));
          str.append("> ");
          str.append(string(msg->buffer));
          relay(str);
          cout << str << endl;
        } break;
        default:
          break;
        }
      }
    }
  }
}
void run_server(int portno) {
  int sockfd, newsockfd, clilen, pid;
  struct sockaddr_in serv_addr, cli_addr;

  // Get port from command line
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
    newsockfd =
        accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0) {
      close(sockfd);
      cerr << "Error on accept" << endl;
    } else {
      cout << "New Client Connected on Socket: " << newsockfd << endl;
      thread t1(handle_connection, newsockfd);
      current_threads.push_back(move(t1));
    }
  }
}
