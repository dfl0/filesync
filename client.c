#include "common.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;     // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    struct addrinfo *server_info;
    int rc = getaddrinfo(argv[1], argv[2], &hints, &server_info);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(1);
    }

    int socket_server;
    struct addrinfo *p;

    for (p = server_info; p != NULL; p = p->ai_next) {
        socket_server = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_server == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(socket_server, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_server);
            perror("client: connect");
            continue;
        }
        break; // successfully connected
    }

    if (p == NULL)
        error("client: failed to connect");

    char ipstr[INET6_ADDRSTRLEN];
    inet_ntop(p->ai_family, get_in_addr(p->ai_addr), ipstr, sizeof ipstr);
    printf("client: connected to %s\n", ipstr);

    freeaddrinfo(server_info);

    char buf[BUFSIZE];
    while (fgets(buf, sizeof buf, stdin) != NULL) {
        size_t len = strlen(buf);

        if (send(socket_server, buf, len, 0) == -1)
            error("send");

        ssize_t n = recv(socket_server, buf, sizeof buf - 1, 0);
        if (n == -1)
            error("recv");
        if (n == 0) {
            printf("client: server closed connection\n");
            break;
        }

        buf[n] = '\0';
        printf("echo: %s", buf);
    }

    close(socket_server);
    return 0;
}
