/*
 * ============================================================================
 * Arquivo : 01_divisibilidade.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 1: dizer se A é divisível por B usando o operador resto (%).
 *
 * EXEMPLO REAL
 *   Executar uma tarefa a cada B ciclos de varredura (if (ciclo % B == 0)),
 *   por exemplo gravar uma amostra a cada 10 ciclos.
 *
 * COMPLEXIDADE
 *   O(1) tempo e espaço: uma divisão e uma comparação.
 *
 * COMO EXECUTAR
 *   echo "12 3" | ./run 01_divisibilidade
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    int a, b;

    printf("Digite A e B: ");
    int lidos = scanf("%d %d", &a, &b);
    if (lidos == EOF) {                 /* entrada vazia: termina sem erro */
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 2) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }
    if (b == 0) {                       /* a % 0 e comportamento indefinido em C */
        printf("Nao e possivel verificar divisibilidade por zero.\n");
        return 1;
    }

    if (a % b == 0)
        printf("%d e divisivel por %d.\n", a, b);
    else
        printf("%d nao e divisivel por %d.\n", a, b);

    return 0;
}
