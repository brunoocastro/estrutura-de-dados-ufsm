/*
 * ============================================================================
 * Arquivo : 04_arrays.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.4 Vetores, 0.6 Funções)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ao receber um vetor, a função recebe na prática um ponteiro para o
 *   primeiro elemento: o vetor NÃO é copiado (passar custa O(1)) e a função
 *   pode alterá-lo. Por isso o tamanho precisa ser passado separadamente e
 *   const indica quando a função só lê.
 *
 * EXEMPLO REAL
 *   aplicar_ganho(amostras, n, 2.0) num buffer de áudio ou de um ADC: o
 *   buffer pode ter milhares de amostras e copiá-lo a cada chamada seria caro.
 *
 * COMPLEXIDADE (n = tamanho do vetor)
 *   Passar o vetor para a função   O(1)  (só o endereço é copiado)
 *   imprime_array                  O(n)
 *   dobra_valores                  O(n)  (altera o vetor do chamador, in-place)
 *   media                          O(n)
 *
 * COMO EXECUTAR
 *   ./run 04_arrays
 * ============================================================================
 */
#include <stdio.h>

/* O(n): só lê -> const. */
void imprime_array(const int array[], size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++)
        printf("%d%s", array[i], i + 1 == tamanho ? "\n" : " ");
}

/* O(n): altera o próprio vetor do chamador (não existe cópia). */
void dobra_valores(int array[], size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++)
        array[i] *= 2;
}

/* O(n): pré-condição tamanho > 0 (senão dividiria por zero). */
double media(const int array[], size_t tamanho) {
    long soma = 0;
    for (size_t i = 0; i < tamanho; i++)
        soma += array[i];
    return (double)soma / (double)tamanho;
}

int main(void) {
    int valores[] = {1, 2, 3, 4};
    size_t tamanho = sizeof valores / sizeof valores[0];

    printf("Original: ");
    imprime_array(valores, tamanho);
    printf("Media: %.2f\n", media(valores, tamanho));
    dobra_valores(valores, tamanho);
    printf("Depois da funcao: ");
    imprime_array(valores, tamanho);
    printf("sizeof no chamador = %zu; a funcao nao descobre esse tamanho sozinha.\n",
           sizeof valores);
    printf("Endereco do vetor em main: %p (a funcao recebe exatamente este endereco).\n",
           (void *)valores);
    return 0;
}
