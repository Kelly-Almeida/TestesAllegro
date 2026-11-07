//
// Created by sants on 2026-10-21.
//

#include "infra.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <stdio.h>
#include "../grafico/graficos.h"



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

void undo(FILA* filaDeTracos, ALLEGRO_BITMAP* canvas, ALLEGRO_DISPLAY* tela){
  if (filaDeTracos->traco_atual > 0) {
    //printf("%s", fila.msg);
    filaDeTracos->traco_atual--;
    int ultimo = filaDeTracos->traco_atual;

    free(filaDeTracos->tracos[ultimo].pontos);
    filaDeTracos->tracos[ultimo].pontos = NULL;

    al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
    al_clear_to_color(al_map_rgb(255,255,255));


    for (int i = 0; i < filaDeTracos->traco_atual; i++) {
        TRACO t = filaDeTracos->tracos[i];
        ALLEGRO_COLOR c = al_map_rgb(t.inicio.cor.r, t.inicio.cor.g, t.inicio.cor.b);

        // Redesenha o ponto inicial (clique do mouse)
        al_draw_filled_circle(t.pontos[0].x, t.pontos[0].y, t.inicio.espessura, c);

        // Redesenha o resto da linha
        VEC ponto_anterior = t.pontos[0];
        for (size_t j = 1; j < t.tamanho; j++) {
            desenharCirc(ponto_anterior, t.pontos[j], t.inicio.cor, t.inicio.espessura);
            ponto_anterior = t.pontos[j];
        }
    }
    al_set_target_backbuffer(tela);//Volta os comandos para a tela

    // printf("CTRL+Z executado! Restam %ld tracos.\n", filaDeTracos->traco_atual);
  }

}
