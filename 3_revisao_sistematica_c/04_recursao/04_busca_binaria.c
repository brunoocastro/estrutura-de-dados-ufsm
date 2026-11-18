/*
 * BUSCA BINARIA RECURSIVA
 *
 * O array precisa estar ordenado. Cada chamada descarta metade do intervalo.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 04_busca_binaria.c -o 04_busca_binaria
 * ./04_busca_binaria
 */
#include <stdio.h>

int busca(const int array[], int inicio, int fim, int procurado, int nivel) {
    printf("%*sintervalo [%d, %d]\n", nivel * 2, "", inicio, fim);
    if (inicio > fim) {
        printf("%*snao encontrado\n", nivel * 2, "");
        return -1;
    }

    int meio = inicio + (fim - inicio) / 2;
    printf("%*smeio = %d, valor = %d\n", nivel * 2, "", meio, array[meio]);
    if (array[meio] == procurado)
        return meio;
    if (procurado < array[meio])
        return busca(array, inicio, meio - 1, procurado, nivel + 1);
    return busca(array, meio + 1, fim, procurado, nivel + 1);
}

int main(void) {
    const int array[] = {3, 8, 12, 17, 25, 31, 42};
    int procurado;
    printf("Informe o valor procurado: ");
    if (scanf("%d", &procurado) != 1)
        return 1;

    int indice = busca(array, 0, 6, procurado, 0);
    printf("Indice retornado: %d\n", indice);
    return 0;
}
