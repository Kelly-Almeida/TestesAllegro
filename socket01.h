#include <stdbool.h>
#import <stdio.h>
#import <stdlib.h>
#include <string.h>

#ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

int socket_comuni() {
    bool round = true;
    char bufferClient[255];
    char saida[] = "FIM";

        //Inicializando o winsock
    #ifdef WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif

    struct sockaddr_in client;
    struct sockaddr_in address ={
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(5000)
    };

    int server = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));

    /*Tempo de aguardo*/
    #ifdef WIN32
        DWORD timeout = 50;
        setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
    #else
        struct timeval timeout;
        timeout.tv_sec = 0; //segundos
        timeout.tv_usec = 1000; //Milisegundos
        setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    #endif

    int cliente, x;
    int sizeClient = sizeof(client);

    if (bind(server, (struct sockaddr*)&address, sizeClient) < 0) {
        printf("Erro no bind!\n");
        return -1;
    }

    listen(server, 5);


    printf("Servidor inicializado!!\n");

    while (round) {
        cliente = accept(server, (struct sockaddr*)&address, (socklen_t*)&sizeClient);

        if (cliente < 0) {
            printf("Fim da conexão");
            round = false;
            break;
        }

        #ifdef WIN32
                setsockopt(cliente, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));
        #else
                setsockopt(cliente, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
        #endif

        x = recv(cliente, bufferClient, sizeof(bufferClient)-1, 0);

        if (x > 0) {
            bufferClient[x] = '\0';
            bufferClient[strcspn(bufferClient, "\r\n")] = '\0';

            puts(bufferClient);
            fflush(stdout);

            char response[255];
            snprintf(response, sizeof(response), "Mensagem recebida: %s\n", bufferClient);
            send(cliente, response, strlen(response), 0);

            if (strcasecmp(bufferClient, saida) == 0) {
                const char *message = "\nFim da conexão\n";
                puts(message);
                send(cliente, message, strlen(message), 0);
                round = false;
            }
        }else {
            printf("Sem mensagem");
            round = false;
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