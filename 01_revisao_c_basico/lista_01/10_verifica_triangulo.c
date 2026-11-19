/*
 * ============================================================================
 * Arquivo : 10_verifica_triangulo.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 10: verificar se A, B e C podem ser lados de um triângulo
 *   (lados positivos e desigualdade triangular), combinando condições com &&.
 *
 * EXEMPLO REAL
 *   Validação de dados de entrada antes de usá-los: um robô que calcula a
 *   posição por triangulação precisa rejeitar medidas geometricamente
 *   impossíveis.
 *
 * COMPLEXIDADE
 *   O(1). O && avalia da esquerda para a direita e para no primeiro falso
 *   (curto-circuito).
 *
 * COMO EXECUTAR
 *   echo "3 4 5" | ./run 10_verifica_triangulo
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    double a, b, c;

    printf("Digite os tres lados: ");
    int lidos = scanf("%lf %lf %lf", &a, &b, &c);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 3) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }

    if (a > 0.0 && b > 0.0 && c > 0.0 &&
        a < b + c && b < a + c && c < a + b)
        printf("Os valores podem formar um triangulo.\n");
    else
        printf("Os valores nao podem formar um triangulo.\n");

    return 0;
}
