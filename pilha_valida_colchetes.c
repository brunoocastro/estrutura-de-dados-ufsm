/* 	Um exemplo de utilização de pilhas de caracteres.
*	Objetivo: verificar o pareamento de símbolos '[' e ']' em uma expressão dada na variável "teste".
*	Autor: Prof. Leonardo R. Emmendorfer e Bruno Silva de Castro
*	versão 2.0
*	Data: 03/09/2026
**************************************************************************
* O programa verifica se cada '[' possui um ']' correspondente.
* Os demais caracteres da expressao sao ignorados.
*
* Exemplos:
*   "[[a+1][b-1]]" -> correta
*   "[[[a]"         -> incorreta, pois sobra um '['
*
* gcc -std=c11 -Wall -Wextra -pedantic 10_valida_colchetes.c -o 10_valida_colchetes
* ./10_valida_colchetes
 */

#include <stdio.h>
#include <string.h>

#define CAPACIDADE 10000

char pilha[CAPACIDADE];
int tamanho = 0;

/* Coloca um colchete de abertura no topo da pilha. */
int coloca(char valor) {
    if (tamanho == CAPACIDADE)
        return 0;
    pilha[tamanho++] = valor;
    return 1;
}

/* Retira um elemento e grava o valor retirado em *valor. */
int retira(char *valor) {
    if (tamanho == 0)
        return 0;
    *valor = pilha[--tamanho];
    return 1;
}

int valida_colchetes(const char expressao[]) {
    tamanho = 0;

    for (size_t i = 0; expressao[i] != '\0'; i++) {
        if (expressao[i] == '[') {
            if (!coloca('[')) {
                printf("Erro: a pilha atingiu sua capacidade maxima.\n");
                return 0;
            }
            printf("posicao %zu: coloca '[' (tamanho = %d)\n", i, tamanho);
        } else if (expressao[i] == ']') {
            char abertura;
            if (!retira(&abertura)) {
                printf("Erro na posicao %zu: encontrou ']' sem '[' correspondente.\n", i);
                return 0;
            }
            printf("posicao %zu: retira '%c' (tamanho = %d)\n", i, abertura, tamanho);
        }
    }

    if (tamanho != 0) {
        printf("Erro: ainda falta fechar %d colchete(s).\n", tamanho);
        return 0;
    }
    return 1;
}

int main(void) {
    const char *expressao_correta = "[[a+1][b-1]]";
    const char *expressao_incorreta = "[[[a]";

    printf("=== Expressao correta ===\n%s\n", expressao_correta);
    if (valida_colchetes(expressao_correta))
        printf("Entrada ok.\n");
    else
        printf("Erro: colchetes desbalanceados.\n");

    printf("\n=== Expressao incorreta ===\n%s\n", expressao_incorreta);
    if (valida_colchetes(expressao_incorreta))
        printf("Entrada ok.\n");
    else
        printf("Erro: colchetes desbalanceados.\n");

    return 0;
}
