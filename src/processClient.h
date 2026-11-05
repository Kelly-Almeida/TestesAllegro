// Created by sants on 2026-10-31.
//

#ifndef TESTC_SOCKCLIENT_H
#define TESTC_SOCKCLIENT_H

#include <allegro5/threads.h>

#define PORT 5000

// Declaração antecipada para o compilador conhecer o tipo sem dependência circular
typedef struct MSG_S MSG_S;

void *client(ALLEGRO_THREAD *thread, void *arg);
void transpCliente(MSG_S *data, const char *msg);
void *chatClient(ALLEGRO_THREAD *thread, void *arg);

#endif // TESTC_SOCKCLIENT_H