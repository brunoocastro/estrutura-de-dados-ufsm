#include <stdio.h>

/* Funcao usada pelo programa de teste abaixo. */

int em_ordem(const float v[], int tamanho) {
    for (int i = 1; i < tamanho; i++)
        if (v[i - 1] > v[i])
            return 0;
    return 1;
}

int main(void) {
    float v[10];
    printf("Informe 10 valores reais:\n");
    for (int i = 0; i < 10; i++)
        if (scanf("%f", &v[i]) != 1)
            return 1;
    if (em_ordem(v, 10))
        printf("O vetor esta em ordem\n");
    else
        printf("O vetor esta desordenado\n");
    return 0;
}
