#include "board.hh"
#include "pieces.hh"
#include <chrono>
#include <iostream>
#include <ncurses.h>
#include <random>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#define ROTATE_KEY 'w'
#define LEFT_KEY 'a'
#define DOWN_KEY 's'
#define RIGHT_KEY 'd'

int main(int argc, char *argv[]) {

  initscr();
  curs_set(0);
  noecho();
  cbreak();

  nodelay(stdscr, TRUE); // For getchr

  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_WHITE, COLOR_RED);
  init_pair(3, COLOR_WHITE, COLOR_GREEN);
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);
  init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(6, COLOR_BLACK, COLOR_CYAN);
  init_pair(7, COLOR_BLACK, COLOR_WHITE);

  int start_y, start_x;
  start_y = start_x = 10;

  WINDOW *game_win = newwin(ROWS + 2, (2 * COLUMNS) + 2, 3, 3);
  WINDOW *piece_win = newwin(7, 12, 3, 27);
  WINDOW *score_win = newwin(8, 17, 13, 27);

  refresh();

  box(game_win, 0, 0);
  box(score_win, 0, 0);
  box(piece_win, 0, 0);

  Board game(game_win, score_win, piece_win, time(NULL));
  game.display_board();
  game.display_score();
  wrefresh(game_win);

  bool piece_flag = true;
  int counter = 0;
  tetromino new_piece;
  while (1) {
    if (piece_flag) {
      new_piece = game.get_next_piece();
      piece_flag = false;
    }
    keypad(stdscr, TRUE);
    switch (getch()) {
    case ROTATE_KEY: {
      auto old_rotation = new_piece.rotation;
      new_piece.rotation = (old_rotation + 1) % 4;
      if (!game.piece_fits(new_piece)) {
        new_piece.rotation = old_rotation;
      }
      break;
    }
    case LEFT_KEY: {
      new_piece.x -= 1;
      if (!game.piece_fits(new_piece)) {
        new_piece.x += 1;
      }
      break;
    }
    case RIGHT_KEY: {
      new_piece.x += 1;
      if (!game.piece_fits(new_piece)) {
        new_piece.x -= 1;
      }
      break;
    }
    case DOWN_KEY: {
      while (game.piece_fits(new_piece)) {
        game.show_piece(new_piece);
        new_piece.y += 1;
        this_thread::sleep_for(chrono::milliseconds(50));
      }
      new_piece.y -= 1;
      game.insert_piece(new_piece);
      game.clear_lines();
      game.display_board();
      piece_flag = true;
      continue;
      break;
    }
    default:
      break;
    }
    keypad(stdscr, FALSE);

    game.show_piece(new_piece);
    if (counter < 8) {
      counter += 1;
      this_thread::sleep_for(chrono::milliseconds(25));
    } else {
      counter = 0;
      new_piece.y += 1;
      if (!game.piece_fits(new_piece)) {
        new_piece.y -= 1;
        game.insert_piece(new_piece);
        game.clear_lines();
        game.display_board();
        piece_flag = true;
      }
    }
  }

  wrefresh(game_win);

  endwin();

  return 0;
}
