#include <stdio.h>

int main(void) {
    double vetor[10];
    for (int i = 0; i < 10; i++)
        if (scanf("%lf", &vetor[i]) != 1) return 1;

    double menor = vetor[0], maior = vetor[0];
    for (int i = 1; i < 10; i++) {
        if (vetor[i] < menor) menor = vetor[i];
        if (vetor[i] > maior) maior = vetor[i];
    }
    printf("menor: %g\nmaior: %g\n", menor, maior);
    return 0;
}
