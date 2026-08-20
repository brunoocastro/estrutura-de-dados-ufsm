/*
 * ESCOPO, VARIAVEIS LOCAIS, GLOBAIS E STATIC
 *
 * Uma variavel local existe apenas dentro do bloco onde foi declarada. Uma
 * global pode ser acessada por varias funcoes. static local preserva o valor
 * entre chamadas, mas continua invisivel fora da funcao.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 02_escopo.c -o 02_escopo
 * ./02_escopo
 */
#include <stdio.h>

int total_global = 0;

void demonstra_escopo(void) {
    int local = 10;
    static int chamadas = 0;
    chamadas++;
    total_global += local;
    printf("local = %d | static chamadas = %d | global = %d\n",
           local, chamadas, total_global);
}

int main(void) {
    printf("Primeira chamada:\n");
    demonstra_escopo();
    printf("Segunda chamada:\n");
    demonstra_escopo();
    printf("A variavel local deixa de existir ao sair da funcao.\n");
    return 0;
}
