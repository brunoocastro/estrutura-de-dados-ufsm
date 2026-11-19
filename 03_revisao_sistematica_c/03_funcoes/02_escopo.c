/*
 * ============================================================================
 * Arquivo : 02_escopo.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Funções)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Uma variável local existe apenas durante a execução do bloco onde foi
 *   declarada (vive na pilha). Uma global é visível por várias funções. Uma
 *   static local preserva o valor entre chamadas, mas continua invisível fora
 *   da função (não vive na pilha, e sim na área de dados estáticos).
 *
 * EXEMPLO REAL
 *   Um filtro de média móvel chamado a cada amostra precisa lembrar as
 *   amostras anteriores: um vetor static dentro da função faz isso sem
 *   expor o estado para o resto do programa.
 *
 * COMPLEXIDADE
 *   demonstra_escopo: O(1).
 *
 * COMO EXECUTAR
 *   ./run 02_escopo
 * ============================================================================
 */
#include <stdio.h>

int total_global = 0;

/* O(1). "local" nasce e morre a cada chamada (sempre vale 10 de novo);
 * "chamadas" é inicializada uma única vez, antes de o programa começar. */
void demonstra_escopo(void) {
    int local = 10;
    static int chamadas = 0;
    chamadas++;
    total_global += local;
    printf("local = %d (endereco %p) | static chamadas = %d (endereco %p) | global = %d\n",
           local, (void *)&local, chamadas, (void *)&chamadas, total_global);
}

int main(void) {
    printf("Primeira chamada:\n");
    demonstra_escopo();
    printf("Segunda chamada:\n");
    demonstra_escopo();
    printf("Terceira chamada:\n");
    demonstra_escopo();
    printf("A variavel local deixa de existir ao sair da funcao; a static permanece.\n");
    return 0;
}
