//Referências: https://rafaeltoledonet.wordpress.com/2011/11/30/tutorial-allegro-5-5-utilizando-o-mouse/
//https://liballeg.org/stabledocs/en/alleg009.html
//https://liballeg.org/a5docs/5.2.1/events.html
//https://www.allegro.cc/manual/5/ALLEGRO_MONITOR_INFO
//https://www.youtube.com/watch?v=EI6z4I8PfUc Para configurações do monitor

#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define FPS 60

typedef struct TELA {
    int x, y;
}TELA;

typedef struct vec2 {
    int x, y;
}vec2;

struct INICIO {
    ALLEGRO_COLOR cor;
    int espessura;
    vec2 ponto;
};

typedef struct traco {
    struct INICIO inicio;
    vec2 *pontos;
    vec2 ponto_final;
    size_t tamanho;
    size_t capacidade;
}traco;

traco* criarTraco(ALLEGRO_COLOR cor, int espessura, vec2 ponto) {
    traco* tracoNovo = malloc(sizeof(traco));
    if (!tracoNovo) return NULL;

    tracoNovo->inicio.cor = cor;
    tracoNovo->inicio.espessura = espessura;
    tracoNovo->inicio.ponto = ponto;
    tracoNovo->tamanho = 0; // ainda não tem nada guardado
    tracoNovo->capacidade = 16; //capacidade inicial para 16 coordenadas
    tracoNovo->pontos = malloc(tracoNovo->capacidade * sizeof(vec2)); //alocando espaço para o array dinâmico que será redimensionado

    if (!tracoNovo->pontos) {
        free(tracoNovo);
        return NULL;
    }
    printf("novo traço\n");
    return tracoNovo;
}

void adicionarPontos(traco* traco, vec2 coordenada) {
    if (traco->tamanho >= traco->capacidade) {
        traco->capacidade *= 2; //dobra a capacidade do traço
        vec2* temp = realloc(traco->pontos, traco->capacidade * sizeof(vec2));
        if (temp == NULL) {
            printf("Erro ao realocar memória!!");
            exit(EXIT_FAILURE);
        }
        traco->pontos = temp;
    }
    traco->pontos[traco->tamanho].x = coordenada.x;
    traco->pontos[traco->tamanho].y = coordenada.y;
    traco->tamanho++;
}

void desenharCirc(vec2 ponto_inicial, vec2 ponto, ALLEGRO_COLOR cor, int espessura) {
    al_draw_line(ponto_inicial.x, ponto_inicial.y, ponto.x, ponto.y, cor, espessura * 2);
    al_draw_filled_circle(ponto.x, ponto.y, espessura, cor);

}
int main() {

    bool round = true, desenhar = false, redesenhar = false;

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

    telinha.x = info.x2 - info.x1 - 500;
    telinha.y = info.y2 - info.y1 - 500;

    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW); //Coloca um restrição do ponteiro somente na tela e tira o fechar

    // Inicializando a tela
    al_set_new_display_flags(ALLEGRO_OPENGL);
    ALLEGRO_DISPLAY* tela = al_create_display(telinha.x, telinha.y);
    if (!tela) {
        printf("Falha ao criar a tela!\n");
        return -1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    
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
    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
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
    ALLEGRO_COLOR AMARELO = al_map_rgb(255, 255, 0);

    ALLEGRO_COLOR COR;
    int espessura;


    // Limpa a tela inicial para branco
    al_clear_to_color(BRANCO);
    al_flip_display();

    /*
     * ========================================================================
     *                             TELA PRIMORDIAL
     * ========================================================================
     */

    float border[4];
    border[0] = telinha.x * 0.15; //canto esquerdo 
    border[1] = telinha.y * 0.15; // canto superior 
    border[2] = telinha.x - border[0]; // canto direito 
    border[3] = telinha.y - border[1]; // canto inferior 

    int canvas_w = border[2] - border[0];
    int canvas_h = border[3] - border[1];
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
    al_draw_filled_rectangle(20, 500, 100, 580, BRANCO);

    //Criando a imagem do canvas
    ALLEGRO_BITMAP* canvas = al_create_bitmap(canvas_w, canvas_h);
    if (!canvas) {
        printf("Erro ao criar o canvas!\n");
        return -1;
    }

    al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
    al_clear_to_color(BRANCO);


    //Lixinho
    //al_draw_filled_rectangle(border[0], border[1], border[2], border[3], BRANCO);



    al_set_target_backbuffer(tela);//Volta os comandos para a tela

    /*Desenha no canvas  -> pinta o fundo dele de preto -> passa pra tela*/

    vec2 ponto = { 0,0 };
    vec2 ponto_inicial;
    vec2 ponto_canvas;
    COR = AZUL;
    espessura = 3;

    traco* traco_novo = NULL;

    traco *filaDeTracos;
    filaDeTracos = (traco*)malloc(16 * sizeof(traco)); // iniciando o array de traços com capacidade inicial para 16 traços
    if (filaDeTracos == NULL) {
        printf("Erro ao alocar memória!\n");
        return -1;
    }
    int numeroDeTracos = 0; // variável para controlar quantos traços estão guardados

    /*
   * ========================================================================
   *                             LOOP DO JOGO
   * ========================================================================
   */

    while (round) {

        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
        }

        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {
                ponto.x = evento.mouse.x;
                ponto.y = evento.mouse.y;

                if (ponto.x >= 20 && ponto.x <= 100 && ponto.y >= 20 && ponto.y <= 100) { COR = PRETO; }
                else if (ponto.x >= 20 && ponto.x <= 100 && ponto.y >= 140 && ponto.y <= 220) { COR = VERMELHO; }
                else if (ponto.x >= 20 && ponto.x <= 100 && ponto.y >= 260 && ponto.y <= 340) { COR = AMARELO; }
                else if (ponto.x >= 20 && ponto.x <= 100 && ponto.y >= 380 && ponto.y <= 460) { COR = AZUL; }
                else if (ponto.x >= 20 && ponto.x <= 100 && ponto.y >= 500 && ponto.y <= 580) { COR = BRANCO; }
            
                else if (ponto.x >= border[0] && ponto.x <= border[2] && ponto.y >= border[1] && ponto.y <= border[3]) {
                    ponto_canvas.x = ponto.x - x_offset;
                    ponto_canvas.y = ponto.y - y_offset;

                    ponto_inicial.x = ponto_canvas.x, ponto_inicial.y = ponto_canvas.y;
                    traco_novo = criarTraco(COR, espessura, ponto_canvas);
                    desenhar = true;

                    al_set_target_bitmap(canvas);
                    printf("ponto inicial: {%d, %d}, espessura: %d, cor: {r:%f g:%f b:%f a:%f} \n", ponto_canvas.x, ponto_canvas.y, espessura, COR.r, COR.g, COR.b, COR.a);
                    al_draw_filled_circle(ponto_canvas.x, ponto_canvas.y, espessura, COR);
                    al_set_target_backbuffer(tela);
                }
            }
        }

        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            ponto.x = evento.mouse.x;
            ponto.y = evento.mouse.y;

            ponto_canvas.x = ponto.x - x_offset;
            ponto_canvas.y = ponto.y - y_offset;

            if (desenhar && traco_novo) {

                if (ponto_canvas.x >= 0 && ponto_canvas.x <= canvas_w &&
                    ponto_canvas.y >= 0 && ponto_canvas.y <= canvas_h) {
                    //Desenhando no canvas
                    al_set_target_bitmap(canvas);
                    adicionarPontos(traco_novo, ponto_canvas);
                    printf("ponto atual: {%d, %d}\n", ponto_canvas.x, ponto_canvas.y);
                    desenharCirc(ponto_inicial, ponto_canvas, COR, espessura);
                    al_set_target_backbuffer(tela);
                }
                ponto_inicial = ponto_canvas;
            }
        }

        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (evento.mouse.button == 1 && desenhar) {
                traco_novo->ponto_final = ponto_canvas;
                printf("ponto final: {%d, %d}\n", ponto_canvas.x, ponto_canvas.y);
                desenhar = false;
            } 
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_Q) {
                if (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
                    if (!al_save_bitmap("teste.bmp", canvas)) {
                        printf("Erro ao salvar o arquivo!\n");
                    }
                    round = false; // fecha o programa
                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_R) {
                if (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
                    al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
                    al_clear_to_color(BRANCO);
                    al_set_target_backbuffer(tela);//Volta os comandos para a tela

                    printf("Limpar_tela\n");
                }
            }
        }


        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            round = false;
        }

        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;

            al_clear_to_color(VERDE);


            //Cartela de cores
            al_draw_filled_rectangle(20, 20, 100, 100, PRETO);
            al_draw_filled_rectangle(20, 140, 100, 220, VERMELHO);
            al_draw_filled_rectangle(20, 260, 100, 340, AMARELO);
            al_draw_filled_rectangle(20, 380, 100, 460, AZUL);
            al_draw_filled_rectangle(20, 500, 100, 580, BRANCO);
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

