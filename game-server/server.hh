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

static vector<thread>
    thread_list; /**< Stores all threads created by the server */
static std::mutex thread_list_mutex;

static map<int, std::string> socket_player;
static std::mutex player_mutex;

void relay(std::string);     /**< Send the message to all clients connected*/
void handle_connection(int); /**< Handles individual clients on a new thread*/
string random_string(size_t length); /**< Creates random strings */
void run_server(int);                /**< Lanuches the server */

#endif // SERVER_H_
