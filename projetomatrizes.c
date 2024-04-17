#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//função soma que irá receber a matriz A, matriz B, numero de linhas e colunas e a matriz D
void somaMatriz(int *mA, int *mB, int n, int *mD){
    int i;
    int j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
    //calculando a soma de cada posição dos vetores das matrizes         
            mD[i * n + j] = mA[i * n + j] + mB[i * n + j];
        }
    }
}
//função multiplicação que irá receber a matriz C, matriz B, numero de linhas e colunas e a matriz E
void multiplicaMatriz(int *mC, int *mD, int n, int *mE){
    int i;
    int j; 
    int k;
    int aux;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            aux = 0;
            for (k = 0; k < n; k++) {
    //fazendo a multiplicação das posições dos vetores das matrizes e guardanado em um auxiliar
                aux += (mC[i * n + k] * mD[k * n + j]);
            }
    //passando o valor atual do auxiliar para a determinada posição do vetor da matriz E
            mE[i * n + j] = aux;
        }
    }
}

//função para ler os arquivos das matrizes, recebe o nome do arquivo, a variavel que irá receber a leitura do arquivo e o numero de linhas e colunas
void lerMatriz(char *nomematriz, int *matriz, int n){
    int i;
    int aux;

    FILE *arqmatriz;
    arqmatriz = fopen(nomematriz, "r");
    if (arqmatriz == NULL) {
    //mensagem de erro caso de problema ao abrir arquivo
        printf("Erro ao abrir o arquivo %s.\n", nomematriz);
        exit(1);
    }

    //lendo todas as posições do arquivo e armazenando na variavel matriz
    for (i = 0; i < n * n; i++){
        fscanf(arqmatriz, "%d", &aux);
        matriz[i] = aux;
    }
    fclose(arqmatriz);
}

//função para gravar a matriz em um arquivo, recebe o nome do arquivo, a variavel da matriz e o numero de linhas e colunas
void gravarMatriz(char *nomeArquivo, int *matriz, int n){
    int i;
    int j;

    FILE *arqmatriz;
    arqmatriz = fopen(nomeArquivo, "w");
    if (arqmatriz == NULL) {
    //mensagem de erro caso de problema ao abrir arquivo
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++) {
    //gravando os valores da matriz em cada posição no arquivo
            fprintf(arqmatriz, "%d ", matriz[i * n + j]);
        }
        fprintf(arqmatriz, "\n");
    }
    fclose(arqmatriz);
}

//função para o calculo da redução da matriz E, recebe a matriz E e o numero de linhas e colunas
int reducaoMatriz(int *mE, int n){
    int i;
    int j; 
    int reducao = 0;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            reducao += mE[i * n + j];
        }
    }
    //retorno do resultado da redução
    return reducao;
}

int main(int argc, char *argv[]){

    //declaração da variaveis onde os argumentos serão armazenados
    int nthreads = atoi(argv[1]);
    int n = atoi(argv[2]);
    char *nomematrizA = argv[3];
    char *nomematrizB = argv[4];
    char *nomematrizC = argv[5];
    char *nomematrizD = argv[6];
    char *nomematrizE = argv[7];

    //criação das matrizes como um ponteiro para que seja possível a alocação dinamica em uma unica etapa
    int *valormatrizA; 
    int *valormatrizB; 
    int *valormatrizC; 
    int *valormatrizD; 
    int *valormatrizE;

    //declaração das variavies para o calculo de tempo de cada função e para o calculo de tempo do programa inteiro
    clock_t iniciofunc, fimfunc, inicioprog, fimprog;
    double tempoSoma, tempoMultiplicacao, tempoReducao, tempoTotal;

    //iniciando contagem do tempo para o porgrama inteiro
    inicioprog = clock();

    //alocação dinâmica de todas as matrizes em uma unica etapa
    valormatrizA = (int *)malloc(n * n * sizeof(int));
    valormatrizB = (int *)malloc(n * n * sizeof(int));
    valormatrizC = (int *)malloc(n * n * sizeof(int));
    valormatrizD = (int *)malloc(n * n * sizeof(int));
    valormatrizE = (int *)malloc(n * n * sizeof(int));

    //lendo arquivos das matrizes A e B
        lerMatriz(nomematrizA, valormatrizA, n);
        lerMatriz(nomematrizB, valormatrizB, n);

    //iniciando contagem de tempo para a função de soma
    iniciofunc = clock();
    //somando as matrizes A e B e guardando o resultado na matriz D
        somaMatriz(valormatrizA, valormatrizB, n, valormatrizD);
    //finaizando a contagem de tempo para a função de soma
    fimfunc = clock();
    //calculando o tempo da função soma
    tempoSoma = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //gravando os valores da Matriz D em um arquivo
        gravarMatriz(nomematrizD, valormatrizD, n);
    //lendo o arquivo da matriz C
        lerMatriz(nomematrizC, valormatrizC, n);

    //iniciando contagem de tempo para a função de multiplicação
    iniciofunc = clock();
    //fazendo a multiplicação das matrizes C e D e guardando o resultado na matriz E
        multiplicaMatriz(valormatrizC, valormatrizD, n, valormatrizE);
    //finalizando a contagem de tempo da função de multiplicação
    fimfunc = clock();
    //calculando o tempo para a função multiplicação
    tempoMultiplicacao = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //gravando os valores da matriz E em um arquivo
        gravarMatriz(nomematrizE, valormatrizE, n);

    //iniciando contagem de tempo para a função redução
    iniciofunc = clock();
    //fazendo a redução da matriz E e retornando o valor como um inteiro para a variavel reducao
        int reducao = reducaoMatriz(valormatrizE, n);
    //finalizando a contagem de tempo da função redução
    fimfunc = clock();
    //calculando o tempo necessário para realizar a redução
    tempoReducao = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //finalizando a contagem de tempo do porgrama
    fimprog = clock();
    //calculando o tempo necessário para a execução do programa
    tempoTotal = ((double)(fimprog - inicioprog)) / CLOCKS_PER_SEC;

    //printando o resultado da redução e de todos os tempos calculados
        printf("Reducao: %d\n", reducao);
        printf("Tempo soma: %.3f segundos.\n", tempoSoma);
        printf("Tempo multiplicacao: %.3f segundos.\n", tempoMultiplicacao);
        printf("Tempo reducao: %.3f segundos.\n", tempoReducao);
        printf("Tempo total: %.3f segundos.\n", tempoTotal);

    //liberando a memoria da alocação dinamica das matrizes
    free(valormatrizA);
    free(valormatrizB);
    free(valormatrizC);
    free(valormatrizD);
    free(valormatrizE);

    return 0;
}