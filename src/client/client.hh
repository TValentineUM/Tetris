#ifndef CLIENT_H_
#define CLIENT_H_

#include <curses.h>

static WINDOW *chat_window;
static WINDOW *text_window;

void start_irc();
void send_message(int);
void recieve_message(int);
void display_chat();

#endif // CLIENT_H_
