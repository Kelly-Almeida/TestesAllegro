//Referências: https://rafaeltoledonet.wordpress.com/2011/11/30/tutorial-allegro-5-5-utilizando-o-mouse/
//https://liballeg.org/stabledocs/en/alleg009.html
//https://liballeg.org/a5docs/5.2.1/events.html


#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 480;
#define FPS 60

void desenharCirc(int x0, int y0, int x, int y, ALLEGRO_COLOR cor) {
    al_draw_line(x0, y0, x0, y0, cor, 10);
    al_draw_filled_circle(x, y, 5, cor);
}

int main() {

    bool round = true;
    bool desenhar = false, redesenhar = false;
    int x = 0, y = 0;

    // Inicializando o Allegro
    if (!al_init()) {
        printf("Falha ao inicializar o Allegro!\n");
        return -1;
    }

    // Inicializando a biblioteca de primitivas gráficas
    if (!al_init_primitives_addon()) {
        printf("Falha ao inicializar a biblioteca de primitivas!\n");
        return -1;
    }

    // Inicializando a tela
    ALLEGRO_DISPLAY *tela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!tela) {
        printf("Falha ao criar a tela!\n");
        return -1;
    }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);

    if (!timer) {
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }

    // Habilitando o mouse
    if (!al_install_mouse()) {
        printf("Falha ao inicializar o mouse!\n");
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }

    // Habilitando o teclado 
    if (!al_install_keyboard()) {
        printf("Falha ao inicializar o teclado!\n");
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }

    // Atribuindo o cursor padrão
    if (!al_set_system_mouse_cursor(tela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
        printf("Falha ao atribuir o ponteiro do mouse!\n");
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }

    // Inicializando a fila de eventos
    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        printf("Erro ao criar fila de eventos!\n");
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }

    // Registrando as fontes de eventos
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(tela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    //Iniciando o timer
    al_start_timer(timer);

    // Cores
    ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR preto = al_map_rgb(0, 0, 0);

    // Limpa a tela inicial para branco
    al_clear_to_color(branco);
    al_flip_display();

    //Criando a imagem do canvas
    ALLEGRO_BITMAP *canvas = al_create_bitmap(LARGURA_TELA, LARGURA_TELA);
    al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
    al_clear_to_color(branco);
    al_set_target_backbuffer(tela);//Volta os comandos para a tela

    /*Desenha no canvas  -> pinta o fundo dele de branco -> passa pra tela*/

    int x0, y0;

    while (round) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);


        if (evento.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            x = evento.mouse.x, y = evento.mouse.y;

            if (desenhar) {
                //Desenhando no canvas
                al_set_target_bitmap(canvas);
                desenharCirc(x0, y0,x,y, preto);
                al_set_target_backbuffer(tela);

                x0 = x, y0 = y;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {
                x = evento.mouse.x, y = evento.mouse.y;
                x0 = x, y0 = y;
                desenhar = true;

                al_set_target_bitmap(canvas);
                al_draw_filled_circle(x, y, 5, preto);
                al_set_target_backbuffer(tela);

            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (evento.mouse.button == 1) {
                desenhar = false;
            }
        }
        
        // Lógica para comandos e teclado (ctrl + Q) && (ctrk + R)
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_Q) {
                if(evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL){
                  round = false; // fecha o programa
                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_R) {
                if(evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL){
                  al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
                  al_clear_to_color(branco);
                  al_set_target_backbuffer(tela);//Volta os comandos para a tela
                }
            }
        }

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            round = false;
        }

        if (redesenhar  && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;

            al_draw_bitmap(canvas, 0, 0,0);
            al_flip_display();
        }
    }

    // Liberação de memória
    al_destroy_bitmap(canvas);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    al_destroy_display(tela);

    return 0;
}

