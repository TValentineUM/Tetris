#include "boomer.hh"
#include <chrono>
#include <iostream>
#include <thread>
using namespace std;

void BoomerGame::run() {

  game_setup();

  bool game_ended = false;
  thread t1(communicate_state, 0, std::ref(state), ips, std::ref(game_ended));

  bool piece_flag = true;
  int counter = 0;
  tetromino new_piece;

  auto start = chrono::high_resolution_clock::now();
  auto current = chrono::high_resolution_clock::now();
  auto time_elapsed =
      chrono::duration_cast<chrono::milliseconds>(current - start);
  while (do_gametick(new_piece, piece_flag, counter) == 0 &&
         time_elapsed < game_duration) {
    current = chrono::high_resolution_clock::now();
    time_elapsed = chrono::duration_cast<chrono::milliseconds>(current - start);
  }
  game_ended = true;
  t1.join();
  clear();
  endwin();
  cout << "game ended" << endl;
}
