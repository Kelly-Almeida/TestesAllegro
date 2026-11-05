
#define TESTESOCKET_SOCK_H
#include "graficos.h"
#include <allegro5/allegro.h>

typedef struct {
    bool evento;
    int contador;
    char msg[256];
    ALLEGRO_MUTEX *mutex;
    ALLEGRO_COND *cond;
}MSG_S;

typedef struct {
    char comando[5];
    VEC p1;
    int espessura;
    int corR;
    int corG;
    int corB;
    bool completa;
    char rsp_F[256];
    bool acertou;
}RESP;

typedef struct {
    char comando[5];
    VEC p1;
    int espessura;
    int corR;
    int corG;
    int corB;
}ENV;
void *sock(ALLEGRO_THREAD *thread,  void *arg);

short parseText(char *msg, int *idx);

RESP tratamento (char msg[]);
