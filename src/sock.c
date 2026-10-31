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

        MSG *fila = (MSG *)arg;

        bool round = true;
        const char *saida = "CABOOO";
        const char *limpar = "LIMPAR";
        const char *desfazer = "UNDO";
        char bufferInfoClient[256];

        struct sockaddr_in socket_cliente;
        struct sockaddr_in enderecoMara = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = htonl(INADDR_ANY),
            .sin_port = htons(5000)
        };
        int server = socket(AF_INET, SOCK_STREAM, 0);

        if (server <0) {
            printf("Erro ao criar o socket\n");
            return 0;
        }

        int opt = 1;
        setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt));

        int cliente;
        socklen_t tam_sock_Cliente = sizeof socket_cliente;

        if (bind(server, (struct sockaddr *) &enderecoMara, sizeof enderecoMara) < 0) {
            printf("Erro no bind!\n");
            close(server);
            return 0;
        }

        listen(server, 5);

        printf("Servidor inicializado\n");

        while (round) {
            cliente = accept(server, (struct sockaddr *) &socket_cliente, &tam_sock_Cliente);


            if (cliente < 0) {
                printf("Erro no accept");
                break;
            }

            ssize_t x = recv(cliente, bufferInfoClient, sizeof(bufferInfoClient) -1, 0);

            if (x > 0) {
                bufferInfoClient[x] = '\0';
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

                strncpy(fila->msg, bufferInfoClient, sizeof(fila->msg));
                al_unlock_mutex(fila->mutex);

            }
            close(cliente);
        }
        close(server);
        return 0;
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

    if (strcmp (resp.comando, "TRSP") != 0) return resp;
    if (strlen(msg) <= 5) return resp;

    int idx = 5;

    resp.p1.x = parseText(msg, &idx);
    resp.p1.y = parseText(msg, &idx);
    resp.p2.x = parseText(msg, &idx);
    resp.p2.y = parseText(msg, &idx);
    resp.espessura = parseText(msg, &idx);

    while (msg[idx] == ' ') (idx)++;

    char cor[16];
    int k = 0;
    while (msg[idx] != ' ' && msg[idx] != '\0' && msg[idx] != '\r' && msg[idx] != '\n' && k < 15) {
        cor[k] = msg[idx];
        k++;
        idx++;
    }
    cor[k] = '\0';

    COR_RGB VERDE = {172, 225, 57};
    COR_RGB VERMELHO = {255, 0, 0};
    COR_RGB AZUL = {0, 0, 255};
    COR_RGB AMARELO = {255, 255, 0};
    COR_RGB BRANCO = {255, 255, 255};
    COR_RGB PRETO = {0, 0, 0};
    COR_RGB LARANJA = {255, 128, 0};
    COR_RGB ROXO = {128, 0, 255};

    if (strcmp(cor, "VERDE") == 0) resp.cor = VERDE;
    else if (strcmp(cor, "VERMELHO") == 0) resp.cor = VERMELHO;
    else if (strcmp(cor, "AZUL") == 0) resp.cor = AZUL;
    else if (strcmp(cor, "AMARELO") == 0) resp.cor = AMARELO;
    else if (strcmp(cor, "BRANCO") == 0) resp.cor = BRANCO;
    else if (strcmp(cor, "LARANJA") == 0) resp.cor = LARANJA;
    else if (strcmp(cor, "ROXO") == 0) resp.cor = ROXO;
    else resp.cor = PRETO;

    return resp;
}
