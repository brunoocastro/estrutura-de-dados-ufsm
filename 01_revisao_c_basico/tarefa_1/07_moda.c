#include <stdio.h>

int main(void) {
    int frequencia[101] = {0};
    int valor, valido = 1;

    for (int i = 0; i < 20; i++) {
        if (scanf("%d", &valor) != 1) return 1;
        if (valor < 0 || valor > 100) valido = 0;
        else frequencia[valor]++;
    }

    if (!valido) {
        printf("ha valores fora do intervalo [0,100]\n");
        return 0;
    }
    int moda = 0;
    for (int i = 1; i <= 100; i++)
        if (frequencia[i] > frequencia[moda]) moda = i;
    printf("todos os valores pertencem a [0,100]\nmoda: %d\n", moda);
    return 0;
}
