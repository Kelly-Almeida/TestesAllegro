#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include "sock.h"
void *sock(ALLEGRO_THREAD *thread, void *arg) {
    MSG_S *fila = (MSG_S *)arg;
    bool round = true;
    char bufferInfoClient[1024];

#if defined(_WIN32) || defined(WIN32)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erro no WSAStartup\n");
        return NULL;
    }
#endif

    struct sockaddr_in socket_cliente;
    struct sockaddr_in enderecoMara = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(5000)
    };

    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        printf("Erro ao criar socket do servidor\n");
        return NULL;
    }

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    socklen_t tam_sock_Cliente = sizeof(socket_cliente);

    if (bind(server, (struct sockaddr *)&enderecoMara, sizeof(enderecoMara)) < 0) {
        printf("Erro no bind!\n");
        close(server);
#if defined(_WIN32) || defined(WIN32)
        WSACleanup();
#endif
        return NULL;
    }

    listen(server, 5);
    printf("Servidor ativo e escutando na porta 5000...\n");

    while (!al_get_thread_should_stop(thread) && round) {
        int cliente = accept(server, (struct sockaddr *)&socket_cliente, &tam_sock_Cliente);
        if (cliente < 0) continue;

        printf("Cliente conectado\n");

        // LOOP DE LEITURA CONTÍNUA
        while (!al_get_thread_should_stop(thread) && round) {
            ssize_t x = recv(cliente, bufferInfoClient, sizeof(bufferInfoClient) - 1, 0);

            // Se der erro/timeout, verifica se foi erro real ou apenas ausência de dados momentânea
            if (x <= 0) {
                // Em conexões TCP normais, x == 0 significa que o cliente fechou a conexão explicitamente.
                if (x == 0) {
                    printf("Cliente fechou a conexao.\n");
                    break;
                }
                // Se x < 0, pode ser apenas o timeout do SO_RCVTIMEO (sem dados no momento)
                // Continuamos o loop para manter a conexão aberta!
                continue;
            }

            bufferInfoClient[x] = '\0';

            //send(cliente, "njbwef", strlen("njbwef"), 0);
            printf("[Servidor Recv]: %s", bufferInfoClient); // Log para depuração

            char *saveptr;
            char *linha = strtok_r(bufferInfoClient, "\r\n", &saveptr);

            while (linha != NULL) {
                if (strlen(linha) > 0) {
                    al_lock_mutex(fila->mutex);

                    // Espera a main consumir a mensagem anterior
                    while (fila->evento && !al_get_thread_should_stop(thread)) {
                        al_unlock_mutex(fila->mutex);
                        al_rest(0.005);
                        al_lock_mutex(fila->mutex);
                    }

                    fila->contador++;
                    fila->evento = true;
                    strncpy(fila->msg, linha, sizeof(fila->msg) - 1);
                    fila->msg[sizeof(fila->msg) - 1] = '\0';

                    al_unlock_mutex(fila->mutex);
                }
                linha = strtok_r(NULL, "\r\n", &saveptr);
            }
        }

        close(cliente);
        printf("Sessao com o cliente encerrada.\n");
    }

    close(server);
#if defined(_WIN32) || defined(WIN32)
    WSACleanup();
#endif
    return NULL;
}

short parseText(char *msg, int *idx) {
    char cord[16];
    int k = 0;
    while (msg[*idx] != '\0' && msg[*idx] == ' ') {
        (*idx)++;
    }

    if (msg[*idx] == '\0') return 0;

    while (msg[*idx] != '\0' && msg[*idx] != ' ' && k < 15) {
        cord[k++] = msg[*idx];
        (*idx)++;
    }
    cord[k] = '\0';

    if (msg[*idx] == ' ') (*idx)++;

    return (short)atoi(cord);
}
RESP tratamento (char msg[]/*, RESP respAnt*/) {
    printf("%s\n", msg);
    RESP resp;
    char comando[5];

    if (strlen(msg) < 4) {
        strcpy(resp.comando, "NADA");
        return resp;
    }

    for (int i = 0; i < 5; i++) {
        comando[i] = msg[i];
        if (i == 4) comando[i] = '\0';
    }

    strcpy(resp.comando, comando);

    if (strcmp (resp.comando, "TRSP") != 0 && strcmp(resp.comando, "NEWP") != 0) return resp;
    if (strlen(msg) <= 5) return resp;

    int idx = 5;

    resp.p1.x = parseText(msg, &idx);
    //if (abs(respAnt.p1.x - resp.p1.x) > 50) resp.p1.x = respAnt.p1.x;

    resp.p1.y = parseText(msg, &idx);
    //if (abs(respAnt.p1.y - resp.p1.y) > 50) resp.p1.y = respAnt.p1.y;

    resp.espessura = parseText(msg, &idx);
    resp.corR = parseText(msg, &idx);
    resp.corG = parseText(msg, &idx);
    resp.corB = parseText(msg, &idx);

    resp.completa = true;

    return resp;
}
