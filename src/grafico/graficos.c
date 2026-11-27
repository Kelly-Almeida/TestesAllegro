//
// Created by sants on 2026-10-21.
//

#include "graficos.h"
#include "../infraestrutura/infra.h"
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>


TRACO* criarTraco(COR_RGB cor, short espessura, VEC ponto ) {

    TRACO* tracoNovo = malloc(sizeof(TRACO));
    if (!tracoNovo) return NULL;

    tracoNovo->inicio.cor.r = cor.r;
    tracoNovo->inicio.cor.g = cor.g;
    tracoNovo->inicio.cor.b = cor.b;
    tracoNovo->inicio.espessura = espessura;
    tracoNovo->tamanho = 1; // índice do próximo espaço disponível (o intem inicial é o 0)
    tracoNovo->capacidade = 16; //capacidade inicial para 16 coordenadas
    tracoNovo->pontos = malloc(tracoNovo->capacidade * sizeof(VEC)); //alocando espaço para o array dinâmico que será redimensionado

    if (!tracoNovo->pontos) {
        free(tracoNovo);
        return NULL;
    }

    tracoNovo->pontos[0] = ponto;

    printf("Novo Traço, Ponto Inicial: {%d, %d}, espessura: %d, cor: {r:%d g:%d b:%d} \n", ponto.x, ponto.y, espessura, cor.r, cor.g, cor.b);

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

void desenharCirc(VEC ponto_inicial, VEC ponto, COR_RGB cor_rgb, float espessura) {
  ALLEGRO_COLOR cor = al_map_rgb(cor_rgb.r,cor_rgb.g,cor_rgb.b);
  al_draw_line(ponto_inicial.x, ponto_inicial.y, ponto.x, ponto.y, cor, espessura * 2);
  al_draw_filled_circle(ponto.x, ponto.y, espessura, cor);

}

void limparCanvas(ALLEGRO_BITMAP* canvas, COR_RGB cor,  ALLEGRO_DISPLAY* tela, FILA *filaDeTracos){
  al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
  al_clear_to_color(al_map_rgb(cor.r, cor.g, cor.b));
  al_set_target_backbuffer(tela);//Volta os comandos para a tela
  clearFila(filaDeTracos);
  printf("Limpar_tela\n");
  printf("Fila limpa!\n");
}


void telaPrincipal(TELA telinha, ALLEGRO_FONT *font, deck *deck, int indice, bool cliente, int nivel) {
    ALLEGRO_COLOR VERDE = al_map_rgb(172, 225, 57);
    ALLEGRO_COLOR VERMELHO = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR AZUL = al_map_rgb(0, 0, 255);
    ALLEGRO_COLOR AMARELO = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR BRANCO = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR PRETO = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR LARANJA = al_map_rgb(255, 128, 0);
    ALLEGRO_COLOR ROXO = al_map_rgb(128, 0, 255);

    struct COR PRETO_PALE    = {PRETO,    {40, 150, 200, 200}};
    struct COR VERMELHO_PALE = {VERMELHO, {40, 215, 200, 265}};
    struct COR AMARELO_PALE  = {AMARELO,  {40, 280, 200, 330}};
    struct COR AZUL_PALE     = {AZUL,     {40, 345, 200, 395}};
    struct COR LARANJA_PALE  = {LARANJA,  {40, 410, 200, 460}};
    struct COR ROXO_PALE     = {ROXO,     {40, 475, 200, 525}};
    struct COR BRANCO_PALE   = {BRANCO,   {40, 540, 200, 590}};
    al_draw_filled_rectangle(0, 0, telinha.x, telinha.y, VERDE);

    /*cores*/
    if (cliente) {
        al_draw_filled_rounded_rectangle(20, 180, 220, 660,15, 15, al_map_rgba(0, 0, 0, 50));

        al_draw_filled_rectangle(PRETO_PALE.coordenadas[0] - 2, PRETO_PALE.coordenadas[1] + 48, PRETO_PALE.coordenadas[2] + 2, PRETO_PALE.coordenadas[3] + 52, al_map_rgb(PRETO_PALE.cor.r + 50, PRETO_PALE.cor.g + 50, PRETO_PALE.cor.b + 50));
        al_draw_filled_rectangle(PRETO_PALE.coordenadas[0], PRETO_PALE.coordenadas[1] + 50, PRETO_PALE.coordenadas[2], PRETO_PALE.coordenadas[3] + 50, PRETO_PALE.cor);

        al_draw_filled_rectangle(VERMELHO_PALE.coordenadas[0] - 2, VERMELHO_PALE.coordenadas[1] + 48, VERMELHO_PALE.coordenadas[2] + 2, VERMELHO_PALE.coordenadas[3] + 52, al_map_rgb(VERMELHO_PALE.cor.r - 20, VERMELHO_PALE.cor.g, VERMELHO_PALE.cor.b));
        al_draw_filled_rectangle(VERMELHO_PALE.coordenadas[0], VERMELHO_PALE.coordenadas[1] + 50, VERMELHO_PALE.coordenadas[2], VERMELHO_PALE.coordenadas[3] + 50, VERMELHO_PALE.cor);

        al_draw_filled_rectangle(AMARELO_PALE.coordenadas[0] - 2, AMARELO_PALE.coordenadas[1] + 48, AMARELO_PALE.coordenadas[2] + 2, AMARELO_PALE.coordenadas[3] + 52, al_map_rgb(AMARELO_PALE.cor.r - 40, AMARELO_PALE.cor.g - 40, AMARELO_PALE.cor.b));
        al_draw_filled_rectangle(AMARELO_PALE.coordenadas[0], AMARELO_PALE.coordenadas[1] + 50, AMARELO_PALE.coordenadas[2], AMARELO_PALE.coordenadas[3] + 50, AMARELO_PALE.cor);

        al_draw_filled_rectangle(AZUL_PALE.coordenadas[0] - 2, AZUL_PALE.coordenadas[1] + 48, AZUL_PALE.coordenadas[2] + 2, AZUL_PALE.coordenadas[3] + 52, al_map_rgb(AZUL_PALE.cor.r, AZUL_PALE.cor.g, AZUL_PALE.cor.b - 40));
        al_draw_filled_rectangle(AZUL_PALE.coordenadas[0], AZUL_PALE.coordenadas[1] + 50, AZUL_PALE.coordenadas[2], AZUL_PALE.coordenadas[3] + 50, AZUL_PALE.cor);

        al_draw_filled_rectangle(BRANCO_PALE.coordenadas[0] - 2, BRANCO_PALE.coordenadas[1] + 48, BRANCO_PALE.coordenadas[2] + 2, BRANCO_PALE.coordenadas[3] + 52, al_map_rgb(BRANCO_PALE.cor.r - 50, BRANCO_PALE.cor.g - 50, BRANCO_PALE.cor.b - 50));
        al_draw_filled_rectangle(BRANCO_PALE.coordenadas[0], BRANCO_PALE.coordenadas[1] + 50, BRANCO_PALE.coordenadas[2], BRANCO_PALE.coordenadas[3] + 50, BRANCO_PALE.cor);

        al_draw_filled_rectangle(LARANJA_PALE.coordenadas[0] - 2, LARANJA_PALE.coordenadas[1] + 48, LARANJA_PALE.coordenadas[2] + 2, LARANJA_PALE.coordenadas[3] + 52, al_map_rgb(LARANJA_PALE.cor.r - 40, LARANJA_PALE.cor.g + 100, LARANJA_PALE.cor.b));
        al_draw_filled_rectangle(LARANJA_PALE.coordenadas[0], LARANJA_PALE.coordenadas[1] + 50, LARANJA_PALE.coordenadas[2], LARANJA_PALE.coordenadas[3] + 50, LARANJA_PALE.cor);

        al_draw_filled_rectangle(ROXO_PALE.coordenadas[0] - 2, ROXO_PALE.coordenadas[1] + 48, ROXO_PALE.coordenadas[2] + 2, ROXO_PALE.coordenadas[3] + 52, al_map_rgb(ROXO_PALE.cor.r - 70, ROXO_PALE.cor.g, ROXO_PALE.cor.b - 70));
        al_draw_filled_rectangle(ROXO_PALE.coordenadas[0], ROXO_PALE.coordenadas[1] + 50, ROXO_PALE.coordenadas[2], ROXO_PALE.coordenadas[3] + 50, ROXO_PALE.cor);

    }


    /*Cad de nível*/
    al_draw_filled_rounded_rectangle(1580,25, 1750, 110, 15, 15 ,al_map_rgba(255,255,255,100));
    char textNivel[10];
    int quant;
    switch (nivel) {
        case 0:
            strcpy(textNivel, "Fácil");
            quant = 5;
            break;
        case 1:
            strcpy(textNivel, "Médio");
            quant = 10;
            break;
        case 2:
            strcpy(textNivel, "Difícil");
            quant = 7;
            break;

    }

    al_draw_text(font, al_map_rgba(0,0,0,170),1645 - quant,50, 0, textNivel);

    if (cliente) {
        al_draw_filled_rounded_rectangle(telinha.x * 0.4, -20, 200 + telinha.x * 0.5, 110,15, 15, al_map_rgba(0,0,0,90));
        al_draw_text(font, al_map_rgb(0,0,0), 900, 35, ALLEGRO_ALIGN_CENTRE, deck->frases[indice].ingles);

        al_draw_filled_rectangle(120, telinha.y*0.9, 160, 40 + telinha.y*0.9, al_map_rgba(0, 0, 0, 100));
        al_draw_filled_rectangle(190, telinha.y*0.9, 230, 40 + telinha.y*0.9, al_map_rgba(0, 0, 0, 100));
        al_draw_text(font, PRETO_PALE.cor,132, telinha.y*0.9 + 3, 0, "+");
        al_draw_text(font, PRETO_PALE.cor,205, telinha.y*0.9 + 3, 0, "-");
    }


    al_draw_filled_rounded_rectangle(telinha.x * 0.80, telinha.y*0.15,telinha.x * 0.98,telinha.y * 0.85,15,15, al_map_rgba(0,0,0,100));
}
void chatzinho(CHATM *chat, TELA telinha, int y, ALLEGRO_FONT *font) {
    if (chat->quant <= 0) return;

    int posY = y + 20;
    int alturaLinha = al_get_font_line_height(font) + 4;

    for (int i = 0; i < chat->quant; i++) {
        char textoFormatado[256];
        strncpy(textoFormatado, chat->msg[i], 255);
        textoFormatado[255] = '\0';

        // Quebra a linha após 300px inserindo '\n'
        char linha[256] = {0};
        int lenLinha = 0, ultimoEspacoTexto = -1, ultimoEspacoLinha = -1, linhas = 1;

        for (int j = 0; textoFormatado[j] != '\0'; j++) {
            if (textoFormatado[j] == ' ') {
                ultimoEspacoTexto = j;
                ultimoEspacoLinha = lenLinha;
            }
            if (lenLinha < 254) {
                linha[lenLinha++] = textoFormatado[j];
                linha[lenLinha] = '\0';
            }
            if (al_get_text_width(font, linha) > 200) {
                if (ultimoEspacoTexto != -1) {
                    textoFormatado[ultimoEspacoTexto] = '\n';
                    int sobra = lenLinha - (ultimoEspacoLinha + 1);
                    memmove(linha, linha + ultimoEspacoLinha + 1, sobra);
                    lenLinha = sobra;
                    linha[lenLinha] = '\0';
                    ultimoEspacoTexto = -1;
                    ultimoEspacoLinha = -1;
                    linhas++;
                } else if (strlen(textoFormatado) + 1 < 256) {
                    memmove(&textoFormatado[j + 1], &textoFormatado[j], strlen(textoFormatado) - j + 1);
                    textoFormatado[j] = '\n';
                    lenLinha = 0;
                    linha[0] = '\0';
                    linhas++;
                }
            }
        }

        // Desenha respeitando os '\n' inseridos
        al_draw_multiline_text(font, al_map_rgb(255, 255, 255), telinha.x - 250, posY, 300, alturaLinha, 0, textoFormatado);

        // Avança o Y baseado em quantas linhas essa mensagem ocupou + margem
        posY += (linhas * alturaLinha) + 15;
    }
}

