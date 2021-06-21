#ifndef SERVER_H_
#define SERVER_H_

#include "tprotocol.hh"
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
using namespace std;

#define QUEUE_SIZE 5
#define MESSAGE_SIZE 255
#define NAMESIZE 10

struct player_data {
  char name[NAMESIZE];
  int chill = 0;                  /**< Highscore for Chill Games*/
  int rising = 0;                 /**< Highscore for Rising Games*/
  int boomer = 0;                 /**< Highscore for Boomer Games*/
  int fasttrack = 0;              /**< Highscore for fasttrack games*/
  pair<int, int> rising_games;    /**< Win/Loss Number for Rising Games*/
  pair<int, int> boomer_games;    /**< Win/Loss Number for Boomer Games*/
  pair<int, int> fasttrack_games; /**< Win/Loss Number for FastTrack Games*/
};

static vector<thread>
    thread_list; /**< Stores all threads created by the server */
static std::mutex thread_list_mutex;

static map<int, player_data> player_list;
static std::mutex player_list_mutex;

void relay(std::string);     /**< Send the message to all clients connected*/
void handle_connection(int); /**< Handles individual clients on a new thread*/
string random_string(size_t length); /**< Creates random strings */
void run_server(int);                /**< Lanuches the server */
void handle_message(tmessage *, int);
void send_chat(int, string);
void send_multiple(int, vector<string>);
vector<string> get_leaderboard(int);
tmessage *decode_message(char *);

#endif // SERVER_H_
