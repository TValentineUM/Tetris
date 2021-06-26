#include "pieces.hh"
#include "tetris_game.hh"
#include <chrono>
#include <iostream>
#include <ncurses.h>
#include <random>
#include <string>
#include <thread>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {
  initscr();
  curs_set(0);
  noecho();
  cbreak();

  keypad(stdscr, TRUE);
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
  WINDOW *player_win = newwin(ROWS + 2, COLUMNS + (COLUMNS / 2), 3, 47);

  refresh();

  box(game_win, 0, 0);
  box(score_win, 0, 0);
  box(piece_win, 0, 0);
  box(player_win, 0, 0);

  TetrisGame game(game_win, score_win, piece_win, player_win, time(NULL));
  game.display_board();
  game.display_score();
  wrefresh(game_win);

  vector<pair<string, string>> ips;

  switch (atoi(argv[1])) {
  case 0: {
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42070"});
    game.state.player_no = 0;

  } break;
  case 1: {
    ips.push_back({"127.0.0.1", "42070"});
    ips.push_back({"127.0.0.1", "42069"});
    game.state.player_no = 1;
  } break;
  }

  thread t1(communicate_state, 0, std::ref(game.state), ips);

  bool piece_flag = true;
  int counter = 0;
  tetromino new_piece;
  while (1) {
    game.display_players();
    if (piece_flag) {
      new_piece = game.get_next_piece();
      piece_flag = false;
    }
    switch (getch()) {
    case 'w':
    case 'W':
    case 'k':
    case 'K':
    case KEY_UP: {
      auto old_rotation = new_piece.rotation;
      new_piece.rotation = (old_rotation + 1) % 4;
      if (!game.piece_fits(new_piece)) {
        new_piece.rotation = old_rotation;
      }
      break;
    }
    case 'a':
    case 'A':
    case 'h':
    case 'H':
    case KEY_LEFT: {
      new_piece.x -= 1;
      if (!game.piece_fits(new_piece)) {
        new_piece.x += 1;
      }
      break;
    }
    case 'd':
    case 'D':
    case 'l':
    case 'L':
    case KEY_RIGHT: {
      new_piece.x += 1;
      if (!game.piece_fits(new_piece)) {
        new_piece.x -= 1;
      }
      break;
    }
    case 's':
    case 'S':
    case 'j':
    case 'J':
    case KEY_DOWN: {
      while (game.piece_fits(new_piece)) {
        // game.show_piece(new_piece);
        new_piece.y += 1;
        // this_thread::sleep_for(chrono::milliseconds(50));
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
    game.show_piece(new_piece);
    if (counter < 12) {
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
