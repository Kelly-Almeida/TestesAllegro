//
// Created by sants on 2026-10-21.
//

#ifndef TESTC_GRAFICOS_H
#define TESTC_GRAFICOS_H

#include <allegro5/allegro.h>

typedef struct TELA {
    int x, y;
}TELA;

typedef struct VEC {
    short x, y;
}VEC;

struct INICIO {
    ALLEGRO_COLOR cor;
    short espessura;
};

typedef struct TRACO {
    struct INICIO inicio;
    VEC *pontos;
    size_t tamanho;
    size_t capacidade;
}TRACO;

typedef struct COR {
    ALLEGRO_COLOR cor;
    float coordenadas[4];
}COR;

TRACO* criarTraco(ALLEGRO_COLOR cor, short espessura, VEC ponto);

void addPontos(TRACO* traco, VEC coordenada);
void desenharCirc(VEC ponto_inicial, VEC ponto, ALLEGRO_COLOR cor, float espessura);

void telaPrincipal(TELA telinha);

#endif //TESTC_GRAFICOS_H
