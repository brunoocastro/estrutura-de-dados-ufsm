/*
 * PASSAGEM POR VALOR E ALTERACAO POR PONTEIRO
 *
 * C sempre passa os parametros por valor. Para alterar uma variavel do
 * chamador, passamos seu endereco e usamos o operador * para acessar o valor.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 03_valor_ponteiro.c -o 03_valor_ponteiro
 * ./03_valor_ponteiro
 */
#include <stdio.h>

void tenta_alterar(int valor) {
    valor = 99; /* Altera somente a copia local. */
    printf("Dentro da funcao, a copia vale %d.\n", valor);
}

void altera(int *endereco) {
    *endereco = 99; /* Altera a variavel no endereco recebido. */
}

int main(void) {
    int numero = 10;
    printf("Antes: numero = %d | endereco = %p\n", numero, (void *)&numero);
    tenta_alterar(numero);
    printf("Depois de passagem por valor: numero = %d\n", numero);
    altera(&numero);
    printf("Depois de passar ponteiro: numero = %d\n", numero);
    return 0;
}
