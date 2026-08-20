#include <stdio.h>

/* Verifica se cada elemento e menor ou igual ao proximo. */

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
    printf("%s\n", em_ordem(v, 10) ? "O vetor esta em ordem" : "O vetor esta desordenado");
    return 0;
}
