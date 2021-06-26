#ifndef BOOMER_H_
#define BOOMER_H_

#include "tetris_game.hh"
#include <chrono>

class BoomerGame : public TetrisGame {

private:
  chrono::seconds game_duration;

public:
  BoomerGame(int seed, vector<pair<string, string>> ips, int player_no,
             int game_no, int game_duration)
      : TetrisGame(seed, ips, player_no, game_no),
        game_duration{chrono::seconds(game_duration)} {}

  void run();
};

#endif // BOOMER_H_
