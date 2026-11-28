//
// Created by sants on 2026-11-27.
//

#ifndef TESTC_TELAS_H
#define TESTC_TELAS_H

bool telaBase(TELA telinha,char *btn ,ALLEGRO_FONT *font, ALLEGRO_EVENT_QUEUE *evento, bool vitoria, bool inicio, int pontuacao);
void telaVitoria(TELA telinha, int pontuacao, ALLEGRO_FONT *font);
void telaDerrota(TELA telinha, int pontuacao, ALLEGRO_FONT *font);
#endif //TESTC_TELAS_H
