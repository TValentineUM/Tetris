#include "../server/tprotocol.hh"
#include "client-server.hh"
#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <exception>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <regex>
#include <sstream>
#include <string>
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
  tmessage msg;
  // Determining if command
  if (str.at(0) == '!') {
    istringstream ss(str);
    string command;
    ss >> command; // Getting the first word
    if (regex_match(command, CMD_LEADERBOARD)) {
      msg.message_type = (tmessage_t)htonl((int32_t)LEADERBOARD);
      if (regex_match(str, CMD_LEADERBOARD_FULL)) {
        ss >> msg.arg1; // Setting arg1 to the gamemode number
        msg.arg1 = htonl(msg.arg1);
      } else {
        throw invalid_argument(
            "Unable to parse: \'" + str +
            "\', the correct sytnax is \'!leaderboard [0-3]\'");
      }
    } else if (regex_match(command, CMD_PLAYERS)) {
      msg.message_type = (tmessage_t)htonl((int32_t)PLAYERS);
      if (ss >> command) {
        throw invalid_argument("Unable to parse: \'" + str +
                               "\', the correct sytnax is \'!players\'");
      }
    } else if (regex_match(str, CMD_PLAYERSTATS)) {
      msg.message_type = (tmessage_t)htonl((int32_t)PLAYERSTATS);

    } else if (regex_match(command, CMD_NICKNAME)) {
      msg.message_type = (tmessage_t)htonl((int32_t)NICKNAME);
      if (regex_match(str, CMD_NICKNAME_FULL)) {
        ss >> command;
        strcpy(msg.buffer, command.c_str());
      } else {
        throw invalid_argument(
            "Unable to parse: \'" + str +
            "\', the correct sytnax is \'!nickname <playername>\'. The "
            "player name must not exceed 10 alphanumeric characters");
      }
    } else if (regex_match(command, CMD_BATTLE)) {
      msg.message_type = (tmessage_t)htonl((int32_t)BATTLE);
      if (regex_match(str, CMD_BATTLE_FULL)) {
        int mode;
        ss >> mode;
        msg.arg1 = htonl(mode); // Setting the gamemode
        int arg_count =
            count(str.begin(), str.end(), '='); // only arguments have an =
        switch (mode) {
        case 0: {
          if (arg_count > 0) {
            throw invalid_argument(
                "Rising Tide gamemode does not take arguments");
          }
          string temp;
          ss >> temp;
          strcpy(msg.buffer, temp.c_str());
        } break;
        case 1: {
          if (arg_count != 2) {
            throw invalid_argument("Fasttrack needs 2 settings");
          } else {
            bool base = false, win = false;
            string temp;
            ss >> temp;
            string argument = temp.substr(0, temp.find("="));
            string value = temp.substr(temp.find("=") + 1, temp.size());
            if (argument == "baselines") {
              base = true;
              msg.arg2 = htonl(stoi(value));
            } else {
              win = true;
              msg.arg3 = htonl(stoi(value));
            }
            ss >> temp;
            argument = temp.substr(0, temp.find("="));
            value = temp.substr(temp.find("=") + 1, temp.size());
            if (argument == "baselines") {
              base = true;
              msg.arg2 = htonl(stoi(value));
            } else {
              win = true;
              msg.arg3 = htonl(stoi(value));
            }
            if (!(base && win)) {
              cout << msg.arg2 << "\t" << msg.arg3 << endl;
              throw invalid_argument("Fasttrack needs two arguments");
            }
            string t1;
            while (ss >> temp) {
              t1.append(temp);
              t1.append(" ");
            }
            strcpy(msg.buffer, t1.c_str());
          }
        } break;
        case 2: {
          if (arg_count != 1) {
            throw invalid_argument("Boomer needs 1 settings");
          } else {
            string temp;
            ss >> temp;
            string argument = temp.substr(0, temp.find("="));
            string value = temp.substr(temp.find("=") + 1, temp.size());
            if (argument != "time") {
              throw invalid_argument(
                  "Boomer Gamemode only Takes time as an argumetn");
            } else {
              msg.arg2 = htonl(stoi(value));
            }
            string t1;
            while (ss >> temp) {
              t1.append(temp);
              t1.append(" ");
            }
            strcpy(msg.buffer, t1.c_str());
          }
        } break;
        default:
          throw logic_error("Invalid gamenumber entered");
        }
        // strcpy(msg.buffer, temp.c_str());
      } else {
        throw invalid_argument("Unable to parse: \'" + str +
                               "\', the correct sytnax is \'!battle <gamemode> "
                               "<gameoptions> <players>{ 1, 7}\'");
      }

    } else if (regex_match(command, CMD_QUICK)) {
      msg.message_type = (tmessage_t)htonl((int32_t)QUICKPLAY);
      if (regex_match(str, CMD_QUICK_FULL)) {
        ss >> msg.arg1;
        msg.arg1 = htonl(msg.arg1);
      } else {
        throw invalid_argument("Unable to parse: \'" + str +
                               "\', the correct sytnax is \'!quick [1-7]");
      }
    } else if (regex_match(command, CMD_CHILL)) {
      msg.message_type = (tmessage_t)htonl((int32_t)CHILL);
      if (!regex_match(str, CMD_QUICK)) {
        throw invalid_argument("Chill does not expect arguments");
      }
    } else if (regex_match(command, CMD_GO)) {
      msg.message_type = (tmessage_t)htonl((int32_t)GO);
      if (regex_match(str, CMD_GO_FULL)) {
        ss >> msg.arg1;
        msg.arg1 = htonl(msg.arg1);
      } else {
        throw invalid_argument("GO needs a number");
      }
    } else if (regex_match(command, CMD_LEADERBOARDS)) {
      msg.message_type = (tmessage_t)htonl((int32_t)LEADERBOARDS);
    } else {
      throw invalid_argument("Unable to parse command");
    }
    return msg;
  } else {
    msg.message_type = CHAT;
    strcpy(msg.buffer, str.c_str());
    return msg;
  }
}

tmessage *decode_message(tmessage *buffer) {
  tmessage *msg = (tmessage *)buffer;
  msg->message_type = (tmessage_t)ntohl((int32_t)msg->message_type);
  msg->arg1 = ntohl(msg->arg1);
  msg->arg2 = ntohl(msg->arg2);
  msg->arg3 = ntohl(msg->arg3);
  msg->arg4 = ntohl(msg->arg4);
  msg->arg5 = ntohl(msg->arg5);
  msg->arg6 = ntohl(msg->arg6);
  return msg;
}

void encode_message(tmessage &msg) {
  msg.message_type = (tmessage_t)htonl((int32_t)msg.message_type);
  msg.arg1 = htonl(msg.arg1);
  msg.arg2 = htonl(msg.arg2);
  msg.arg3 = htonl(msg.arg3);
  msg.arg4 = htonl(msg.arg4);
  msg.arg5 = htonl(msg.arg5);
  msg.arg6 = htonl(msg.arg6);
}
