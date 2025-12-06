#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>

#include "../include/network.h"
#include "../include/handle_client.h"

int main() {
  int server_socket = create_server_socket_fd();

  while (1) {

    int client_socket = accept(server_socket, 0, 0);

    if (client_socket == -1) {
      if (errno == EINTR) {
        continue;
      }
      perror("accept");
      continue;
    }
    
    handle_client(client_socket);
  }
  return 0;
}
