/*
 * FUNCOES COM VETORES E STRINGS
 *
 * Ao receber um vetor, a funcao recebe na pratica um ponteiro para seu primeiro
 * elemento. Por isso o tamanho precisa ser passado separadamente.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 04_vetores.c -o 04_vetores
 * ./04_vetores
 */
#include <stdio.h>

void imprime_vetor(const int vetor[], size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++)
        printf("%d%s", vetor[i], i + 1 == tamanho ? "\n" : " ");
}

void dobra_valores(int vetor[], size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++)
        vetor[i] *= 2;
}

int main(void) {
    int valores[] = {1, 2, 3, 4};
    size_t tamanho = sizeof valores / sizeof valores[0];

    printf("Original: ");
    imprime_vetor(valores, tamanho);
    dobra_valores(valores, tamanho);
    printf("Depois da funcao: ");
    imprime_vetor(valores, tamanho);
    printf("sizeof no chamador = %zu; a funcao nao descobre esse tamanho sozinha.\n",
           sizeof valores);
    return 0;
}
