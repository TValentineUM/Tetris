#ifndef CLIENT_H_
#define CLIENT_H_

#include "../game-server/tprotocol.hh"
#include <curses.h>
#include <mutex>
#include <string>
#include <vector>
using namespace std;

static vector<string> chat_messages;
static mutex message_lock;

int establish_connection(char *hostname, char *port);
void consume_chat(int);
void display_chat(WINDOW *win);
void send_chat(string);
void buffer_message(string);
void send_messages(int sockfd, WINDOW *win);

#endif // CLIENT_H_
