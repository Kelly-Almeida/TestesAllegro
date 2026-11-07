#ifndef TESTESOCKET_SOCK_H
#define TESTESOCKET_SOCK_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include "../../grafico/graficos.h"

typedef struct {
    bool acertou;
    bool povoado;
} TENT;

typedef struct {
    ALLEGRO_MUTEX *mutex;
    ALLEGRO_COND *cond;
    bool evento;
    int contador;
    char msg_in[256];

    int conexao;
    int tam_con;
    const char *resposta;
    TENT *tent;
    bool finalizar;
} MSG_C;

typedef struct MSG_S {
    ALLEGRO_MUTEX *mutex;
    ALLEGRO_COND *cond;
    bool evento;
    int contador;
    char msg_in[256];
    bool chat_env;

    MSG_C *msg_c;
} MSG_S;


typedef struct {
    char comando[5];
    VEC p1;
    int espessura;
    int corR;
    int corG;
    int corB;

    bool acertou;
} RESP;

typedef struct {
    char comando[5];
    VEC p1;
    int espessura;
    int corR;
    int corG;
    int corB;
} ENV;

void *sock(ALLEGRO_THREAD *thread, void *arg);
short parseText(char *msg, int *idx);
RESP tratamento(char msg[]);
void *chatComun(ALLEGRO_THREAD *thread, void *arg);
void transpServer(MSG_C *data, const char *msg);

#endif // TESTESOCKET_SOCK_H