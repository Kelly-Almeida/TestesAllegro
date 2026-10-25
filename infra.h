//
// Created by sants on 2026-10-21.
//

#ifndef TESTC_INFRA_H
#define TESTC_INFRA_H

#endif //TESTC_INFRA_H

typedef struct FILA {
    TRACO *tracos;
    int traco_atual;
    size_t tamanho;
    size_t capacidade;
}FILA;


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

