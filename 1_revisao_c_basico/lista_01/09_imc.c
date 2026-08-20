#include <stdio.h>

int main(void) {
    double peso, altura, imc;

    printf("Digite peso (kg) e altura (m): ");
    if (scanf("%lf %lf", &peso, &altura) != 2 || peso <= 0.0 || altura <= 0.0) {
        fprintf(stderr, "Peso e altura devem ser positivos.\n");
        return 1;
    }

    imc = peso / (altura * altura);
    printf("IMC: %.2f\n", imc);
    if (imc < 20.0)
        printf("Situacao: abaixo do peso.\n");
    else if (imc < 25.0)
        printf("Situacao: peso normal.\n");
    else if (imc < 30.0)
        printf("Situacao: sobrepeso.\n");
    else if (imc < 40.0)
        printf("Situacao: obeso.\n");
    else
        printf("Situacao: obeso morbido.\n");

    return 0;
}
