#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "graficos.h"
#include "sock.h"


void *sock(void *arg) {

    MSG *fila = (MSG *)arg;

    bool round = true;
    const char *saida = "CABOOO";
    const char *limpar = "LIMPAR";
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

        /*Quando dispara a mensagem de finalização*/
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
            pthread_mutex_lock(&fila->lock);

            fila->evento = 1;

            strncpy(fila->msg, bufferInfoClient, sizeof(fila->msg));
            pthread_mutex_unlock(&fila->lock);

        }
        close(cliente);
    }
    close(server);
    return 0;
}
