/*
 * SOMA RECURSIVA DE VETOR
 *
 * A cada chamada, a funcao resolve o primeiro elemento e delega o restante.
 * O tamanho diminui ate o caso base tamanho == 0.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 03_soma_vetor.c -o 03_soma_vetor
 * ./03_soma_vetor
 */
#include <stdio.h>

int soma_vetor(const int vetor[], size_t tamanho, size_t nivel) {
    printf("%*sENTRA: tamanho = %zu\n", (int)(nivel * 2), "", tamanho);
    if (tamanho == 0) {
        printf("%*sCASO BASE: soma = 0\n", (int)(nivel * 2), "");
        return 0;
    }

    int restante = soma_vetor(vetor + 1, tamanho - 1, nivel + 1);
    int resultado = vetor[0] + restante;
    printf("%*sRETORNA: %d + %d = %d\n",
           (int)(nivel * 2), "", vetor[0], restante, resultado);
    return resultado;
}

int main(void) {
    int vetor[] = {4, 8, 15, 16};
    size_t tamanho = sizeof vetor / sizeof vetor[0];
    printf("Soma = %d\n", soma_vetor(vetor, tamanho, 0));
    return 0;
}
