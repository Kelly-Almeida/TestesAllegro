#include "processServer.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/threads.h>
#if defined(_WIN32) || defined(WIN32)
    #define strcasecmp _stricmp
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <BaseTsd.h>
    typedef SSIZE_T ssize_t;
    #define strtok_r strtok_s
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

void *sock(ALLEGRO_THREAD *thread, void *arg) {
        MSG_S *fila = (MSG_S *)arg;
        MSG_C *chat = fila->msg_c;
        chat->contador = 0;
        chat->evento = false;
        // chat->mutex é criado uma única vez no main(); recriar aqui
        // trocava o mutex enquanto o main podia estar segurando o antigo.

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

            #if defined (_WIN32) || defined(WIN32)
                closesocket(server);
                WSACleanup();
            #else
                close(server);
            #endif
            return NULL;
        }

        listen(server, 5);

        printf("Servidor inicializado\n");
        while (!al_get_thread_should_stop(thread) && round) {
            cliente = accept(server, (struct sockaddr *) &socket_cliente, &tam_sock_Cliente);

            if (cliente < 0) {
                continue;
            }

            chat->conexao = cliente;
            ALLEGRO_THREAD *chatzinho = al_create_thread(chatComun, chat);
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
                char *saveptr;
                char *linha = strtok_r(bufferInfoClient, "\r\n", &saveptr);

                while (linha != NULL) {
                    if (strlen(linha) > 0) {
                        al_lock_mutex(fila->mutex);
                        while (fila->evento && !al_get_thread_should_stop(thread)) {
                            al_unlock_mutex(fila->mutex);

                            /*SE NÃO FOR O MATHEUS IGNORE
                             * Capeta, abaixo está a função que dá um gap no desenho.
                             * POR HIPOTESE ALGUMA MEXA!!
                             * Se vc fazer isso vc tá fudido
                             */
                            al_rest(0.01);
                            al_lock_mutex(fila->mutex);
                        }

                        fila->contador++;
                        fila->evento = true;
                        strncpy(fila->msg_in, linha, sizeof(fila->msg_in) -1);
                        fila->msg_in[sizeof(fila->msg_in) - 1] = '\0';
                        al_unlock_mutex(fila->mutex);
                    }
                    linha = strtok_r(NULL, "\r\n", &saveptr);
                }
            }
            al_set_thread_should_stop(chatzinho);
            al_join_thread(chatzinho, NULL);
            al_destroy_thread(chatzinho);

            #if defined(_WIN32) || defined(WIN32)
                closesocket(cliente);
            #else
                close(cliente);
            #endif

        }

        #if defined(_WIN32) || defined(WIN32)
            closesocket(server);
            WSACleanup();
        #else
            close(server);
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
    int idx = 5;
    strcpy(resp.comando, comando);

    if (strcmp (resp.comando, "TRSP") != 0 && strcmp(resp.comando, "NEWP") != 0 && strcmp(resp.comando, "TENT") != 0) return resp;
    if (strlen(msg) <= 5) return resp;


    if (strcmp(resp.comando, "TENT") == 0) {
        resp.acertou = parseText(msg, &idx);
    }

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
        //bool temMsg = chat->evento;

        if (chat->evento) {
            if (chat->conexao > 0) {
                char buffer[280];
                snprintf(buffer, sizeof(buffer), "%s\n", chat->msg_in);

                ssize_t envio = send(chat->conexao, buffer, (int)strlen(buffer), 0);
                if (envio > 0) {
                    printf("Chat: %s\n", buffer);
                    chat->contador++;
                }else {
                    printf("Erro no chat\n");
                }
            }
            chat->evento = false;
        }
        al_unlock_mutex(chat->mutex);
        al_rest(0.001);
    }
    return NULL;
}

void transpServer(MSG_C *data, const char *msg) {
    al_lock_mutex(data->mutex);
    while (data->evento) {
        al_unlock_mutex(data->mutex);
        al_rest(0.001);
        al_lock_mutex(data->mutex);
    }

    strncpy(data->msg_in, msg, sizeof(data->msg_in) - 1);
    data->msg_in[sizeof(data->msg_in) - 1] = '\0';
    data->evento = true;
    printf("Mensagem no transporte: %s\n", data->msg_in);

    al_signal_cond(data->cond);
    al_unlock_mutex(data->mutex);
}
