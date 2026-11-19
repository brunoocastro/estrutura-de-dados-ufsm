#include <stdio.h>

int main(void) {
    int x;
    if (scanf("%d", &x) != 1 || x == 0) return 1;

    int valor = x < 0 ? -x : x;
    for (int i = 1; i <= valor; i++) {
        if (x % i == 0) printf("%d ", i);
    }
    printf("\n");
    return 0;
}
