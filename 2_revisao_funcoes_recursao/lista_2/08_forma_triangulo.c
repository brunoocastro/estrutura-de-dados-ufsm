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
    if (scanf("%f %f %f", &a, &b, &c) != 3)
        return 1;
    printf("%d\n", forma_triangulo(a, b, c));
    return 0;
}
