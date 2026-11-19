#include <stdio.h>

int main(void) {
    int n;
    if (scanf("%d", &n) != 1 || n < 1 || n > 100) return 1;

    int soma = 0, valor;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (scanf("%d", &valor) != 1) return 1;
            if (i == j) soma += valor;
        }
    }
    printf("soma da diagonal principal: %d\n", soma);
    return 0;
}
