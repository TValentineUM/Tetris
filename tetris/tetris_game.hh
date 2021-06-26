#ifndef TETRIS_GAME_H_
#define TETRIS_GAME_H_

#define ROWS 20
#define COLUMNS 10
#define STATIC_ROW 8

#include "../p2p/p2p.hh"
#include "pieces.hh"
#include <curses.h>
#include <random>
#include <tuple>
#include <vector>

class TetrisGame {

protected:
  WINDOW *game_window;
  WINDOW *score_window;
  WINDOW *piece_window;
  WINDOW *players_window;
  int score, lines_cleared, seed;
  tetromino next_piece;
  std::vector<std::vector<char>> playing_field;
  std::mt19937 rng;
  std::uniform_int_distribution<> distrib;
  std::vector<std::pair<std::string, std::string>> ips;
  void game_setup();
  int do_gametick(tetromino &, bool &, int &);
  int server_socket;

  gamestate state;
  bool piece_fits(tetromino);
  int insert_piece(tetromino);
  tetromino get_next_piece();
  void clear_lines();
  void display_board();
  void display_score();
  void display_next_piece();
  void display_players();
  void show_piece(tetromino);

public:
  TetrisGame(int seed, std::vector<std::pair<std::string, std::string>> ips,
             int player_no, int game_no, int server_socket);
  void run();
};

#endif // BOARD_H_
