#ifndef CLIENT_H_
#define CLIENT_H_

#include "../game-server/tprotocol.hh"
#include <curses.h>
#include <mutex>
#include <string>
#include <vector>
using namespace std;

#define CMD_LEADERBOARD regex("!leaderboard")
#define CMD_LEADERBOARD_FULL regex("!leaderboard [0-3]")
#define CMD_PLAYERS regex("!players")
#define CMD_PLAYERSTATS regex("!playerstats")
#define CMD_PLAYERSTATS_FULL regex("!playerstats \\w{1,11}")
#define CMD_NICKNAME regex("!nickname")
#define CMD_NICKNAME_FULL regex("!nickname \\w{1,11}")
#define CMD_BATTLE regex("!battle")
#define CMD_BATTLE_FULL                                                        \
  regex("!battle [0-2] ((baselines|time|winlines)=\\d* ){0,2}((\\w{1,11} "     \
        "){1,7})?(\\w{1,11} *)")
#define CMD_QUICK regex("!quick")
#define CMD_QUICK_FULL regex("!quick [1-7]")
#define CMD_CHILL regex("!chill")
#define CMD_GO regex("!go")
#define CMD_GO_FULL regex("!go [:d:]*")

static vector<string> chat_messages;
static mutex message_lock;

int establish_connection(char *hostname, char *port);
void consume_chat(int);
void display_chat(WINDOW *win);
void send_chat(string);
void buffer_message(string);
void send_messages(int sockfd, WINDOW *win);
tmessage parse_message(string);

#endif // CLIENT_H_
