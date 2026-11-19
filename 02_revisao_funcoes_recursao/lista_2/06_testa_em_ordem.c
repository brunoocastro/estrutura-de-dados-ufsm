/*
 * ============================================================================
 * Arquivo : 06_testa_em_ordem.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 6: programa de teste para em_ordem. Roda casos automáticos de
 *   borda (vazio, um elemento, repetidos, desordem no fim) e depois testa os
 *   10 valores digitados.
 *
 * EXEMPLO REAL
 *   Testes de unidade: a função é validada isoladamente, com entradas
 *   escolhidas para quebrar implementações erradas (ex.: usar < em vez de
 *   <= reprova o caso com valores repetidos).
 *
 * COMPLEXIDADE (n = tamanho do vetor)
 *   em_ordem: O(n) no pior caso, O(1) espaço.
 *
 * COMO EXECUTAR
 *   echo "1 2 3 4 5 6 7 8 9 10" | ./run 06_testa_em_ordem
 *   ./run 06_testa_em_ordem < /dev/null      (só os testes automáticos)
 * ============================================================================
 */
#include <stdio.h>

/* Funcao usada pelo programa de teste abaixo. */
int em_ordem(const float v[], int tamanho) {
    for (int i = 1; i < tamanho; i++)
        if (v[i - 1] > v[i])
            return 0;
    return 1;
}

/* Compara o resultado de em_ordem com o esperado. Devolve 1 se passou. */
int testa_caso(const char nome[], const float v[], int n, int esperado) {
    int obtido = em_ordem(v, n);
    printf("  %s: %s (esperado %d, obtido %d)\n",
           obtido == esperado ? "ok" : "FALHOU", nome, esperado, obtido);
    return obtido == esperado;
}

int main(void) {
    const float vazio[1] = {0.0f};
    const float um[1] = {3.5f};
    const float repetidos[4] = {1.0f, 2.0f, 2.0f, 3.0f};
    const float erro_no_fim[4] = {1.0f, 2.0f, 3.0f, 2.5f};
    printf("Testes automaticos de em_ordem:\n");
    int passou = testa_caso("vetor vazio", vazio, 0, 1)
               + testa_caso("um elemento", um, 1, 1)
               + testa_caso("valores repetidos", repetidos, 4, 1)
               + testa_caso("desordem no ultimo par", erro_no_fim, 4, 0);
    printf("  %d de 4 casos passaram.\n\n", passou);

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
    if (em_ordem(v, 10))
        printf("O vetor esta em ordem\n");
    else
        printf("O vetor esta desordenado\n");
    return 0;
}
