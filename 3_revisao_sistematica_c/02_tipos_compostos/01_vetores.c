/*
 * VETORES (arrays unidimensionais) EM C
 *
 * Compile e execute nesta pasta:
 *   gcc -std=c11 -Wall -Wextra -pedantic 01_vetores.c -o 01_vetores
 *   ./01_vetores
 */
#include <stdio.h>

int main(void) {
    /* Tamanho explicito: o numero de elementos aparece entre colchetes. */
    int explicito[5] = {10, 20, 30, 40, 50};

    /* Tamanho implicito: o compilador conta os inicializadores (5 elementos). */
    int implicito[] = {10, 20, 30, 40, 50};

    /* Inicializacao parcial: os elementos restantes recebem zero. */
    int parcial[5] = {10, 20};

    /* O tamanho pode ser calculado apenas enquanto o nome ainda e um array. */
    size_t quantidade = sizeof explicito / sizeof explicito[0];

    printf("=== Tamanho e inicializacao ===\n");
    printf("quantidade de elementos = %zu\n", quantidade);
    printf("sizeof(explicito) = %zu byte(s)\n", sizeof explicito);
    printf("sizeof(implicito) = %zu byte(s)\n", sizeof implicito);
    printf("sizeof(int) = %zu byte(s)\n", sizeof(int));
    printf("sizeof(parcial) = %zu byte(s)\n", sizeof parcial);

    printf("\n=== Memoria contigua ===\n");
    for (size_t i = 0; i < quantidade; i++) {
        printf("explicito[%zu] = %d | endereco = %p\n",
               i, explicito[i], (void *)&explicito[i]);
    }
    printf("\nCada endereco seguinte avanca sizeof(int) byte(s).\n");
    printf("O nome do vetor geralmente decai para ponteiro ao ser passado a uma funcao.\n");
    printf("Dentro de main, sizeof(explicito) ainda conhece o array inteiro.\n");

    /* Indices validos sao 0 a quantidade - 1; explicito[5] seria invalido. */
    printf("\nPrimeiro elemento: %d\n", explicito[0]);
    printf("Ultimo elemento: %d\n", explicito[quantidade - 1]);

    /* Um array tem tamanho fixo: nao cresce automaticamente. */
    printf("\nArrays locais possuem tamanho fixo definido na declaracao.\n");
    return 0;
}
