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
        const char *saida = "CABOOO";
        const char *limpar = "LIMPAR";
        const char *desfazer = "UNDO";
        char bufferInfoClient[256];

        #if defined(_WIN32) || defined(WIN32)
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                printf("Erro ao inicializar a WSASDATA()\n");
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
            printf("Erro ao criar o socket\n");
            return NULL;
        }

        int opt = 1;
        setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt));

        #if defined(_WIN32) || defined(WIN32)
            DWORD tv = 1000;
            setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        #else
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        #endif

        int cliente;
        socklen_t tam_sock_Cliente = sizeof socket_cliente;

        if (bind(server, (struct sockaddr *) &enderecoMara, sizeof enderecoMara) < 0) {
            printf("Erro no bind!\n");
            close(server);
            return NULL;
        }

        listen(server, 5);

        printf("Servidor inicializado\n");

        while (!al_get_thread_should_stop(thread) && round) {
            cliente = accept(server, (struct sockaddr *) &socket_cliente, &tam_sock_Cliente);

            if (cliente < 0) {
                continue;
            }
            while (!al_get_thread_should_stop(thread) && round) {
                ssize_t x = recv(cliente, bufferInfoClient, sizeof(bufferInfoClient) -1, 0);
                if (x <= 0) break;
/*
                bufferInfoClient[x] = '\0';
                char *linha = strtok(bufferInfoClient, "\r\n");

                while (linha != NULL) {
                    if (strlen(linha) > 0) {
                        al_lock_mutex(fila->mutex);
                        while
                    }
                }
*/
                bufferInfoClient[strcspn(bufferInfoClient, "\r\n")] = '\0';

                puts(bufferInfoClient);
                fflush(stdout);

                char resposta[512];
                snprintf(resposta, sizeof(resposta), "Mensagem enviada: %s\n", bufferInfoClient);
                send(cliente, resposta, (int)strlen(resposta), 0);

                if (strcmp(bufferInfoClient, saida) == 0) {
                    const char *message = "\nFim da conexao\n";
                    puts(message);
                    send(cliente, message, (int)strlen(message), 0);
                    round = false;
                }
                if (strcmp(bufferInfoClient, limpar) == 0) {
                    const char *message = "\nlimpar a tela remotamente\n";
                    puts(message);
                    send(cliente, message, (int)strlen(message), 0);
                }
                if (strcmp(bufferInfoClient, desfazer) == 0) {
                    const char *message = "\nCtrl + z\n";
                    puts(message);
                    send(cliente, message, (int)strlen(message), 0);
                }
                al_lock_mutex(fila->mutex);

                fila->contador++;
                fila->evento = 1;

                strncpy(fila->msg, bufferInfoClient, sizeof(fila->msg) - 1);
                fila->msg[sizeof(fila->msg) - 1] = '\0';
                al_unlock_mutex(fila->mutex);
            }

            close(cliente);
        }
        close(server);
        #if defined(_WIN32) || defined(WIN32)
            WSACleanup();
        #endif
        return NULL;
    }


short parseText(char *msg, int *idx) {
    char *cord;
    int num = 12;
    cord = (char *) malloc(num * sizeof(char));
    int k = 0;

    bool scanNum = true;
    while (scanNum) {
        if (msg[*idx] != ' ' && msg[*idx] != '\0') {
            cord[k] = msg[*idx];
            k++;
            (*idx)++;
        }else {
            scanNum = false;
            if (msg[*idx] == ' ') (*idx)++;
        }
    }

    cord[k] = '\0';
    short resp = (short) atoi(cord);
    free(cord);
    return resp;
}

RESP tratamento (char msg[]) {
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
    resp.p1.y = parseText(msg, &idx);
    resp.espessura = parseText(msg, &idx);
    resp.corR = parseText(msg, &idx);
    resp.corG = parseText(msg, &idx);
    resp.corB = parseText(msg, &idx);

    return resp;
}
