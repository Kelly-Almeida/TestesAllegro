//Referências: https://rafaeltoledonet.wordpress.com/2011/11/30/tutorial-allegro-5-5-utilizando-o-mouse/
//https://liballeg.org/stabledocs/en/alleg009.html
//https://liballeg.org/a5docs/5.2.1/events.html
//https://www.allegro.cc/manual/5/ALLEGRO_MONITOR_INFO
//https://www.youtube.com/watch?v=EI6z4I8PfUc Para configurações do monitor
//https://rafaeltoledonet.wordpress.com/2011/11/10/tutorial-allegro-5-4-escrevendo-texto-usando-fontes-true-type-ttf/
//https://www.youtube.com/watch?v=XfZ6WrV5Z7Y

#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#ifdef WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif


/*Nossa bibliotecas :p*/
#include "sock.h"
#include "infra.h"


#define FPS 60


int main() {

    bool round = true, desenhar = false, redesenhar = false;

    // Inicializando o Allegro
    if (!al_init()) {
        printf("Falha ao inicializar o Allegro!\n");
        return -1;
    }

    // Inicializando a bal_mutex_unlockiblioteca de primitivas gráficas
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


    //al_set_new_display_flags(ALLEGRO_OPENGL);

    // Inicializando a tela
    ALLEGRO_DISPLAY* tela = al_create_display(telinha.x, telinha.y);
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
    if (!al_init_ttf_addon()) {
        printf("Erro ao inicializar a biblioteca de fontes ttf\n");
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }
    ALLEGRO_FONT *font = al_load_font("./resources/Roboto-Regular.ttf", 20, 0);
    if (!font) {
        printf("Não foi possível carregar a fonte!\n");
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

    COR_RGB VERDE = {172, 225, 57};
    COR_RGB VERMELHO = {255, 0, 0};
    COR_RGB AZUL = {0, 0, 255};
    COR_RGB AMARELO = {255, 255, 0};
    COR_RGB BRANCO = {255, 255, 255};
    COR_RGB PRETO = {0, 0, 0};
    COR_RGB LARANJA = {255, 128, 0};
    COR_RGB ROXO = {128, 0, 255};

    struct COR PRETO_PALE = {al_map_rgb(PRETO.r, PRETO.g, PRETO.b),{20, 20, 90, 75}};
    struct COR VERMELHO_PALE = {al_map_rgb(VERMELHO.r, VERMELHO.g, VERMELHO.b), {20, 105, 90, 160}};
    struct COR AMARELO_PALE = {al_map_rgb(AMARELO.r, AMARELO.g, AMARELO.b), {20, 190, 90, 245}};
    struct COR AZUL_PALE = {al_map_rgb(AZUL.r, AZUL.g, AZUL.b), {20, 280, 90, 335}};
    struct COR LARANJA_PALE = {al_map_rgb(LARANJA.r, LARANJA.g, LARANJA.b), {20, 370, 90, 425}};
    struct COR ROXO_PALE = {al_map_rgb(ROXO.r, ROXO.g, ROXO.b), {20, 450, 90, 500}};

    struct COR BRANCO_PALE = {al_map_rgb(BRANCO.r, BRANCO.g, BRANCO.b), {20, 530, 90, 580}};


    COR_RGB COR;
    short espessura;

    //Variáveis globais
    COR = AZUL;
    espessura = 3;
    VEC ponto = { 0,0 };
    VEC ponto_inicial;
    VEC ponto_canvas;

    al_clear_to_color(al_map_rgb(BRANCO.r, BRANCO.g, BRANCO.b));
    al_flip_display();

    float scaling = 0.25;
    float border[4];

    border[0] = telinha.x * 0.15; //canto esquerdo
    border[1] = telinha.y * 0.15; // canto superior
    border[2] = telinha.x - border[0]; // canto direito
    border[3] = telinha.y - border[1]; // canto inferior

    double canvas_w = border[2] - border[0];
    double canvas_h = border[3] - border[1];

    //Converter sistema de coordenada da tela para o Canvas:

    int x_offset = border[0];
    int y_offset = border[1];

    al_clear_to_color(al_map_rgb(VERDE.r,VERDE.g, VERDE.b));


    /*espessura*/
    al_draw_filled_rectangle(telinha.x * 0.01, telinha.y * 0.85, telinha.x *0.01 + 75, (telinha.y * 0.85) + 75, al_map_rgb(BRANCO.r, BRANCO.g, BRANCO.b));
    al_draw_filled_circle(telinha.x * 0.01 + 37, telinha.y * 0.85 + 35.5,espessura / scaling, al_map_rgb(COR.r, COR.g, COR.b));

    ALLEGRO_BITMAP* canvas = al_create_bitmap(canvas_w*scaling, canvas_h*scaling);
    if (!canvas) {
        printf("Erro ao criar o canvas!\n");
        return -1;
    }

    al_set_target_bitmap(canvas);
    al_clear_to_color(al_map_rgb(BRANCO.r, BRANCO.g, BRANCO.b));

    al_set_target_backbuffer(tela);//Volta os comandos para a tela


    TRACO* traco_novo = NULL;
    FILA *filaDeTracos = criarFila();

    MSG dados;
    dados.contador = 0;
    dados.evento = false;
    dados.mutex = al_create_mutex();

    if (!dados.mutex) {
        printf("Erro ao criar mutex!\n");
        return -1;
    }

    ALLEGRO_THREAD *socket = al_create_thread(sock, &dados);
    al_start_thread(socket);

    while (round) {

        al_lock_mutex(dados.mutex);
        if (dados.evento == 1) {
            RESP resposta = tratamento(dados.msg);
            printf("Mensagem recebida: %s\n", resposta.comando);

            if (strcmp(resposta.comando, "CABO") == 0) {
                dados.evento = 0;
                al_unlock_mutex(dados.mutex);
                break;
            }
            if (strcmp(resposta.comando, "LIMP") == 0) {
                limparCanvas(canvas, BRANCO, tela, filaDeTracos);
            }
            if (strcmp(resposta.comando, "UNDO") == 0) {
                undo(filaDeTracos, canvas, tela);
            }
            if (strcmp(resposta.comando, "TRSP") == 0) {
                COR_RGB cor = {resposta.corR, resposta.corG, resposta.corB};

                traco_novo = criarTraco(cor, espessura, ponto_canvas);
                desenhar = true;
                al_set_target_bitmap(canvas);
                al_draw_filled_circle(resposta.p1.x, resposta.p1.y, resposta.espessura, al_map_rgb(resposta.corR, resposta.corG, resposta.corB));
                al_set_target_backbuffer(tela);
                ponto_inicial = resposta.p1;

            }
            if (strcmp(resposta.comando, "NEWP") == 0) {
                if (desenhar && traco_novo) {
                    COR_RGB cor = {resposta.corR, resposta.corG, resposta.corB};

                    al_set_target_bitmap(canvas);
                    addPontos(traco_novo, ponto_canvas);
                    desenharCirc(ponto_inicial, ponto_canvas, cor, resposta.espessura);
                    al_set_target_backbuffer(tela);
                    ponto_inicial = ponto_canvas;

                    pushFila(filaDeTracos,traco_novo);
                    desenhar = false;
                }

            }
            dados.evento = 0;
        }
        al_unlock_mutex(dados.mutex);

      ALLEGRO_EVENT evento;
      al_wait_for_event(fila_eventos, &evento);
      //ENV envio;

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


              //envio.p1.x = ponto_canvas.x;
              //envio.p1.y = ponto_canvas.y;

              if (ponto.x - x_offset >= 0 && ponto.x - x_offset <= canvas_w && ponto.y - y_offset >= 0 && ponto.y - y_offset <= canvas_h ){
                  ponto_inicial.x = ponto_canvas.x, ponto_inicial.y = ponto_canvas.y;
                  traco_novo = criarTraco(COR, espessura, ponto_canvas);

                  desenhar = true;

                  al_set_target_bitmap(canvas);
                  al_draw_filled_circle(ponto_canvas.x, ponto_canvas.y, espessura, al_map_rgb(COR.r, COR.g, COR.b));
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
                  limparCanvas(canvas, BRANCO, tela, filaDeTracos);
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
              undo(filaDeTracos, canvas, tela);
            }
          }
      }


      if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
          round = false;
          break;
      }

      if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
          redesenhar = false;

          telaPrincipal(telinha);

          al_draw_text(font, al_map_rgb(0,50,255), 20, 600, 0, "Teste texto\n");
          /*espessura*/
          al_draw_filled_rectangle(telinha.x * 0.01, telinha.y * 0.85, telinha.x *0.01 + 75, (telinha.y * 0.85) + 75, al_map_rgb(BRANCO.r, BRANCO.g, BRANCO.b));
          al_draw_filled_circle(telinha.x * 0.01 + 37, telinha.y * 0.85 + 35.5,espessura / scaling, al_map_rgb(COR.r, COR.g, COR.b));

          al_draw_scaled_bitmap(canvas,0.0,0.0,canvas_w*scaling,canvas_h*scaling, x_offset, y_offset,canvas_w,canvas_h, 0);
          al_flip_display();

      }

    }

    // Liberação de memória
    al_set_thread_should_stop(socket);
    al_join_thread(socket, NULL);
    al_destroy_thread(socket);
    al_destroy_mutex(dados.mutex);
    al_destroy_font(font);
    al_destroy_bitmap(canvas);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    al_destroy_display(tela);
    destroyFila(filaDeTracos);

    return 0;
}
