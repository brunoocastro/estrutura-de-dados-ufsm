#include <stdio.h>

int main(void) {
    int soma = 0;

    for (int numero = 1; numero <= 100; numero++) {
        printf("%d%s", numero, numero == 100 ? "\n" : " ");
        soma += numero;
    }

    printf("Soma: %d\n", soma);
    return 0;
}
