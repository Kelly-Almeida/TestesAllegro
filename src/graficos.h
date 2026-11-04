//
// Created by sants on 2026-10-21.
//

#ifndef TESTC_GRAFICOS_H
#define TESTC_GRAFICOS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "csv.h"

typedef struct TELA {
    int x, y;
}TELA;

typedef struct VEC {
    short x, y;
}VEC;

typedef struct COR_RGB {
    unsigned char r, g, b;
}COR_RGB;

struct INICIO {
    COR_RGB cor;
    short espessura;
};

struct FILA;

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


TRACO* criarTraco(COR_RGB cor, short espessura, VEC ponto);

void addPontos(TRACO* traco, VEC coordenada);
void desenharCirc(VEC ponto_inicial, VEC ponto, COR_RGB cor, float espessura);
void telaPrincipal(TELA telinha, ALLEGRO_FONT *font, deck *frases, int frase);
void limparCanvas(ALLEGRO_BITMAP* canvas, COR_RGB cor,  ALLEGRO_DISPLAY* tela,struct FILA *filaDeTracos);



#endif //TESTC_GRAFICOS_H
