#ifndef TPROTOCOL_H_
#define TPROTOCOL_H_

#define MESSAGE_LENGTH 256

enum tmessage_t {
  CHAT,
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
