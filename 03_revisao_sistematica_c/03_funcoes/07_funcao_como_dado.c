/*
 * ============================================================================
 * Arquivo : 07_funcao_como_dado.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Funções, 0.7 Ponteiros)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Uma função também tem endereço e pode ser guardada num ponteiro para
 *   função. Assim, a mesma rotina (aplica) executa operações diferentes, e uma
 *   tabela de ponteiros escolhe a ação pelo índice (como qsort usa o
 *   comparador que você passa).
 *
 * EXEMPLO REAL
 *   Tabela de tratadores de comandos de um protocolo: o código da função
 *   Modbus (1, 3, 6, 16...) indexa um vetor de ponteiros para as rotinas que
 *   tratam cada comando. Também é assim que vetores de interrupção funcionam.
 *
 * COMPLEXIDADE
 *   Chamar via ponteiro: O(1), igual a uma chamada direta.
 *   Escolher pela tabela (operacoes[i]): O(1), em vez de um switch com n casos.
 *
 * COMO EXECUTAR
 *   ./run 07_funcao_como_dado
 * ============================================================================
 */
#include <stdio.h>

/* Operacao é "ponteiro para função que recebe (int, int) e retorna int". */
typedef int (*Operacao)(int, int);

int soma(int a, int b) { return a + b; }
int maior(int a, int b) { return a > b ? a : b; }
int menor(int a, int b) { return a < b ? a : b; }

/* O(1): função de ordem superior; não sabe qual operação vai executar. */
int aplica(Operacao operacao, int a, int b) {
    return operacao(a, b);
}

int main(void) {
    printf("soma = %d\n", aplica(soma, 7, 5));
    printf("maior = %d\n", aplica(maior, 7, 5));

    /* Tabela de despacho: índice -> função. */
    const Operacao operacoes[] = {soma, maior, menor};
    const char *nomes[] = {"soma", "maior", "menor"};
    for (size_t i = 0; i < sizeof operacoes / sizeof operacoes[0]; i++)
        printf("operacoes[%zu] (%s)(7, 5) = %d\n", i, nomes[i], operacoes[i](7, 5));

    printf("O typedef torna legivel a assinatura do ponteiro para funcao.\n");
    return 0;
}
