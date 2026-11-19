/*
 * ============================================================================
 * Arquivo : 02_soma_1_a_100.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 2: imprimir os números de 1 a 100 e a soma deles com um laço for.
 *
 * EXEMPLO REAL
 *   Totalizador: somar a produção de cada um dos n ciclos de uma máquina.
 *
 * COMPLEXIDADE (n = 100)
 *   O(n) tempo, O(1) espaço: uma volta por número.
 *   Curiosidade: a fórmula de Gauss n(n+1)/2 = 5050 dá a soma em O(1);
 *   o laço continua necessário para IMPRIMIR os n números.
 *
 * COMO EXECUTAR
 *   ./run 02_soma_1_a_100
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    int soma = 0;

    for (int numero = 1; numero <= 100; numero++) {
        printf("%d%s", numero, numero == 100 ? "\n" : " ");
        soma += numero;
    }

    printf("Soma: %d (formula de Gauss: %d)\n", soma, 100 * 101 / 2);
    return 0;
}
