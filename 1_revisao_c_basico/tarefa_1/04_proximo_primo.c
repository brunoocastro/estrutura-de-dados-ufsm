#include <stdio.h>

static int eh_primo(int x) {
    if (x < 2) return 0;
    for (int divisor = 2; divisor <= x / divisor; divisor++)
        if (x % divisor == 0) return 0;
    return 1;
}

int main(void) {
    int g;
    if (scanf("%d", &g) != 1) return 1;

    int candidato = g + 1;
    while (!eh_primo(candidato)) candidato++;
    printf("%d\n", candidato);
    return 0;
}
