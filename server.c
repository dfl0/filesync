#include "common.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    struct addrinfo hints = {
        .ai_family   = AF_UNSPEC,   // IPv4 or IPv6
        .ai_socktype = SOCK_STREAM, // TCP
        .ai_flags    = AI_PASSIVE,  // fill in own IP automaticall
    };

    struct addrinfo *self_info;
    int rc = getaddrinfo(NULL, argv[1], &hints, &self_info);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(1);
    }

    int socket_client;
    struct addrinfo *p;

    for (p = self_info; p != NULL; p = p->ai_next) {
        socket_client = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_client == -1) {
            perror("server: socket");
            continue;
        }

        int yes = 1;
        if (setsockopt(socket_client, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
            error("setsockopt");

        if (bind(socket_client, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_client);
            perror("server: bind");
            continue;
        }

        break; /* successfully bound */
    }

    if (p == NULL)
        error("server: failed to bind");

    freeaddrinfo(self_info);

    if (listen(socket_client, BACKLOG) == -1)
        error("listen");

    printf("server: waiting for connections on port %s...\n", argv[1]);

    while (1) {
        struct sockaddr_storage client_addr;
        socklen_t addrlen = sizeof client_addr;

        int connfd = accept(socket_client, (struct sockaddr *)&client_addr, &addrlen);
        if (connfd == -1) {
            perror("accept");
            continue;
        }

        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr *)&client_addr),
                  ipstr, sizeof ipstr);
        printf("server: connection from %s\n", ipstr);

        char buf[BUFSIZE];
        ssize_t n;

        while ((n = recv(connfd, buf, sizeof buf, 0)) > 0) {
            if (send(connfd, buf, (size_t)n, 0) == -1) {
                perror("send");
                break;
            }
        }

        if (n == -1)
            perror("recv");

        close(connfd);
        printf("server: connection closed\n");
    }

    close(socket_client);
    return 0;
}
