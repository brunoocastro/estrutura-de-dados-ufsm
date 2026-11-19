/*
 * ============================================================================
 * Arquivo : 08_forma_triangulo.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 8: a verificação de triângulo da Lista 01 (exercício 10)
 *   transformada em função forma_triangulo(a, b, c), que devolve 1 ou 0.
 *
 * EXEMPLO REAL
 *   Funções de validação reutilizáveis (predicados), chamadas antes de usar
 *   um dado em vários pontos do programa.
 *
 * COMPLEXIDADE
 *   O(1) tempo e espaço.
 *
 * COMO EXECUTAR
 *   echo "3 4 5" | ./run 08_forma_triangulo
 * ============================================================================
 */
#include <stdio.h>

/* Um triangulo existe quando todos os lados sao positivos e obedecem
   a desigualdade triangular: cada lado e menor que a soma dos outros dois. */
int forma_triangulo(float a, float b, float c) {
    return a > 0.0f && b > 0.0f && c > 0.0f &&
           a < b + c && b < a + c && c < a + b;
}

int main(void) {
    float a, b, c;
    printf("Informe os tres lados do triangulo (A B C): ");
    int lidos = scanf("%f %f %f", &a, &b, &c);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 3) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }
    printf("%d\n", forma_triangulo(a, b, c));
    return 0;
}
