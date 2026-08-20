#include <stdio.h>

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

    if (scanf("%d", &tamanho) != 1 || tamanho < 0)
        return 1;

    float vetor[tamanho > 0 ? tamanho : 1];
    for (int i = 0; i < tamanho; i++)
        if (scanf("%f", &vetor[i]) != 1)
            return 1;
    if (scanf("%f", &procurado) != 1)
        return 1;

    printf("%s\n", existe_vec(vetor, tamanho, procurado) ? "encontrado" : "nao encontrado");
    return 0;
}
