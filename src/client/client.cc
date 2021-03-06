#include "../client-server/client-server.hh"
#include "../server/tprotocol.hh"
#include "../tetris/boomer.hh"
#include "../tetris/fasttrack.hh"
#include "../tetris/risingtide.hh"
#include "client.hh"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
using namespace std;

void start_irc() {
  initscr();
  cbreak();
  echo();
  curs_set(0);

  int xMax, yMax;
  getmaxyx(stdscr, yMax, xMax);
  chat_window = newwin(yMax - 10, xMax, 0, 0);
  text_window = newwin(5, xMax, yMax - 5, 0);

  box(chat_window, 0, 0);
  box(text_window, 0, 0);
  refresh();
  wmove(text_window, 1, 1);

  wrefresh(chat_window);
  wrefresh(text_window);
}

void display_chat() {
  wclear(chat_window);
  box(chat_window, 0, 0);

  int yMax;
  yMax = getmaxy(stdscr);

  if (chat_messages.size() > (yMax - 12)) {
    chat_messages.erase(chat_messages.begin());
  }
  for (int i = 0; i < chat_messages.size(); i++) {
    mvwprintw(chat_window, i + 1, 1, chat_messages[i].c_str());
  }
  wrefresh(chat_window);
}

void send_message(int sockfd) {
  char buffer[MESSAGE_LENGTH];
  memset(buffer, 0, MESSAGE_LENGTH * sizeof(char));
  getstr(buffer);
  wclear(text_window);
  box(text_window, 0, 0);
  // Checking theere is actual data to send
  if (strlen(buffer)) {
    try {
      tmessage msg = parse_message(buffer);
      if (send_message(msg, sockfd) == EXIT_FAILURE) {
        endwin();
        exit(EXIT_FAILURE);
      }
    } catch (exception &err) {
      chat_messages.push_back(err.what());
      display_chat();
      this_thread::sleep_for(chrono::seconds(3));
      chat_messages.pop_back();
      display_chat();
    }
  }
  wrefresh(text_window);
}

void refresh_irc() {
  delwin(text_window);
  delwin(chat_window);
  start_irc();
  echo();
  cbreak();

  // Un doing the tetris settings
  keypad(stdscr, FALSE);
  nodelay(stdscr, FALSE);
}

void recieve_message(int sockfd) {
  int xMax, yMax;
  getmaxyx(stdscr, yMax, xMax);

  tmessage msg;
  int rv;
  if ((rv = recieve_message(msg, sockfd)) != 0) {
    if (rv == 2) {
      exit(EXIT_SUCCESS);
    } else {
      exit(EXIT_FAILURE);
    }
  }

  switch (msg.message_type) {
  case CHAT: {
    chat_messages.push_back(string(msg.buffer));
    display_chat();
    break;
  }
  case INIT_GAME: {
    erase();

    auto ips = decode_hostnames(string(msg.buffer));
    gamestate score;
    switch (msg.arg2) {
    case BOOMER: {
      BoomerGame game(msg.arg6, ips, msg.arg1, msg.arg3, sockfd, msg.arg4);
      game.run();
      score = game.get_final_score();
    } break;
    case RISING_TIDE: {
      RisingTide game(msg.arg6, ips, msg.arg1, msg.arg3, sockfd);
      game.run();
      score = game.get_final_score();
    } break;
    case FAST_TRACK: {
      FastTrack game(msg.arg6, ips, msg.arg1, msg.arg3, sockfd, msg.arg4,
                     msg.arg5);
      game.run();
      score = game.get_final_score();
    } break;
    case CHILLER: {
      TetrisGame game(msg.arg6);
      game.run();
      score = game.get_final_score();
    }
    default:
      break;
    }
    tmessage reply_msg;
    reply_msg.message_type = GAME_END;
    reply_msg.arg1 = msg.arg3; // Game ID
    reply_msg.arg2 = score.local.score;
    reply_msg.arg3 = score.local.lines;
    encode_message(reply_msg);
    if (send_message(reply_msg, sockfd) != 0) {
      exit(EXIT_FAILURE);
    };

    erase();
    refresh_irc();
    display_chat();
    break;
  }
  default:
    break;
  }
}
