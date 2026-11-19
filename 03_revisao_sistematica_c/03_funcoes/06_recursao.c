/*
 * ============================================================================
 * Arquivo : 06_recursao.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Primeira recursão: uma função que chama a si mesma. Toda recursão precisa
 *   de um caso base (para parar) e de um passo que aproxime a entrada desse
 *   caso. A pasta ../04_recursao/ aprofunda o tema com a pilha desenhada.
 *
 * EXEMPLO REAL
 *   Percorrer as pastas de um sistema de arquivos ou os nós de uma árvore de
 *   tags de um SCADA: cada pasta/nó contém outros do mesmo tipo.
 *
 * COMPLEXIDADE (n = valor de entrada)
 *   fatorial(n):            O(n) tempo, O(n) pilha (n quadros empilhados)
 *   contagem_regressiva(n): O(n) tempo, O(n) pilha
 *
 * COMO EXECUTAR
 *   ./run 06_recursao
 * ============================================================================
 */
#include <stdio.h>

/* O(n). "nivel" só serve para mostrar a profundidade da chamada.
 * unsigned long tem pelo menos 32 bits: 12! cabe; 13! já pode estourar. */
unsigned long fatorial(unsigned int n, unsigned int nivel) {
    printf("[entrada nivel %u] fatorial(%u)\n", nivel, n);

    if (n == 0) { /* Caso base: 0! = 1. */
        printf("[retorno nivel %u] fatorial(0) = 1\n", nivel);
        return 1;
    }

    /* A chamada desce até o caso base antes de qualquer multiplicação. */
    unsigned long resultado_anterior = fatorial(n - 1, nivel + 1);
    unsigned long resultado = n * resultado_anterior;
    printf("[retorno nivel %u] %u * %lu = %lu\n",
           nivel, n, resultado_anterior, resultado);
    return resultado;
}

/* O(n). Imprime ANTES de chamar: os números aparecem na ordem da descida. */
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
