#include <stdio.h>

int main(void) {
    int x;
    if (scanf("%d", &x) != 1) return 1;

    int primo = x >= 2;
    for (int divisor = 2; divisor <= x / divisor && primo; divisor++) {
        if (x % divisor == 0) primo = 0;
    }
    printf("%s\n", primo ? "primo" : "nao primo");
    return 0;
}
