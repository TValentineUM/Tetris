#include "tetris_game.hh"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>
using namespace std;

TetrisGame::TetrisGame(int seed, vector<pair<string, string>> ips,
                       int player_no, int game_no, int server_socket)
    : playing_field{vector<vector<char>>(ROWS, vector<char>(COLUMNS, 0))},
      next_piece{tetrominos[0]}, distrib{uniform_int_distribution<>(0, 6)},
      rng{mt19937(seed)}, score{0}, lines_cleared{0}, ips{ips}, seed{seed},
      server_socket{server_socket} {
  state.player_no = player_no;
  state.game_no = game_no;
}

TetrisGame::TetrisGame(int seed)
    : playing_field{vector<vector<char>>(ROWS, vector<char>(COLUMNS, 0))},
      next_piece{tetrominos[seed % 7]}, distrib{uniform_int_distribution<>(0,
                                                                           6)},
      rng{mt19937(seed)}, score{0}, lines_cleared{0}, seed{seed} {}

bool TetrisGame::piece_fits(tetromino t) {
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

int TetrisGame::insert_piece(tetromino t) {
  if (!piece_fits(t)) {
    return -1;
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
  return 0;
}

void TetrisGame::clear_lines() {
  queue<int> lines;
  for (int y = 0; y < ROWS; y++) {
    int count = 0;
    for (int x = 0; x < COLUMNS; x++) {
      count += (playing_field[y][x] != 0 && playing_field[y][x] != STATIC_ROW);
    }
    if (count == COLUMNS) {
      lines.push(y);
      lines_cleared += 1;
      state.local.lines += 1;
    }
  }
  switch (lines.size()) {
  case 1:
    state.local.score += 40;
    score += 40;
    break;
  case 2:
    state.local.score += 100;
    score += 100;
    break;
  case 3:
    state.local.score += 300;
    score += 300;
    break;
  case 4:
    state.local.score += 1200;
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

void TetrisGame::display_board() {
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLUMNS; x++) {
      switch (playing_field[y][x]) {
      case 0:
        mvwprintw(game_window, y + 1, (2 * x) + 1, "  ");
        break;
      default:
        wattron(game_window, COLOR_PAIR(playing_field[y][x]));
        mvwprintw(game_window, y + 1, (2 * x) + 1, "  ");
        wattroff(game_window, COLOR_PAIR(playing_field[y][x]));
        break;
      }
    }
  }
  wrefresh(game_window);
}

void TetrisGame::show_piece(tetromino t) {
  TetrisGame temp_game = *this;
  temp_game.insert_piece(t);
  temp_game.display_board();
}

tetromino TetrisGame::get_next_piece() {
  tetromino temp = next_piece;
  next_piece = tetrominos[distrib(rng)];
  display_next_piece();
  show_piece(temp);
  return temp;
}

void TetrisGame::display_next_piece() {
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

void TetrisGame::display_score() {
  mvwprintw(score_window, 1, 1, "Score: ");
  mvwprintw(score_window, 2, 1, to_string(score).c_str());
  mvwprintw(score_window, 4, 1, "Lines Cleared:");
  mvwprintw(score_window, 5, 1, to_string(lines_cleared).c_str());
  wrefresh(score_window);
}

void TetrisGame::display_players() {
  int counter = 1;
  for (auto &[k, v] : state.players) {
    string str = "Player: " + to_string(k);
    mvwprintw(players_window, counter++, 1, str.c_str());
    str = "Score: " + to_string(v.score);
    mvwprintw(players_window, counter++, 1, str.c_str());
    counter++;
  }
  wrefresh(players_window);
}

void TetrisGame::game_setup() {
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

  init_color(STATIC_ROW, 255, 215, 0);
  init_pair(STATIC_ROW, COLOR_BLACK, STATIC_ROW);

  game_window = newwin(ROWS + 2, (2 * COLUMNS) + 2, 3, 3);
  piece_window = newwin(7, 12, 3, 27);
  score_window = newwin(8, 17, 13, 27);
  players_window = newwin(ROWS + 2, COLUMNS + (COLUMNS / 2), 3, 47);

  box(game_window, 0, 0);
  box(score_window, 0, 0);
  box(piece_window, 0, 0);
  box(players_window, 0, 0);

  refresh();

  display_board();
  display_score();
  display_players();
  wrefresh(game_window);
}

int TetrisGame::do_gametick(tetromino &new_piece, bool &piece_flag,
                            int &counter) {
  display_players();
  if (piece_flag) {
    new_piece = get_next_piece();
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
    if (!piece_fits(new_piece)) {
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
    if (!piece_fits(new_piece)) {
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
    if (!piece_fits(new_piece)) {
      new_piece.x -= 1;
    }
    break;
  }
  case 's':
  case 'S':
  case 'j':
  case 'J':
  case KEY_DOWN: {
    while (piece_fits(new_piece)) {
      new_piece.y += 1;
    }
    new_piece.y -= 1;
    if (insert_piece(new_piece) != 0) {
      return -1;
    };
    clear_lines();
    display_board();
    piece_flag = true;
    return 0;
    break;
  }
  default:
    break;
  }
  show_piece(new_piece);
  if (counter < 12) {
    counter += 1;
    this_thread::sleep_for(chrono::milliseconds(25));
  } else {
    counter = 0;
    new_piece.y += 1;
    if (!piece_fits(new_piece)) {
      new_piece.y -= 1;
      if (insert_piece(new_piece) != 0) {
        return -1;
      };
      clear_lines();
      display_board();
      piece_flag = true;
    }
  }
  return 0;
}

void TetrisGame::run() {

  game_setup();
  bool piece_flag = true;
  int counter = 0;
  tetromino new_piece;
  while (do_gametick(new_piece, piece_flag, counter) == 0) {
  }
  clear();
  endwin();
}

gamestate &TetrisGame::get_final_score() { return std::ref(state); }
