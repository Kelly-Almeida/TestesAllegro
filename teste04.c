//Referências: https://rafaeltoledonet.wordpress.com/2011/11/30/tutorial-allegro-5-5-utilizando-o-mouse/
//https://liballeg.org/stabledocs/en/alleg009.html
//https://liballeg.org/a5docs/5.2.1/events.html
//https://www.allegro.cc/manual/5/ALLEGRO_MONITOR_INFO
//https://www.youtube.com/watch?v=EI6z4I8PfUc Para configurações do monitor


#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define FPS 60

void desenharCirc(int x0, int y0, int x, int y, ALLEGRO_COLOR cor) {
    al_draw_line(x0, y0, x0, y0, cor, 10);
    al_draw_filled_circle(x, y, 5, cor);

}

typedef struct TELA {
    int x, y;
}TELA;

int main() {

    bool round = true, desenhar = false, redesenhar = false;
    int x = 0, y = 0, canvas_x, canvas_y;

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

    // Inicializando a biblioteca de IO de imagens
    if (!al_init_image_addon()) {
        printf("Falha ao inicializar a biblioteca de IO de imagens!\n");
        return -1;
    }

    //Descobrindo tamanho da tela
    ALLEGRO_MONITOR_INFO info;

    TELA telinha;

    al_get_monitor_info(0, &info); //Estrutura que pega os valores da tela do computador

    telinha.x = info.x2 - info.x1;
    telinha.y = info.y2 - info.y1;

    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW); //Coloca um restrição do ponteiro somente na tela e tira o fechar

    // Inicializando a tela
    ALLEGRO_DISPLAY *tela = al_create_display(telinha.x, telinha.y);
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

    if (!al_install_keyboard()) {
        printf("Falha ao inicializar o keyboard!\n");
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
    al_register_event_source(fila_eventos, al_get_display_event_source(tela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    //Iniciando o timer
    al_start_timer(timer);

    // Cores
    ALLEGRO_COLOR BRANCO = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR PRETO = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR VERDE = al_map_rgb(172, 225, 57);
    ALLEGRO_COLOR VERMELHO = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR AZUL = al_map_rgb(0, 0, 255);
    ALLEGRO_COLOR AMARELO = al_map_rgb(255, 255,0);

    ALLEGRO_COLOR COR;


    // Limpa a tela inicial para branco
    al_clear_to_color(BRANCO);
    al_flip_display();

    /*
     * ========================================================================
     *                             TELA PRIMORDIAL
     * ========================================================================
     */

    float border[4];
    border[0] = telinha.x *0.15; //canto esquerdo 
    border[1] = telinha.y*0.15; // canto superior 
    border[2] = telinha.x- border[0]; // canto direito 
    border[3] = telinha.y - border[1]; // canto inferior 

    int canvas_w = border[2] - border [0];
    int canvas_h = border[3] - border [1];
    printf("largura do canvas: %d\n", canvas_w);
    printf("altura do canvas: %d\n", canvas_h);


    //Converter sistema de coordenada da tela para o Canvas:

    int x_offset = border[0];
    int y_offset = border[1];

    al_clear_to_color(VERDE);


    //Cartela de cores
    al_draw_filled_rectangle(20, 20, 100, 100, PRETO);
    al_draw_filled_rectangle(20, 140, 100, 220, VERMELHO);
    al_draw_filled_rectangle(20, 260, 100, 340, AMARELO);
    al_draw_filled_rectangle(20, 380, 100, 460, AZUL);

    //Criando a imagem do canvas
    ALLEGRO_BITMAP *canvas = al_create_bitmap(canvas_w, canvas_h);


    al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
    al_clear_to_color(BRANCO);


    //Lixinho
    //al_draw_filled_rectangle(border[0], border[1], border[2], border[3], BRANCO);



    al_set_target_backbuffer(tela);//Volta os comandos para a tela

    /*Desenha no canvas  -> pinta o fundo dele de preto -> passa pra tela*/

    int x0, y0;
    COR = AZUL;

    /*
   * ========================================================================
   *                             LOOP DO JOGO
   * ========================================================================
   */


    while (round) {
        

        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);
        
        canvas_x = x - x_offset;
        canvas_y = y - y_offset;

        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {
                x = evento.mouse.x, y = evento.mouse.y;
                //al_draw_filled_rectangle(20, 20, 100, 100, PRETO);
                if (x >= 20 && x <= 100 && y >= 20 && y <= 100) {COR = PRETO;}
                //al_draw_filled_rectangle(20, 140, 100, 220, VERMELHO);
                else if (x >= 20 && x <= 100 && y >= 140 && y <= 220){COR = VERMELHO;}
                //al_draw_filled_rectangle(20, 260, 100, 340, AMARELO);
                else if (x >= 20 && x <= 100 && y >= 260 && y <= 340){COR = AMARELO;}
                //al_draw_filled_rectangle(20, 380, 100, 460, AZUL);
                else if (x >= 20 && x <= 100 && y >= 380 && y <= 460){COR=AZUL;}
            }
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
        }

        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            x = evento.mouse.x, y = evento.mouse.y;

            //bool dentroCanvas = (x >= border[0] + 10 && x <= border[2] - 10  && y >= border[1]  + 10 && y <= border[3] - 10);

            if (desenhar) {
                //Desenhando no canvas
                al_set_target_bitmap(canvas);
                desenharCirc(x0, y0,canvas_x,canvas_y, COR);
                al_set_target_backbuffer(tela);

                x0 = x, y0 = y;
            }
        }

        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {
                x = evento.mouse.x, y = evento.mouse.y;

                if (x >= border[0] && x <= border[2] && y >= border[1] && y <= border[3]) {
                    x0 = x, y0 = y;
                    desenhar = true;

                    al_set_target_bitmap(canvas);
                    //al_draw_filled_circle(x, y, 5, COR);
                    al_set_target_backbuffer(tela);
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (evento.mouse.button == 1) {
                desenhar = false;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_Q) {
                if(evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL){
                  if (!al_save_bitmap("teste.bmp", canvas)){
                    printf("Erro ao salvar o arquivo!\n");
                  }
                    round = false; // fecha o programa
                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_R) {
                if(evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL){
                    al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
                    //al_draw_filled_rectangle(border[0], border[1], border[2], border[3], BRANCO);
                    al_clear_to_color(BRANCO);
                    al_set_target_backbuffer(tela);//Volta os comandos para a tela

                    printf("ok\n");
                }
            }
        }


        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            round = false;
        }

        if (redesenhar  && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;
            
            // Importante desenhar o canvas usando o border de Referência.
            al_draw_bitmap(canvas, x_offset, y_offset, 0);
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

