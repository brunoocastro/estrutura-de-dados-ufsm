/*
 * ============================================================================
 * Arquivo : 08_equacao_segundo_grau.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 8: raízes de Ax² + Bx + C = 0 pela fórmula de Bhaskara, com
 *   if/else if/else para os casos delta > 0, delta = 0 e delta < 0 (e o
 *   caso degenerado A = 0).
 *
 * EXEMPLO REAL
 *   Polos de um sistema de 2ª ordem (massa-mola-amortecedor, circuito RLC):
 *   as raízes de s² + 2ζωs + ω² dizem se a resposta oscila (delta < 0) ou
 *   não (delta >= 0).
 *
 * COMPLEXIDADE
 *   O(1) tempo e espaço.
 *   Armadilha: comparar double com == 0.0 é frágil (delta = 1e-17 por erro
 *   de arredondamento não é "zero"); em código de produção, use tolerância.
 *
 * COMO EXECUTAR
 *   echo "1 -3 2" | ./run 08_equacao_segundo_grau
 * ============================================================================
 */
#include <math.h>
#include <stdio.h>

int main(void) {
    double a, b, c, delta;

    printf("Digite A, B e C: ");
    int lidos = scanf("%lf %lf %lf", &a, &b, &c);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 3) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }
    if (a == 0.0) {
        if (b == 0.0)
            printf("Nao e uma equacao do segundo grau.\n");
        else
            printf("Equacao linear: x = %.6f\n", -c / b);
        return 0;
    }

    delta = b * b - 4.0 * a * c;
    if (delta > 0.0) {
        printf("Duas raizes reais: x1 = %.6f, x2 = %.6f\n",
               (-b + sqrt(delta)) / (2.0 * a),
               (-b - sqrt(delta)) / (2.0 * a));
    } else if (delta == 0.0) {
        printf("Uma raiz real: x = %.6f\n", -b / (2.0 * a));
    } else {
        /* Raízes complexas conjugadas: parte real ± parte imaginária. */
        printf("Raizes complexas: x = %.6f +/- %.6fi\n",
               -b / (2.0 * a), sqrt(-delta) / (2.0 * fabs(a)));
    }
    return 0;
}
