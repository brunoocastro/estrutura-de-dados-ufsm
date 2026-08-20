/*
 * MDC RECURSIVO — algoritmo de Euclides
 *
 * mdc(a,b) = mdc(b, a % b), ate b ser zero.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 06_mdc.c -o 06_mdc
 * ./06_mdc
 */
#include <stdio.h>

int mdc(int a, int b, int nivel) {
    printf("%*smdc(%d, %d)\n", nivel * 2, "", a, b);
    if (b == 0) {
        printf("%*scaso base: retorna %d\n", nivel * 2, "", a);
        return a;
    }
    return mdc(b, a % b, nivel + 1);
}

int main(void) {
    int a, b;
    printf("Informe dois inteiros positivos: ");
    if (scanf("%d %d", &a, &b) != 2 || a <= 0 || b <= 0)
        return 1;
    printf("MDC = %d\n", mdc(a, b, 0));
    return 0;
}
