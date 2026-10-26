//
// Created by sants on 2026-10-21.
//

#ifndef TESTC_INFRA_H
#define TESTC_INFRA_H
#include "graficos.h"


typedef struct FILA {
    TRACO *tracos;
    size_t traco_atual;
    size_t capacidade;
}FILA;

FILA* criarFila();
void pushFila(FILA* filaDeTracos, TRACO *traco_novo);
void clearFila(FILA* filaDeTracos);
void destroyFila(FILA* filaDeTracos);

#endif //TESTC_INFRA_H
