#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "processClient.h"

#if defined(_WIN32) || defined(WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include "processServer.h"

void *sock(ALLEGRO_THREAD *thread, void *arg) {
        MSG_S *fila = (MSG_S *)arg;

        MSG_C chat;
        chat.contador = 0;
        chat.evento = false;
        chat.mutex = al_create_mutex();

        bool round = true;
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

            chat.conexao = cliente;
            ALLEGRO_THREAD *chatzinho = al_create_thread(chatComun, &chat);
            al_start_thread(chatzinho);

            while (!al_get_thread_should_stop(thread) && round) {
                ssize_t x = recv(cliente, bufferInfoClient, sizeof(bufferInfoClient) -1, 0);
                if (x <= 0) {
                    if (x== 0) {
                        printf("Cliente desconectou\n");
                        break;
                    }
                    continue;
                }

                bufferInfoClient[x] = '\0';
                char *linha = strtok(bufferInfoClient, "\r\n");

                while (linha != NULL) {
                    if (strlen(linha) > 0) {
                        al_lock_mutex(fila->mutex);
                        while (fila->evento && !al_get_thread_should_stop(thread)) {
                            al_unlock_mutex(fila->mutex);
                            al_rest(0.001);
                            al_lock_mutex(fila->mutex);
                        }

                        fila->contador++;
                        fila->evento = true;
                        strncpy(fila->msg_in, linha, sizeof(fila->msg_in) -1);
                        fila->msg_in[sizeof(fila->msg_in) - 1] = '\0';
                        al_unlock_mutex(fila->mutex);
                    }
                    linha = strtok(NULL, "\r\n");
                }
            }
            al_set_thread_should_stop(chatzinho);
            al_join_thread(chatzinho, NULL);
            al_destroy_thread(chatzinho);

            close(cliente);
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

void *chatComun(ALLEGRO_THREAD *thread, void *arg) {
    MSG_C *chat = (MSG_C *) arg;

    while (!al_get_thread_should_stop(thread)) {
        al_lock_mutex(chat->mutex);
        bool temMsg = chat->evento;

        al_unlock_mutex(chat->mutex);

        if (temMsg) {
            al_lock_mutex(chat->mutex);

            send(chat->conexao, chat->msg_in, strlen(chat->msg_in), 0);
            printf("Mensagem de chat enviada\n");

            chat->contador++;
            chat->evento = false;

            al_unlock_mutex(chat->mutex);
        }else {
            al_rest(0.001);
        }

    }
    return NULL;
}