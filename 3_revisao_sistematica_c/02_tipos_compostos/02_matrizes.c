/*
 * MATRIZES (arrays multidimensionais) EM C
 *
 * Compile e execute nesta pasta:
 *   gcc -std=c11 -Wall -Wextra -pedantic 02_matrizes.c -o 02_matrizes
 *   ./02_matrizes
 */
#include <stdio.h>

int main(void) {
    int matriz[2][3] = {{1, 2, 3}, {4, 5, 6}};

    printf("=== Dimensoes e memoria ===\n");
    printf("linhas = %zu\n", sizeof matriz / sizeof matriz[0]);
    printf("colunas = %zu\n", sizeof matriz[0] / sizeof matriz[0][0]);
    printf("elementos = %zu\n", sizeof matriz / sizeof matriz[0][0]);
    printf("tamanho total = %zu byte(s)\n", sizeof matriz);

    /* C armazena matrizes em ordem de linha (row-major). */
    printf("\n=== Elementos em ordem de memoria ===\n");
    for (size_t linha = 0; linha < 2; linha++) {
        for (size_t coluna = 0; coluna < 3; coluna++) {
            printf("matriz[%zu][%zu] = %d | endereco = %p\n",
                   linha, coluna, matriz[linha][coluna],
                   (void *)&matriz[linha][coluna]);
        }
    }

    /* A matriz e um array de 2 arrays; cada linha possui 3 int. */
    printf("\nsizeof(matriz[0]) = %zu byte(s), uma linha completa.\n", sizeof matriz[0]);
    printf("sizeof(matriz[0][0]) = %zu byte(s), um elemento.\n", sizeof matriz[0][0]);
    printf("O endereco de matriz[0][3] coincide com o de matriz[1][0],\n");
    printf("mas acessar matriz[0][3] nao e uma forma valida de indexar a linha.\n");

    for (size_t linha = 0; linha < 2; linha++) {
        for (size_t coluna = 0; coluna < 3; coluna++)
            printf("%d%s", matriz[linha][coluna], coluna == 2 ? "\n" : " ");
    }
    return 0;
}
