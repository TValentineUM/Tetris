#ifndef FASTTRACK_H_
#define FASTTRACK_H_

#include "tetris_game.hh"

#define STATIC_ROW 8

class FastTrack : public TetrisGame {

protected:
  int init_lines;
  int win_lines;

  void clear_lines();

public:
  FastTrack(int seed, std::vector<std::pair<std::string, std::string>> ips,
            int player_no, int game_no, int server_socket, int init_lines,
            int win_lines)
      : TetrisGame(seed, ips, player_no, game_no, server_socket),
        init_lines{init_lines}, win_lines{win_lines} {}

  void run();
};

#endif // FASTTRACK_H_
