#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BACKLOG 10
#define BUFSIZE 1024

static inline void error(const char *msg)
{
    perror(msg);
    exit(1);
}

static inline void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &((struct sockaddr_in  *)sa)->sin_addr;
    return     &((struct sockaddr_in6 *)sa)->sin6_addr;
}

#endif // COMMON_H
