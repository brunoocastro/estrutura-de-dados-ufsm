#include <math.h>
#include <stdio.h>

/* A mediana depende de o vetor estar ordenado. */

int em_ordem(const float v[], int tamanho) {
    for (int i = 1; i < tamanho; i++)
        if (v[i - 1] > v[i])
            return 0;
    return 1;
}

float mediana(const float v[], int tamanho) {
    /* Sem elementos ou fora de ordem, nao existe mediana valida. */
    if (tamanho <= 0 || !em_ordem(v, tamanho))
        return NAN;
    if (tamanho % 2 != 0)
        return v[tamanho / 2];
    return (v[tamanho / 2 - 1] + v[tamanho / 2]) / 2.0f;
}

int main(void) {
    int tamanho;
    printf("Informe a quantidade de valores: ");
    if (scanf("%d", &tamanho) != 1 || tamanho < 0)
        return 1;
    float v[tamanho > 0 ? tamanho : 1];
    printf("Informe os valores em ordem crescente: ");
    for (int i = 0; i < tamanho; i++)
        if (scanf("%f", &v[i]) != 1)
            return 1;
    float resultado = mediana(v, tamanho);
    if (isnan(resultado))
        printf("vetor vazio ou desordenado\n");
    else
        printf("%g\n", resultado);
    return 0;
}
