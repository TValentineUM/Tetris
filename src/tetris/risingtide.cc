#include "risingtide.hh"
#include <iostream>
#include <thread>
using namespace std;

int RisingTide::insert_lines() {

  for (int i = 0; i < new_lines; i++) {
    for (auto c : playing_field[i]) {
      if (c != 0) {
        return -1;
      }
    }
  }
  // We can push the lines up
  //
  playing_field.erase(playing_field.begin(), playing_field.begin() + new_lines);
  for (int i = 0; i < new_lines; i++) {
    playing_field.push_back(vector<char>(COLUMNS, STATIC_ROW));
  }
  new_lines = 0;
  return 0;
}

void RisingTide::run() {

  game_setup();
  bool game_ended = false;

  thread t1(communicate_state, server_socket, std::ref(state), ips,
            std::ref(game_ended));

  bool piece_flag = true;
  int total_lines = 0;
  int counter = 0;
  int old_lines = total_lines;
  tetromino new_piece;

  while (do_gametick(new_piece, piece_flag, counter) == 0) {
    new_lines = 0;
    if (state.players.size()) {
      old_lines = total_lines;
      total_lines = 0;
      for (auto &[k, v] : state.players) {
        total_lines += v.lines;
      }
      new_lines = total_lines - old_lines;
      // total_lines = total_lines - old_lines;
      // old_lines = total_lines;
      if (new_lines > 0) {
        if (insert_lines() != 0) {
          break;
        } else {
          for (int i = 0; i < new_lines; i++) {
            if (piece_fits(new_piece)) {
              break;
            } else {
              new_piece.y--;
            }
          }
        }
        new_lines = 0;
      }
    }
  }
  game_ended = true;
  t1.join();
  clear();
  endwin();
}
