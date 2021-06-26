#include "boomer.hh"
#include "pieces.hh"
#include "tetris_game.hh"
#include <chrono>
#include <iostream>
#include <ncurses.h>
#include <random>
#include <string>
#include <thread>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {

  vector<pair<string, string>> ips;
  int player_no;
  int game_id = 0;
  switch (atoi(argv[1])) {
  case 0: {
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42070"});
    player_no = 0;

  } break;
  case 1: {
    ips.push_back({"127.0.0.1", "42070"});
    ips.push_back({"127.0.0.1", "42069"});
    player_no = 1;
  } break;
  }

  BoomerGame game(time(NULL), ips, player_no, game_id, 10);
  game.run();
}
