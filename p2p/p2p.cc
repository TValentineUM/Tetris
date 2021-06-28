#include "../server/tprotocol.hh"
#include "p2p.hh"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
using namespace std;

vector<pair<string, string>> decode_hostnames(string str) {

  vector<pair<string, string>> hostnames;
  if (str.size() != 0) {
    stringstream ss(str);
    string temp;
    while (ss >> temp) {
      string ip = temp.substr(0, temp.find(':'));
      string port = temp.substr(temp.find(':') + 1, temp.size());
      hostnames.push_back({ip, port});
    }
  }
  return hostnames;
}

void communicate_state(int sock_fd, gamestate &state,
                       vector<pair<string, string>> ips, bool &terminate) {

  termination_flag = &terminate;
  struct addrinfo hints, *servinfo, *p;
  int new_sock_fd;
  int rv;
  int numbytes;

  // Your IP and Port will be the last element

  auto local = ips.back();
  ips.pop_back();

  // Open Sockets to send each of the peers
  vector<pair<int, struct sockaddr_in>> broadcast_sockets;
  for (auto &[ip, portno] : ips) {
    struct sockaddr_in si_other;

    if ((new_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      perror("socket");
      return;
    }

    memset(&si_other, '\0', sizeof(sockaddr_in));
    si_other.sin_family = AF_INET; // IPV4
    si_other.sin_port = stoi(portno);
    if (inet_aton(ip.c_str(), &si_other.sin_addr) == 0) {
      close(new_sock_fd);
      fprintf(stderr, "inet_aton() failed\n");
      exit(1);
    }

    broadcast_sockets.push_back({new_sock_fd, si_other});
  }

  if ((new_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("socket");
  }
  // Bind to the local recieving port
  struct sockaddr_in myself, other;
  memset(&myself, 0, sizeof(sockaddr_in));
  myself.sin_family = AF_INET;
  myself.sin_port = stoi(get<1>(local));
  myself.sin_addr.s_addr = INADDR_ANY;

  if (bind(new_sock_fd, (struct sockaddr *)&myself, sizeof(myself)) == -1) {
    close(new_sock_fd);
    perror("bind");
  }

  thread t1(recieve_state, std::ref(state), new_sock_fd);
  thread t2(broadcast_state, sock_fd, std::ref(state), broadcast_sockets);

  t1.join();
  t2.join();
  close(new_sock_fd);
}
void recieve_state(gamestate &state, int sock_fd) {

  struct timeval read_timeout;
  read_timeout.tv_sec = 0;
  read_timeout.tv_usec = 10;
  setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
             sizeof read_timeout);
  while (1) {
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;
    char *buffer = (char *)malloc(sizeof(update_msg));

    int num_bytes;

    if ((num_bytes = recvfrom(sock_fd, buffer, sizeof(update_msg), 0,
                              (struct sockaddr *)&their_addr, &addr_len)) ==
        -1) {
      if (*termination_flag) {
        return;
      }
      // perror("recvfrom");
      // return;
    } else {
      update_msg *msg = (update_msg *)buffer;
      decode_state(msg);
      if (state.players.find(msg->player_no) != state.players.end()) {
        auto player = state.players.find(msg->player_no);
        player->second.score = msg->score;
        player->second.lines = msg->lines;
      } else {
        state.players.insert(
            {msg->player_no, (playstate){.score = 0, .lines = 0}});
      }
    }
    // Update the entry
  }
}

void broadcast_state(int sock_fd, gamestate &state,
                     vector<pair<int, struct sockaddr_in>> peers) {

  int counter = 0;
  while (1) {
    if (*termination_flag) {
      break;
    }
    counter = (counter + 1) %
              TICKRATE; // To send information to the server less frequently
    struct update_msg msg;
    msg.score = state.local.score;
    msg.lines = state.local.lines;
    msg.player_no = state.player_no;
    encode_state(msg);
    for (auto &[sock, other] : peers) {
      int num_bytes;
      if (sendto(sock, (char *)&msg, sizeof(update_msg), 0,
                 (struct sockaddr *)&other, sizeof(other)) == -1) {
        close(sock);
        perror("sendto()");
        return;
      }
    }
    if (counter == 0) {
      tmessage server_msg;
      server_msg.message_type = (tmessage_t)htonl((int32_t)SCORE_UPDATE);
      server_msg.arg1 = htonl(state.game_no);
      server_msg.arg2 = htonl(state.local.score);
      server_msg.arg3 = htonl(state.local.lines);
      send(sock_fd, (char *)&server_msg, sizeof(server_msg), 0);
    }

    this_thread::sleep_for(
        TICKDURATION); // 20 updates per second - inline with titanfall
  }
}

void encode_state(update_msg &msg) {
  msg.score = htonl(msg.score);
  msg.lines = htonl(msg.lines);
  msg.player_no = htonl(msg.player_no);
}

void decode_state(update_msg *msg) {
  msg->score = ntohl(msg->score);
  msg->lines = ntohl(msg->lines);
  msg->player_no = ntohl(msg->player_no);
}
