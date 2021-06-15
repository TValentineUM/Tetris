#ifndef SERVER_H_
#define SERVER_H_

#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
using namespace std;

#define QUEUE_SIZE 5
#define MESSAGE_SIZE 255
#define PORTNO "42069"

static vector<thread> threads; /**< Stores all threads created by the server */
static std::mutex thread_list_mutex;

static map<std::string, int>
    player_sockets; /**< Map from player names to sockets */
static std::mutex player_socket_mutex;

/**
** @brief Handles Players Connecting to the Server
**
*/
void handle_connection(int);

void print_players();

/**
** @brief Launches the server
**
*/
void run_server();

#endif // SERVER_H_
