#include "client.hh"
#include <cstdlib>
#include <cstring>
#include <curses.h>
#include <exception>
#include <iostream>
#include <netdb.h> // addrinfo types
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h> // Linux fds
using namespace std;

int main(int argc, char *argv[]) {

  char *hostname = argv[1];
  char *port = argv[2];
  int sockfd = establish_connection(hostname, port);

  initscr();
  cbreak();
  echo();
  curs_set(0);

  int xMax, yMax;
  getmaxyx(stdscr, yMax, xMax);
  WINDOW *chat_window = newwin(yMax - 10, xMax, 0, 0);
  WINDOW *text_window = newwin(5, xMax, yMax - 5, 0);

  box(chat_window, 0, 0);
  box(text_window, 0, 0);
  refresh();
  wmove(text_window, 1, 1);

  wrefresh(chat_window);
  wrefresh(text_window);

  struct pollfd pfds[2];
  pfds[0].fd = 0; // STDIN
  pfds[0].events = POLLIN;

  pfds[1].fd = sockfd;
  pfds[1].events = POLLIN;

  for (;;) {
    move(yMax - 4, 1);
    int poll_count = poll(pfds, 2, -1); // Wait till we have input

    if (poll_count == -1) {
      exit(1);
    } else {
      for (int i = 0; i < 2; i++) {
        if (pfds[i].revents & POLLIN) {
          if (pfds[i].fd == 0) {
            // Standard Input is ready
            char buffer[MESSAGE_LENGTH];
            memset(buffer, 0, MESSAGE_LENGTH * sizeof(char));
            getstr(buffer);
            wclear(text_window);
            box(text_window, 0, 0);
            // Checking theere is actual data to send
            if (strlen(buffer)) {
              tmessage msg = parse_message(buffer);
              if (send(sockfd, (char *)&msg, sizeof(tmessage), 0) < 0) {
                perror("ERROR writing to socket");
                close(sockfd);
                endwin();
                exit(1);
              }
            }
            wrefresh(text_window);
          } else if (pfds[i].fd == sockfd) {
            char *buffer = (char *)malloc(sizeof(tmessage));
            int numbytes = recv(sockfd, (char *)buffer, sizeof(tmessage), 0);
            if (numbytes == 0) {
              endwin();
              cout << "Server Disconnected" << endl;
              close(sockfd);
              exit(0);
            } else if (numbytes < 0) {
              endwin();
              perror("ERROR reading from socket");
              close(sockfd);
              exit(1);
            }
            tmessage *msg = (tmessage *)buffer;
            switch (msg->message_type) {
            case CHAT: {
              chat_messages.push_back(string(msg->buffer));
              wclear(chat_window);
              box(chat_window, 0, 0);

              if (chat_messages.size() > (yMax - 12)) {
                chat_messages.erase(chat_messages.begin(),
                                    chat_messages.begin() +
                                        (chat_messages.size() - (yMax - 12)));
              }
              for (int i = 0; i < chat_messages.size(); i++) {
                mvwprintw(chat_window, i + 1, 1, chat_messages[i].c_str());
                wrefresh(chat_window);
              }
            }

            default:
              break;
            }
            free(buffer);
          }
        }
      }
    }
  }
}
