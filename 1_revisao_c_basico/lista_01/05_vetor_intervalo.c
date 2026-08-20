#include <stdio.h>

#define TAMANHO 20

int main(void) {
    int valor;
    int todos_validos = 1;

    printf("Digite %d valores inteiros:\n", TAMANHO);
    for (int i = 0; i < TAMANHO; i++) {
        if (scanf("%d", &valor) != 1) {
            fprintf(stderr, "Entrada invalida na posicao %d.\n", i);
            return 1;
        }
        if (valor < 0 || valor > 100)
            todos_validos = 0;
    }

    printf("%s\n", todos_validos ?
           "Todos os valores pertencem a [0, 100]." :
           "Nem todos os valores pertencem a [0, 100].");
    return 0;
}
