//Referências: https://rafaeltoledonet.wordpress.com/2011/11/30/tutorial-allegro-5-5-utilizando-o-mouse/
//https://liballeg.org/stabledocs/en/alleg009.html
//https://liballeg.org/a5docs/5.2.1/events.html
//https://www.allegro.cc/manual/5/ALLEGRO_MONITOR_INFO
//https://www.youtube.com/watch?v=EI6z4I8PfUc Para configurações do monitor
//https://rafaeltoledonet.wordpress.com/2011/11/10/tutorial-allegro-5-4-escrevendo-texto-usando-fontes-true-type-ttf/

#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "socket01.h"

#define FPS 60

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

typedef struct FILA {
    TRACO *tracos;
    size_t traco_atual;
    size_t capacidade;
}FILA;

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
    //TODO:
    //função para controlar a verbosidade dos logs
    
    //printf("novo ponto: {%d, %d}\n", coordenada.x, coordenada.y);

}

FILA* criarFila() {
    FILA* filaNova = malloc(sizeof(FILA));
    if (!filaNova) return NULL;

    filaNova->traco_atual = 0;
    filaNova->capacidade = 16;
    filaNova->tracos = malloc(filaNova->capacidade * sizeof(TRACO));

    if (!filaNova->tracos) {
        free(filaNova);
        return NULL;
    }
    return filaNova;
}

void pushFila(FILA* filaDeTracos, TRACO *traco_novo) {
    if (filaDeTracos->traco_atual >= filaDeTracos->capacidade) {
        filaDeTracos->capacidade *= 2; //dobra a capacidade da fila
        TRACO* temp = realloc(filaDeTracos->tracos, filaDeTracos->capacidade * sizeof(TRACO));
        if (temp == NULL) {
            printf("Erro ao realocar memória!!");
            exit(EXIT_FAILURE);
        }
        filaDeTracos->tracos = temp;
    }
    filaDeTracos->tracos[filaDeTracos->traco_atual] = *traco_novo;
    filaDeTracos->traco_atual++;
    free(traco_novo);
}

void clearFila(FILA* filaDeTracos){
  if (filaDeTracos != NULL) {
    for (int i = 0; i < filaDeTracos->traco_atual; i++) {
      if (filaDeTracos->tracos[i].pontos != NULL) {
          free(filaDeTracos->tracos[i].pontos);
      }
    }
  filaDeTracos->traco_atual = 0;
  }
}

void destroyFila(FILA* filaDeTracos){
    clearFila(filaDeTracos);
    if (filaDeTracos != NULL) {
      if (filaDeTracos->tracos != NULL) {
        free(filaDeTracos->tracos);
    }
    free(filaDeTracos);
  }
}

void desenharCirc(VEC ponto_inicial, VEC ponto, ALLEGRO_COLOR cor, float espessura) {
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

    telinha.x = info.x2 - info.x1 - 100;
    telinha.y = info.y2 - info.y1 - 50;

    //al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW); //Coloca um restrição do ponteiro somente na tela e tira o fechar

    //Opções para habilitar o anti-aliasing 
    
    /*
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    */

    al_set_new_display_flags(ALLEGRO_OPENGL);

    // Inicializando a tela
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

    //Inicializando as fontes
    if (!al_init_font_addon()) {
        printf("Erro ao inicializar a biblioteca de fontes\n");
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }


    ALLEGRO_FONT *font = al_load_font("Roboto-VariableFont_wdth,wght.ttf", 48, 0);
    
    //Gambiarra, já que o debug mode do vs code gera o binário em um diretório abaixo do que o make do linux.
    if (!font) { 
        font = al_load_font("../Roboto-VariableFont_wdth,wght.ttf", 48, 0); 
    }
    if (!font) { 
        font = al_load_font("../../Roboto-VariableFont_wdth,wght.ttf", 48, 0); 
    }
    if (!font) { 
        font = al_load_font("../../../Roboto-VariableFont_wdth,wght.ttf", 48, 0); 
    }
    if (!font) {
        printf("Erro ao carregar a fonte\n");
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


    ALLEGRO_COLOR COR;
    short espessura;

    //Variáveis globais
    COR = AZUL;
    espessura = 3;
    VEC ponto = { 0,0 };
    VEC ponto_inicial;
    VEC ponto_canvas;

    al_clear_to_color(BRANCO);
    al_flip_display();

    float scaling = 0.25;
    int border[4];
    border[0] = telinha.x * 0.15; //canto esquerdo
    border[1] = telinha.y * 0.15; // canto superior
    border[2] = telinha.x - border[0]; // canto direito
    border[3] = telinha.y - border[1]; // canto inferior

    double canvas_w = border[2] - border[0];
    double canvas_h = border[3] - border[1];

    //Converter sistema de coordenada da tela para o Canvas:

    int x_offset = border[0];
    int y_offset = border[1];

    al_clear_to_color(VERDE);

    /*cores*/
    al_draw_filled_rectangle(PRETO_PALE.coordenadas[0],PRETO_PALE.coordenadas[1], PRETO_PALE.coordenadas[2], PRETO_PALE.coordenadas[3], PRETO_PALE.cor);
    al_draw_filled_rectangle(VERMELHO_PALE.coordenadas[0],VERMELHO_PALE.coordenadas[1], VERMELHO_PALE.coordenadas[2], VERMELHO_PALE.coordenadas[3], VERMELHO_PALE.cor);
    al_draw_filled_rectangle(AMARELO_PALE.coordenadas[0],AMARELO_PALE.coordenadas[1], AMARELO_PALE.coordenadas[2], AMARELO_PALE.coordenadas[3], AMARELO_PALE.cor);
    al_draw_filled_rectangle(AZUL_PALE.coordenadas[0],AZUL_PALE.coordenadas[1], AZUL_PALE.coordenadas[2], AZUL_PALE.coordenadas[3], AZUL_PALE.cor);
    al_draw_filled_rectangle(BRANCO_PALE.coordenadas[0],BRANCO_PALE.coordenadas[1], BRANCO_PALE.coordenadas[2], BRANCO_PALE.coordenadas[3], BRANCO_PALE.cor);
    al_draw_filled_rectangle(LARANJA_PALE.coordenadas[0], LARANJA_PALE.coordenadas[1], LARANJA_PALE.coordenadas[2], LARANJA_PALE.coordenadas[3], LARANJA_PALE.cor);
    al_draw_filled_rectangle(ROXO_PALE.coordenadas[0], ROXO_PALE.coordenadas[1], ROXO_PALE.coordenadas[2], ROXO_PALE.coordenadas[3], ROXO_PALE.cor);

    /*espessura*/
    al_draw_filled_rectangle(telinha.x * 0.01, telinha.y * 0.85, telinha.x *0.01 + 75, (telinha.y * 0.85) + 75, BRANCO);
    al_draw_filled_circle(telinha.x * 0.01 + 37, telinha.y * 0.85 + 35.5,espessura / scaling, COR);

    //Adicionando Anti-aliasing no bitmap
    //al_set_new_bitmap_samples(8);
    
    //Criando a imagem do canvas
    ALLEGRO_BITMAP* canvas = al_create_bitmap(canvas_w*scaling, canvas_h*scaling);
    if (!canvas) {
        printf("Erro ao criar o canvas!\n");
        return -1;
    }

    al_set_target_bitmap(canvas);
    al_clear_to_color(BRANCO);

    al_set_target_backbuffer(tela);//Volta os comandos para a tela


    TRACO* traco_novo = NULL;

    FILA *filaDeTracos = criarFila();

    //double agora, temp_loop = 0; // Variáveis não utilizadas.

    while (round) {
         // agora = al_get_time();

        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
        }

        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {
                ponto.x = evento.mouse.x;
                ponto.y = evento.mouse.y;

                if (ponto.x >= PRETO_PALE.coordenadas[0] && ponto.x <= PRETO_PALE.coordenadas[2] && ponto.y >= PRETO_PALE.coordenadas[1] && ponto.y <= PRETO_PALE.coordenadas[3]) { COR = PRETO; printf("Preto!\n"); }
                else if (ponto.x >= VERMELHO_PALE.coordenadas[0] && ponto.x <= VERMELHO_PALE.coordenadas[2] && ponto.y >= VERMELHO_PALE.coordenadas[1] && ponto.y <= VERMELHO_PALE.coordenadas[3]) { COR = VERMELHO; printf("Vermelho!\n"); }
                else if (ponto.x >= AMARELO_PALE.coordenadas[0] && ponto.x <= AMARELO_PALE.coordenadas[2] && ponto.y >= AMARELO_PALE.coordenadas[1] && ponto.y <= AMARELO_PALE.coordenadas[3]) { COR = AMARELO; printf("Amarelo!\n"); }
                else if (ponto.x >= AZUL_PALE.coordenadas[0] && ponto.x <= AZUL_PALE.coordenadas[2] && ponto.y >= AZUL_PALE.coordenadas[1] && ponto.y <= AZUL_PALE.coordenadas[3]) { COR = AZUL; printf("Azul!\n"); }
                else if (ponto.x >= BRANCO_PALE.coordenadas[0] && ponto.x <= BRANCO_PALE.coordenadas[2] && ponto.y >= BRANCO_PALE.coordenadas[1] && ponto.y <= BRANCO_PALE.coordenadas[3]) { COR = BRANCO; printf("Branco!\n"); }
                else if (ponto.x >= LARANJA_PALE.coordenadas[0] && ponto.x <= LARANJA_PALE.coordenadas[2] && ponto.y >= LARANJA_PALE.coordenadas[1] && ponto.y <= LARANJA_PALE.coordenadas[3]) { COR = LARANJA; printf("Laranja!\n"); }
                else if (ponto.x >= ROXO_PALE.coordenadas[0] && ponto.x <= ROXO_PALE.coordenadas[2] && ponto.y >= ROXO_PALE.coordenadas[1] && ponto.y <= ROXO_PALE.coordenadas[3]) { COR = ROXO; printf("Roxo!\n"); }


                ponto_canvas.x = (ponto.x - x_offset) * scaling;
                ponto_canvas.y = (ponto.y - y_offset) * scaling;
                
                if (ponto.x - x_offset >= 0 && ponto.x - x_offset <= canvas_w && ponto.y - y_offset >= 0 && ponto.y - y_offset <= canvas_h ){
                    ponto_inicial.x = ponto_canvas.x, ponto_inicial.y = ponto_canvas.y;
                    traco_novo = criarTraco(COR, espessura, ponto_canvas);

                    desenhar = true;

                    al_set_target_bitmap(canvas);
                    al_draw_filled_circle(ponto_canvas.x, ponto_canvas.y, espessura, COR);
                    al_set_target_backbuffer(tela);
                }
            }
        }

        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            ponto.x = evento.mouse.x;
            ponto.y = evento.mouse.y;

            ponto_canvas.x = (ponto.x - x_offset) * scaling;
            ponto_canvas.y = (ponto.y - y_offset) * scaling;

            if (desenhar && traco_novo) {

                al_set_target_bitmap(canvas);
                addPontos(traco_novo, ponto_canvas);
                desenharCirc(ponto_inicial, ponto_canvas, COR, espessura);
                al_set_target_backbuffer(tela);
                ponto_inicial = ponto_canvas;
            }
        }

        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (evento.mouse.button == 1 && desenhar) {
                pushFila(filaDeTracos,traco_novo);
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
                    clearFila(filaDeTracos);

                    printf("Limpar_tela\n");
                    printf("Fila limpa!\n");

                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_PAD_PLUS) {
                if (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
                    espessura++;

                    if (espessura > 10) espessura = 10;
                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_PAD_MINUS) {
                if (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
                    espessura--;
                    if (espessura < 1) espessura = 1;
                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_Z) {
                if (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
                    if (filaDeTracos->traco_atual > 0) {
                        filaDeTracos->traco_atual--;
                        int ultimo = filaDeTracos->traco_atual;

                        //libera a memória do último traço desfeito
                        free(filaDeTracos->tracos[ultimo].pontos);

                        //Anulando o ponteiro que liberamos com free, evitamos novos bugs como "double free", já que ele se torna inválido.
                        filaDeTracos->tracos[ultimo].pontos = NULL;

                        al_set_target_bitmap(canvas);//Todo comando de desenho vai pra imagem
                        al_clear_to_color(BRANCO);


                        for (int i = 0; i < filaDeTracos->traco_atual; i++) {
                            TRACO t = filaDeTracos->tracos[i];

                            // Redesenha o ponto inicial (clique do mouse)
                            al_draw_filled_circle(t.pontos[0].x, t.pontos[0].y, t.inicio.espessura, t.inicio.cor);

                            // Redesenha o resto da linha
                            VEC ponto_anterior = t.pontos[0];
                            for (size_t j = 1; j < t.tamanho; j++) {
                                desenharCirc(ponto_anterior, t.pontos[j], t.inicio.cor, t.inicio.espessura);
                                ponto_anterior = t.pontos[j];
                            }
                        }
                        al_set_target_backbuffer(tela);//Volta os comandos para a tela

                        printf("CTRL+Z executado! Restam %ld tracos.\n", filaDeTracos->traco_atual);
                    }
                }
            }
        }


        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            round = false;
        }

        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;

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

            /*espessura*/
            al_draw_filled_rectangle(telinha.x * 0.01, telinha.y * 0.85, telinha.x *0.01 + 75, (telinha.y * 0.85) + 75, BRANCO);
            al_draw_filled_circle(telinha.x * 0.01 + 37, telinha.y * 0.85 + 35.5,espessura / scaling, COR);

            al_draw_scaled_bitmap(canvas,0.0,0.0,canvas_w*scaling,canvas_h*scaling, x_offset, y_offset,canvas_w,canvas_h, 0);
            al_flip_display();

        }

        // temp_loop = al_get_time();
        /*if (agora - temp_loop >= 5) {
            socket01();

        }*/

    }

    // Liberação de memória
    al_destroy_font(font);
    al_destroy_bitmap(canvas);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    al_destroy_display(tela);
    destroyFila(filaDeTracos);

    return 0;
}

