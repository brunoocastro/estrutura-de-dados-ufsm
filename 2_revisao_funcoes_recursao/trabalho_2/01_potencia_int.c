#include <math.h>

/* Calcula potencia com exponenciacao recursiva por quadratura. */
#include <stdio.h>

static float potencia_positiva(float a, unsigned int b) {
    if (b == 0)
        return 1.0f;

    float metade = potencia_positiva(a, b / 2);
    float resultado = metade * metade;
    if (b % 2 != 0)
        resultado *= a;
    return resultado;
}

float potencia_int(float a, int b) {
    if (b < 0) {
        if (a == 0.0f)
            return NAN;
        /* Esta forma também funciona para b == INT_MIN. */
        unsigned int modulo = (unsigned int)(-(b + 1)) + 1u;
        return 1.0f / potencia_positiva(a, modulo);
    }
    return potencia_positiva(a, (unsigned int)b);
}

int main(void) {
    float a;
    int b;

    printf("Informe a base real e o expoente inteiro (a b): ");
    if (scanf("%f %d", &a, &b) != 2)
        return 1;

    float resultado = potencia_int(a, b);
    if (isnan(resultado))
        printf("potencia indefinida\n");
    else
        printf("%g\n", resultado);
    return 0;
}
