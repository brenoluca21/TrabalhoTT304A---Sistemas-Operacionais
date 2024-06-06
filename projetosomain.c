#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "projetoso.h"

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

    //Passagem dos parametros para realizar as funções apenas na Thread principal
    parametrosThread parametrosOperacoes = {valoresmatrizA, valoresmatrizB, valoresmatrizC, valoresmatrizD, valoresmatrizE, 0, (n*n)-1, n};
    parametrosArquivo parametrosA = {valoresmatrizA, nomematrizA, n};
    parametrosArquivo parametrosB = {valoresmatrizB, nomematrizB, n};
    parametrosArquivo parametrosC = {valoresmatrizC, nomematrizC, n};
    parametrosArquivo parametrosD = {valoresmatrizD, nomematrizD, n};
    parametrosArquivo parametrosE = {valoresmatrizE, nomematrizE, n};
    
    //Passo 1
    //Leitura dos arquivos das matrizes A e B
    lerMatriz((void*) &parametrosA);
    lerMatriz((void*) &parametrosB);

    //Passo 2
    //Iniciando contagem de tempo para a função de soma
    iniciofunc = clock();
    //Somando as matrizes A e B e guardando o resultado na matriz D
    somaParcial((void*) &parametrosOperacoes);
    fimfunc = clock();
    //Finalizando contagem de tempo para a função de soma
    tempoSoma = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //Passo 3
    //Gravando os valores da Matriz D em um arquivo
    gravarMatriz((void*) &parametrosD);

    //Passo 4
    //Leitura do arquivo da matriz C
    lerMatriz((void*) &parametrosC);

    //Passo 5
    //Iniciando contagem de tempo para a função de multiplicação
    iniciofunc = clock();
    //Multiplicando as matrizes C e D e guardando o resultado na matriz E
    multiplicacaoParcial((void*) &parametrosOperacoes);
    fimfunc = clock();
    //Finalizando contagem de tempo para a função de multplicação
    tempoMultiplicacao = ((double)(fimfunc - iniciofunc)) / CLOCKS_PER_SEC;

    //Passo 6
    //Gravando os valores da matriz E em um arquivo
    gravarMatriz((void*) &parametrosE);

    //Passo 7
    //Iniciando contagem de tempo para a função de redução
    iniciofunc = clock();
    //Reduzindo a matriz E
    int *resultado = (int*)reducaoParcial((void*) &parametrosOperacoes);
    int reducao = *resultado;
    free(resultado);
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
    lerMatrizesAeB(valoresmatrizA, nomematrizA, valoresmatrizB, nomematrizB, n);
    
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
    gravarMatrizDlerMatrizC(valoresmatrizD, nomematrizD, valoresmatrizC, nomematrizC, n);

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
    gravarMatrizEeReducao(valoresmatrizE, nomematrizE, n, nthreads, &reducao, &temporeducao);


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
