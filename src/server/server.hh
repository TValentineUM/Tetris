#ifndef SERVER_H_
#define SERVER_H_

#include "tprotocol.hh"
#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#define QUEUE_SIZE 5
#define MESSAGE_SIZE 255
#define NAMESIZE 10
#define LOBBY_TIME 10
#define PRIVATE_P2P 50000  // Using a linux private port
#define PUBLIC_P2P 42000   // Using a Linux user port
#define LOCALHOST 16777343 // Integer representation of localhost ip
#define MAX_BOOMER_DURATION                                                    \
  300 // 5 minutes
      //
#define SEND_WAIT_MS 50 * (1000)

// Ports to be used when connected to localhost
#define DEFAULT_PORT 42001

class player_data {
public:
  char name[NAMESIZE];
  int chill = 0;                    /**< Highscore for Chill Games*/
  int rising = 0;                   /**< Highscore for Rising Games*/
  int boomer = 0;                   /**< Highscore for Boomer Games*/
  int fasttrack = 0;                /**< Highscore for fasttrack games*/
  std::pair<int, int> rising_games; /**< Win/Plays Number for Rising Games*/
  std::pair<int, int> boomer_games; /**< Win/Plays Number for Boomer Games*/
  std::pair<int, int>
      fasttrack_games; /**< Win/Plays Number for FastTrack Games*/
  std::vector<int> games;
  struct sockaddr_in address; /**< Connection Details for Client*/
};

class game {
public:
  int gamemode;
  int arg1;                    /**< Argument for time or baselines*/
  int arg2;                    /**< Argument for lines to complete*/
  std::map<int, bool> players; /**< List of participating socket ids, can be
                             decodede from player_list*/
};

class game_data {
public:
  int score = 0;
  int lines = 0;
  std::chrono::duration<double> duration;
};

class active_game {
private:
  int gamemode;
  int arg;
  std::chrono::time_point<std::chrono::steady_clock> start; // getting the time
  std::map<int, std::pair<game_data, bool>> state; // Bool is set on game_end

public:
  active_game(int gamemode, std::vector<int>, int arg);
  void remove_player(int);
  void check_winner();
  void finished_player(int, int, int);
  void update_player(int, int, int);
  std::vector<std::string> get_info();
};

static std::mutex cout_mutex;

static std::vector<std::thread>
    thread_list; /**< Stores all threads created by the server */
static std::mutex thread_list_mutex;

static std::map<int, player_data> player_list;
static std::mutex player_list_mutex;

static std::atomic_uint32_t
    game_counter; /**< To have a unique number for each game, so long as there
                     are no more than 2^32 games*/
static std::map<uint32_t, game> game_list;
static std::mutex game_list_mutex;

static std::map<uint32_t, active_game> ongoing_games;
static std::mutex ongoing_game_mutex;

static std::map<uint32_t, game_data> chill_games;
static std::mutex chill_games_mutex;

void relay(std::string);     /**< Send the message to all clients connected*/
void handle_connection(int); /**< Handles individual clients on a new thread*/
std::string random_string(size_t length); /**< Creates random strings */
void run_server(int);                     /**< Lanuches the server */
void handle_message(
    tmessage *, int); /**< Implements handler code for various message types*/
void handle_game(int);
void send_chat(int, std::string);
void send_multiple(int, std::vector<std::string>);
std::vector<std::string> get_leaderboard(int);
tmessage *decode_message(char *);
void encode_message(tmessage &);
void send_message(tmessage, int);
std::vector<int> get_socket_from_playername(std::vector<std::string>);

#endif // SERVER_H_
