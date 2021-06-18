#ifndef BOARD_H_
#define BOARD_H_

#define ROWS 20
#define COLUMNS 10

#include "pieces.hh"
#include <curses.h>
#include <random>
#include <tuple>
#include <vector>
using namespace std;

class Board {

private:
  WINDOW *game_window;
  WINDOW *score_window;
  WINDOW *piece_window;
  int score, lines_cleared, seed;
  tetromino next_piece;
  vector<vector<char>> playing_field;
  mt19937 rng;
  uniform_int_distribution<> distrib;

public:
  Board(WINDOW *game_window, WINDOW *score_window, WINDOW *piece_window,
        int seed)
      : playing_field{vector<vector<char>>(ROWS, vector<char>(COLUMNS, 0))},
        game_window{game_window}, score_window{score_window},
        piece_window{piece_window}, seed{seed},
        next_piece{tetrominos[seed % 7]}, distrib{uniform_int_distribution<>(
                                              0, 6)},
        rng{mt19937(seed)}, score{0}, lines_cleared{0} {}

  bool piece_fits(tetromino);
  void insert_piece(tetromino);
  tetromino get_next_piece();
  void clear_lines();
  void display_board();
  void display_score();
  void display_next_piece();
  void show_piece(tetromino);
};

#endif // BOARD_H_
