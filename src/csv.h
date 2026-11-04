#ifndef CSVH 
#define CSVH 
#include <stdio.h>


typedef struct {
  const char* ingles;
  const char* traducao;
  const char* dica1;
  const char* dica2;
  const char* dica3;
}frase;

typedef struct {
  char* arena; // "Linguição" onde serão guardado os dados brutos
  frase* frases; // array paralelo onde serão guardados os ponteiros para os inícios de cada item
  int tamanho;
}deck;

deck* novo_deck(FILE *fptr, char dificuldade);
void destroy_deck(deck* d);

#endif 
