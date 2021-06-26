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

  bool game_ended = false;

  thread t1(communicate_state, server_socket, std::ref(state), ips,
            std::ref(game_ended));

  bool piece_flag = true;
  int counter = 0;
  lines_cleared = 0;
  tetromino new_piece;
  while (do_gametick(new_piece, piece_flag, counter) == 0 &&
         lines_cleared < win_lines) {
  }
  game_ended = true;
  t1.join();
  clear();
  endwin();
  cout << "game ended" << endl;
}
