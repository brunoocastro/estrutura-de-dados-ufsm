#include <stdio.h>

int main(void) {
    int a, b;
    if (scanf("%d %d", &a, &b) != 2) return 1;

    int inicio = a < b ? a : b;
    int fim = a < b ? b : a;
    for (int i = inicio; i <= fim; i++) {
        if (i % 2 != 0) printf("%d ", i);
    }
    printf("\n");
    return 0;
}
