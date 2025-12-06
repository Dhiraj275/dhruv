#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/log.h"
#include "../include/util.h"

#define CRLF "\r\n"
#define BUFFER_SIZE 4096

typedef struct {
  char *status;
  char *reason;
  char *version;
} state_line;

typedef struct {
  char *method;
  char *uri;
  char *host;
  char *user_agent;
} http_request;

typedef struct {
  state_line status_line;
  uint32_t content_length;
  char *content_type;
} http_response;

void cleanup(char **buf) {
  free(*buf);
  *buf = NULL;
}

int parse_http_request(char *msg, http_request *req) {

  char *saveptr;
  char *line;
  line = strtok_r(msg, CRLF, &saveptr);
  char *token1_ptr;
  char *token1;

  // extract method
  token1 = strtok_r(line, " ", &token1_ptr);
  size_t token1_len = strlen(token1);
  req->method = calloc(token1_len + 1, sizeof(char));
  if (req->method == NULL) {
    goto free_alloc;
    return -1;
  }
  strncpy(req->method, token1, token1_len + 1);

  // extract uri
  char *token2;

  token2 = strtok_r(NULL, " ", &token1_ptr);
  size_t token2_len = strlen(token2);

  req->uri = calloc(token2_len + 1, sizeof(char));
  if (req->uri == NULL) {
    goto free_alloc;
    return -1;
  }

  strncpy(req->uri, token2, token2_len);

  // user_agent and host;
  char *field;
  while (line != NULL) {
    line = strtok_r(NULL, CRLF, &saveptr);
    char *token;
    char *token_ptr;

    token = strtok_r(line, " ", &token_ptr);
    while (token != NULL) {
      size_t token_len = strlen(token);
      if (strncmp(token, "Host:", token_len) == 0) {
        field = req->host;
      } else if (strncmp(token, "User-Agent:", token_len) == 0) {
        field = req->user_agent;
      } else {
        if (field == req->host) {
          req->host = calloc(token_len + 1, sizeof(char));
          if (req->host == NULL) {
            goto free_alloc;
          }
          strncpy(req->host, token, token_len + 1);
        } else if (field == req->user_agent) {
          req->user_agent = calloc(token_len + 1, sizeof(char));
          if (req->user_agent == NULL) {
            goto free_alloc;
          }
          strncpy(req->user_agent, token, token_len + 1);
        }
      }
      token = strtok_r(NULL, " ", &token_ptr);
    }
  }
  return 0;

free_alloc:
  cleanup(&req->method);
  cleanup(&req->uri);
  cleanup(&req->host);
  cleanup(&req->user_agent);
  return -1;
}

char *buffer = "HTTP/1.1 200 OK\r\n"
               "Content-Length: 5\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n"
               "hello";

// create http header
char *create_http_header(http_response *res) {
  res->status_line.status = "200";
  res->status_line.reason = "OK";
  res->status_line.version = "HTTP/1.1";

  char *response_string = calloc(res->content_length + 100, sizeof(char));
  strcat(response_string, res->status_line.version);
  strcat(response_string, " ");
  strcat(response_string, res->status_line.status);
  strcat(response_string, " ");
  strcat(response_string, CRLF);
  strcat(response_string, "Content-Type: ");
  strcat(response_string, res->content_type);
  strcat(response_string, CRLF);
  strcat(response_string, "Content-Length: ");
  char *content_len_str = calloc(33, sizeof(char));
  if (content_len_str == NULL) {
    return NULL;
  }
  sprintf(content_len_str, "%d", res->content_length);
  strcat(response_string, content_len_str);
  strcat(response_string, CRLF);
  strcat(response_string, "Connection: close");
  strcat(response_string, CRLF);
  strcat(response_string, CRLF);
  free(content_len_str);
  content_len_str = NULL;

  return response_string;
}

// handle client
int handle_client(int client_sock) {
  int recived_bytes;
  char recived_buffer[BUFFER_SIZE];
  while ((recived_bytes = recv(client_sock, recived_buffer,
                               sizeof(recived_buffer), 0)) != 0) {
    if (recived_bytes == -1) {
      return -1;
    }
    recived_buffer[recived_bytes] = '\0';
    http_request req = {0};
    if (parse_http_request(recived_buffer, &req) == -1) {
      return -1;
    }
    http_response res = {0};

    char path[512];

    if (!resolve_path(req.uri, path)) {
      send(client_sock, "HTTP/1.1 400 Bad Request\r\n", 30, 0);
      close(client_sock);
      dv_log(LOG_ERROR, "'%s %s HTTP/1.1' 400", req.method, req.uri);
      return 0;
    }

    if (!file_exists(path)) {
      send(client_sock, "HTTP/1.1 404 File Not Found\r\n", 30, 0);
      close(client_sock);
      dv_log(LOG_ERROR, "'%s %s HTTP/1.1' 404", req.method, req.uri);
      return 0;
    }

    struct stat st;
    stat(path, &st);
    res.content_type = get_mime_type(path);
    res.content_length = (uint32_t)st.st_size;
    char *buffer = create_http_header(&res);
    int fd = open(path, O_RDONLY);
    send(client_sock, buffer, strlen(buffer), 0);
    sendfile(client_sock, fd, 0, st.st_size);
    dv_log(LOG_INFO, "'%s %s HTTP/1.1' 200", req.method, req.uri);
    close(fd);
    // clean up request
    cleanup(&req.method);
    cleanup(&req.uri);
    cleanup(&req.host);
    cleanup(&req.user_agent);
    // clean up response
    cleanup(&buffer);
  }

  close(client_sock);
  return 0;
}

// #define TEST_DRIVER
#ifdef TEST_DRIVER
int main() {
  char header[1024];

  sprintf(header, "POST /main.css HTTP/1.1\r\n"
                  "Host: localhost:8080\r\n"
                  "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:145.0) "
                  "Gecko/20100101\r\n");

  http_request req;
  parse_http_request(header, &req);
  printf("the requested method is: %s \n", req.method);
  printf("the requested uri is: %s\n", req.uri);
  printf("the requested host is: %s\n", req.host);
  printf("the requested user agent is: %s\n", req.user_agent);
  free(req.method);
  free(req.uri);
  free(req.host);
  free(req.user_agent);
}
#endif
