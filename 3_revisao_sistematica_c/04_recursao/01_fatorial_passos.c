/*
 * FATORIAL RECURSIVO COM VISUALIZACAO DA PILHA
 *
 * A entrada mostra a descida das chamadas e a subida dos retornos.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 01_fatorial_passos.c -o 01_fatorial_passos
 * ./01_fatorial_passos
 */
#include <stdio.h>

unsigned long fatorial(unsigned int n, unsigned int nivel) {
    printf("%*sENTRA: fatorial(%u)\n", nivel * 2, "", n);
    if (n == 0) {
        printf("%*sCASO BASE: retorna 1\n", nivel * 2, "");
        return 1;
    }

    unsigned long parcial = fatorial(n - 1, nivel + 1);
    unsigned long resultado = n * parcial;
    printf("%*sSAI: %u * %lu = %lu\n", nivel * 2, "", n, parcial, resultado);
    return resultado;
}

int main(void) {
    unsigned int n;
    printf("Informe um inteiro entre 0 e 12: ");
    if (scanf("%u", &n) != 1 || n > 12)
        return 1;
    printf("\n=== Rastreamento ===\n");
    printf("Resultado: %lu\n", fatorial(n, 0));
    return 0;
}
