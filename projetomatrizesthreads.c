#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//Estrutura para passar argumentos para as threads de Soma, Multiplicação e Redução
typedef struct{
    int *matrizA;
    int *matrizB;
    int *matrizC;
    int *matrizD;
    int *matrizE;
    int inicio;
    int final;
    int n;
} parametrosThread;

//Estrutura para passar argumentos para as threads de Leitura e Gravação das matrizes
typedef struct{
    int *matriz;
    char *nomearquivo;
    int n;
} parametrosArquivo;

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

    for (i = inicio; i < final; i++) {
      //Somando as posições correspondentes das matrizes A e B e armazenando o resultado na posição correspondente de D
        matrizD[i] = matrizA[i] + matrizB[i];
    }

    pthread_exit(NULL);
}

//Função multiplicação parcial para ser executada em uma unica thread
void *multiplicacaoParcial(void *args){
    register unsigned int i, inicio, final, j, k;
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

    for (int i = inicio; i < final; i++){
        int lin = i / n;
        int col = i % n;
        int aux = 0;
        for (int k = 0; k < n; k++) {
    //Realizando a multiplicação da linha da matrizC pela coluna da matrizD
            aux += matrizC[lin * n + k] * matrizD[k * n + col];
        }
        matrizE[i] = aux;
    }

    pthread_exit(NULL);
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

  for (i=inicio; i<final; i++){
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
        parametros[i].final = (i == nthreads - 1) ? (n * n) : elementosPorThread * (i + 1);
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
        parametros[i].final = (i == nthreads - 1) ? (n * n) : elementosPorThread * (i + 1);
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
    parametros[i].final  = (i == nthreads - 1) ? (n * n) : elementosPorThread * (i + 1);
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
void *lerMatrizThread(void *args){
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
    pthread_exit(NULL);
}

//Função para ler os arquivos das matrizes normalmente
void lerMatriz(char *nomematriz, int *matriz, int n){
    int i;
    int aux;

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
}


//Função para gravar a matriz em um arquivo normalmente
void gravarMatriz(char *nomeArquivo, int *matriz, int n){
    int i;
    int j;

    FILE *arqmatriz;
    arqmatriz = fopen(nomeArquivo, "w");
    if (arqmatriz == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++) {
            fprintf(arqmatriz, "%d ", matriz[i * n + j]);
        }
        fprintf(arqmatriz, "\n");
    }
    fclose(arqmatriz);
}


//Função para gravar as matrizes em um arquivo utilizando threads
void *gravarMatrizThread(void *args){
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
    pthread_exit(NULL);
}

//Passo 1
//Função que irá realizar as leituras simultaneas da matriz A e B  
void lerMatrizThreadesAeB(int *matrizA, char *nomearqA, int *matrizB, char *nomearqB, int n){
    pthread_t leituraA, leituraB;

    parametrosArquivo parametrosA = {matrizA, nomearqA, n};
    parametrosArquivo parametrosB = {matrizB, nomearqB, n};

    pthread_create(&leituraA, NULL, lerMatrizThread, &parametrosA);
    pthread_create(&leituraB, NULL, lerMatrizThread, &parametrosB);

    pthread_join(leituraA, NULL);
    pthread_join(leituraB, NULL);
}

//Passo 3 e Passo 4
//Função para gravar a matrizD e ler a matrizC simultaneamente
void gravarMatrizThreadDlerMatrizThreadC(int *matrizD, char *nomearqD, int *matrizC, char *nomearqC, int n){
    pthread_t gravacaoD, leituraC;

    parametrosArquivo parametrosD = {matrizD, nomearqD, n};
    parametrosArquivo parametrosC = {matrizC, nomearqC, n};

    pthread_create(&gravacaoD, NULL, gravarMatrizThread, &parametrosD);
    pthread_create(&leituraC, NULL, lerMatrizThread, &parametrosC);

    pthread_join(gravacaoD, NULL);
    pthread_join(leituraC, NULL);
}

//Passo 6 e Passo 7
//Função para gravar matrizE e realizar a Redução da matrizE simultaneamente 
void gravarMatrizThreadEeReducao(int *matrizE, char *nomearqE, int n, int nthreads, int *reducao, double *tempored){
    pthread_t gravacaoE;
    clock_t iniciofunc, fimfunc;

    parametrosArquivo parametrosE = {matrizE, nomearqE, n};

    pthread_create(&gravacaoE, NULL, gravarMatrizThread, &parametrosE);

    //Iniciando contagem de tempo para a função de redução
    iniciofunc = clock();
    *reducao = reducaoTotal(matrizE, n, nthreads);
    fimfunc = clock();
    //Finalizando contagem de tempo para a função Redução
    *tempored = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;
    pthread_join(gravacaoE, NULL);
}


//Função Main
int main(int argc, char *argv[]){
    //Guardando os argumentos passados
    int nthreads = atoi(argv[1]);
    int n = atoi(argv[2]);
    char *nomematrizA = argv[3];
    char *nomematrizB = argv[4];
    char *nomematrizC = argv[5];
    char *nomematrizD = argv[6];
    char *nomematrizE = argv[7];

    //Declaração das variáveis das matrizes
    int *valoresmatrizA; 
    int *valoresmatrizB; 
    int *valoresmatrizC; 
    int *valoresmatrizD; 
    int *valoresmatrizE;

    //Declaração das variáveis para o cálculo de tempo
    clock_t iniciofunc, fimfunc, inicioprog, fimprog;
    double tempoSoma, tempoMultiplicacao, tempoReducao, tempoTotal;

    //Caso tenha sido escolhido uma unica thread
    if(nthreads == 1){
    //Inicio da contagem do tempo para o programa inteiro
    inicioprog = clock();

    //Alocação dinâmica das matrizes
    valoresmatrizA = alocaMatriz(n);
    valoresmatrizB = alocaMatriz(n);
    valoresmatrizC = alocaMatriz(n);
    valoresmatrizD = alocaMatriz(n);
    valoresmatrizE = alocaMatriz(n);

    //Passo 1
    //Leitura dos arquivos das matrizes A e B
    lerMatriz(nomematrizA, valoresmatrizA, n);
    lerMatriz(nomematrizB, valoresmatrizB, n);

    //Passo 2
    //Iniciando contagem de tempo para a função de soma
    iniciofunc = clock();
    //Somando as matrizes A e B e guardando o resultado na matriz D
    somaTotal(valoresmatrizA, valoresmatrizB, valoresmatrizD, n, nthreads);
    fimfunc = clock();
    //Finalizando contagem de tempo para a função de soma
    tempoSoma = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //Passo 3
    //Gravando os valores da Matriz D em um arquivo
    gravarMatriz(nomematrizD, valoresmatrizD, n);

    //Passo 4
    //Leitura do arquivo da matriz C
    lerMatriz(nomematrizC, valoresmatrizC, n);

    //Passo 5
    //Iniciando contagem de tempo para a função de multiplicação
    iniciofunc = clock();
    //Multiplicando as matrizes C e D e guardando o resultado na matriz E
    multiplicacaoTotal(valoresmatrizC, valoresmatrizD, valoresmatrizE, n, nthreads);
    fimfunc = clock();
    //Finalizando contagem de tempo para a função de multplicação
    tempoMultiplicacao = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //Passo 6
    //Gravando os valores da matriz E em um arquivo
    gravarMatriz(nomematrizE, valoresmatrizE, n);

    //Passo 7
    //Iniciando contagem de tempo para a função de redução
    iniciofunc = clock();
    //Reduzindo a matriz E
    int reducao = reducaoTotal(valoresmatrizE, n, nthreads);
    fimfunc = clock();
    //Finalizando contagem de tempo para a função de redução
    tempoReducao = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //Liberando a memória da alocação dinâmica das matrizes
    free(valoresmatrizA);
    free(valoresmatrizB);
    free(valoresmatrizC);
    free(valoresmatrizD);
    free(valoresmatrizE);

    //Finalizando a contagem de tempo do programa
    fimprog = clock();
    tempoTotal = ((double)(fimprog - inicioprog)) / CLOCKS_PER_SEC;

    // Imprimindo os resultados
    printf("Reducao: %d\n", reducao);
    printf("Tempo soma: %.3f segundos.\n", tempoSoma);
    printf("Tempo multiplicacao: %.3f segundos.\n", tempoMultiplicacao);
    printf("Tempo reducao: %.3f segundos.\n", tempoReducao);
    printf("Tempo total: %.3f segundos.\n", tempoTotal);

    }

    //Caso tenha sido passado mais de uma thread
    if(nthreads > 1){
    //Inicio da contagem do tempo para o programa inteiro
    inicioprog = clock();

    //Alocação dinâmica das matrizes
    valoresmatrizA = alocaMatriz(n);
    valoresmatrizB = alocaMatriz(n);
    valoresmatrizC = alocaMatriz(n);
    valoresmatrizD = alocaMatriz(n);
    valoresmatrizE = alocaMatriz(n);

    //Passo 1
    //Leitura dos arquivos das matrizes A e B simultaneamnete
    lerMatrizThreadesAeB(valoresmatrizA, nomematrizA, valoresmatrizB, nomematrizB, n);
    
    //Passo 2
    //Iniciando contagem de tempo para a função de soma
    iniciofunc = clock();
    //Somando as matrizes A e B e guardando o resultado na matriz D
    somaTotal(valoresmatrizA, valoresmatrizB, valoresmatrizD, n, nthreads);
    //Finalizando contagem de tempo para a função soma
    fimfunc = clock();
    tempoSoma = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //Passo 3 e Passo 4
    //Gravando os valores da Matriz D em um arquivo e Lendo o arquivo da matrizC
    gravarMatrizThreadDlerMatrizThreadC(valoresmatrizD, nomematrizD, valoresmatrizC, nomematrizC, n);

    //Passo 5
    //Iniciando contagem de tempo para a função de multiplicação
    iniciofunc = clock();
    //Multiplicando as matrizes C e D e guardando o resultado na matriz E
    multiplicacaoTotal(valoresmatrizC, valoresmatrizD, valoresmatrizE, n, nthreads);
    //Finalizando contagem de tempo para a função de multiplição
    fimfunc = clock();
    tempoMultiplicacao = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //Passo 6 e Passo 7
    //Gravando os valores da matriz E em um arquivo e reduzindo a matriz E simultanemante
    int reducao;
    double temporeducao;
    //Calculo da função redução esta sendo calculado dentro desta função
    //Tempo total para a função de redução tambem esta sendo calculado dentro da funçãp
    gravarMatrizThreadEeReducao(valoresmatrizE, nomematrizE, n, nthreads, &reducao, &temporeducao);


    //Liberando a memória da alocação dinâmica das matrizes
    free(valoresmatrizA);
    free(valoresmatrizB);
    free(valoresmatrizC);
    free(valoresmatrizD);
    free(valoresmatrizE);

    // Finalizando a contagem de tempo do programa
    fimprog = clock();
    tempoTotal = ((double)(fimprog - inicioprog)) / CLOCKS_PER_SEC;

    // Imprimindo os resultados
    printf("Reducao: %d\n", reducao);
    printf("Tempo soma: %.3f segundos.\n", tempoSoma);
    printf("Tempo multiplicacao: %.3f segundos.\n", tempoMultiplicacao);
    printf("Tempo reducao: %.3f segundos.\n", temporeducao);
    printf("Tempo total: %.3f segundos.\n", tempoTotal);

    }

    return 0;
}
