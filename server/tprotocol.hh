#ifndef TPROTOCOL_H_
#define TPROTOCOL_H_

#include <string>

#define MESSAGE_LENGTH 512
#define RISING_TIDE 0
#define FAST_TRACK 1
#define BOOMER 2
#define CHILLER 3

enum tmessage_t : int32_t {
  CHAT,
  NICKNAME,
  LEADERBOARD,
  PLAYERS,
  PLAYERSTATS,
  BATTLE,
  QUICKPLAY,
  CHILL,
  GO,
  SCORE_UPDATE,

};

/**
 * @brief Global Structure to be used for all communications
 *
 * Basically since I only really wanna deal with 1 structure, its gonna be used
 * by every command. How individual commands use the args will be well defined
 * so that we can save server time by cosntructing the proper commands clients
 * side.
 */
struct tmessage {
  tmessage_t message_type;
  int32_t arg1, arg2, arg3, arg4;
  char buffer[MESSAGE_LENGTH];

} typedef tmessage;

#endif // TPROTOCOL_H_
