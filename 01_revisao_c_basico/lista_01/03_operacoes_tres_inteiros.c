/*
 * ============================================================================
 * Arquivo : 03_operacoes_tres_inteiros.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 3: com três inteiros A, B e C, verificar sinais, maior, menor
 *   e avaliar A + B + C, A * B > C e A / C < B (operadores relacionais e o
 *   operador ternário ?:).
 *
 * EXEMPLO REAL
 *   Seleção de sensor redundante: com três transmissores medindo a mesma
 *   grandeza, o CLP compara os valores para escolher maior, menor ou mediano.
 *
 * COMPLEXIDADE
 *   O(1) tempo e espaço: número fixo de comparações.
 *   Armadilhas: A / C com inteiros descarta a parte fracionária (7 / 2 = 3);
 *   A * B pode estourar int com valores grandes; C = 0 não pode dividir.
 *
 * COMO EXECUTAR
 *   echo "2 3 4" | ./run 03_operacoes_tres_inteiros
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    int a, b, c;

    printf("Digite A, B e C: ");
    int lidos = scanf("%d %d %d", &a, &b, &c);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 3) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }

    printf("Positivos: A=%s, B=%s, C=%s\n",
           a > 0 ? "sim" : "nao", b > 0 ? "sim" : "nao",
           c > 0 ? "sim" : "nao");
    printf("Maior: %d\n", a > b ? (a > c ? a : c) : (b > c ? b : c));
    printf("Menor: %d\n", a < b ? (a < c ? a : c) : (b < c ? b : c));
    printf("A + B + C = %d\n", a + b + c);
    printf("A * B > C: %s\n", a * b > c ? "verdadeiro" : "falso");

    if (c == 0)
        printf("A / C < B: indefinido (divisao por zero)\n");
    else
        printf("A / C < B: %s (divisao inteira)\n", a / c < b ? "verdadeiro" : "falso");

    return 0;
}
