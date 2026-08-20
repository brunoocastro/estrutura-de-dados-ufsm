#include <math.h>
#include <stdio.h>

int main(void) {
    double a, b, c, delta;

    printf("Digite A, B e C: ");
    if (scanf("%lf %lf %lf", &a, &b, &c) != 3) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }
    if (a == 0.0) {
        if (b == 0.0)
            printf("Nao e uma equacao do segundo grau.\n");
        else
            printf("Equacao linear: x = %.6f\n", -c / b);
        return 0;
    }

    delta = b * b - 4.0 * a * c;
    if (delta > 0.0) {
        printf("Duas raizes reais: x1 = %.6f, x2 = %.6f\n",
               (-b + sqrt(delta)) / (2.0 * a),
               (-b - sqrt(delta)) / (2.0 * a));
    } else if (delta == 0.0) {
        printf("Uma raiz real: x = %.6f\n", -b / (2.0 * a));
    } else {
        printf("Nao existem raizes reais (delta negativo).\n");
    }
    return 0;
}
