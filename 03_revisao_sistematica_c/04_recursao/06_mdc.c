/*
 * ============================================================================
 * Arquivo : 06_mdc.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Algoritmo de Euclides: mdc(a, b) = mdc(b, a % b), até b ser zero. O rastro
 *   mostra como os números encolhem rapidamente (o resto é sempre menor que o
 *   divisor) e compara com a tentativa ingênua de testar todos os divisores.
 *
 * EXEMPLO REAL
 *   Calcular a relação de engrenagens ou polias (ex.: 1071:462 = 51:22), ou
 *   simplificar a razão entre duas frequências de amostragem para achar o
 *   período comum de dois sinais.
 *
 * COMPLEXIDADE (a, b > 0)
 *   Euclides:        O(log min(a, b)) chamadas; pilha O(log min(a, b)).
 *   Força bruta:     O(min(a, b)) divisões (testa d = min, min-1, ..., 1).
 *   É recursão de cauda: o compilador (com -O2) pode transformá-la em laço.
 *
 * COMO EXECUTAR
 *   ./run 06_mdc               (pede dois inteiros positivos)
 *   ./run 06_mdc < /dev/null   (usa 1071 e 462)
 * ============================================================================
 */
#include <stdio.h>

static int chamadas = 0;

/* O(log min(a, b)). */
static int mdc(int a, int b, int nivel) {
    chamadas++;
    printf("%*smdc(%d, %d)", nivel * 2, "", a, b);
    if (b == 0) {
        printf("  -> caso base: retorna %d\n", a);
        return a;
    }
    printf("  -> %d %% %d = %d\n", a, b, a % b);
    return mdc(b, a % b, nivel + 1); /* última ação: recursão de cauda */
}

/* O(min(a, b)): testa os candidatos do maior para o menor. */
static int mdc_forca_bruta(int a, int b, int *divisoes) {
    int d = a < b ? a : b;
    *divisoes = 0;
    for (; d > 1; d--) {
        *divisoes += 2;
        if (a % d == 0 && b % d == 0)
            return d;
    }
    return 1;
}

int main(void) {
    int a = 1071, b = 462;
    printf("Informe dois inteiros positivos: ");
    if (scanf("%d %d", &a, &b) != 2 || a <= 0 || b <= 0) {
        a = 1071;
        b = 462;
        printf("(entrada vazia ou invalida: usando %d e %d)\n", a, b);
    }
    printf("\n=== Euclides recursivo ===\n");
    int r = mdc(a, b, 0);
    int divisoes;
    int r2 = mdc_forca_bruta(a, b, &divisoes);
    printf("\nMDC = %d em %d chamadas (%d divisoes; o caso base nao divide).\n",
           r, chamadas, chamadas - 1);
    printf("Forca bruta: MDC = %d apos ate %d divisoes.\n", r2, divisoes);
    printf("Relacao simplificada: %d:%d = %d:%d\n", a, b, a / r, b / r);
    return 0;
}
