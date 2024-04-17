#include <stdio.h>

#define n 3

int main(){
    int i, j;

 FILE *arq1,*arq2,*arq3;
    arq1 = fopen("arqA.dat", "w");
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++) {
            fprintf(arq1, "%d ", 1);
        }
        fprintf(arq1, "\n");
    }

    arq2 = fopen("arqB.dat", "w");
    for (i = 0; i < n; i++){
        for (int j = 0; j < n; j++) {
            fprintf(arq2, "%d ", 1);
        }
        fprintf(arq2, "\n");
    }

    arq3 = fopen("arqC.dat", "w");
    for (i = 0; i < n; i++){
        for (int j = 0; j < n; j++) {
            fprintf(arq3, "%d ", 1);
        }
        fprintf(arq3, "\n");
    }

    fclose(arq1);
    fclose(arq2);
    fclose(arq3);

    return 0;
}