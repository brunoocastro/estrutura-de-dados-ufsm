/*
 * CHAR: caracteres e pequenos inteiros
 *
 * Compile e execute na pasta 01_tipos_simples:
 *   gcc -std=c11 -Wall -Wextra -pedantic 01_char.c -o 01_char
 *   ./01_char
 */
#include <stdio.h>

int main(void) {
    /* Aspas simples representam um unico caractere. */
    char letra = 'A';
    char digito = '7';
    char nova_linha = '\n';

    printf("letra = %c\n", letra);
    printf("digito = %c\n", digito);
    printf("valor numerico de letra = %d\n", letra);
    printf("valor numerico de digito = %d\n", digito);
    printf("tamanho de letra = %zu byte(s)\n", sizeof(letra));
    printf("endereco de letra = %p\n", (void *)&letra);

    /* '\n' e uma sequencia de escape que representa uma quebra de linha. */
    printf("Este texto vem antes de nova_linha%cEste texto vem depois.\n", nova_linha);
    return 0;
}
