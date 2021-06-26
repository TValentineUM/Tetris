#include "fasttrack.hh"
#include <iostream>
#include <queue>
#include <thread>
using namespace std;
void FastTrack::run() {

  game_setup();
  for (int i = playing_field.size() - 1; i >= playing_field.size() - init_lines;
       i--) {
    playing_field[i] = vector<char>(playing_field[i].size() + 1, 8);
  }
  init_color(STATIC_ROW, 255, 215, 0);
  init_pair(STATIC_ROW, COLOR_BLACK, STATIC_ROW);
  thread t1(communicate_state, 0, std::ref(state), ips);
  bool piece_flag = true;
  int counter = 0;
  lines_cleared = 0;
  tetromino new_piece;
  while (do_gametick(new_piece, piece_flag, counter) == 0 &&
         lines_cleared < win_lines) {
  }
  clear();
  endwin();
  cout << "game ended" << endl;
}
