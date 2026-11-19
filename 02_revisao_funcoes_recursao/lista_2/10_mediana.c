/*
 * ============================================================================
 * Arquivo : 10_mediana.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.4 e 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 10: função mediana(v, n) para um vetor em ordem crescente. Reusa
 *   em_ordem (exercício 5) e devolve NAN quando o vetor está vazio ou
 *   desordenado (valor especial "não é um número" para sinalizar erro).
 *
 * EXEMPLO REAL
 *   Filtro de mediana: em sensores com picos espúrios (ruído impulsivo), a
 *   mediana de uma janela descarta o pico, enquanto a média seria puxada
 *   por ele.
 *
 * COMPLEXIDADE (n = tamanho do vetor)
 *   em_ordem: O(n). Cálculo da mediana com o vetor ordenado: O(1) (acesso
 *   direto ao meio). Total: O(n), espaço O(1).
 *
 * COMO EXECUTAR
 *   echo "5 1 2 3 4 5" | ./run 10_mediana      (n e os n valores)
 * ============================================================================
 */
#include <math.h>
#include <stdio.h>

#define MAX 100

/* A mediana depende de o vetor estar ordenado. */
int em_ordem(const float v[], int tamanho) {
    for (int i = 1; i < tamanho; i++)
        if (v[i - 1] > v[i])
            return 0;
    return 1;
}

float mediana(const float v[], int tamanho) {
    /* Sem elementos ou fora de ordem, nao existe mediana valida. */
    if (tamanho <= 0 || !em_ordem(v, tamanho))
        return NAN;
    if (tamanho % 2 != 0)
        return v[tamanho / 2];
    return (v[tamanho / 2 - 1] + v[tamanho / 2]) / 2.0f;
}

int main(void) {
    int tamanho;
    float v[MAX];
    printf("Informe a quantidade de valores (0 a %d): ", MAX);
    int lidos = scanf("%d", &tamanho);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 1 || tamanho < 0 || tamanho > MAX) {
        fprintf(stderr, "Quantidade invalida.\n");
        return 1;
    }
    printf("Informe os valores em ordem crescente: ");
    for (int i = 0; i < tamanho; i++) {
        if (scanf("%f", &v[i]) != 1) {
            printf("\nEntrada incompleta.\n");
            return 0;
        }
    }
    float resultado = mediana(v, tamanho);
    if (isnan(resultado))
        printf("vetor vazio ou desordenado\n");
    else
        printf("%g\n", resultado);
    return 0;
}
