/*
 * ============================================================================
 * Arquivo : 02_juros_compostos.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 2: montante M = C (1 + i)^t reaproveitando a função de
 *   potência do exercício 1 (reuso de função).
 *
 * EXEMPLO REAL
 *   Análise de investimento em automação (payback com taxa de juros);
 *   qualquer crescimento ou decaimento por período fixo.
 *
 * COMPLEXIDADE (t = número de meses)
 *   O(t) tempo, O(1) espaço: a potência faz t multiplicações.
 *
 * COMO EXECUTAR
 *   echo "1000 0.02 12" | ./run 02_juros_compostos
 * ============================================================================
 */
#include <stdio.h>

/* Reaproveita a potencia por multiplicacoes sucessivas. Pré-condição: b >= 0. */
float potencia_int(float a, int b) {
    float resultado = 1.0f;
    for (int i = 0; i < b; i++)
        resultado *= a;
    return resultado;
}

int main(void) {
    float capital, taxa;
    int meses;

    printf("Informe o capital inicial, a taxa mensal e a quantidade de meses: ");
    int lidos = scanf("%f %f %d", &capital, &taxa, &meses);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 3 || capital < 0.0f || taxa <= 0.0f || meses <= 0) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }

    /* M = C * (1 + i)^t. */
    printf("Montante: %g\n", capital * potencia_int(1.0f + taxa, meses));
    return 0;
}
