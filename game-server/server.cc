#include "server.hh"
#include "tprotocol.hh"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <netdb.h>
#include <poll.h>
#include <sstream>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

void relay(std::string message) {
  tmessage *msg = (tmessage *)malloc(sizeof(tmessage));
  msg->message_type = CHAT;
  strcpy(msg->buffer, message.c_str());
  for (auto &[k, v] : player_list) {
    if (lockf(k, F_ULOCK, 0) < 0) {
      perror("lockf");
    }
    if (send(k, msg, sizeof(tmessage), 0) < 0) {
      perror("send");
    };
    if (lockf(k, F_ULOCK, 0) < 0) {
      perror("lockf");
    }
  }
}

void send_multiple(int sock_fd, std::vector<string> msgs) {
  if (lockf(sock_fd, F_LOCK, 0) < 0) {
    perror("lockf");
  }
  for (auto msg : msgs) {
    send_chat(sock_fd, msg);
  }
  if (lockf(sock_fd, F_ULOCK, 0) < 0) {
    perror("lockf");
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
  string name = random_string(NAMESIZE);
  player_data info;
  strcpy(info.name, name.c_str());
  player_list_mutex.lock();
  player_list.emplace(sock, info);
  player_list_mutex.unlock();

  int nfds, epollfd;
  struct pollfd pfds[1];   // More if you want to monitor more
  pfds[0].fd = sock;       // Socket
  pfds[0].events = POLLIN; // Tell me when ready to read

  for (;;) {
    if (poll(pfds, 1, -1) == -1) {
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
        player_list_mutex.lock();
        player_list.erase(sock);
        player_list_mutex.unlock();
        close(sock);
        return;
      } else {
        tmessage *msg = decode_message(buffer);
        handle_message(msg, sock);
      }
      free(buffer);
    }
  }
}

tmessage *decode_message(char *buffer) {
  tmessage *msg = (tmessage *)buffer;
  msg->message_type = (tmessage_t)ntohl((int32_t)msg->message_type);
  msg->arg1 = ntohl(msg->arg1);
  msg->arg2 = ntohl(msg->arg2);
  msg->arg3 = ntohl(msg->arg3);
  return msg;
}

void handle_message(tmessage *msg, int sock) {

  switch (msg->message_type) {
  case CHAT: {
    string str;
    auto iter = player_list.find(sock);
    if (iter != player_list.end()) {
      str = iter->second.name;
    }
    str.append("> ");
    str.append(string(msg->buffer));
    relay(str);
    break;
  }
  case NICKNAME: {

    player_list_mutex.lock();
    auto iter = player_list.find(sock);
    if (iter != player_list.end()) {
      strcpy(iter->second.name, msg->buffer);
    }
    player_list_mutex.unlock();
    string str;
    str = "Updated Nickname to: " + string(msg->buffer);
    send_chat(sock, str);
    break;
  }
  case PLAYERS: {
    string str("Players Online: ");
    player_list_mutex.lock();
    for (auto &[k, v] : player_list) {
      str.append(v.name);
      str.append("; ");
    }
    player_list_mutex.unlock();
    send_chat(sock, str);
    break;
  }
  case LEADERBOARD: {
    cout << "ARG1 is " << (msg->arg1) << endl;
    auto scores = get_leaderboard(msg->arg1);
    send_multiple(sock, scores);
    break;
  }
  default:
    cout << "Another Message Type: " << msg->message_type << endl;
    cout << msg->buffer << endl;
    break;
  }
}

vector<string> get_leaderboard(int game) {
  // Create a local copy to read from, basically taking a snapshot of the
  // scores
  player_list_mutex.lock();
  auto local_plist(player_list);
  player_list_mutex.unlock();

  vector<player_data> plist;
  for (auto &[k, v] : player_list) {
    plist.push_back(v);
  }

  auto sort_order = [game](player_data &p1, player_data &p2) -> bool {
    switch (game) {
    case RISING_TIDE:
      return get<0>(p1.rising_games) > get<0>(p2.rising_games);
    case BOOMER:
      return get<0>(p1.boomer_games) > get<0>(p2.boomer_games);
    case FAST_TRACK:
      return get<0>(p1.fasttrack_games) > get<0>(p2.fasttrack_games);
    case CHILLER:
      return p1.chill > p2.chill;
    }
  };

  sort(plist.begin(), plist.end(), sort_order);
  auto formatted_out = [](string p1, string p2, string p3, string p4) {
    stringstream ss;
    ss << left << setw(NAMESIZE + 4) << setfill(' ') << p1;
    ss << left << setw(NAMESIZE + 4) << setfill(' ') << p2;
    ss << left << setw(NAMESIZE + 4) << setfill(' ') << p3;
    ss << left << setw(NAMESIZE + 4) << setfill(' ') << p4;
    return ss.str();
  };

  vector<string> scores;

  scores.push_back(formatted_out("Player", "Score", "Win", "Loss"));

  for (auto iter = plist.begin();
       iter != plist.end() || iter < plist.begin() + 2; iter++) {
    switch (game) {
    case RISING_TIDE:
      scores.push_back(formatted_out(iter->name, to_string(iter->rising),
                                     to_string(get<0>(iter->rising_games)),
                                     to_string(get<1>(iter->rising_games))));
      break;
    case BOOMER:
      scores.push_back(formatted_out(iter->name, to_string(iter->boomer),
                                     to_string(get<0>(iter->boomer_games)),
                                     to_string(get<1>(iter->boomer_games))));
      break;
    case FAST_TRACK:
      scores.push_back(formatted_out(iter->name, to_string(iter->fasttrack),
                                     to_string(get<0>(iter->fasttrack_games)),
                                     to_string(get<1>(iter->fasttrack_games))));
      break;
    case CHILLER:
      scores.push_back(
          formatted_out(iter->name, to_string(iter->rising), "N/A", "N/A"));
      break;
    }
  }

  return scores;
}

void send_chat(int sock, string str) {
  tmessage *msg = (tmessage *)malloc(sizeof(tmessage));
  msg->message_type = CHAT;
  strcpy(msg->buffer, str.c_str());
  if (send(sock, msg, sizeof(tmessage), 0) < 0) {
    perror("send");
  };
  free(msg);
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
      close(newsockfd);
      cerr << "Error on accept" << endl;
    } else {
      struct sockaddr_in new_connection;
      socklen_t addrlen = sizeof(new_connection);
      if (getsockname(newsockfd, (struct sockaddr *)&new_connection, &addrlen) <
          0) {
        perror("getsockname");
      } else {
        struct sockaddr_in addr_in;
        socklen_t addrlen = sizeof(addr_in);
        if (getpeername(newsockfd, (struct sockaddr *)&addr_in, &addrlen) < 0) {
          perror("getpeername");
        }
        cout << "New Client with IP: " << inet_ntoa(addr_in.sin_addr)
             << " On port: " << new_connection.sin_port
             << " with socket number " << newsockfd << endl;
      }
      thread t1(handle_connection, newsockfd);
      current_threads.push_back(move(t1));
    }
  }
}
