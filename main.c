//Tela com 10 segundo de duração

#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>

void main() {
    if (!al_init()) {
        fprintf(stderr,"Erro ao iniciar o Allegro\n");
        return;
    }

    ALLEGRO_DISPLAY *display = al_create_display(800, 800);

    if (!display) {
        fprintf(stderr, "Erro ao iniciar a tela");
    }

    ALLEGRO_COLOR background_Color = al_map_rgb(0, 0, 255);
    al_clear_to_color(background_Color);

    al_flip_display();

    al_rest(10.0);

    al_destroy_display(display);
}