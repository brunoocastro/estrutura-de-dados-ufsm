#include <stdio.h>

/* Primeiro valida a existencia; depois compara os comprimentos dos lados. */

int forma_triangulo(float a, float b, float c) {
    return a > 0.0f && b > 0.0f && c > 0.0f &&
           a < b + c && b < a + c && c < a + b;
}

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
    if (scanf("%f %f %f", &a, &b, &c) != 3)
        return 1;
    printf("%s\n", classifica_triangulo(a, b, c));
    return 0;
}
