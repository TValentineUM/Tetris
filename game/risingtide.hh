#ifndef RISINGTIDE_H_
#define RISINGTIDE_H_

#include "tetris_game.hh"

class RisingTide : public TetrisGame {

  int total_lines = 0;
  int new_lines = 0;

  int insert_lines();

public:
  RisingTide(int seed, vector<pair<string, string>> ips, int player_no,
             int game_no)
      : TetrisGame(seed, ips, player_no, game_no) {}

  void run();
};

#endif // RISINGTIDE_H_
