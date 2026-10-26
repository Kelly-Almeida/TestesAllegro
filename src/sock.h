#ifndef TESTESOCKET_SOCK_H
#define TESTESOCKET_SOCK_H
#include "graficos.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

typedef struct {
    int evento;
    char msg[256];
    pthread_mutex_t lock;
    TRACO *traco;
}MSG;

void *sock(void *arg);

#endif
