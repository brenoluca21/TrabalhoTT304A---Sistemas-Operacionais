#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "projetoso.h"

//Função para alocação das matrizes
int *alocaMatriz(unsigned int n){
  int *matriz;

  if (n*n<=0)
    return NULL;

  if ((matriz = (int *)malloc(n * n * sizeof(int))) == NULL){
    fprintf(stderr,"Problemas na alocacao do matriz\n");
    exit(EXIT_FAILURE);
  }

  return matriz;

}

//Função soma parcial para ser executada em uma unica thread
void *somaParcial(void *args){
    register unsigned int i, inicio, final;
    int *matrizA;
    int *matrizB;
    int *matrizD;
    //Extracao dos parametros da estrutura args.
    inicio = ( (parametrosThread *) args)->inicio;
    final  = ( (parametrosThread *) args)->final;
    matrizA  = ( (parametrosThread *) args)->matrizA;
    matrizB  = ( (parametrosThread *) args)->matrizB;
    matrizD  = ( (parametrosThread *) args)->matrizD;

    for (i = inicio; i <= final; i++) {
      //Somando as posições correspondentes das matrizes A e B e armazenando o resultado na posição correspondente de D
        matrizD[i] = matrizA[i] + matrizB[i];
    }
    return NULL;
}

//Função multiplicação parcial para ser executada em uma unica thread
void *multiplicacaoParcial(void *args){
    register unsigned int i, inicio, final, k;
    int *matrizC;
    int *matrizD;
    int *matrizE;
    int n;
    //Extracao dos parametros da estrutura args.
    inicio = ((parametrosThread *)args)->inicio;
    final = ((parametrosThread *)args)->final;
    matrizC = ((parametrosThread *)args)->matrizC;
    matrizD = ((parametrosThread *)args)->matrizD;
    matrizE = ((parametrosThread *)args)->matrizE;
    n = ((parametrosThread *)args)->n;

    for (i = inicio; i <= final; i++){
        int lin = i / n;
        int col = i % n;
        int aux = 0;
        for (k = 0; k < n; k++) {
    //Realizando a multiplicação da linha da matrizC pela coluna da matrizD
            aux += matrizC[lin * n + k] * matrizD[k * n + col];
        }
        matrizE[i] = aux;
    }

    return NULL;
}

//Função redução parcial para ser executada em uma unica thread
void *reducaoParcial(void *args){
  register unsigned int i, inicio, final;
  int *soma;
  int *matriz;

  //Extracao dos parametros da estrutura args.
  inicio = ( (parametrosThread *) args)->inicio;
  final  = ( (parametrosThread *) args)->final;
  matriz  = ( (parametrosThread *) args)->matrizE;


  if ((soma = (int *) malloc(sizeof(int))) == NULL){
    fprintf(stderr,"Problemas na alocacao para armazenamento da soma parcial\n");
    exit(EXIT_FAILURE);
  }

  *soma = 0;

  for (i=inicio; i <= final; i++){
     *soma += matriz[i];
  }

  return ((void *) soma);

}

//Passo 2
//Função para realizar a soma das matrizes A e B usando threads
void somaTotal(int *matrizA, int *matrizB, int *matrizD, int n, int nthreads){
    pthread_t idsThread[nthreads];
    parametrosThread parametros[nthreads];
    register unsigned int i;
    int tamanho = n*n;
    int elementosPorThread = tamanho/nthreads;

    if (tamanho % nthreads != 0){
    fprintf(stderr,"O tamanho nao e divisivel pelo numero de threads\n");
    exit(EXIT_FAILURE);
  }

    for (i = 0; i < nthreads; i++) {
        parametros[i].inicio = elementosPorThread * i;
        parametros[i].final = (elementosPorThread * (i+1)) - 1;
        parametros[i].matrizA = matrizA;
        parametros[i].matrizB = matrizB;
        parametros[i].matrizD = matrizD;

        pthread_create(&idsThread[i], NULL, somaParcial, (void *)&parametros[i]);
    }

    for (i = 0; i < nthreads; i++) {
        pthread_join(idsThread[i], NULL);
    }
}

//Passo 5
//Função para realizar a multiplicação das matrizes C e D usando threads
void multiplicacaoTotal(int *matrizC, int *matrizD, int *matrizE, int n, int nthreads){
    pthread_t idsThread[nthreads];
    parametrosThread parametros[nthreads];
    register unsigned int i;
    int tamanho = n*n;
    int elementosPorThread = tamanho/nthreads;

    if (tamanho % nthreads != 0){
    fprintf(stderr,"O tamanho nao e divisivel pelo numero de threads\n");
    exit(EXIT_FAILURE);
  }

    for (i = 0; i < nthreads; i++) {
        parametros[i].inicio = elementosPorThread * i;
        parametros[i].final = (elementosPorThread * (i+1)) - 1;
        parametros[i].matrizC = matrizC;
        parametros[i].matrizD = matrizD;
        parametros[i].matrizE = matrizE;
        parametros[i].n = n;

        pthread_create(&idsThread[i], NULL, multiplicacaoParcial, (void *)&parametros[i]);
    }

    for (i = 0; i < nthreads; i++) {
        pthread_join(idsThread[i], NULL);
    }
}

//Função para realizar a redução da matriz E usando threads
int reducaoTotal(int *matrizE, unsigned int n, unsigned int nthreads){
  void *somaParcial=NULL;
  parametrosThread parametros[nthreads];
  pthread_t idsThread[nthreads];
  register unsigned int i;
  int soma=0;
  int err;
  int tamanho = n*n;
  int elementosPorThread = tamanho/nthreads;

  if (tamanho % nthreads != 0){
    fprintf(stderr,"O tamanho nao e divisivel pelo numero de threads\n");
    exit(EXIT_FAILURE);
  }

  for (i=0; i<nthreads; i++){
    parametros[i].inicio = elementosPorThread * i;
    parametros[i].final  = (elementosPorThread * (i+1)) - 1;
    parametros[i].matrizE = matrizE;

    err = pthread_create(&idsThread[i],NULL,reducaoParcial,(void *) &parametros[i]);

    if (err != 0){
      fprintf(stderr,"Erro na criacao do thread %d\n",i);
      exit(EXIT_FAILURE);
    }
  }

  for (i=0; i<nthreads; i++){
    err = pthread_join(idsThread[i], &somaParcial);

    if (err != 0){
      fprintf(stderr,"Erro na juncao do thread %d\n",i);
      exit(EXIT_FAILURE);
    }

    soma += *((int *) somaParcial);
    free(somaParcial);

  }

  return soma;
}

//Função para ler os arquivos das matrizes utilizando threads
void *lerMatriz(void *args){
    unsigned int i, aux;
    int *matriz;
    char *nomematriz;
    int n;
    matriz = ((parametrosArquivo *) args)->matriz;
    nomematriz = ((parametrosArquivo *) args)->nomearquivo;
    n = ((parametrosArquivo *) args)->n;

    FILE *arqmatriz;
    arqmatriz = fopen(nomematriz, "r");
    if (arqmatriz == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomematriz);
        exit(1);
    }

    for (i = 0; i < n * n; i++){
        fscanf(arqmatriz, "%d", &aux);
        matriz[i] = aux;
    }
    fclose(arqmatriz);
    return NULL;
}

//Função para gravar as matrizes em um arquivo utilizando threads
void *gravarMatriz(void *args){
    register unsigned int i, j;
    int *matriz;
    char *nomematriz;
    int n;
    matriz = ((parametrosArquivo *) args)->matriz;
    nomematriz = ((parametrosArquivo *) args)->nomearquivo;
    n = ((parametrosArquivo *) args)->n;


    FILE *arqmatriz;
    arqmatriz = fopen(nomematriz, "w");
    if (arqmatriz == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomematriz);
        exit(1);
    }
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++) {
            fprintf(arqmatriz, "%d ", matriz[i * n + j]);
        }
        fprintf(arqmatriz, "\n");
    }
    fclose(arqmatriz);
    return NULL;
}

//Passo 1
//Função que irá realizar as leituras simultaneas da matriz A e B  
void lerMatrizesAeB(int *matrizA, char *nomearqA, int *matrizB, char *nomearqB, int n){
    pthread_t leituraA, leituraB;

    parametrosArquivo parametrosA = {matrizA, nomearqA, n};
    parametrosArquivo parametrosB = {matrizB, nomearqB, n};

    pthread_create(&leituraA, NULL, lerMatriz, &parametrosA);
    pthread_create(&leituraB, NULL, lerMatriz, &parametrosB);

    pthread_join(leituraA, NULL);
    pthread_join(leituraB, NULL);
}

//Passo 3 e Passo 4
//Função para gravar a matrizD e ler a matrizC simultaneamente
void gravarMatrizDlerMatrizC(int *matrizD, char *nomearqD, int *matrizC, char *nomearqC, int n){
    pthread_t gravacaoD, leituraC;

    parametrosArquivo parametrosD = {matrizD, nomearqD, n};
    parametrosArquivo parametrosC = {matrizC, nomearqC, n};

    pthread_create(&gravacaoD, NULL, gravarMatriz, &parametrosD);
    pthread_create(&leituraC, NULL, lerMatriz, &parametrosC);

    pthread_join(gravacaoD, NULL);
    pthread_join(leituraC, NULL);
}

//Passo 6 e Passo 7
//Função para gravar matrizE e realizar a Redução da matrizE simultaneamente 
void gravarMatrizEeReducao(int *matrizE, char *nomearqE, int n, int nthreads, int *reducao, double *tempored){
    pthread_t gravacaoE;
    clock_t iniciofunc, fimfunc;

    parametrosArquivo parametrosE = {matrizE, nomearqE, n};

    pthread_create(&gravacaoE, NULL, gravarMatriz, &parametrosE);

    //Iniciando contagem de tempo para a função de redução
    iniciofunc = clock();
    *reducao = reducaoTotal(matrizE, n, nthreads);
    fimfunc = clock();
    //Finalizando contagem de tempo para a função Redução
    *tempored = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;
    pthread_join(gravacaoE, NULL);
}
