/*
 * ============================================================================
 * Arquivo : 02_fibonacci_passos.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Fibonacci faz DUAS chamadas recursivas por vez, formando uma árvore de
 *   chamadas. O rastro indentado mostra a profundidade de cada chamada e
 *   marca as chamadas REPETIDAS (o mesmo F(k) calculado de novo), que são a
 *   causa do custo exponencial. A comparação completa com memoização e laço
 *   está em 07_fibonacci_memoizacao.c.
 *   F(0) = 0, F(1) = 1 e F(n) = F(n-1) + F(n-2).
 *
 * EXEMPLO REAL
 *   Padrões "divida em dois subproblemas" aparecem em mergesort, quicksort e
 *   em árvores. Quando os subproblemas se SOBREPÕEM (como aqui), a recursão
 *   ingênua desperdiça trabalho: é o sinal para usar memoização.
 *
 * COMPLEXIDADE (n = índice)
 *   Tempo: O(2^n) (mais precisamente O(φ^n), φ ≈ 1,618): chamadas = 2·F(n+1) − 1.
 *   Pilha: O(n): só um caminho da raiz até uma folha fica empilhado por vez.
 *
 * COMO EXECUTAR
 *   ./run 02_fibonacci_passos               (pede n entre 0 e 10)
 *   ./run 02_fibonacci_passos < /dev/null   (usa n = 5)
 * ============================================================================
 */
#include <stdio.h>

#define N_MAX 10
#define N_PADRAO 5

static unsigned long chamadas = 0;
static unsigned long repetidas = 0;
static int ja_calculado[N_MAX + 1]; /* 1 se F(k) já foi calculado antes */

/* O(2^n) tempo, O(n) pilha. */
static unsigned long fibonacci(unsigned int n, int nivel) {
    chamadas++;
    int repetida = ja_calculado[n];
    if (repetida) repetidas++;
    printf("%*sENTRA F(%u)%s\n", nivel * 3, "", n,
           repetida ? "   <- REPETIDA: ja foi calculada antes!" : "");
    if (n <= 1) {
        printf("%*sRETORNA F(%u) = %u (caso base)\n", nivel * 3, "", n, n);
        ja_calculado[n] = 1;
        return n;
    }

    unsigned long esquerda = fibonacci(n - 1, nivel + 1);
    unsigned long direita = fibonacci(n - 2, nivel + 1);
    unsigned long resultado = esquerda + direita;
    printf("%*sRETORNA F(%u) = %lu + %lu = %lu\n",
           nivel * 3, "", n, esquerda, direita, resultado);
    ja_calculado[n] = 1;
    return resultado;
}

int main(void) {
    unsigned int n = N_PADRAO;
    printf("Informe n entre 0 e %d: ", N_MAX);
    if (scanf("%u", &n) != 1 || n > N_MAX) {
        n = N_PADRAO;
        printf("(entrada vazia ou fora da faixa: usando n = %u)\n", n);
    }
    printf("\n=== Arvore de chamadas de F(%u) (indentacao = profundidade) ===\n", n);
    unsigned long r = fibonacci(n, 0);
    printf("\nResultado: F(%u) = %lu\n", n, r);
    printf("Chamadas: %lu | repetidas: %lu (%.0f%% de desperdicio)\n",
           chamadas, repetidas, chamadas ? 100.0 * (double)repetidas / (double)chamadas : 0.0);
    printf("Somente %u valores distintos (F(0)..F(%u)) precisavam ser calculados.\n", n + 1, n);
    printf("Veja 07_fibonacci_memoizacao.c: guardar os resultados reduz para O(n).\n");
    return 0;
}
