//
// Created by sants on 2026-10-21.
//

#ifndef TESTC_GRAFICOS_H
#define TESTC_GRAFICOS_H

#endif //TESTC_GRAFICOS_H

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

TRACO* criarTraco(ALLEGRO_COLOR cor, short espessura, VEC ponto) {
    TRACO* tracoNovo = malloc(sizeof(TRACO));
    if (!tracoNovo) return NULL;

    tracoNovo->inicio.cor = cor;
    tracoNovo->inicio.espessura = espessura;
    tracoNovo->tamanho = 1; // índice do próximo espaço disponível (o intem inicial é o 0)
    tracoNovo->capacidade = 16; //capacidade inicial para 16 coordenadas
    tracoNovo->pontos = malloc(tracoNovo->capacidade * sizeof(VEC)); //alocando espaço para o array dinâmico que será redimensionado

    if (!tracoNovo->pontos) {
        free(tracoNovo);
        return NULL;
    }
  
    tracoNovo->pontos[0] = ponto;

    printf("Novo Traço, Ponto Inicial: {%d, %d}, espessura: %d, cor: {r:%f g:%f b:%f a:%f} \n", ponto.x, ponto.y, espessura, cor.r, cor.g, cor.b, cor.a);
    
    return tracoNovo;
}

void addPontos(TRACO* traco, VEC coordenada) {
  if (traco->tamanho >= traco->capacidade) {
      traco->capacidade *= 2; //dobra a capacidade do traço
      VEC* temp = realloc(traco->pontos, traco->capacidade * sizeof(VEC));
      if (temp == NULL) {
          printf("Erro ao realocar memória!!");
          exit(EXIT_FAILURE);
      }
      traco->pontos = temp;
  }

  // check para evitar pontos duplicados no array, já que, com a resolução reduzida, dois pontos próximos acabam sendo mapeados para o mesmo pixel.
  if(traco->pontos[traco->tamanho - 1].x != coordenada.x || traco->pontos[traco->tamanho - 1].y != coordenada.y){
    traco->pontos[traco->tamanho].x = coordenada.x;
    traco->pontos[traco->tamanho].y = coordenada.y;
    traco->tamanho++;
  }

}

void desenharCirc(VEC ponto_inicial, VEC ponto, ALLEGRO_COLOR cor, float espessura) {
    al_draw_line(ponto_inicial.x, ponto_inicial.y, ponto.x, ponto.y, cor, espessura * 2);
    al_draw_filled_circle(ponto.x, ponto.y, espessura, cor);

}


void telaPrincipal(TELA telinha) {
    ALLEGRO_COLOR VERDE = al_map_rgb(172, 225, 57);
    ALLEGRO_COLOR VERMELHO = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR AZUL = al_map_rgb(0, 0, 255);
    ALLEGRO_COLOR AMARELO = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR BRANCO = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR PRETO = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR LARANJA = al_map_rgb(255, 128, 0);
    ALLEGRO_COLOR ROXO = al_map_rgb(128, 0, 255);

    struct COR PRETO_PALE = {PRETO,{20, 20, 90, 75}};
    struct COR VERMELHO_PALE = {VERMELHO, {20, 105, 90, 160}};
    struct COR AMARELO_PALE = {AMARELO, {20, 190, 90, 245}};
    struct COR AZUL_PALE = {AZUL, {20, 280, 90, 335}};
    struct COR LARANJA_PALE = {LARANJA, {20, 370, 90, 425}};
    struct COR ROXO_PALE = {ROXO, {20, 450, 90, 500}};

    struct COR BRANCO_PALE = {BRANCO, {20, 530, 90, 580}};

    al_draw_filled_rectangle(0, 0, telinha.x * 0.15, telinha.y, VERDE);
    al_draw_filled_rectangle(0, 0, telinha.x, telinha.y * 0.15, VERDE);
    al_draw_filled_rectangle(0, telinha.y, telinha.x, telinha.y * 0.85, VERDE );
    al_draw_filled_rectangle(telinha.x, 0, telinha.x *0.85, telinha.y, VERDE);

    /*cores*/
    al_draw_filled_rectangle(PRETO_PALE.coordenadas[0],PRETO_PALE.coordenadas[1], PRETO_PALE.coordenadas[2], PRETO_PALE.coordenadas[3], PRETO_PALE.cor);
    al_draw_filled_rectangle(VERMELHO_PALE.coordenadas[0],VERMELHO_PALE.coordenadas[1], VERMELHO_PALE.coordenadas[2], VERMELHO_PALE.coordenadas[3], VERMELHO_PALE.cor);
    al_draw_filled_rectangle(AMARELO_PALE.coordenadas[0],AMARELO_PALE.coordenadas[1], AMARELO_PALE.coordenadas[2], AMARELO_PALE.coordenadas[3], AMARELO_PALE.cor);
    al_draw_filled_rectangle(AZUL_PALE.coordenadas[0],AZUL_PALE.coordenadas[1], AZUL_PALE.coordenadas[2], AZUL_PALE.coordenadas[3], AZUL_PALE.cor);
    al_draw_filled_rectangle(BRANCO_PALE.coordenadas[0],BRANCO_PALE.coordenadas[1], BRANCO_PALE.coordenadas[2], BRANCO_PALE.coordenadas[3], BRANCO_PALE.cor);
    al_draw_filled_rectangle(LARANJA_PALE.coordenadas[0], LARANJA_PALE.coordenadas[1], LARANJA_PALE.coordenadas[2], LARANJA_PALE.coordenadas[3], LARANJA_PALE.cor);
    al_draw_filled_rectangle(ROXO_PALE.coordenadas[0], ROXO_PALE.coordenadas[1], ROXO_PALE.coordenadas[2], ROXO_PALE.coordenadas[3], ROXO_PALE.cor);


}
