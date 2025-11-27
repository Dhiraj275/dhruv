#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

const char *get_mime_type(const char *path) {

  char *type;
  char *ext = strrchr(path, '.');
  ext++;

  if (strcmp(ext, "html") == 0)
    return "text/html";
  if (strcmp(ext, "css") == 0)
    return "text/css";
  if (strcmp(ext, "js") == 0)
    return "text/javascript";
  return "application/octet-stream";
}

int main() {

  int server_socket, client_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr, client_addr;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_family = AF_INET;

  bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  listen(server_socket, 5);

  while (1) {
    client_socket = accept(server_socket, 0, 0);

    char buffer[512] = {0};
    int n = recv(client_socket, buffer, 512, 0);
    // printf("Requested Header: %s", buffer);
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

    printf("Length of file string: %lu\nThe actual string: %s \n", strlen(f),
           f);

    if (strcmp(f, "/")==0) {
      f = "/index.html";
    }

    char filepath[1024];
    snprintf(filepath, 512, "public%s", f);
    int fd = open(filepath, O_RDONLY);
    struct stat fst;
    fstat(fd, &fst);

    const char *mime_type = get_mime_type(f);
    char http_header[1024];
    sprintf(http_header,
            "HTTP/1.1 200 OK \r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "\r\n",
            mime_type, fst.st_size);

    write(client_socket, http_header, strlen(http_header));
    sendfile(client_socket, fd, NULL, fst.st_size);
    // #endif
    close(client_socket);

  }

  close(server_socket);

  return 0;
}
