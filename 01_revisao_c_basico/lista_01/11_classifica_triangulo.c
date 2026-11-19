/*
 * ============================================================================
 * Arquivo : 11_classifica_triangulo.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 11: verificar se A, B e C formam um triângulo e classificá-lo
 *   (equilátero, isósceles, escaleno). A ordem dos testes importa: o
 *   equilátero precisa ser testado antes do isósceles, senão todo
 *   equilátero seria classificado como isósceles.
 *
 * EXEMPLO REAL
 *   Classificação em etapas, como numa máquina de inspeção: primeiro rejeita
 *   a peça inválida, depois separa por categoria.
 *
 * COMPLEXIDADE
 *   O(1).
 *
 * COMO EXECUTAR
 *   echo "3 3 4" | ./run 11_classifica_triangulo
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

    if (a <= 0.0 || b <= 0.0 || c <= 0.0 ||
        a >= b + c || b >= a + c || c >= a + b) {
        printf("Os valores nao podem formar um triangulo.\n");
    } else if (a == b && b == c) {
        printf("Triangulo equilatero.\n");
    } else if (a == b || a == c || b == c) {
        printf("Triangulo isosceles.\n");
    } else {
        printf("Triangulo escaleno.\n");
    }

    return 0;
}
