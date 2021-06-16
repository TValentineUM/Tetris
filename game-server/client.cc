#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

int main(int argc, char *argv[]) {
  // Declare variables
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];

  // Make sure that server name and port are available in command line arguments
  if (argc < 3) {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }

  // Get port number
  portno = atoi(argv[2]);

  // Create a socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  // Get server name
  if ((server = gethostbyname(argv[1])) == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    close(sockfd);
    exit(0);
  }

  // Populate serv_addr structure
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET; // Set to AF_INET
  memcpy(&serv_addr.sin_addr.s_addr,
         server->h_addr, // Set server address
         server->h_length);
  serv_addr.sin_port =
      htons(portno); // Set port (convert to network byte ordering)

  // Connect to the server
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    close(sockfd);
    exit(1);
  }

  memset(buffer, 0, 256);
  if ((n = read(sockfd, buffer, 255)) < 0) {
    perror("ERROR reading from socket");
    close(sockfd);
    exit(1);
  }
  cout << string(buffer) << endl;

  // Ask for a message from the user
  // printf("Please enter the message: ");
  memset(buffer, 0, 256);
  cin >> buffer;
  // fgets(buffer, 255, stdin);

  // Send message to the server
  if ((n = write(sockfd, buffer, strlen(buffer))) < 0) {
    perror("ERROR writing to socket");
    close(sockfd);
    exit(1);
  }

  memset(buffer, 0, 256);
  if ((n = read(sockfd, buffer, 255)) < 0) {
    perror("ERROR reading from socket");
    close(sockfd);
    exit(1);
  }
  // Read response from server response
  printf("%s\n", buffer);

  while (1)
    ;

  // All done, close socket
  close(sockfd);
  return 0;
}
