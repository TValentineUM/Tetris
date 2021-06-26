#ifndef BOARD_H_
#define BOARD_H_

#define ROWS 20
#define COLUMNS 10
#define STATIC_ROW 8

#include "../p2p/p2p.hh"
#include "pieces.hh"
#include <curses.h>
#include <random>
#include <tuple>
#include <vector>
using namespace std;

class TetrisGame {

protected:
  WINDOW *game_window;
  WINDOW *score_window;
  WINDOW *piece_window;
  WINDOW *players_window;
  int score, lines_cleared, seed;
  tetromino next_piece;
  vector<vector<char>> playing_field;
  mt19937 rng;
  uniform_int_distribution<> distrib;
  vector<pair<string, string>> ips;
  void game_setup();
  int do_gametick(tetromino &, bool &, int &);

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
  TetrisGame(int, vector<pair<string, string>>, int, int);
  TetrisGame(WINDOW *game_window, WINDOW *score_window, WINDOW *piece_window,
             WINDOW *players_window, int seed)
      : playing_field{vector<vector<char>>(ROWS, vector<char>(COLUMNS, 0))},
        game_window{game_window}, score_window{score_window},
        piece_window{piece_window}, players_window{players_window}, seed{seed},
        next_piece{tetrominos[seed % 7]}, distrib{uniform_int_distribution<>(
                                              0, 6)},
        rng{mt19937(seed)}, score{0}, lines_cleared{0} {}

  void run();
};

#endif // BOARD_H_
