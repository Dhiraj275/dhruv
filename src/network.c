#include <netinet/in.h>
#include <sys/socket.h>

#include "../include/log.h"
#define PORT 8080

int create_server_socket_fd() {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_family = AF_INET;

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    dv_log(LOG_ERROR, "Failed to bind socket");
    return -1;
  }

  if (listen(server_socket, 100) == -1) {    
    dv_log(LOG_ERROR, "Failed to listen");
    return -1;
  }
  dv_log(LOG_INFO, "Listening on port no. %i", PORT);
  return server_socket;
}
