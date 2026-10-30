
#define TESTESOCKET_SOCK_H
#include "graficos.h"

#if defined(_WIN32) || defined(WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>

    typedef struct {
        int evento;
        char msg[256];
        CRITICAL_SECTION lock;
        TRACO *traco;
    }MSG;
DWORD WINAPI sock(void *arg);
#else
#include <pthread.h>

typedef struct {
    int evento;
    char msg[256];
    pthread_mutex_t lock;
    TRACO *traco;
}MSG;
void *sock(void *arg);
#endif
