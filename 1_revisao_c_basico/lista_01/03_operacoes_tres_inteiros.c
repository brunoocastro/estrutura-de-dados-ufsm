#include <stdio.h>

int main(void) {
    int a, b, c;

    printf("Digite A, B e C: ");
    if (scanf("%d %d %d", &a, &b, &c) != 3) {
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
