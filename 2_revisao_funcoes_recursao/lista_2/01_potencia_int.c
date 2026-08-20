#include <stdio.h>

/* Calcula a^b por multiplicações sucessivas. */

float potencia_int(float a, int b) {
    /* O produto vazio representa a^0 = 1. */
    float resultado = 1.0f;

    for (int i = 0; i < b; i++) {
        resultado *= a;
        printf("  multiplicacao %d: resultado parcial = %g\\n", i + 1, resultado);
    }

    return resultado;
}

int main(void) {
    float a;
    int b;

    printf("Informe a base real e o expoente inteiro nao negativo (a b): ");
    if (scanf("%f %d", &a, &b) != 2 || b < 0)
        return 1;

    printf("%g\n", potencia_int(a, b));
    return 0;
}
