
// test_client.c
// Simple client that connects and sends multiple crafted GETs.
// Build: gcc -o test_client test_client.c
// Usage: ./test_client <server-ip> <port> <case-number>
// case-number: 1..7 as per tests

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const char *cases[] = {
    // 0 unused
    "",
    // 1 canonical traversal
    "GET /../server.c HTTP/1.1\r\nHost: test\r\nConnection: close\r\n\r\n",
    // 2 url-encoded traversal
    "GET /%2e%2e/server.c HTTP/1.1\r\nHost: test\r\nConnection: close\r\n\r\n",
    // 3 mixed encoding
    "GET /.%2e/server.c HTTP/1.1\r\nHost: test\r\nConnection: close\r\n\r\n",
    // 4 double-encoded
    "GET /%252e%252e/server.c HTTP/1.1\r\nHost: test\r\nConnection: close\r\n\r\n",
    // 5 null byte attempt
    "GET /..%00/server.c HTTP/1.1\r\nHost: test\r\nConnection: close\r\n\r\n",
    // 6 backslash
    "GET /..\\server.c HTTP/1.1\r\nHost: test\r\nConnection: close\r\n\r\n",
    // 7 simple ok (index)
    "GET /index.html HTTP/1.1\r\nHost: test\r\nConnection: close\r\n\r\n"
};

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <server-ip> <port> <case# 1-7>\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int tcase = atoi(argv[3]);
    if (tcase < 1 || tcase > 7) { fprintf(stderr, "case must be 1..7\n"); return 1; }

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return 1; }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) { perror("inet_pton"); return 1; }

    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("connect"); close(s); return 1; }

    const char *buf = cases[tcase];
    write(s, buf, strlen(buf));

    char rbuf[4096];
    ssize_t n;
    while ((n = read(s, rbuf, sizeof(rbuf))) > 0) {
        fwrite(rbuf, 1, n, stdout);
    }
    close(s);
    return 0;
}
