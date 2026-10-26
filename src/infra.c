//
// Created by sants on 2026-10-21.
//

#include <stdlib.h>
#include <stdio.h>
#include "graficos.h"
#include "infra.h"


FILA* criarFila() {
    FILA* filaNova = malloc(sizeof(FILA));
    if (!filaNova) return NULL;

    filaNova->traco_atual = 0;
    filaNova->capacidade = 16;
    filaNova->tracos = malloc(filaNova->capacidade * sizeof(TRACO));

    if (!filaNova->tracos) {
        free(filaNova);
        return NULL;
    }
    return filaNova;
}

void pushFila(FILA* filaDeTracos, TRACO *traco_novo) {
    if (filaDeTracos->traco_atual >= filaDeTracos->capacidade) {
        filaDeTracos->capacidade *= 2; //dobra a capacidade da fila
        TRACO* temp = realloc(filaDeTracos->tracos, filaDeTracos->capacidade * sizeof(TRACO));
        if (temp == NULL) {
            printf("Erro ao realocar memória!!");
            exit(EXIT_FAILURE);
        }
        filaDeTracos->tracos = temp;
    }
    filaDeTracos->tracos[filaDeTracos->traco_atual] = *traco_novo;
    filaDeTracos->traco_atual++;
    free(traco_novo);
}

void clearFila(FILA* filaDeTracos){
  if (filaDeTracos != NULL) {
    for (int i = 0; i < filaDeTracos->traco_atual; i++) {
      if (filaDeTracos->tracos[i].pontos != NULL) {
          free(filaDeTracos->tracos[i].pontos);
      }
    }
  filaDeTracos->traco_atual = 0;
  }
}

void destroyFila(FILA* filaDeTracos){
    clearFila(filaDeTracos);
    if (filaDeTracos != NULL) {
      if (filaDeTracos->tracos != NULL) {
        free(filaDeTracos->tracos);
    }
    free(filaDeTracos);
  }
}
