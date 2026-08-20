/*
 * FIBONACCI RECURSIVO
 *
 * Fibonacci possui duas chamadas recursivas e evidencia a arvore de chamadas.
 * F(0)=0, F(1)=1 e F(n)=F(n-1)+F(n-2).
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 02_fibonacci_passos.c -o 02_fibonacci_passos
 * ./02_fibonacci_passos
 */
#include <stdio.h>

unsigned long fibonacci(unsigned int n, unsigned int nivel) {
    printf("%*sENTRA: F(%u)\n", nivel * 2, "", n);
    if (n <= 1) {
        printf("%*sRETORNA: F(%u) = %u\n", nivel * 2, "", n, n);
        return n;
    }

    unsigned long esquerda = fibonacci(n - 1, nivel + 1);
    unsigned long direita = fibonacci(n - 2, nivel + 1);
    unsigned long resultado = esquerda + direita;
    printf("%*sRETORNA: F(%u) = %lu + %lu = %lu\n",
           nivel * 2, "", n, esquerda, direita, resultado);
    return resultado;
}

int main(void) {
    unsigned int n;
    printf("Informe n entre 0 e 10: ");
    if (scanf("%u", &n) != 1 || n > 10)
        return 1;
    printf("\n=== Arvore de chamadas ===\n");
    printf("Resultado: %lu\n", fibonacci(n, 0));
    return 0;
}
