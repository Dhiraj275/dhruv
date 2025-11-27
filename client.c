#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(){
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  
  getaddrinfo("10.255.212.46", "8080", &hints, &res);

  int app_socket = socket(AF_INET, SOCK_STREAM, 0); 
  connect(app_socket, res->ai_addr, res->ai_addrlen); 
    
  char data[4096];
  const char *buffer = "GET /index.html HTTP/1.1\r\n";

  send(app_socket, buffer, strlen(buffer), 0);
  ssize_t bytes;
  // recv(app_socket, data, 4096, 0);
  while ((bytes = recv(app_socket, data, 4096, 0)) > 0) {
    fwrite(data, 1, bytes, stdout);   // print to terminal
  }

  freeaddrinfo(res);
  
  // printf("Recieved Message: %s", data);
  return 0;
} 
