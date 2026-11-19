/*
 * ============================================================================
 * Arquivo : 01_potencia_int.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 1: função potencia_int(a, b) que calcula a^b (b >= 0) por
 *   multiplicações sucessivas, mostrando cada resultado parcial.
 *
 * EXEMPLO REAL
 *   Ganhos em cascata: n estágios de amplificação de ganho a dão ganho a^n;
 *   juros e crescimento compostos (exercício 2 reaproveita esta função).
 *
 * COMPLEXIDADE (b = expoente)
 *   O(b) tempo (b multiplicações), O(1) espaço.
 *   Existe um método O(log b) (exponenciação por quadrados), que eleva ao
 *   quadrado em vez de multiplicar por a a cada passo.
 *
 * COMO EXECUTAR
 *   echo "2 3" | ./run lista_2/01_potencia_int
 * ============================================================================
 */
#include <stdio.h>

/* Calcula a^b por multiplicações sucessivas. Pré-condição: b >= 0. */
float potencia_int(float a, int b) {
    /* O produto vazio representa a^0 = 1. */
    float resultado = 1.0f;

    for (int i = 0; i < b; i++) {
        resultado *= a;
        printf("  multiplicacao %d: resultado parcial = %g\n", i + 1, resultado);
    }

    return resultado;
}

int main(void) {
    float a;
    int b;

    printf("Informe a base real e o expoente inteiro nao negativo (a b): ");
    int lidos = scanf("%f %d", &a, &b);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 2 || b < 0) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }

    printf("%g\n", potencia_int(a, b));
    return 0;
}
