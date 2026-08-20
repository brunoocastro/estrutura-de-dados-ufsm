#include <stdio.h>

/* Funcao auxiliar: inverte o vetor sem criar um segundo vetor. */

void inverte_vetor(int v[], int tamanho) {
    for (int inicio = 0, fim = tamanho - 1; inicio < fim; inicio++, fim--) {
        int temporario = v[inicio];
        v[inicio] = v[fim];
        v[fim] = temporario;
    }
}

int main(void) {
    int v[100], tamanho = 0, valor;
    printf("Informe inteiros (zero encerra a leitura):\n");
    while (tamanho < 100 && scanf("%d", &valor) == 1 && valor != 0)
        v[tamanho++] = valor;
    inverte_vetor(v, tamanho); /* Testa a funcao solicitada. */
    for (int i = 0; i < tamanho; i++)
        printf("%d%s", v[i], i == tamanho - 1 ? "\n" : " ");
    return 0;
}
