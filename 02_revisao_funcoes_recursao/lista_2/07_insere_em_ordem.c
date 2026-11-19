/*
 * ============================================================================
 * Arquivo : 07_insere_em_ordem.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.4 e 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 7: função insere_em_ordem(v, n, x) que insere x em um vetor
 *   ordenado de n elementos, deslocando para a direita os maiores que x.
 *   O vetor precisa ter espaço para n + 1 elementos.
 *
 * EXEMPLO REAL
 *   Manter a tabela de setpoints ou a lista de alarmes sempre ordenada a
 *   cada inclusão, para consultar depois com busca binária.
 *
 * COMPLEXIDADE (n = elementos já no vetor)
 *   Melhor caso O(1) (x é o maior: nenhum deslocamento);
 *   pior caso O(n) (x é o menor: todos andam uma casa). Espaço O(1).
 *   É o passo central da ordenação por inserção (../../08_ordenacao_busca/).
 *
 * COMO EXECUTAR
 *   echo "4 1 3 5 7 4" | ./run 07_insere_em_ordem    (n, n valores, x)
 * ============================================================================
 */
#include <stdio.h>

#define MAX 100

/* Desloca os maiores elementos para abrir espaco para x.
   Pré-condições: v[0..n-1] ordenado e v com capacidade para n + 1. */
void insere_em_ordem(float v[], int n, float x) {
    int posicao = n;
    while (posicao > 0 && v[posicao - 1] > x) {
        v[posicao] = v[posicao - 1];
        posicao--;
    }
    v[posicao] = x;
}

int main(void) {
    int n;
    float x;
    float v[MAX + 1];      /* tamanho fixo: um n digitado não decide o tamanho da pilha */

    printf("Informe o tamanho inicial do vetor (0 a %d): ", MAX);
    int lidos = scanf("%d", &n);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 1 || n < 0 || n > MAX) {
        fprintf(stderr, "Tamanho invalido.\n");
        return 1;
    }
    printf("Informe %d valores reais ja ordenados: ", n);
    for (int i = 0; i < n; i++) {
        if (scanf("%f", &v[i]) != 1) {
            printf("\nEntrada incompleta.\n");
            return 0;
        }
    }
    printf("Informe o valor a inserir: ");
    if (scanf("%f", &x) != 1) {
        printf("\nEntrada incompleta.\n");
        return 0;
    }
    insere_em_ordem(v, n, x);
    for (int i = 0; i <= n; i++)
        printf("%g%s", v[i], i == n ? "\n" : " ");
    return 0;
}
