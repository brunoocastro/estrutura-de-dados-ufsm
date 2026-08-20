/*
 * PONTEIROS PARA FUNCAO
 *
 * Uma funcao pode ser escolhida por meio de um ponteiro. O exemplo recebe uma
 * operacao e aplica a mesma funcao de ordem superior a dois numeros.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 07_funcao_como_dado.c -o 07_funcao_como_dado
 * ./07_funcao_como_dado
 */
#include <stdio.h>

typedef int (*Operacao)(int, int);

int soma(int a, int b) { return a + b; }
int maior(int a, int b) { return a > b ? a : b; }

int aplica(Operacao operacao, int a, int b) {
    return operacao(a, b);
}

int main(void) {
    printf("soma = %d\n", aplica(soma, 7, 5));
    printf("maior = %d\n", aplica(maior, 7, 5));
    printf("O typedef torna legivel a assinatura do ponteiro para funcao.\n");
    return 0;
}
