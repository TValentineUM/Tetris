#ifndef P2P_H_
#define P2P_H_

#include <atomic>
#include <map>
#include <string>
#include <sys/types.h>
#include <tuple>
#include <vector>

#define TICKRATE 5
#define TICKDURATION chrono::milliseconds(1000) / TICKRATE

struct update_msg {
  int score;
  int lines;
  int player_no;
};

struct playstate {
  int score = 0;
  int lines = 0;
};

struct gamestate {
  playstate local;
  int player_no;
  int game_no;
  std::map<int, playstate> players; /**< Map from Player Number to State*/
};

static int player_no;
static bool *termination_flag;

std::vector<std::pair<std::string, std::string>> decode_hostnames(
    std::string); /**< Converts a string of ips and port numbers
                into a vector of tuples with <ip, portno>*/

/**
 * Given the server socket fd, the gamestate, and the peers playing with you.
 * The function will launch 2 new threads,
 * 1. Update the gamestate with information from other players
 * 2. Periodically sends data to peers, and to the server
 */
void communicate_state(int, gamestate &,
                       std::vector<std::pair<std::string, std::string>>,
                       bool &);

void recieve_state(
    gamestate &,
    int); /**< Polls the file descriptors to update the local state*/

void broadcast_state(
    int, gamestate &,
    std::vector<std::pair<int, struct sockaddr_in>>); /**< Broadcast the local
                                        state to peers and to server*/

#endif // CLIENT_H_
