/*
 * ============================================================================
 * Arquivo : 04_testa_inverte_vetor.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 4: programa de teste para inverte_vetor. Além de inverter o
 *   que o usuário digitar, roda casos automáticos que comparam o resultado
 *   com o esperado (vetor vazio, 1 elemento, tamanho par e ímpar), que é o
 *   jeito profissional de testar uma função.
 *
 * EXEMPLO REAL
 *   Testes de unidade de blocos de função antes de carregar no CLP ou no
 *   firmware (casos de borda primeiro: vazio, um elemento, limites).
 *
 * COMPLEXIDADE (n = tamanho do vetor)
 *   inverte_vetor: O(n) tempo, O(1) espaço. Cada caso de teste: O(n).
 *
 * COMO EXECUTAR
 *   echo "1 2 3 0" | ./run 04_testa_inverte_vetor
 *   ./run 04_testa_inverte_vetor < /dev/null      (só os testes automáticos)
 * ============================================================================
 */
#include <stdio.h>

#define MAX 100

/* Funcao auxiliar: inverte o vetor sem criar um segundo vetor. */
void inverte_vetor(int v[], int tamanho) {
    for (int inicio = 0, fim = tamanho - 1; inicio < fim; inicio++, fim--) {
        int temporario = v[inicio];
        v[inicio] = v[fim];
        v[fim] = temporario;
    }
}

/* Inverte 'v' e compara com 'esperado'. Devolve 1 se passou. O(n). */
int testa_caso(const char nome[], int v[], const int esperado[], int n) {
    inverte_vetor(v, n);
    for (int i = 0; i < n; i++) {
        if (v[i] != esperado[i]) {
            printf("  FALHOU: %s (posicao %d)\n", nome, i);
            return 0;
        }
    }
    printf("  ok: %s\n", nome);
    return 1;
}

int main(void) {
    printf("Testes automaticos de inverte_vetor:\n");
    int vazio[1] = {0};
    int um[1] = {7}, um_esp[1] = {7};
    int par[4] = {1, 2, 3, 4}, par_esp[4] = {4, 3, 2, 1};
    int impar[5] = {1, 2, 3, 4, 5}, impar_esp[5] = {5, 4, 3, 2, 1};
    int passou = testa_caso("vetor vazio", vazio, vazio, 0)
               + testa_caso("um elemento", um, um_esp, 1)
               + testa_caso("tamanho par", par, par_esp, 4)
               + testa_caso("tamanho impar", impar, impar_esp, 5);
    printf("  %d de 4 casos passaram.\n\n", passou);

    int v[MAX], tamanho = 0, valor;
    printf("Informe inteiros (zero encerra a leitura):\n");
    while (tamanho < MAX && scanf("%d", &valor) == 1 && valor != 0)
        v[tamanho++] = valor;
    inverte_vetor(v, tamanho); /* Testa a funcao solicitada. */
    for (int i = 0; i < tamanho; i++)
        printf("%d%s", v[i], i == tamanho - 1 ? "\n" : " ");
    if (tamanho == 0) printf("Nenhum valor lido.\n");
    return 0;
}
