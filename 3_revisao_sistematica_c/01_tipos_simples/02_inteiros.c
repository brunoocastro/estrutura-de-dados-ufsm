/*
 * INTEIROS: int, short, long, long long, signed e unsigned
 *
 * Compile e execute na pasta 01_tipos_simples:
 *   gcc -std=c11 -Wall -Wextra -pedantic 02_inteiros.c -o 02_inteiros
 *   ./02_inteiros
 */
#include <limits.h>
#include <stdio.h>

int main(void) {
    int com_sinal = -42;
    unsigned int sem_sinal = 42U; /* U: literal sem valores negativos. */
    short curto = -32000;
    long longo = 123456789L; /* L: literal com tipo long. */
    long long muito_longo = 1234567890123LL; /* LL: tipo long long. */

    printf("=== Valores e memoria ===\n");
    printf("int: %d | tamanho = %zu | endereco = %p\n", com_sinal, sizeof(com_sinal), (void *)&com_sinal);
    printf("unsigned int: %u | tamanho = %zu | endereco = %p\n", sem_sinal, sizeof(sem_sinal), (void *)&sem_sinal);
    printf("short: %hd | tamanho = %zu | endereco = %p\n", curto, sizeof(curto), (void *)&curto);
    printf("long: %ld | tamanho = %zu | endereco = %p\n", longo, sizeof(longo), (void *)&longo);
    printf("long long: %lld | tamanho = %zu | endereco = %p\n", muito_longo, sizeof(muito_longo), (void *)&muito_longo);

    printf("\n=== Limites desta compilacao ===\n");
    printf("INT_MIN = %d, INT_MAX = %d\n", INT_MIN, INT_MAX);
    printf("UINT_MAX = %u\n", UINT_MAX);
    printf("SHRT_MIN = %d, SHRT_MAX = %d\n", SHRT_MIN, SHRT_MAX);
    printf("LONG_MIN = %ld, LONG_MAX = %ld\n", LONG_MIN, LONG_MAX);
    printf("LLONG_MIN = %lld, LLONG_MAX = %lld\n", LLONG_MIN, LLONG_MAX);
    printf("\nO tamanho de int depende da implementacao; sizeof mostra o valor real.\n");
    return 0;
}
