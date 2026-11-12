#include "processClient.h"
#include <string.h>
#include <stdio.h>

#if defined(_WIN32) || defined(WIN32)
    #define strcasecmp _stricmp
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define CLOSE_SOCKET(s) closesocket(s)
    #define SEND_FLAGS 0
#else
    #include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#define CLOSE_SOCKET(s) close(s)
    #ifndef MSG_NOSIGNAL
        #define MSG_NOSIGNAL 0
    #endif
    #define SEND_FLAGS MSG_NOSIGNAL
#endif

#include <allegro5/threads.h>


#define PORT 5000

void *client(ALLEGRO_THREAD *thread, void *arg) {
    MSG_S *fila = (MSG_S *) arg;
    MSG_C chat = {0};

    #if !defined(_WIN32) && !defined(WIN32)
        signal(SIGPIPE, SIG_IGN);
    #else
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            printf("Erro ao inicializar Winsock\n");
            return NULL;
        }
    #endif

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Inicializados para que a limpeza no final seja segura mesmo se
    // nunca houver conexão (antes eram lixo de pilha -> crash ao fechar).
    ALLEGRO_THREAD *chatCliente = NULL;
    int sock = -1;

    while (!al_get_thread_should_stop(thread)) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            printf("Erro na criacao do Socket\n");
            al_rest(1.0);
            continue;
        }

        if (inet_pton(AF_INET, "10.135.113.51", &serv_addr.sin_addr) <= 0) {
            printf("Endereco invalido / nao suportado\n");
            CLOSE_SOCKET(sock);
            sock = -1;
            al_rest(1.0);
            continue;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("Falha na conexao com o servidor\n");
            CLOSE_SOCKET(sock);
            sock = -1;
            al_rest(1.0); // evita loop ocupado (100% CPU) enquanto o servidor não sobe
            continue;
        }

        printf("Cliente conectado ao servidor!\n");

            chat.contador = 0;
            chat.evento = false;
            chat.mutex = al_create_mutex();
            chat.conexao = sock;
            chat.resposta = fila->msg_c->resposta;
            chat.finalizar = false;
            TENT tent = {false, false};
            chat.tent = &tent;

            al_lock_mutex(fila->mutex);
            fila->msg_c = &chat;
            al_unlock_mutex(fila->mutex);

            chatCliente = al_create_thread(chatClient, &chat);
            al_start_thread(chatCliente);

            while (!al_get_thread_should_stop(thread) && !fila->msg_c->finalizar) {
                al_lock_mutex(fila->mutex);

                while (!fila->evento && !al_get_thread_should_stop(thread) && !fila->msg_c->tent->povoado) {
                    ALLEGRO_TIMEOUT timeout;
                    al_init_timeout(&timeout, 0.5);
                    al_wait_cond_until(fila->cond, fila->mutex, &timeout);
                }

                char msg_env[256] = "";

                strncpy(msg_env, fila->msg_in, sizeof(msg_env) - 1);
                msg_env[sizeof(msg_env) - 1] = '\0';

                fila->evento = false;
                al_unlock_mutex(fila->mutex);

                if (strlen(msg_env) > 0 || fila->msg_c->tent->povoado) {
                    char buffer_formatado[280];

                    if (fila->msg_c->tent->povoado) {
                        snprintf(buffer_formatado, sizeof(buffer_formatado), "TENT %d\n", fila->msg_c->tent->acertou);
                        al_lock_mutex(fila->mutex);
                        fila->msg_c->tent->povoado = false;
                        al_unlock_mutex(fila->mutex);
                    }else {
                        snprintf(buffer_formatado, sizeof(buffer_formatado), "%s\n", msg_env);
                    }
                    int enviou = send(sock, buffer_formatado, (int)strlen(buffer_formatado), SEND_FLAGS);
                    if (enviou <= 0) {
                        printf("Servidor encerrou a conexao! Finalizando thread do cliente.\n");
                        break;
                    }
                }

            }

            // Limpa ESTA conexão antes de tentar outra: senão cada reconexão
            // deixava uma thread chatClient antiga rodando e vazava mutex/socket.
            al_set_thread_should_stop(chatCliente);
            al_join_thread(chatCliente, NULL);
            al_destroy_thread(chatCliente);
            chatCliente = NULL;

            al_destroy_mutex(chat.mutex);
            chat.mutex = NULL;
            CLOSE_SOCKET(sock);
            sock = -1;

        printf("Desconectado do servidor :P\n");
    }

    if (chatCliente) {
        al_set_thread_should_stop(chatCliente);
        al_join_thread(chatCliente, NULL);
        al_destroy_thread(chatCliente);
        chatCliente = NULL;
    }
    if (chat.mutex) {
        al_destroy_mutex(chat.mutex);
    }
    if (sock >= 0) CLOSE_SOCKET(sock);

    #if defined(_WIN32) || defined(WIN32)
        WSACleanup();
    #endif

    return NULL;
}

void transpCliente(MSG_S *data, const char *msg) {
    al_lock_mutex(data->mutex);

    while (data->evento) {
        al_unlock_mutex(data->mutex);
        al_rest(0.01);
        al_lock_mutex(data->mutex);
    }

    strncpy(data->msg_in, msg, sizeof(data->msg_in) - 1);
    data->msg_in[sizeof(data->msg_in) - 1] = '\0';
    data->evento = true;
    printf("Mensagem no transporte: %s\n", data->msg_in);

    al_signal_cond(data->cond);
    al_unlock_mutex(data->mutex);

}

void *chatClient(ALLEGRO_THREAD *thread, void *arg) {
    MSG_C *msg = (MSG_C *)arg;

    while (!al_get_thread_should_stop(thread)) {
        char buffer_ten[256];
        al_lock_mutex(msg->mutex);
        int conexao = msg->conexao;
        al_unlock_mutex(msg->mutex);

        #if defined(_WIN32) || defined(WIN32)
                u_long mode = 1;
                ioctlsocket(conexao, FIONBIO, &mode);
                int receb = recv(conexao, buffer_ten, sizeof(buffer_ten) -1, 0);
        #else
                int receb = recv(conexao, buffer_ten, sizeof(buffer_ten) - 1, MSG_DONTWAIT);
        #endif

        if (receb > 0) {
            al_lock_mutex(msg->mutex);
            buffer_ten[receb] = '\0';

            buffer_ten[strcspn(buffer_ten, "\r\n")] = '\0';

            strcpy(msg->msg_in, buffer_ten);
            printf("%s\n", msg->msg_in);


            printf("Correios!!! %s \n", buffer_ten);
            if (strcasecmp(msg->msg_in, msg->resposta) == 0) {
                printf("Acertou!\n");
                msg->tent->acertou = true;
            }else if (strcasecmp(msg->msg_in, "FIM") == 0) {
                msg->finalizar = true;
            }else {
                msg->tent->acertou = false;

                printf("%d\n",msg->tent->acertou);
            }
            msg->tent->povoado = true;

            al_unlock_mutex(msg->mutex);
        }else {
            al_rest(0.001);
        }

    }
    return NULL;
}
