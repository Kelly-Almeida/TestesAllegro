#ifndef CKECKIP
#define CHECKIP
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

#endif