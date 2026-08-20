#include <math.h>
#include <stdio.h>

/* Encontra a raiz de f(x) = x^3 - x - 2 no intervalo informado. */
static double funcao(double x) {
    return x * x * x - x - 2.0;
}

int main(void) {
    double inicio, fim, tolerancia;

    printf("Digite inicio, fim e tolerancia: ");
    if (scanf("%lf %lf %lf", &inicio, &fim, &tolerancia) != 3 ||
        tolerancia <= 0.0 || inicio >= fim) {
        fprintf(stderr, "Parametros invalidos.\n");
        return 1;
    }
    if (funcao(inicio) * funcao(fim) > 0.0) {
        printf("O intervalo nao possui extremos com sinais opostos.\n");
        return 1;
    }

    while (fim - inicio > tolerancia) {
        double meio = (inicio + fim) / 2.0;
        if (funcao(inicio) * funcao(meio) <= 0.0)
            fim = meio;
        else
            inicio = meio;
    }

    double raiz = (inicio + fim) / 2.0;
    printf("Raiz aproximada: %.10f\n", raiz);
    printf("f(raiz): %.10f\n", funcao(raiz));
    return 0;
}
