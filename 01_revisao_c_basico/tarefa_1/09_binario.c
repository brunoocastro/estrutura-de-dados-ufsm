#include <stdio.h>

#define N 5

int main(void) {
    int x;
    if (scanf("%d", &x) != 1 || x < 0) return 1;

    int digitos[N] = {0};
    int temporario = x;
    int quantidade = 0;
    while (temporario > 0) {
        if (quantidade == N) {
            printf("numero excede o maximo de %d digitos\n", N);
            return 0;
        }
        digitos[quantidade++] = temporario % 10;
        temporario /= 10;
    }

    int valido = 1;
    for (int i = 0; i < N; i++) {
        if (digitos[i] != 0 && digitos[i] != 1) valido = 0;
    }
    for (int i = N - 1; i >= 0; i--)
        printf("d%d=%d%c", i + 1, digitos[i], i ? ' ' : '\n');

    if (!valido) {
        printf("numero fora do formato binario\n");
        return 0;
    }
    int decimal = 0;
    for (int i = N - 1; i >= 0; i--) decimal = decimal * 2 + digitos[i];
    printf("valor decimal: %d\n", decimal);
    return 0;
}
