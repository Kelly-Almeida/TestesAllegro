#include <stdio.h>
#include <allegro5/allegro.h>


#define FPS 60.0

void main() {
    //Iniciando
    if (!al_init()) {
        fprintf(stderr, "Erro ao iniciar o allegro!\n");
        return;
    }

    //Criando a tela
    ALLEGRO_DISPLAY *display = al_create_display(640, 480);
    if (!display) return;

    //Criando um timer
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        al_destroy_display(display);
        return;
    }

    //Criando a fila de eventos
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

    if (!queue) {
        al_destroy_timer(timer);
        al_destroy_display(display);
        return;
    }

    //Marcando as partes que serão referenciadas
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_start_timer(timer);

    bool round = true;
    bool return_draw = true;

    while (round) {
        ALLEGRO_EVENT event;

        al_wait_for_event(queue, &event);

        //Teste de eventos
        if (event.type == ALLEGRO_EVENT_TIMER) {
            return_draw = true;
        }else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            round = false;
        }

        if (return_draw && al_is_event_queue_empty(queue)) {
            return_draw = false;

            al_clear_to_color(al_map_rgb(20, 20, 30));



            al_flip_display();
        }
    }

    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_display(display);

}