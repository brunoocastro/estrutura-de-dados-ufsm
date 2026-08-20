#include <stdio.h>

/* Reaproveita a potencia por multiplicacoes sucessivas. */

float potencia_int(float a, int b) {
    float resultado = 1.0f;
    for (int i = 0; i < b; i++)
        resultado *= a;
    return resultado;
}

int main(void) {
    float capital, taxa;
    int meses;

    printf("Informe o capital inicial, a taxa mensal e a quantidade de meses: ");
    if (scanf("%f %f %d", &capital, &taxa, &meses) != 3 ||
        capital < 0.0f || taxa <= 0.0f || meses <= 0)
        return 1;

    /* M = C * (1 + i)^t. */
    printf("Montante: %g\n", capital * potencia_int(1.0f + taxa, meses));
    return 0;
}
