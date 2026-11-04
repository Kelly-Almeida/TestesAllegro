//
// Created by sants on 2026-10-31.
//

#ifndef TESTC_SOCKCLIENT_H
#define TESTC_SOCKCLIENT_H
#include <allegro5/threads.h>

#endif //TESTC_SOCKCLIENT_H
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000

void *client (ALLEGRO_THREAD *thread, void *arg);
void transpCliente(MSG_S *data, const char *msg);