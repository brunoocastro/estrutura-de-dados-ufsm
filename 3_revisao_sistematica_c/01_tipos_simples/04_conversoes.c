/*
 * CONVERSOES E PROMOCOES DE TIPOS
 *
 * Compile e execute na pasta 01_tipos_simples:
 *   gcc -std=c11 -Wall -Wextra -pedantic 04_conversoes.c -o 04_conversoes
 *   ./04_conversoes
 */
#include <stdio.h>

int main(void) {
    int parte_inteira = 7;
    int divisor = 2;
    float resultado_inteiro = parte_inteira / divisor; /* Divisao inteira antes de guardar. */
    float resultado_real = (float)parte_inteira / divisor; /* Cast força divisao real. */
    int truncado = (int)3.99f; /* Conversao para int descarta a parte fracionaria. */

    printf("7 / 2 armazenado em float = %.2f\n", resultado_inteiro);
    printf("(float)7 / 2 = %.2f\n", resultado_real);
    printf("(int)3.99f = %d\n", truncado);

    /* Conversao implicita entre tipos pode alterar precisao ou faixa. */
    char pequeno = 65;
    int inteiro = pequeno; /* char promove para int na expressao. */
    printf("char 65 impresso como caractere = %c\n", pequeno);
    printf("mesmo valor promovido para int = %d\n", inteiro);

    printf("\nUse casts quando a conversao for intencional e documente a perda possivel.\n");
    return 0;
}
