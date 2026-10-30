//#include <stdio.h>
//#include <stdlib.h>

printf("Abrindo o arquivo...!\n");
FILE *file = fopen("resources/palavra em ingles_dicas preenchidas.csv", "r");
if (!file){
  printf("Erro ao abrir o arquivo!\n");
  return -1;
}

typedef struct {
  const char* frase;
  const char* traducao;
  const char* dica1;
  const char* dica2;
  const char* dica3;
} frase;

typedef struct {
  char* buffer; // "Linguição" onde será guardado os dados brutos
  frase* frases; // array paralelo onde serão guardados os ponteiros para os inícios de cada item
  int tamanho;
} deck;


//TODO:
frase* nova_frase(FILE * fptr, char *arena){

  size_t buff_size = 1000;
  char *buff = malloc(sizeof(char) * buff_size);

  fgets(buff, sizeof(char) * buff_size, fptr);
  printf("%s", buff);

  free(buff);

  return NULL;
}

  /*
long get_file_size(FILE* fp) {

  if (fseek(fp, 0L, SEEK_END) != 0) {
      fclose(fp);
      return -1;
  }

  long size = ftell(fp);
  rewind(fp);
  return size;
}

printf("O tamanho do arquivo em bytes é: %ld \n", get_file_size(file));
*/ 



// ignorando a primeira linha (cabeçalho).
for (int i = 0; i < 1; i++) {
  char ch = fgetc(file);
  while(ch != 10){
    ch = fgetc(file);
  }
}
char* arena =  malloc(10000);

frase *a = nova_frase(file, arena);
deck *b = NULL;

free(arena);

if (!a || !b){
  printf("echo\n");
}


/*
// imprimindo o arquivo um caractere de cada vez.
while(ch != EOF){
  putchar(ch);
  ch = fgetc(file);
}
*/ 
// fechando o arquivo.
fclose(file);

