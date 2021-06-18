#include "ascii_art.hh"
#include "board.hh"
#include <cstdlib>
#include <cstring>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>
using namespace std;

bool Board::piece_fits(tetromino t) {
  auto x_offset = t.x;
  auto y_offset = t.y;

  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 5; x++) {
      if (t.rotations[t.rotation][y][x] !=
          0) { // Checking all the pieces inside the shape can
               // fit on the board
        auto x_position = x_offset + x;
        if (x_position < 0 || x_position >= COLUMNS) {
          return false;
        }
        auto y_position = y_offset + y;
        if (y_position >= ROWS || y_position < 0) {
          return false;
        }
        if (playing_field[y_position][x_position]) {
          return false;
        }
      }
    }
  }
  return true;
}

void Board::insert_piece(tetromino t) {
  if (!piece_fits(t)) {
    erase();
    refresh();
    int xMax, yMax;
    getmaxyx(stdscr, yMax, xMax);
    mvprintw((yMax / 2) - 5, 20, GAMEOVER);
    refresh();
    this_thread::sleep_for(chrono::seconds(1));
    endwin();
    exit(0);
  }

  auto x_offset = t.x;
  auto y_offset = t.y;

  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 5; x++) {
      if (t.rotations[t.rotation][y][x]) {
        auto x_position = x_offset + x;
        auto y_position = y_offset + y;
        playing_field[y_position][x_position] = t.colour;
      }
    }
  }
}

void Board::clear_lines() {
  queue<int> lines;
  for (int y = 0; y < ROWS; y++) {
    int count = 0;
    for (int x = 0; x < COLUMNS; x++) {
      count += (playing_field[y][x] != 0);
    }
    if (count == COLUMNS) {
      lines.push(y);
      lines_cleared += 1;
    }
  }
  switch (lines.size()) {
  case 1:
    score += 40;
    break;
  case 2:
    score += 100;
    break;
  case 3:
    score += 300;
    break;
  case 4:
    score += 1200;
    break;
  default:
    break;
  }
  while (lines.size()) {
    int line = lines.front();
    lines.pop();
    for (int i = line; i > 0; i--) {
      for (int x = 0; x < COLUMNS; x++) {
        playing_field[i][x] = playing_field[i - 1][x];
      }
    }
  }
  display_score();
}

void Board::display_board() {
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLUMNS; x++) {
      switch (playing_field[y][x]) {
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        wattron(game_window, COLOR_PAIR(playing_field[y][x]));
        mvwprintw(game_window, y + 1, (2 * x) + 1, "  ");
        wattroff(game_window, COLOR_PAIR(playing_field[y][x]));
        break;
      default:
        mvwprintw(game_window, y + 1, (2 * x) + 1, "  ");
        break;
      }
    }
  }
  wrefresh(game_window);
}

void Board::show_piece(tetromino t) {
  Board temp_game = *this;
  temp_game.insert_piece(t);
  temp_game.display_board();
}

tetromino Board::get_next_piece() {
  tetromino temp = next_piece;
  next_piece = tetrominos[distrib(rng)];
  display_next_piece();
  show_piece(temp);
  return temp;
}

void Board::display_next_piece() {
  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 5; x++) {
      switch (next_piece.rotations[next_piece.rotation][y][x]) {
      case 1:
        wattron(piece_window, COLOR_PAIR(next_piece.colour));
        mvwprintw(piece_window, y + 1, (2 * x) + 1, "  ");
        wattroff(piece_window, COLOR_PAIR(next_piece.colour));
        break;
      default:
        mvwprintw(piece_window, y + 1, (2 * x) + 1, "  ");
        break;
      }
    }
  }
  wrefresh(piece_window);
}

void Board::display_score() {
  mvwprintw(score_window, 1, 1, "Score: ");
  mvwprintw(score_window, 2, 1, to_string(score).c_str());
  mvwprintw(score_window, 4, 1, "Lines Cleared:");
  mvwprintw(score_window, 5, 1, to_string(lines_cleared).c_str());
  wrefresh(score_window);
}
