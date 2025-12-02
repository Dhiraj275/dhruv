#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/network.h"
#include "../include/handle_client.h"

//todo: move this to saparate header file. utils.h maybe
const char *get_mime_type(const char *path);
bool file_exists(char *path);
bool resolve_path(const char *path, char *out);

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
  
//code that will be implemented in handle client.c file
#if 0
    char buffer[512] = {0};
    int n = recv(client_socket, buffer, 512, 0);
    printf("Requested Header: %s", buffer);
    if (n <= 0) {
      close(client_socket);
      continue;
    }
    if (n < 4) {
      close(client_socket);
      continue;
    }

    char *f = buffer + 4;

    *strchr(f, ' ') = 0;
    char filename[512];

    if (f[strlen(f) - 1] == '/') {
      f = "index.html";
    } else {
      f++;
    }
    printf("raw request path: %s \n", f);
    if (!resolve_path(f, filename)) {
      send(client_socket, "HTTP/1.1 400 Bad Request", 25, 0);
      close(client_socket);
      continue;
    }

    printf("resolved filename: %s \n", filename);
    struct stat st;
    if (stat(filename, &st) == -1) {
      send(client_socket, "HTTP/1.1 404 File Not Found", 28, 0);
      close(client_socket);
      continue;
    }
    // if (strcmp(filename, "/") == 0) {
    //   f = "/index.html";
    // }

    int fd = open(filename, O_RDONLY);
    const char *mime_type = get_mime_type(f);
    char http_header[1024];
    sprintf(http_header,
            "HTTP/1.1 200 OK \r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "\r\n",
            mime_type, st.st_size);

    write(client_socket, http_header, strlen(http_header));
    sendfile(client_socket, fd, 0, st.st_size);
    close(fd);
    #endif
  }

  close(server_socket);

  return 0;
}
