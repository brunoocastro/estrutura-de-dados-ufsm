/*
 * NUMEROS REAIS: float, double e long double
 *
 * Compile e execute na pasta 01_tipos_simples:
 *   gcc -std=c11 -Wall -Wextra -pedantic 03_reais.c -o 03_reais
 *   ./03_reais
 */
#include <float.h>
#include <stdio.h>

int main(void) {
    float simples = 3.1415926535f; /* f evita conversao de double para float. */
    double dupla = 3.1415926535; /* Sem sufixo: literal de ponto e double. */
    long double estendida = 3.141592653589793238L; /* L: long double neste contexto. */

    printf("float:       %.10f | tamanho = %zu | endereco = %p\n",
           simples, sizeof(simples), (void *)&simples);
    printf("double:      %.10f | tamanho = %zu | endereco = %p\n",
           dupla, sizeof(dupla), (void *)&dupla);
    printf("long double: %.15Lf | tamanho = %zu | endereco = %p\n",
           estendida, sizeof(estendida), (void *)&estendida);

    printf("\nPrecisao aproximada informada pelo compilador:\n");
    printf("FLT_DIG = %d, DBL_DIG = %d, LDBL_DIG = %d\n", FLT_DIG, DBL_DIG, LDBL_DIG);
    printf("FLT_MAX = %e\n", FLT_MAX);
    printf("DBL_MAX = %e\n", DBL_MAX);
    printf("\nNumeros reais geralmente sao aproximacoes; nem todo decimal e representado exatamente.\n");
    return 0;
}
