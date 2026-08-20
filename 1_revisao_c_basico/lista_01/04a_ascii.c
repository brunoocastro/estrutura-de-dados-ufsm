#include <stdio.h>

int main(void) {
    int a;

    printf("Digite A: ");
    if (scanf("%d", &a) != 1) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }

    if (a > 64 && a < 123)
        printf("O valor %d representa o caractere ASCII '%c'.\n", a, (char)a);
    else
        printf("A deve estar no intervalo aberto (64, 123).\n");

    return 0;
}
