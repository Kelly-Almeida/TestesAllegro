#include <allegro5/allegro_primitives.h>
#include <allegro5/altime.h>
#include "../graficos.h"
#include "telas.h"
bool telaBase(TELA telinha, char *btn,ALLEGRO_FONT *font, ALLEGRO_EVENT_QUEUE *fila_eventos) {

    al_draw_filled_rectangle(0, 0, telinha.x, telinha.y, al_map_rgba(0, 0, 0, 50));
    al_draw_filled_rounded_rectangle((telinha.x/2) - 500, (telinha.y/2) - 350, (telinha.x/2) + 500, (telinha.y/2) + 350, 15, 15, al_map_rgb(255, 255, 255));

    int cord[4] = {telinha.x/2 - 154, (telinha.y/2) + 146, (telinha.x/2) + 154, (telinha.y/2) + 254};
    al_draw_filled_rounded_rectangle(cord[0], cord[1], cord[2], cord[3], 15, 15, al_map_rgb(0, 0, 0));
    al_draw_filled_rounded_rectangle(cord[0] + 3, cord[1] + 3, cord[2] - 3, cord[3] - 3, 15, 15, al_map_rgb(172, 225, 57));
    al_draw_text(font, al_map_rgb(0, 0, 0), telinha.x/2 - 50, (telinha.y/2) + 180, 0, btn);

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