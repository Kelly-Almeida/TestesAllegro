#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include "sock.h"
#include "infra.h"
#include "sockClient.h"
#include "csv.h"

#define FPS 60

int main() {

    srand(time(NULL));

    bool round = true, desenhar = false, redesenhar = false;

    printf("Abrindo o arquivo...!\n");
    FILE *file = fopen("resources/palavra em ingles_dicas preenchidas.csv", "r");
    if (!file){
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }
    deck *faceis =    novo_deck(file, '0');
    deck *medias =    novo_deck(file, '1');
    deck *dificeis =  novo_deck(file, '2');
    deck *dificuldades[3] = {faceis, medias, dificeis};


    fclose(file);

    if (!al_init() || !al_init_primitives_addon() || !al_init_image_addon()) {
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

    ALLEGRO_MONITOR_INFO info;
    TELA telinha;
    al_get_monitor_info(0, &info);

    telinha.x = info.x2 - info.x1 - 100;
    telinha.y = info.y2 - info.y1 - 50;

    ALLEGRO_DISPLAY* tela = al_create_display(telinha.x, telinha.y);
    if (!tela) return -1;

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }

    if (!al_install_mouse() || !al_install_keyboard()) {
        al_destroy_timer(timer);
        al_destroy_display(tela);
        return -1;
    }

    al_set_system_mouse_cursor(tela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    if (!al_init_font_addon() || !al_init_ttf_addon()) return -1;

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

    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(tela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    al_start_timer(timer);

    COR_RGB /*VERDE = {172, 225, 57},*/ VERMELHO = {255, 0, 0}, AZUL = {0, 0, 255};
    COR_RGB AMARELO = {255, 255, 0}, BRANCO = {255, 255, 255}, PRETO = {0, 0, 0};
    COR_RGB LARANJA = {255, 128, 0}, ROXO = {128, 0, 255};

    struct COR PRETO_PALE = {al_map_rgb(PRETO.r, PRETO.g, PRETO.b), {20, 20, 90, 75}};
    struct COR VERMELHO_PALE = {al_map_rgb(VERMELHO.r, VERMELHO.g, VERMELHO.b), {20, 105, 90, 160}};
    struct COR AMARELO_PALE = {al_map_rgb(AMARELO.r, AMARELO.g, AMARELO.b), {20, 190, 90, 245}};
    struct COR AZUL_PALE = {al_map_rgb(AZUL.r, AZUL.g, AZUL.b), {20, 280, 90, 335}};
    struct COR LARANJA_PALE = {al_map_rgb(LARANJA.r, LARANJA.g, LARANJA.b), {20, 370, 90, 425}};
    struct COR ROXO_PALE = {al_map_rgb(ROXO.r, ROXO.g, ROXO.b), {20, 450, 90, 500}};
    struct COR BRANCO_PALE = {al_map_rgb(BRANCO.r, BRANCO.g, BRANCO.b), {20, 530, 90, 580}};

    COR_RGB COR = PRETO;
    short espessura = 3;
    VEC ponto = {0, 0}, ponto_inicial, ponto_canvas;
    int DIFICULDADE = 0;
    int indice = rand() % dificuldades[DIFICULDADE]->tamanho;

    float scaling = 0.25;
    float border[4] = { telinha.x * 0.15, telinha.y * 0.15, telinha.x - border[0], telinha.y - border[1] };

    double canvas_w = border[2] - border[0];
    double canvas_h = border[3] - border[1];

    int x_offset = border[0];
    int y_offset = border[1];

    ALLEGRO_BITMAP* canvas = al_create_bitmap(canvas_w * scaling, canvas_h * scaling);
    al_set_target_bitmap(canvas);
    al_clear_to_color(al_map_rgb(BRANCO.r, BRANCO.g, BRANCO.b));
    al_set_target_backbuffer(tela);

    TRACO* traco_novo = NULL;
    FILA *filaDeTracos = criarFila();

    MSG_S dados;
    dados.contador = 0;
    dados.evento = false;
    dados.mutex = al_create_mutex();

    /*ALTERAÇÃO*/
    bool cliente = false;

    if (cliente) {
        dados.cond = al_create_cond();
    }

    ALLEGRO_THREAD *socket;
    if (cliente) {
        socket = al_create_thread(client, &dados);
    } else {
        socket = al_create_thread(sock, &dados);
    }

    al_start_thread(socket);
    //RESP respostaAntiga;

    while (round) {

        if (!cliente) {
            al_lock_mutex(dados.mutex);
            if (dados.evento == 1) {
                RESP resposta = tratamento(dados.msg);

                /*
                if (resposta.corR < 0 || resposta.corR > 255) {
                    if (resposta.corR < 0 || resposta.corR > 255) {
                        resposta.corR = 255;
                    }else {
                        resposta.corR = respostaAntiga.corR;
                    }
                }

                if (resposta.corG < 0 || resposta.corG > 255) {
                    if (resposta.corG < 0 || resposta.corG > 255) {
                        resposta.corG = 255;
                    }else {
                        resposta.corG = respostaAntiga.corG;
                    }
                }

                if (resposta.corB < 0 || resposta.corB > 255) {
                    if (resposta.corB < 0 || resposta.corB > 255) {
                        resposta.corB = 255;
                    }else {
                        resposta.corB = respostaAntiga.corB;
                    }
                }

                if (resposta.espessura < 0 || resposta.espessura > 10) {
                    if (respostaAntiga.espessura < 0 || respostaAntiga.espessura > 10) resposta.espessura = 1;
                    else resposta.espessura = respostaAntiga.espessura;
                }

                if (resposta.p1.x < 0 || resposta.p1.x > 800) {
                    if (respostaAntiga.p1.x < 0 || respostaAntiga.p1.x > 800) resposta.p1.x = 1;
                    else resposta.p1.x = respostaAntiga.p1.x;
                }

                if (resposta.p1.y < 0 || resposta.p1.y > 800) {
                    if (respostaAntiga.p1.y < 0 || respostaAntiga.p1.y > 800) resposta.p1.y = 1;
                    else resposta.p1.y = respostaAntiga.p1.y;
                }
                */

                if (strcmp(resposta.comando, "TRSP") == 0 || strcmp(resposta.comando, "NEWP") == 0) {
                    if (resposta.corR >= 0 && resposta.corR <= 255 &&
                        resposta.corG >= 0 && resposta.corG <= 255 &&
                        resposta.corB >= 0 && resposta.corB <= 255) {

                        if (strcmp(resposta.comando, "TRSP") == 0) {
                            COR_RGB cor = {resposta.corR, resposta.corG, resposta.corB};
                            al_set_target_bitmap(canvas);
                            al_draw_filled_circle(resposta.p1.x, resposta.p1.y, resposta.espessura, al_map_rgb(cor.r, cor.g, cor.b));
                            al_set_target_backbuffer(tela);

                            traco_novo = criarTraco(cor, resposta.espessura, resposta.p1);
                            ponto_inicial = resposta.p1;
                        }
                        else if (strcmp(resposta.comando, "NEWP") == 0) {
                            if (traco_novo != NULL) {

                                if (resposta.p1.x >=0 && resposta.p1.y >= 0) {
                                    COR_RGB cor = {resposta.corR, resposta.corG, resposta.corB};
                                    al_set_target_bitmap(canvas);
                                    addPontos(traco_novo, resposta.p1);
                                    desenharCirc(ponto_inicial, resposta.p1, cor, resposta.espessura);
                                    al_set_target_backbuffer(tela);
                                    ponto_inicial = resposta.p1;
                                }

                            }
                        }

                        }
                }
                else if (strcmp(resposta.comando, "NEWT") == 0) {
                    if (traco_novo != NULL) {
                        pushFila(filaDeTracos, traco_novo);
                        traco_novo = NULL;
                    }
                }
                else if (strcmp(resposta.comando, "LIMP") == 0) {
                    limparCanvas(canvas, BRANCO, tela, filaDeTracos);
                }
                else if (strcmp(resposta.comando, "UNDO") == 0) {
                    undo(filaDeTracos, canvas, tela);
                }
                else if (strcmp(resposta.comando, "FIMP") == 0) {
                    round = false;
                }

                //respostaAntiga = resposta;
                dados.evento = 0;
            }
            al_unlock_mutex(dados.mutex);
        }

        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (evento.mouse.button == 1) {
                ponto.x = evento.mouse.x;
                ponto.y = evento.mouse.y;

                if (ponto.x >= PRETO_PALE.coordenadas[0] && ponto.x <= PRETO_PALE.coordenadas[2] && ponto.y >= PRETO_PALE.coordenadas[1] && ponto.y <= PRETO_PALE.coordenadas[3]) COR = PRETO;
                else if (ponto.x >= VERMELHO_PALE.coordenadas[0] && ponto.x <= VERMELHO_PALE.coordenadas[2] && ponto.y >= VERMELHO_PALE.coordenadas[1] && ponto.y <= VERMELHO_PALE.coordenadas[3]) COR = VERMELHO;
                else if (ponto.x >= AMARELO_PALE.coordenadas[0] && ponto.x <= AMARELO_PALE.coordenadas[2] && ponto.y >= AMARELO_PALE.coordenadas[1] && ponto.y <= AMARELO_PALE.coordenadas[3]) COR = AMARELO;
                else if (ponto.x >= AZUL_PALE.coordenadas[0] && ponto.x <= AZUL_PALE.coordenadas[2] && ponto.y >= AZUL_PALE.coordenadas[1] && ponto.y <= AZUL_PALE.coordenadas[3]) COR = AZUL;
                else if (ponto.x >= BRANCO_PALE.coordenadas[0] && ponto.x <= BRANCO_PALE.coordenadas[2] && ponto.y >= BRANCO_PALE.coordenadas[1] && ponto.y <= BRANCO_PALE.coordenadas[3]) COR = BRANCO;
                else if (ponto.x >= LARANJA_PALE.coordenadas[0] && ponto.x <= LARANJA_PALE.coordenadas[2] && ponto.y >= LARANJA_PALE.coordenadas[1] && ponto.y <= LARANJA_PALE.coordenadas[3]) COR = LARANJA;
                else if (ponto.x >= ROXO_PALE.coordenadas[0] && ponto.x <= ROXO_PALE.coordenadas[2] && ponto.y >= ROXO_PALE.coordenadas[1] && ponto.y <= ROXO_PALE.coordenadas[3]) COR = ROXO;

                ponto_canvas.x = (ponto.x - x_offset) * scaling;
                ponto_canvas.y = (ponto.y - y_offset) * scaling;

                if (ponto.x - x_offset >= 0 && ponto.x - x_offset <= canvas_w && ponto.y - y_offset >= 0 && ponto.y - y_offset <= canvas_h) {
                    ponto_inicial = ponto_canvas;
                    traco_novo = criarTraco(COR, espessura, ponto_canvas);
                    desenhar = true;

                    al_set_target_bitmap(canvas);
                    al_draw_filled_circle(ponto_canvas.x, ponto_canvas.y, espessura, al_map_rgb(COR.r, COR.g, COR.b));
                    al_set_target_backbuffer(tela);

                    if (cliente) {
                        char msg_env[256];
                        snprintf(msg_env, sizeof(msg_env), "TRSP %d %d %d %d %d %d",
                                 ponto_canvas.x, ponto_canvas.y, espessura, COR.r, COR.g, COR.b);
                        transpCliente(&dados, msg_env);
                    }
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

                if (cliente) {
                    int dx = abs(ponto_canvas.x - ponto_inicial.x);
                    int dy = abs(ponto_canvas.y - ponto_inicial.y);

                    if (dx >= 2 || dy >= 2) {
                        char msg_env[256];
                        snprintf(msg_env, sizeof(msg_env), "NEWP %d %d %d %d %d %d",
                                 ponto_canvas.x, ponto_canvas.y, espessura, COR.r, COR.g, COR.b);
                        transpCliente(&dados, msg_env);

                        ponto_inicial = ponto_canvas;
                    }
                }else {
                    ponto_inicial = ponto_canvas;
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (evento.mouse.button == 1 && desenhar) {
                pushFila(filaDeTracos, traco_novo);
                desenhar = false;

                if (cliente) {
                    transpCliente(&dados, "NEWT");
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_Q && (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)) {
                al_save_bitmap("teste.bmp", canvas);

                if (cliente) {
                        transpCliente(&dados, "FIMP");
                }

                round = false;
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_R && (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)) {
                limparCanvas(canvas, BRANCO, tela, filaDeTracos);
                if (cliente) {
                    transpCliente(&dados, "LIMP");
                }
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_PAD_PLUS && (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)) {
                espessura++;
                if (espessura > 10) espessura = 10;
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_PAD_MINUS && (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)) {
                espessura--;
                if (espessura < 1) espessura = 1;
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_Z && (evento.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)) {
                undo(filaDeTracos, canvas, tela);
                if (cliente) {
                    transpCliente(&dados, "UNDO");
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            round = false;
        }

        // --- ATUALIZAÇÃO DO DISPLAY ---
        if (redesenhar && al_is_event_queue_empty(fila_eventos)) {
            redesenhar = false;

            telaPrincipal(telinha, font, dificuldades[DIFICULDADE], indice);
            al_draw_text(font, al_map_rgb(0, 50, 255), 20, 600, 0, "Teste texto\n");

            al_draw_filled_rectangle(telinha.x * 0.01, telinha.y * 0.85, telinha.x * 0.01 + 75, (telinha.y * 0.85) + 75, al_map_rgb(BRANCO.r, BRANCO.g, BRANCO.b));
            al_draw_filled_circle(telinha.x * 0.01 + 37, telinha.y * 0.85 + 35.5, espessura / scaling, al_map_rgb(COR.r, COR.g, COR.b));

            al_draw_scaled_bitmap(canvas, 0.0, 0.0, canvas_w * scaling, canvas_h * scaling, x_offset, y_offset, canvas_w, canvas_h, 0);
            al_flip_display();
        }
    }

    if (cliente) al_destroy_cond(dados.cond);
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