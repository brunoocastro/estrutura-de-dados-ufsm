#include <stdio.h>

/* Procura recursivamente no primeiro elemento e no restante do vetor. */

int existe_vec(const float vetor[], int tamanho, float procurado) {
    if (tamanho <= 0)
        return 0;
    if (vetor[0] == procurado)
        return 1;
    return existe_vec(vetor + 1, tamanho - 1, procurado);
}

int main(void) {
    int tamanho;
    float procurado;

    printf("Informe o tamanho do vetor: ");
    if (scanf("%d", &tamanho) != 1 || tamanho < 0)
        return 1;

    float vetor[tamanho > 0 ? tamanho : 1];
    printf("Informe os %d elementos do vetor: ", tamanho);
    for (int i = 0; i < tamanho; i++)
        if (scanf("%f", &vetor[i]) != 1)
            return 1;
    printf("Informe o valor procurado: ");
    if (scanf("%f", &procurado) != 1)
        return 1;

    printf("%s\n", existe_vec(vetor, tamanho, procurado) ? "encontrado" : "nao encontrado");
    return 0;
}
