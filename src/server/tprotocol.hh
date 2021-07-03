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
  LEADERBOARDS,
  PLAYERS,
  PLAYERSTATS,
  BATTLE,
  QUICKPLAY,
  CHILL,
  GAMESTATS,
  GO,
  IGNORE,
  SCORE_UPDATE,
  INIT_GAME,
  GAME_END
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
  tmessage_t message_type; /**< Enumeration denoting the type of the message*/
  int32_t arg1, arg2, arg3, arg4, arg5,
      arg6;                    /**< Optional argument sfor each message type*/
  char buffer[MESSAGE_LENGTH]; /**< Char buffer for chat messages or information
                                  to decode */

} typedef tmessage;

#endif // TPROTOCOL_H_
