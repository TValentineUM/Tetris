#ifndef TPROTOCOL_H_
#define TPROTOCOL_H_

#define MESSAGE_LENGTH 256

#define CMD_LEADERBOARD regex("!leaderboard")
#define CMD_PLAYERS regex("!players")
#define CMD_PLAYERSTATS regex("!playerstats")
#define CMD_NICKNAME regex("!nickname")
#define CMD_BATTLE regex("!battle")
#define CMD_QUICK regex("!quick")
#define CMD_CHILL regex("!chill")
#define CMD_GO regex("!go")

enum tmessage_t {
  CHAT,
  NICKNAME,
  LEADERBOARD,
  PLAYER,
  PLAYERSTATS,
  BATTLE,
  QUICKPLAY,
  CHILL,
  GO,
  SCORE_UPDATE,

};

struct tmessage {
  tmessage_t message_type;
  char buffer[MESSAGE_LENGTH];

} typedef tmessage;

#endif // TPROTOCOL_H_
