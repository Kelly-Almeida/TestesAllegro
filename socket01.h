#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
        #include <winsock2.h>
        #include <ws2tcpip.h>
#else
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <unistd.h>
#endif

int socket01() {
     bool round = true;
     const char *saida = "Meu voto é do Lobão";
     char bufferInfoClient[256];

    #ifdef WIN32
            WSADATA wsa;
            WSAStartup(MAKEWORD(2, 0), &wsa);
    #endif

    struct sockaddr_in socket_cliente;
    struct sockaddr_in enderecoMara = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(5000)
    };

    int server = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt));


#ifdef WIN32

    DWORD timeout = 50;
    setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 50000;
    setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif


    int cliente, x;
    int tamanho_tam_sock_Cliente = sizeof socket_cliente;

    if (bind(server, (struct sockaddr *) &enderecoMara, sizeof enderecoMara) < 0) {
        printf("Erro no bind!\n");
        return -1;
    }

    listen(server, 5);

    int i = 1;
    printf("Servidor aguardando conexao na porta 5000 (Timeout ativo: 0.05 segundos)...\n");

    while (i) {
        cliente = accept(server, (struct sockaddr *) &socket_cliente, &tamanho_tam_sock_Cliente);


        if (cliente < 0) {
            printf("\n[TIMEOUT] Nenhum cliente se conectou em 0.05 segundos. Fechando servidor...\n");
            i = 0;
            break;
        }


        #ifdef WIN32
            setsockopt(cliente, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        #else
            setsockopt(cliente, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
        #endif

        x = recv(cliente, bufferInfoClient, sizeof (bufferInfoClient) -1, 0);

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
                i = 0;
            }
            i = 0;
        }
        else {
            printf("\n[TIMEOUT] Cliente conectou mas nao enviou dados em 0.05 segundos. Fechando...\n");
            i = 0;
        }

        #ifdef WIN32
            closesocket(cliente);
        #else
            close(cliente);
        #endif
    }

    #ifdef WIN32
        closesocket(server);
        WSACleanup();
    #else
        close(server);
    #endif

    return 0;
}
