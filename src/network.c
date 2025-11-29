#include <sys/socket.h>   
#include <netinet/in.h>
#include <stdio.h>

#define PORT 8080


int create_server_socket_fd(){
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket==-1){
    return 1;
  }

  struct sockaddr_in server_addr, client_addr;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_family = AF_INET;

  if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))==-1){
    printf("bind: error");
    return -1;
  }
  
  if(listen(server_socket, 5)==-1){
    printf("listen: error");
    return -1;
  }
  return server_socket;
}
