#include <stdio.h>

int main(void) {
    int a, b;

    printf("Digite A e B: ");
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }
    if (b == 0) {
        printf("Nao e possivel verificar divisibilidade por zero.\n");
        return 1;
    }

    if (a % b == 0)
        printf("%d e divisivel por %d.\n", a, b);
    else
        printf("%d nao e divisivel por %d.\n", a, b);

    return 0;
}
