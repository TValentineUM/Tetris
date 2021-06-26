#include "boomer.hh"
#include "fasttrack.hh"
#include "pieces.hh"
#include "risingtide.hh"
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

  switch (atoi(argv[2])) {
  case 0: {
    TetrisGame game(time(NULL), ips, player_no, game_id);
    game.run();
  } break;
  case 1: {
    cout << "Starting a Boomer Game with 30 second timeout" << endl;
    this_thread::sleep_for(chrono::seconds(2));
    BoomerGame game(time(NULL), ips, player_no, game_id, 45);
    game.run();
  } break;
  case 2: {
    cout << "Starting a FastTrack Game with 6 baselines and 2 winlines" << endl;
    this_thread::sleep_for(chrono::seconds(2));
    FastTrack game(time(NULL), ips, player_no, game_id, 6, 2);
    game.run();

  } break;
  default: {
    cout << "Starting a Rising Tide Game" << endl;
    this_thread::sleep_for(chrono::seconds(2));
    RisingTide game(time(NULL), ips, player_no, game_id);
    game.run();
  }
  }
}
