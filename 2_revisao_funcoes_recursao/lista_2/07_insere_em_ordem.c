#include <stdio.h>

/* Desloca os maiores elementos para abrir espaco para x. */

void insere_em_ordem(float v[], int n, float x) {
    int posicao = n;
    while (posicao > 0 && v[posicao - 1] > x) {
        v[posicao] = v[posicao - 1];
        posicao--;
    }
    v[posicao] = x;
}

int main(void) {
    int n;
    float x;
    printf("Informe o tamanho inicial do vetor: ");
    if (scanf("%d", &n) != 1 || n < 0)
        return 1;
    float v[n + 1];
    printf("Informe %d valores reais ja ordenados: ", n);
    for (int i = 0; i < n; i++)
        if (scanf("%f", &v[i]) != 1)
            return 1;
    printf("Informe o valor a inserir: ");
    if (scanf("%f", &x) != 1)
        return 1;
    insere_em_ordem(v, n, x);
    for (int i = 0; i <= n; i++)
        printf("%g%s", v[i], i == n ? "\n" : " ");
    return 0;
}
