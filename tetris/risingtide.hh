#ifndef RISINGTIDE_H_
#define RISINGTIDE_H_

#include "tetris_game.hh"

class RisingTide : public TetrisGame {

  int total_lines = 0;
  int new_lines = 0;

  int insert_lines();

public:
  RisingTide(int seed, std::vector<std::pair<std::string, std::string>> ips,
             int player_no, int game_no, int server_socket)
      : TetrisGame(seed, ips, player_no, game_no, server_socket) {}

  void run();
};

#endif // RISINGTIDE_H_
