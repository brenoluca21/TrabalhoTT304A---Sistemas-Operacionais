#ifndef PROJETOSO_H
#define PROJETOSO_H
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
int *alocaMatriz(unsigned int n);

//Função soma parcial para ser executada em uma unica thread
void *somaParcial(void *args);

//Função multiplicação parcial para ser executada em uma unica thread
void *multiplicacaoParcial(void *args);

//Função redução parcial para ser executada em uma unica thread
void *reducaoParcial(void *args);

//Passo 2
//Função para realizar a soma das matrizes A e B usando threads
void somaTotal(int *matrizA, int *matrizB, int *matrizD, int n, int nthreads);

//Passo 5
//Função para realizar a multiplicação das matrizes C e D usando threads
void multiplicacaoTotal(int *matrizC, int *matrizD, int *matrizE, int n, int nthreads);

//Função para realizar a redução da matriz E usando threads
int reducaoTotal(int *matrizE, unsigned int n, unsigned int nthreads);

//Função para ler os arquivos das matrizes utilizando threads
void *lerMatriz(void *args);

//Função para gravar as matrizes em um arquivo utilizando threads
void *gravarMatriz(void *args);

//Passo 1
//Função que irá realizar as leituras simultaneas da matriz A e B  
void lerMatrizesAeB(int *matrizA, char *nomearqA, int *matrizB, char *nomearqB, int n);

//Passo 3 e Passo 4
//Função para gravar a matrizD e ler a matrizC simultaneamente
void gravarMatrizDlerMatrizC(int *matrizD, char *nomearqD, int *matrizC, char *nomearqC, int n);

//Passo 6 e Passo 7
//Função para gravar matrizE e realizar a Redução da matrizE simultaneamente 
void gravarMatrizEeReducao(int *matrizE, char *nomearqE, int n, int nthreads, int *reducao, double *tempored);

#endif