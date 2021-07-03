#ifndef BOOMER_H_
#define BOOMER_H_

#include "tetris_game.hh"
#include <chrono>

class BoomerGame : public TetrisGame {

private:
  std::chrono::seconds game_duration;

public:
  BoomerGame(int seed, std::vector<std::pair<std::string, std::string>> ips,
             int player_no, int game_no, int server_socket, int game_duration)
      : TetrisGame(seed, ips, player_no, game_no, server_socket),
        game_duration{std::chrono::seconds(game_duration)} {}

  void run();
};

#endif // BOOMER_H_
