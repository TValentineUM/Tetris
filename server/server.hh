#ifndef SERVER_H_
#define SERVER_H_

#include "tprotocol.hh"
#include <atomic>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
using namespace std;

#define QUEUE_SIZE 5
#define MESSAGE_SIZE 255
#define NAMESIZE 10
#define LOBBY_TIME 5
#define PRIVATE_P2P 50000 // Using a linux private port
#define PUBLIC_P2P 42000  // Using a Linux user port
#define LOCALHOST                                                              \
  16777343 // Integer representation of localhost ip
           //
// Ports to be used when connected to localhost
#define DEFAULT_PORT 42001

struct player_data {
  char name[NAMESIZE];
  int chill = 0;                  /**< Highscore for Chill Games*/
  int rising = 0;                 /**< Highscore for Rising Games*/
  int boomer = 0;                 /**< Highscore for Boomer Games*/
  int fasttrack = 0;              /**< Highscore for fasttrack games*/
  pair<int, int> rising_games;    /**< Win/Loss Number for Rising Games*/
  pair<int, int> boomer_games;    /**< Win/Loss Number for Boomer Games*/
  pair<int, int> fasttrack_games; /**< Win/Loss Number for FastTrack Games*/
  vector<int> games;
  struct sockaddr_in address; /**< Connection Details for Client*/
};

struct game {
  int gamemode;
  int arg1;               /**< Argument for time or baselines*/
  int arg2;               /**< Argument for lines to complete*/
  map<int, bool> players; /**< List of participating socket ids, can be decodede
                             from player_list*/
};

static std::mutex cout_mutex;

static vector<thread>
    thread_list; /**< Stores all threads created by the server */
static std::mutex thread_list_mutex;

static map<int, player_data> player_list;
static std::mutex player_list_mutex;

static atomic_uint32_t
    game_counter; /**< To have a unique number for each game, so long as there
                     are no more than 2^32 games*/
static map<uint32_t, game> game_list;
static std::mutex game_list_mutex;

void relay(std::string);     /**< Send the message to all clients connected*/
void handle_connection(int); /**< Handles individual clients on a new thread*/
string random_string(size_t length); /**< Creates random strings */
void run_server(int);                /**< Lanuches the server */
void handle_message(tmessage *, int);
void handle_game(int);
void send_chat(int, string);
void send_multiple(int, vector<string>);
vector<string> get_leaderboard(int);
tmessage *decode_message(char *);
void encode_message(tmessage *);
vector<int> get_socket_from_playername(vector<string>);

#endif // SERVER_H_
