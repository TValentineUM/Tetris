#include "client.hh"
#include <iostream>
#include <random>
using namespace std;

int main(int argc, char *argv[]) {

  vector<pair<string, string>> ips;

  gamestate player1;
  switch (atoi(argv[1])) {
  case 0: {
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});

    // ips.push_back({"127.0.0.1", "42070"});
    // ips.push_back({"127.0.0.1", "42071"});
    // ips.push_back({"127.0.0.1", "42072"});
    // ips.push_back({"127.0.0.1", "42073"});
    player_no = 0;
  } break;
  case 1: {
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});

    // ips.push_back({"127.0.0.1", "42069"});
    // ips.push_back({"127.0.0.1", "42070"});
    // ips.push_back({"127.0.0.1", "42071"});
    // ips.push_back({"127.0.0.1", "42073"});
    // ips.push_back({"127.0.0.1", "42072"});
    player_no = 1;
  } break;
  case 2: {
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});

    // ips.push_back({"127.0.0.1", "42069"});
    // ips.push_back({"127.0.0.1", "42070"});
    // ips.push_back({"127.0.0.1", "42072"});
    // ips.push_back({"127.0.0.1", "42073"});
    // ips.push_back({"127.0.0.1", "42071"});
    player_no = 2;
  } break;
  case 3: {
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});

    // ips.push_back({"127.0.0.1", "42069"});
    // ips.push_back({"127.0.0.1", "42071"});
    // ips.push_back({"127.0.0.1", "42072"});
    // ips.push_back({"127.0.0.1", "42073"});
    // ips.push_back({"127.0.0.1", "42070"});
    player_no = 3;
  } break;
  case 4: {
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});
    ips.push_back({"127.0.0.1", "42069"});

    // ips.push_back({"127.0.0.1", "42070"});
    // ips.push_back({"127.0.0.1", "42071"});
    // ips.push_back({"127.0.0.1", "42072"});
    // ips.push_back({"127.0.0.1", "42073"});
    // ips.push_back({"127.0.0.1", "42069"});
    player_no = 4;
  } break;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib1(1, 4);
  std::uniform_int_distribution<> distrib2(100, 1200);

  player1.local.lines = distrib1(gen);
  player1.local.score = distrib2(gen);
  player1.player_no = atoi(argv[1]);
  communicate_state(0, player1, ips);
  return 0;
}
