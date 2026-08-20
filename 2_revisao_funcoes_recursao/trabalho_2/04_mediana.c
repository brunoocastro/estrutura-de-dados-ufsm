#include <math.h>
#include <stdio.h>

static int esta_ordenado(const float vetor[], int n) {
    if (n <= 1)
        return 1;
    if (vetor[0] > vetor[1])
        return 0;
    return esta_ordenado(vetor + 1, n - 1);
}

float mediana(const float vetor[], int n) {
    if (n <= 0 || !esta_ordenado(vetor, n))
        return NAN;

    if (n % 2 != 0)
        return vetor[n / 2];
    return (vetor[n / 2 - 1] + vetor[n / 2]) / 2.0f;
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1 || n < 0)
        return 1;

    float vetor[n > 0 ? n : 1];
    for (int i = 0; i < n; i++)
        if (scanf("%f", &vetor[i]) != 1)
            return 1;

    float resultado = mediana(vetor, n);
    if (isnan(resultado))
        printf("vetor vazio ou nao ordenado\n");
    else
        printf("%g\n", resultado);
    return 0;
}
