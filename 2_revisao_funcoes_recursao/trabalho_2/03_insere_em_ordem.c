#include <stdio.h>

void insere_em_ordem(float vetor[], int n, float x) {
    if (n == 0 || vetor[n - 1] <= x) {
        vetor[n] = x;
        return;
    }

    vetor[n] = vetor[n - 1];
    insere_em_ordem(vetor, n - 1, x);
}

int main(void) {
    int n;
    float x;

    if (scanf("%d", &n) != 1 || n < 0)
        return 1;

    float vetor[n + 1];
    for (int i = 0; i < n; i++)
        if (scanf("%f", &vetor[i]) != 1)
            return 1;
    if (scanf("%f", &x) != 1)
        return 1;

    insere_em_ordem(vetor, n, x);
    for (int i = 0; i <= n; i++)
        printf("%g%s", vetor[i], i == n ? "\n" : " ");
    return 0;
}
