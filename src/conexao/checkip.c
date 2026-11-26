/*
 * Obtém o IP privado (IPv4) da interface Ethernet e grava em um buffer.
 *
 * Compilar:
 *   Linux:           gcc ip_privado.c -o ip_privado
 *   Windows (MSVC):  cl ip_privado.c        (as libs são linkadas via #pragma)
 *   Windows (MinGW): gcc ip_privado.c -o ip_privado.exe -liphlpapi -lws2_32
 */

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 /* Vista+: necessário para inet_ntop */
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdlib.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#else
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <string.h>

 /*
  * Grava em `buf` o IP (ex.: "10.0.0.5") da primeira interface Ethernet encontrada.
  * `tamanho` deve ser pelo menos INET_ADDRSTRLEN (16).
  * Retorna 0 em caso de sucesso, -1 se não encontrou ou em caso de erro.
  */
int obter_ip_ethernet(char* buf, size_t tamanho);

#ifdef _WIN32
/* ===================== Windows ===================== */
int obter_ip_ethernet(char* buf, size_t tamanho) {
    ULONG tam_lista = 15000; /* tamanho inicial recomendado pela Microsoft */
    IP_ADAPTER_ADDRESSES* adaptadores = NULL;
    ULONG ret;
    int tentativas = 0;
    int resultado = -1;
    ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

    if (buf == NULL || tamanho < INET_ADDRSTRLEN) return -1;
    buf[0] = '\0';

    /* O buffer pode ser pequeno demais: a API devolve o tamanho certo e tentamos de novo */
    do {
        adaptadores = (IP_ADAPTER_ADDRESSES*)malloc(tam_lista);
        if (adaptadores == NULL) return -1;
        ret = GetAdaptersAddresses(AF_INET, flags, NULL, adaptadores, &tam_lista);
        if (ret == ERROR_BUFFER_OVERFLOW) {
            free(adaptadores);
            adaptadores = NULL;
        }
    } while (ret == ERROR_BUFFER_OVERFLOW && ++tentativas < 3);

    if (ret != NO_ERROR) {
        free(adaptadores);
        return -1;
    }

    for (IP_ADAPTER_ADDRESSES* a = adaptadores; a != NULL && resultado != 0; a = a->Next) {
        /* Só interfaces Ethernet ativas */
        if (a->OperStatus != IfOperStatusUp) continue;
        if (a->IfType != IF_TYPE_ETHERNET_CSMACD) continue;

        IP_ADAPTER_UNICAST_ADDRESS* u = a->FirstUnicastAddress;
        if (u == NULL) continue;

        struct sockaddr_in* pAddr = (struct sockaddr_in*)u->Address.lpSockaddr;
        if (inet_ntop(AF_INET, &pAddr->sin_addr, buf, tamanho) != NULL) {
            resultado = 0;
        }
    }

    free(adaptadores);
    return resultado;
}

#else
/* ===================== Linux / Unix ===================== */

/* Retorna 1 se o nome é de uma interface Ethernet, 0 caso contrário.
 *   en  -> enp3s0, eno1, ens33 (nomes previsíveis do systemd)
 *   eth -> eth0                (esquema antigo) */
static int eh_ethernet(const char* nome) {
    static const char* prefixos[] = { "en", "eth" };
    size_t i;

    for (i = 0; i < sizeof(prefixos) / sizeof(prefixos[0]); i++) {
        if (strncmp(nome, prefixos[i], strlen(prefixos[i])) == 0) {
            return 1;
        }
    }
    return 0;
}

int obter_ip_ethernet(char* buf, size_t tamanho) {
    struct ifaddrs* ifaddr, * ifa;
    int resultado = -1;

    if (buf == NULL || tamanho < INET_ADDRSTRLEN) return -1;
    buf[0] = '\0';

    if (getifaddrs(&ifaddr) == -1) return -1;

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (ifa->ifa_addr->sa_family != AF_INET) continue;
        if (!eh_ethernet(ifa->ifa_name)) continue;

        struct sockaddr_in* pAddr = (struct sockaddr_in*)ifa->ifa_addr;
        if (inet_ntop(AF_INET, &pAddr->sin_addr, buf, (socklen_t)tamanho) != NULL) {
            resultado = 0;
            break;
        }
    }

    freeifaddrs(ifaddr);
    return resultado;
}
#endif
