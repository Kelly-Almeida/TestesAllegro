#include "csv.h"
#include "stdio.h"
#include "stdlib.h"

deck* novo_deck(FILE *fptr, char dificuldade){
  deck *d = malloc(sizeof(deck));

  size_t temp_size = 1000;
  char *temp = malloc(sizeof(char) * temp_size);

  // pulando o cabeçalho e as dificuldades menores.
  // inicio guarda a posição ANTES de cada leitura, então ao sair do laço
  // ele aponta para o começo da linha que está em temp.
  long int inicio = ftell(fptr);
  char *flag = fgets(temp, temp_size, fptr);
  while (flag != NULL && temp[0] != dificuldade) {
    inicio = ftell(fptr);
    flag = fgets(temp, temp_size, fptr);
  }

  // Primeira leitura: contando caracteres e quantidade de linhas.
  int num_linhas = 0;
  int total = 0;
  while (flag != NULL && temp[0] == dificuldade) {
    int j = 0;
    while (temp[j] != '\0') {
      j++;
    }
    total += j;
    num_linhas++;
    flag = fgets(temp, temp_size, fptr);
  }

  //printf("total: %d\n", total);
  d->arena = malloc(sizeof(char) * total);
  d->frases = malloc(sizeof(frase) * num_linhas);
  d->tamanho = num_linhas;

  // rebobinando para o início dos dados
  fseek(fptr, inicio, SEEK_SET);

  char *indice_arena = d->arena; // cabeça de escrita, indicando o próximo espaço vazio.
  for (int t = 0; t < num_linhas; t++) {
    fgets(temp, temp_size, fptr);
    char *linha = temp + 2; // ignorando a dificuldade e seu delimitador

    frase *f = &d->frases[t];
    const char **campos[] = { &f->ingles, &f->traducao, &f->dica1, &f->dica2, &f->dica3 };

    int i = 0;
    for (int c = 0; c < 5; c++) {
      *campos[c] = indice_arena + i;
      while (linha[i] != ';' && linha[i] != '\n' && linha[i] != '\r' && linha[i] != '\0') {
        indice_arena[i] = linha[i];
        i++;
      }
      indice_arena[i] = '\0';
      i++;
    }
    indice_arena = indice_arena + i;
  }

  free(temp);

  return d;
}

void destroy_deck(deck* d){
  free(d->arena);
  free(d->frases);
  free(d);
  return;
}


/*


printf("%s", "Imprimindo as frases guardadas no struct\n");
for (int i = 0; i < faceis->tamanho; i++) {

  printf("%s\n", faceis->frases[i].frase);
  printf("%s\n", faceis->frases[i].traducao);
  printf("%s\n", faceis->frases[i].dica1);
  printf("%s\n", faceis->frases[i].dica2);
  printf("%s\n", faceis->frases[i].dica3);
  printf("\n");

}
*/
