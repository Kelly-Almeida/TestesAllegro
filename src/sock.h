
#define TESTESOCKET_SOCK_H
#include "graficos.h"
#include <allegro5/allegro.h>


typedef struct {
    bool evento;
    int contador;
    char msg[256];
    ALLEGRO_MUTEX *mutex;
}MSG;

typedef struct {
    char comando[5];
    VEC p1;
    VEC p2;
    int espessura;
    COR_RGB cor;
}RESP;

void *sock(ALLEGRO_THREAD *thread,  void *arg);

short parseText(char *msg, int *idx);

RESP tratamento (char msg[]);
