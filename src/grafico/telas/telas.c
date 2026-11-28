#include <allegro5/allegro_primitives.h>
#include <allegro5/altime.h>
#include "../graficos.h"
#include "telas.h"
bool telaBase(TELA telinha,char *btn ,ALLEGRO_FONT *font, ALLEGRO_EVENT_QUEUE *fila_eventos, bool vitoria, bool inicio, int pontuacao) {

    al_draw_filled_rectangle(0, 0, telinha.x, telinha.y, al_map_rgba(0, 0, 0, 50));
    al_draw_filled_rounded_rectangle((telinha.x/2) - 500, (telinha.y/2) - 350, (telinha.x/2) + 500, (telinha.y/2) + 350, 15, 15, al_map_rgb(255, 255, 255));

    int cord[4] = {telinha.x/2 - 190, (telinha.y/2) + 146, (telinha.x/2) + 200, (telinha.y/2) + 254};
    al_draw_filled_rounded_rectangle(cord[0], cord[1], cord[2], cord[3], 15, 15, al_map_rgb(0, 0, 0));
    al_draw_filled_rounded_rectangle(cord[0] + 3, cord[1] + 3, cord[2] - 3, cord[3] - 3, 15, 15, al_map_rgb(172, 225, 57));

    if (strcmp(btn, "continuar") == 0) {
        al_draw_text(font, al_map_rgb(0, 0, 0), telinha.x/2 - 170, (telinha.y/2) + 180, 0, btn);
    }else {
        al_draw_text(font, al_map_rgb(0, 0, 0), telinha.x/2 -130, (telinha.y/2) + 180, 0, btn);
    }

    ALLEGRO_FONT *fontP = al_load_font("./resources/PressStart2P.ttf", 80, 0);
    if (!fontP) {
        al_destroy_font(fontP);
        return true;
    }

    if (vitoria && !inicio)  telaVitoria(telinha, pontuacao, fontP);
    else if (!inicio) telaDerrota(telinha, pontuacao, fontP);

    al_flip_display();

    bool sair = false;
    ALLEGRO_EVENT evento;
    VEC ponto;
    bool resp = true;

    while (!sair) {
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {
                ponto.x = evento.mouse.x;
                ponto.y = evento.mouse.y;

                if (ponto.x >= telinha.x/2 - 154 && ponto.y >= (telinha.y/2) + 146 &&
                    ponto.x <= (telinha.x/2) + 154 && ponto.y <= (telinha.y/2) + 254) {
                    sair = true;
                    }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = true;
            resp = false;
        }
    }

    return resp;
}

void telaVitoria(TELA telinha, int pontuacao, ALLEGRO_FONT *fontP) {

    int tam = 0;
    int mult = 15;
    char pontuacaoChar[5];
    snprintf(pontuacaoChar, sizeof(pontuacaoChar), "%d", pontuacao);

    if (pontuacao != 0){
        tam = sizeof(pontuacaoChar);
    }

    if (pontuacao >= 1000) mult = 20;

    al_draw_text(fontP, al_map_rgb(0,0,0), telinha.x/2 - 50 - (tam*mult), (telinha.y/4) + 200, 0, pontuacaoChar);
    al_draw_text(fontP,al_map_rgb(0,0, 0) , telinha.x/2 - 270, (telinha.y/4) , 0, "Vitória");
    al_destroy_font(fontP);
}

void telaDerrota(TELA telinha, int pontuacao, ALLEGRO_FONT *fontP) {

    int tam = 0;
    int mult = 15;
    char pontuacaoChar[5];
    snprintf(pontuacaoChar, sizeof(pontuacaoChar), "%d", pontuacao);

    if (pontuacao != 0){
        tam = sizeof(pontuacaoChar);
    }

    if (pontuacao >= 1000) mult = 20;

    al_draw_text(fontP, al_map_rgb(0,0,0), telinha.x/2 - 50 - (tam*mult), (telinha.y/4) + 200, 0, pontuacaoChar);
    al_draw_text(fontP,al_map_rgb(0,0, 0) , telinha.x/2 - 270, (telinha.y/4) , 0, "Derrota");
    al_destroy_font(fontP);
}