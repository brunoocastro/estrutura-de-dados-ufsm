/*
 * RECURSAO
 *
 * Uma funcao recursiva chama a si mesma. Toda recursao precisa de um caso base
 * para parar e de um passo que aproxime a entrada desse caso.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 06_recursao.c -o 06_recursao
 * ./06_recursao
 */
#include <stdio.h>

unsigned long fatorial(unsigned int n, unsigned int nivel) {
    printf("[entrada nivel %u] fatorial(%u)\n", nivel, n);

    if (n == 0) { /* Caso base: 0! = 1. */
        printf("[retorno nivel %u] fatorial(0) = 1\n", nivel);
        return 1;
    }

    /* A chamada desce até o caso base antes de qualquer multiplicacao. */
    unsigned long resultado_anterior = fatorial(n - 1, nivel + 1);
    unsigned long resultado = n * resultado_anterior;
    printf("[retorno nivel %u] %u * %lu = %lu\n",
           nivel, n, resultado_anterior, resultado);
    return resultado;
}

void contagem_regressiva(int n) {
    if (n < 0)
        return;
    printf("%d%s", n, n == 0 ? "\n" : " ");
    contagem_regressiva(n - 1);
}

int main(void) {
    printf("=== Calculo detalhado de 5! ===\n");
    printf("5! = %lu\n", fatorial(5, 0));
    printf("Contagem: ");
    contagem_regressiva(5);
    printf("Recursao usa a pilha de chamadas; entradas grandes podem causar estouro.\n");
    return 0;
}
