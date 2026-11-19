/*
 * ============================================================================
 * Arquivo : 09_classifica_triangulo.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 9: função classifica_triangulo(a, b, c) que REUSA
 *   forma_triangulo (exercício 8) e devolve o nome da classificação.
 *
 * EXEMPLO REAL
 *   Composição de funções: uma função de alto nível (classificar) apoiada em
 *   funções menores já testadas (validar).
 *
 * COMPLEXIDADE
 *   O(1) tempo e espaço.
 *
 * COMO EXECUTAR
 *   echo "3 3 4" | ./run 09_classifica_triangulo
 * ============================================================================
 */
#include <stdio.h>

/* Primeiro valida a existencia; depois compara os comprimentos dos lados. */
int forma_triangulo(float a, float b, float c) {
    return a > 0.0f && b > 0.0f && c > 0.0f &&
           a < b + c && b < a + c && c < a + b;
}

/* Devolve um texto constante (literal de string): ele existe durante todo o
   programa, então pode ser retornado com segurança. */
const char *classifica_triangulo(float a, float b, float c) {
    if (!forma_triangulo(a, b, c))
        return "nao e um triangulo";
    if (a == b && b == c)
        return "equilatero";
    if (a == b || a == c || b == c)
        return "isosceles";
    return "escaleno";
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
    printf("%s\n", classifica_triangulo(a, b, c));
    return 0;
}
