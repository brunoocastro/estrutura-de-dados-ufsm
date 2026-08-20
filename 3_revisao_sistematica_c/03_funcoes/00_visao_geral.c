/*
 * Funções em C: protótipos, escopo e passagem de parâmetros.
 *
 * Compile e execute:
 *   gcc -std=c11 -Wall -Wextra -pedantic 03_funcoes.c -o 03_funcoes
 *   ./03_funcoes
 */
#include <stdio.h>

/* Protótipos: permitem chamar as funções antes de suas definições. */
void soma_por_valor(int a, int b);
void soma_por_ponteiro(const int *a, const int *b, int *resultado);
void troca_por_valor(int a, int b);
void troca_por_ponteiro(int *a, int *b);

/* Variável global: existe durante toda a execução do programa. */
int chamadas = 0;

int main(void) {
    int x = 10;
    int y = 20;
    int resultado;

    printf("=== Funcoes e passagem de parametros ===\n");
    printf("Informe dois inteiros: ");
    if (scanf("%d %d", &x, &y) != 2)
        return 1;

    printf("\nAntes das chamadas: x = %d, y = %d\n", x, y);
    soma_por_valor(x, y);
    soma_por_ponteiro(&x, &y, &resultado);
    printf("Soma escrita em resultado por ponteiros: %d\n", resultado);

    troca_por_valor(x, y);
    printf("Depois de troca_por_valor: x = %d, y = %d\n", x, y);
    troca_por_ponteiro(&x, &y);
    printf("Depois de troca_por_ponteiro: x = %d, y = %d\n", x, y);
    printf("Funcoes chamadas: %d\n", chamadas);
    return 0;
}

/* Os parâmetros a e b são cópias: alterar as cópias não altera o chamador. */
void soma_por_valor(int a, int b) {
    int soma_local = a + b; /* Variável local: só existe nesta função. */
    chamadas++;
    printf("soma_por_valor: %d + %d = %d\n", a, b, soma_local);
}

/* C não possui passagem por referência nativa; ponteiros simulam esse efeito. */
void soma_por_ponteiro(const int *a, const int *b, int *resultado) {
    chamadas++;
    *resultado = *a + *b;
    printf("soma_por_ponteiro: acessou os valores nos enderecos %p e %p\n",
           (const void *)a, (const void *)b);
}

void troca_por_valor(int a, int b) {
    int temporario = a;
    a = b;
    b = temporario;
    chamadas++;
    printf("troca_por_valor: trocou apenas as copias locais (%d, %d)\n", a, b);
}

void troca_por_ponteiro(int *a, int *b) {
    int temporario = *a;
    *a = *b;
    *b = temporario;
    chamadas++;
    printf("troca_por_ponteiro: alterou os valores nos enderecos %p e %p\n",
           (void *)a, (void *)b);
}
