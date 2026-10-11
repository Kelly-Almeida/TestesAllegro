#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32 //Em caso do sistema operacional ser Windows
        #include <winsock2.h> //Biblioteca do Windows para trazer os comandos do socket
        #include <ws2tcpip.h> //Bibliotea para suportar o IPv6

#else
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <unistd.h> //Para o Linux Usarmos a função close
#endif

void test() {
     bool round = true;
     const char *saida = "Meu voto é do Lobão";

    char bufferInfoClient[256]; //Buffer para guardar os dados enviados para o cliente

    #ifdef WIN32
            WSADATA wsa; //Cria uma struct WSADATA que terá as variáveis de inicialização do Winsockets
            WSAStartup(MAKEWORD(2, 0), &wsa);//Inicaliza a biblioteca de de sockets do windows com a versão 2.0 (pelo comando MAKEWORD), essa ação é feita também passando o ponteiro de wsa criado anteriormente
    #endif

    struct sockaddr_in socket_cliente; //retorno um novo socket criado que descreve o so do cliente

    struct sockaddr_in enderecoMara = { //Estrutura de endereco onde o servidor var usar para registrar a rede
        .sin_family = AF_INET, //Define a Familia de protocolos (No caso IPv4[Não é a mais moderna, mas vai])
        .sin_addr.s_addr = htonl(INADDR_ANY), //Define os endereços que serão aceitos (No caso qualquer um)
        .sin_port =htons(5000) //Define a porta que será usada

        /*htonl e htons convertem os valores passados para um formato correto*/
    };

    //Criando Um socket ;>
    int server = socket(AF_INET, SOCK_STREAM, 0); //Para criar um socket é necessário a família de protocolos,Tipo de socket e o  protolo específico
    /*
     * No caso do exemplo acima:
     * O socket usa rede IPv4, protocolo TCP e apresenta a escolha de protocolo automática (IPv4)
     */

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt));

        /*
         * O comando abaixo obriga o sistema a reutilizar a porta (SIM, se não tiver isso vai
         * rodar e quando recompilar não vai ;< a IA [Gemini ai] me explicou fiquei 1 HORA reniciando
         * IDE, quase cometi o oposto de viver)
         *
         * fd: o servidor
         * level: nível do protocolor (SOL_SOCKET -> IP ou TCP)
         * optname: Nome que diz que retiuliza a porta
         * optvalue: onde vai mecher
         * oplen: tamanho da opção a ser mechida
         */
    int cliente, x;
    int tamanho_tam_sock_Cliente = sizeof(socket_cliente);


    //bind(server, (struct sockaddr *) &enderecoMara, sizeof enderecoMara); //La vem explicação grande...
    /*
     * O bind vincula o socket ao endereço IP e a porta configurada;
     * a parte (struct sockaddr *) converte a estrutura IPv4 para um ponteito genérico para a função bind
     */
     if (bind(server, (struct  sockaddr *)&enderecoMara, sizeof(enderecoMara)) < 0) {
             printf("Erro ao fazer bind na porta 5000\n");
             return;
     }


    listen(server, 5); //Ditamos a quantidade conexões aceitas
    printf("Servidor runpendo\n");

    while (round) {
        cliente = accept(server, (struct sockaddr *) &socket_cliente, &tamanho_tam_sock_Cliente); //Registra a conexão entre cliente e servidor, guardando as informções do cliente em um socket


        x = recv(cliente, bufferInfoClient, sizeof (bufferInfoClient) -1, 0); //Serve para receber os dados do cliente e tacar no buffer, Retorna o tamanho dos dados recebidos

                /*Curiosidades do Titio Gemnini (a IA só usada com suporte para dúvidas e curiosidades,
        *Ministerio do Saguim Adverte uso para ajudar na preguiça e burrice)
        *
        * Curiosidade sobre o parametro flags (o quarto parametro de recv)
        *
        * Quando assumi o valor de:
       * "Quando você passa 0, está dizendo ao sistema para usar o comportamento padrão, que é:
               Bloquear a execução do programa até que chegue algum dado do cliente.
               Copiar os dados da memória do sistema (kernel buffer) para a sua variável (bufferInfoClient).
               Remover esses dados da memória do sistema assim que forem lidos (para que a próxima chamada do recv leia a próxima mensagem)."
       "MSG_PEEK (Espiar):
               Lê os dados do cliente e copia para o seu buffer, mas não os apaga da fila do sistema. Uma chamada posterior a recv() lerá exatamente os mesmos dados novamente. Útil para inspecionar o cabeçalho de uma mensagem sem consumi-la.
       MSG_WAITALL (Esperar preenchimento total):
               Por padrão, o recv() retorna assim que chega qualquer quantidade de dados do cliente (mesmo que seja apenas 1 byte). Ao passar MSG_WAITALL, a função bloqueia a execução até preencher exatamente a quantidade de bytes informada no sizeof bufferInfoClient.
       MSG_DONTWAIT (Modo Não-Bloqueante estático):
               Faz a função recv() retornar imediatamente. Se não houver dados disponíveis do cliente naquele exato milissegundo, ela não fica travada esperando; em vez disso, retorna -1 com o erro EWOULDBLOCK ou EAGAIN.
       Combinando Flags:
               É possível combinar mais de um comportamento usando o operador ou lógico (|), por exemplo: MSG_PEEK | MSG_DONTWAIT."
        */

        if (x > 0) {
                bufferInfoClient[x] = '\0';//Como recv não coloca um "ponto final" nessa frase, temos que ccolocar manualmente

                //bufferInfoClient[strcspn(bufferInfoClient, "\r\n")] = '\0';

                puts(bufferInfoClient); //Printa do terminal a mensagem
                fflush(stdout); //stdout limpa os buffer, e o fflush imprime imediatamente a mensagem


                /*char resposta[512];

                snprintf(resposta, sizeof (resposta), "Messagem envida: %d", x);

                send(cliente,resposta,strlen(resposta),0);
*/
                send(cliente, bufferInfoClient, x, 0);//Envia a mesagem de volta ao cliente, demonstrando se a comunicação deu bom ou mióend(cliente, bufferInfoClient, x, 0);//Envia a mesagem de volta ao cliente, demonstrando se a comunicação deu bom ou mió


                bufferInfoClient[strcspn(bufferInfoClient, "\r\n")] = '\0';

                if (strcmp(bufferInfoClient, saida) == 0) {
                        const char *message = "Fim conexão\n";
                        puts(message);

                        send(cliente, message, strlen(message), 0);

                        round = false;

                        #ifdef WIN32
                                closesocket(cliente);
                        #else
                                close(cliente);
                        #endif
                        break;
                }
        }


        #ifdef WIN32
        closesocket(cliente); //encerra do cliente no Windowns
        #else
        close(cliente); //encerra a conexão LINUX
        #endif


    }
#ifdef WIN32
        closesocket(cliente); //encerra do cliente no Windowns
        WSACleanup(); //Limpa a biblioteca Winsocket

#else
        close(cliente); //encerra a conexão LINUX
#endif

}
