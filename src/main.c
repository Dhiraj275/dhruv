#include "../include/handle_client.h"
#include "../include/log.h"
#include "../include/network.h"
#include <errno.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

static int server_socket;
static volatile sig_atomic_t shutdown_requested = 0;

void handle_sigint(int sig) { shutdown_requested = 1; }

int main() {
  struct sigaction sa = {0};
  sa.sa_handler = handle_sigint;
  sigaction(SIGINT, &sa, NULL);
  server_socket = create_server_socket_fd();
  while (!shutdown_requested) {

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
  close(server_socket);
  dv_log(LOG_WARN, "Shutting down the server");
  return 0;
}
