/*
 * ============================================================================
 * Arquivo : 05_em_ordem.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 5: função em_ordem(v, n) que devolve 1 se o vetor está em
 *   ordem não decrescente e 0 caso contrário.
 *
 * EXEMPLO REAL
 *   Verificar se os registros de um datalogger chegaram em ordem de
 *   carimbo de tempo antes de usar uma busca binária (Unidade 4).
 *
 * COMPLEXIDADE (n = tamanho do vetor)
 *   Melhor caso O(1) (os dois primeiros já estão fora de ordem);
 *   pior caso O(n) (vetor ordenado: precisa olhar todos os pares).
 *   Espaço O(1).
 *
 * COMO EXECUTAR
 *   echo "1 2 3 4 5 6 7 8 9 10" | ./run 05_em_ordem
 * ============================================================================
 */
#include <stdio.h>

/* Verifica se cada elemento e menor ou igual ao proximo. */
int em_ordem(const float v[], int tamanho) {
    for (int i = 1; i < tamanho; i++)
        if (v[i - 1] > v[i])
            return 0;      /* basta um par fora de ordem */
    return 1;
}

int main(void) {
    float v[10];
    printf("Informe 10 valores reais:\n");
    for (int i = 0; i < 10; i++) {
        int lidos = scanf("%f", &v[i]);
        if (lidos == EOF) {
            printf("Entrada terminou apos %d valor(es).\n", i);
            return 0;
        }
        if (lidos != 1) {
            fprintf(stderr, "Entrada invalida.\n");
            return 1;
        }
    }
    printf("%s\n", em_ordem(v, 10) ? "O vetor esta em ordem" : "O vetor esta desordenado");
    return 0;
}
