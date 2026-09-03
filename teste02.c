#include <allegro5/allegro.h>
#include <stdio.h>
#include <allegro5/allegro_primitives.h>

#define FPS 60.0
#define LARGURA_TELA 640
#define ALTURA_TELA 480

enum TECLAS {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

void main() {
    if (!al_init()) return;
    if (!al_install_keyboard()) return;
    if (!al_init_primitives_addon()) return;

    ALLEGRO_DISPLAY *display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    float Px = 300.0f, Py = 220.0f, vel = 4.0f, tam = 40.0f;

    bool teclas[4] = {false, false, false, false};

    al_start_timer(timer);

    bool round = true;
    bool re_draw = true;

    while (round) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {

            if (teclas[UP] && Py > 0) Py -= vel;
            if (teclas[DOWN] && Py < LARGURA_TELA) Py += vel;
            if (teclas[LEFT] && Px > 0) Px -= vel;
            if (teclas[RIGHT] && Px < ALTURA_TELA) Px += vel;

            re_draw = true;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP: teclas[UP] = true; break;
                case ALLEGRO_KEY_DOWN: teclas[DOWN] = true; break;
                case ALLEGRO_KEY_LEFT: teclas[LEFT] = true; break;
                case ALLEGRO_KEY_RIGHT: teclas[RIGHT] = true; break;
                case ALLEGRO_KEY_ESCAPE: round = false; break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP: teclas[UP] = false; break;
                case ALLEGRO_KEY_DOWN: teclas[DOWN] = false; break;
                case ALLEGRO_KEY_LEFT: teclas[LEFT] = false; break;
                case ALLEGRO_KEY_RIGHT: teclas[RIGHT] = false; break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            round = false;
        }

        if (re_draw && al_is_event_queue_empty(queue)) {
            re_draw = false;
            al_clear_to_color(al_map_rgb(10, 0, 255));

            al_draw_filled_rectangle(Px,Py, Px + tam, Py + tam, al_map_rgb(255, 0, 255));

            al_flip_display();
        }

    }

    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_display(display);

}